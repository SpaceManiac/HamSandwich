#include "aegea.h"
#include <time.h>
#include <stdlib.h>
#include <fstream>
#include <filesystem>
#include <json.h>
#include "websocket.h"

#ifdef NDEBUG
#define debug_log(...)
#else
#define debug_log(fmt, ...) fprintf(stderr, "[Aegea] " fmt "\n", __VA_ARGS__)
#endif

namespace
{
	const jt::Json json_null;

	ArchipelagoCache::FileSystem default_cache { "archipelago/" };

	namespace IncomingCmd
	{
		constexpr std::string_view RoomInfo = "RoomInfo";
		constexpr std::string_view ConnectionRefused = "ConnectionRefused";
		constexpr std::string_view Connected = "Connected";
		constexpr std::string_view ReceivedItems = "ReceivedItems";
		constexpr std::string_view LocationInfo = "LocationInfo";
		constexpr std::string_view RoomUpdate = "RoomUpdate";
		constexpr std::string_view PrintJSON = "PrintJSON";
		constexpr std::string_view DataPackage = "DataPackage";
		constexpr std::string_view Bounced = "Bounced";
		constexpr std::string_view InvalidPacket = "InvalidPacket";
		constexpr std::string_view Retrieved = "Retrieved";
		constexpr std::string_view SetReply = "SetReply";
	}

	namespace OutgoingCmd
	{
		constexpr std::string_view Connect = "Connect";
		constexpr std::string_view ConnectUpdate = "ConnectUpdate";
		constexpr std::string_view Sync = "Sync";
		constexpr std::string_view LocationChecks = "LocationChecks";
		constexpr std::string_view LocationScouts = "LocationScouts";
		//constexpr std::string_view UpdateHint = "UpdateHint";
		constexpr std::string_view StatusUpdate = "StatusUpdate";
		constexpr std::string_view Say = "Say";
		constexpr std::string_view GetDataPackage = "GetDataPackage";
		constexpr std::string_view Bounce = "Bounce";
		constexpr std::string_view Get = "Get";
		constexpr std::string_view Set = "Set";
		constexpr std::string_view SetNotify = "SetNotify";
	}

	// https://github.com/ArchipelagoMW/Archipelago/blob/main/docs/network%20protocol.md#items_handling-flags
	namespace ItemsHandling
	{
		constexpr int OtherWorlds = 0b001;
		constexpr int OwnWorld = 0b010 | OtherWorlds;
		constexpr int StartingInventory = 0b110 | OtherWorlds;
		constexpr int All = OtherWorlds | OwnWorld | StartingInventory;
	}

	// https://github.com/ArchipelagoMW/Archipelago/blob/main/docs/network%20protocol.md#clientstatus
	namespace ClientStatus
	{
		//constexpr int Unknown = 0;
		//constexpr int Connected = 5;
		//constexpr int Ready = 10;
		//constexpr int Playing = 20;
		constexpr int Goal = 30;
	}
}

// ------------------------------------------------------------------------

std::string ArchipelagoCache::FileSystem::read(std::string_view key)
{
	std::string filename { prefix };
	filename.append("/");
	filename.append(key);
	filename.append(".cache");

	std::ifstream ifs(filename, std::ios::in | std::ios::binary | std::ios::ate);
	if (!ifs)
	{
		return "";
	}

	auto size = ifs.tellg();
	ifs.seekg(0, std::ios::beg);
	std::string buffer(size, '\0');
	ifs.read(buffer.data(), size);
	return buffer;
}

void ArchipelagoCache::FileSystem::write(std::string_view key, std::string_view data)
{
	std::error_code ec;
	std::filesystem::create_directories(prefix, ec);

	std::string filename { prefix };
	filename.append("/");
	filename.append(key);
	filename.append(".cache");

	std::string tempFilename = filename;
	for (int i = 0; i < 10; ++i)
	{
		tempFilename += "0123456789"[rand() % 10];
	}

	std::ofstream ofs(tempFilename, std::ios::out | std::ios::binary);
	if (ofs)
	{
		ofs.write(data.data(), data.size());
		ofs.close();
		std::filesystem::rename(tempFilename, filename);
	}
}

// ------------------------------------------------------------------------

// Declaring and defining this lets `#include`rs not need Json's definition.
ArchipelagoClient::~ArchipelagoClient() = default;

ArchipelagoClient::ArchipelagoClient(std::string_view game, std::string_view address, std::string_view slot, std::string_view password)
	: cache(&default_cache)
	, game(game)
	, address(address)
	, slot(slot)
	, password(password)
{
	if (address.find("://") == std::string_view::npos)
	{
		// Address has no protocol. First try wss:// then fall back to ws://
		std::string fullAddress = "wss://";
		fullAddress += address;
		socket = WebSocket::connect(fullAddress.c_str());
		status = ConnectingAutoWss;
	}
	else
	{
		socket = WebSocket::connect(this->address.c_str());
		status = ConnectingExact;
	}
}

void ArchipelagoClient::use_cache(ArchipelagoCache *cache)
{
	this->cache = cache;
}

std::string_view ArchipelagoClient::error_message() const
{
	return !error.empty() ? error :
		!socket ? "Not connected" :
		socket->error_message();
}

std::string_view ArchipelagoClient::connection_status() const
{
	if (!socket)
	{
		return "Not connected";
	}
	else if (!socket->is_connected())
	{
		return "Connecting...";
	}
	else if (status == WaitingForRoomInfo)
	{
		// Won't be seen normally as this is the first message we recv().
		return "Waiting for room info...";
	}
	else if (status == WaitingForDataPackage)
	{
		return "Downloading data package...";
	}
	else if (status == WaitingForConnected)
	{
		return "Authenticating...";
	}
	return "Active";
}

bool ArchipelagoClient::is_active() const
{
	return socket && socket->is_connected() && status == Active;
}

void ArchipelagoClient::set_tag(std::string_view tag, bool present)
{
	if (present)
	{
		tags.emplace(tag);
	}
	else if (auto iter = tags.find(tag); iter != tags.end())
	{
		tags.erase(iter);
	}

	// If we've already sent tags, update tags now.
	if (status >= WaitingForConnected)
	{
		jt::Json& packet = outgoing.emplace_back();
		packet["cmd"] = OutgoingCmd::ConnectUpdate;
		auto& tagsJ = packet["tags"].setArray();
		for (const auto& tag : tags)
		{
			tagsJ.emplace_back(tag);
		}
	}
}

void ArchipelagoClient::send_connect()
{
	jt::Json outgoingJ;
	jt::Json& connect = outgoingJ.setArray().emplace_back();
	connect["cmd"] = OutgoingCmd::Connect;
	connect["password"] = std::string(password);
	connect["game"] = std::string(game);
	connect["name"] = std::string(slot);
	connect["uuid"] = ""; // mandatory, but not actually used
	connect["version"]["class"] = "Version";
	connect["version"]["major"] = 0;
	connect["version"]["minor"] = 5;
	connect["version"]["build"] = 1;
	connect["items_handling"] = ItemsHandling::All;
	auto& tagsJ = connect["tags"].setArray();
	for (const auto& tag : tags)
	{
		tagsJ.emplace_back(tag);
	}
	// Bypass outgoing queue during connection setup.
	debug_log("--> %s", connect.toString().c_str());
	socket->send_text(outgoingJ.toString().c_str());
	status = WaitingForConnected;
}

static void decode(ArchipelagoClient::Item* item, const jt::Json& json)
{
	item->item = json["item"].isLong() ? json["item"].getLong() : 0;
	item->location = json["location"].isLong() ? json["location"].getLong() : 0;
	item->player = json["player"].isLong() ? static_cast<int>(json["player"].getLong()) : 0;
	item->flags = json["flags"].isLong() ? static_cast<int>(json["flags"].getLong()) : 0;
}

static void decode(ArchipelagoClient::ScoutedItem* item, const jt::Json& json)
{
	item->item = json["item"].isLong() ? json["item"].getLong() : 0;
	item->location = json["location"].isLong() ? json["location"].getLong() : 0;
	item->player = json["player"].isLong() ? static_cast<int>(json["player"].getLong()) : 0;
	item->flags = json["flags"].isLong() ? static_cast<int>(json["flags"].getLong()) : 0;
}

void ArchipelagoClient::update()
{
	if (!socket)
	{
		return;
	}
	if (!socket->error_message().empty())
	{
		if (status == ConnectingAutoWss)
		{
			debug_log("Auto-TLS failed, trying insecure: %.*s", (int)socket->error_message().size(), socket->error_message().data());
			// wss://X failed, try ws://X
			std::string fullAddress = "ws://" + address;
			socket = WebSocket::connect(fullAddress.c_str());
			status = ConnectingAutoWs;
		}
		else if (status == Active || status == Reconnecting)
		{
			auto now = std::chrono::steady_clock::now();
			if (now > reconnect_after)
			{
				debug_log("Lost connection, retrying: %.*s", (int)socket->error_message().size(), socket->error_message().data());
				// We had a connection, but it died, so try to re-establish it.
				socket = WebSocket::connect(address.c_str());
				status = Reconnecting;
				error = "";  // Clear possible "Server closed WebSocket connection".
				reconnect_after = now + std::chrono::seconds(10);
			}
			else
			{
				// Last reconnect failed, so waiting to try again.
				return;
			}
		}
		else
		{
			// We never had a connection. Maybe the address was wrong or the 'net is out.
			return;
		}
	}

	while (const WebSocket::Message* message = socket->recv())
	{
		// Has to be in the loop so it's after the recv() call, otherwise we
		// might fail to commit the final address and reconnects will die.
		if (socket->is_connected() && (status == ConnectingAutoWss || status == ConnectingAutoWs || status == ConnectingExact))
		{
			// Commit whichever of wss:// or ws:// succeeded to the address field for later use.
			if (status == ConnectingAutoWss)
			{
				address.insert(0, "wss://");
			}
			else if (status == ConnectingAutoWs)
			{
				address.insert(0, "ws://");
			}
			// This will probably be overwritten immediately.
			status = WaitingForRoomInfo;
		}

		if (message->type == WebSocket::Close)
		{
			error = "Server closed WebSocket connection";
			return;
		}

		auto [parseStatus, body] = jt::Json::parse(message->data);
		if (parseStatus != jt::Json::success)
		{
			error = "Server sent invalid JSON: ";
			error += jt::Json::StatusToString(parseStatus);
			debug_log("Server sent invalid JSON: %s in %s", jt::Json::StatusToString(parseStatus), message->data.c_str());
			continue;
		}
		if (!body.isArray())
		{
			error = "Server sent non-array JSON";
			debug_log("Server sent non-array JSON: %s", message->data.c_str());
			continue;
		}

		for (auto& packet : body.getArray())
		{
			debug_log("<-- %s", packet.toString().c_str());

			const auto& cmdJ = packet["cmd"];
			if (!cmdJ.isString())
			{
				error = "Server sent non-string \"cmd\"";
				continue;
			}
			const auto& cmd = cmdJ.getString();

			// https://github.com/ArchipelagoMW/Archipelago/blob/main/docs/network%20protocol.md#server---client
			if (cmd == IncomingCmd::RoomInfo)
			{
				jt::Json outgoingJ;
				jt::Json& getDataPackage = outgoingJ.setArray().emplace_back();
				getDataPackage["cmd"] = OutgoingCmd::GetDataPackage;
				auto& games = getDataPackage["games"].setArray();
				for (const auto& pair : packet["datapackage_checksums"].getObject())
				{
					if (cache)
					{
						std::string cachedStr = cache->read(pair.first);
						if (!cachedStr.empty())
						{
							auto [status, json] = jt::Json::parse(cachedStr);
							if (status == jt::Json::success && json.isObject())
							{
								if (json["checksum"].isString() && pair.second.isString() && json["checksum"].getString() == pair.second.getString())
								{
									load_data_package(pair.first, std::move(json));
									continue;
								}
							}
						}
					}
					games.emplace_back(pair.first);
				}

				if (games.empty())
				{
					send_connect();
				}
				else
				{
					// Bypass outgoing queue during connection setup.
					debug_log("--> %s", getDataPackage.toString().c_str());
					socket->send_text(outgoingJ.toString().c_str());
					status = WaitingForDataPackage;
				}

				room_info_.clear();
				for (auto& pair : packet.getObject())
				{
					if (pair.first != "cmd")
					{
						room_info_.insert(std::move(pair));
					}
				}
			}
			else if (cmd == IncomingCmd::DataPackage)
			{
				for (auto& pair : packet["data"]["games"].getObject())
				{
					if (cache)
					{
						cache->write(pair.first, pair.second.toString());
					}
					load_data_package(std::move(pair.first), std::move(pair.second));
				}
				send_connect();
			}
			else if (cmd == IncomingCmd::ConnectionRefused)
			{
				error = "Server refused connection: ";
				error += packet["errors"].toString();
			}
			else if (cmd == IncomingCmd::Connected)
			{
				player_id_ = packet["slot"].getLong();
				storage_private_prefix = "aegea_";
				storage_private_prefix += std::to_string(player_id_);
				storage_private_prefix += "_";
				storage_private_prefix += std::to_string(std::hash<std::string>{}(slot));
				storage_private_prefix += "_";

				checked_locations_.clear();
				missing_locations_.clear();
				if (packet["checked_locations"].isArray())
				{
					for (const auto& loc : packet["checked_locations"].getArray())
					{
						if (loc.isLong())
						{
							checked_locations_.insert(loc.getLong());
						}
					}
				}
				if (packet["missing_locations"].isArray())
				{
					for (const auto& loc : packet["missing_locations"].getArray())
					{
						if (loc.isLong())
						{
							missing_locations_.insert(loc.getLong());
						}
					}
				}

				// Just dump both RoomInfo and Connected into the same object.
				// There are no specified overlapping fields.
				for (auto& pair : packet.getObject())
				{
					if (pair.first != "cmd" && pair.first != "checked_locations" && pair.first != "missing_locations")
					{
						room_info_.insert(std::move(pair));
					}
				}
				connected_pending = true;
				status = Active;
			}
			else if (cmd == IncomingCmd::ReceivedItems)
			{
				int index = packet["index"].getLong();

				if (index == 0)
				{
					// Server is overwriting our entire items list, e.g. due to a Sync.
					handled_received_items = 0;
					received_items.clear();
				}

				if (static_cast<size_t>(index) == received_items.size())
				{
					// Either because index was 0 or because it was correct.
					for (const auto& item : packet["items"].getArray())
					{
						decode(&received_items.emplace_back(), item);
					}
				}
				else
				{
					debug_log(
						"ReceivedItems expected index=%u, got index=%d",
						static_cast<unsigned int>(received_items.size()),
						index
					);

					// We're confused. Ask for a full Sync.
					jt::Json& sync = outgoing.emplace_back();
					sync["cmd"] = OutgoingCmd::Sync;

					// And send all our checked locations while we're at it.
					jt::Json& locations = outgoing.emplace_back();
					locations["cmd"] = OutgoingCmd::LocationChecks;
					auto& array = locations["locations"].setArray();
					for (int64_t location : checked_locations_)
					{
						array.emplace_back(location);
					}
				}
			}
			else if (cmd == IncomingCmd::LocationInfo)
			{
				if (packet["locations"].isArray())
				{
					for (const auto& item : packet["locations"].getArray())
					{
						int64_t location = item["location"].getLong();
						decode(&scouted_locations[location], item);
					}
				}
			}
			else if (cmd == IncomingCmd::RoomUpdate)
			{
				// Can update anything, but stock server sends:
				// - From RoomInfo: permissions, hint_cost, location_check_points
				// - From Connected: hint_points, players, checked_locations
				for (auto& pair : packet.getObject())
				{
					if (pair.first == "checked_locations" && pair.second.isArray())
					{
						// checked_locations is special.
						// The server only sends new locations in this packet.
						// We also store checked locations separately.
						for (const auto& loc : pair.second.getArray())
						{
							if (loc.isLong())
							{
								checked_locations_.insert(loc.getLong());
								missing_locations_.erase(loc.getLong());
							}
						}
					}
					else
					{
						room_info_.insert(std::move(pair));
					}
				}
			}
			else if (cmd == IncomingCmd::PrintJSON)
			{
				Message& message = messages_pending.emplace_back();
				if (packet["data"].isArray())
				{
					for (auto& partJ : packet["data"].getArray())
					{
						MessagePart& part = message.data.emplace_back();
						if (partJ["type"].isString())
						{
							part.type = std::move(partJ["type"].getString());
						}
						if (partJ["text"].isString())
						{
							part.text = std::move(partJ["text"].getString());
						}
						if (partJ["color"].isString())
						{
							part.color = std::move(partJ["color"].getString());
						}
						if (partJ["flags"].isLong())
						{
							part.flags = static_cast<int>(partJ["flags"].getLong());
						}
						if (partJ["player"].isLong())
						{
							part.player = static_cast<int>(partJ["player"].getLong());
						}
					}
				}
				if (packet["type"].isString())
				{
					message.type = std::move(packet["type"].getString());
				}
				if (packet["receiving"].isLong())
				{
					message.receiving = static_cast<int>(packet["receiving"].getLong());
				}
				decode(&message.item, packet["item"]);
				if (packet["found"].isBool())
				{
					message.found = packet["found"].getBool();
				}
				if (packet["team"].isLong())
				{
					message.team = static_cast<int>(packet["team"].getLong());
				}
				if (packet["slot"].isLong())
				{
					message.slot = static_cast<int>(packet["slot"].getLong());
				}
				if (packet["message"].isString())
				{
					message.message = std::move(packet["message"].getString());
				}
				if (packet["tags"].isArray())
				{
					for (auto& tagJ : packet["tags"].getArray())
					{
						if (tagJ.isString())
						{
							message.tags.emplace_back(std::move(tagJ.getString()));
						}
					}
				}
				if (packet["countdown"].isLong())
				{
					message.countdown = static_cast<int>(packet["countdown"].getLong());
				}
			}
			else if (cmd == IncomingCmd::Bounced)
			{
				if (packet["tags"].isArray())
				{
					for (const auto& tag : packet["tags"].getArray())
					{
						if (tag.isString() && tag.getString() == "DeathLink")
						{
							death_link_pending = true;
						}
					}
				}
			}
			else if (cmd == IncomingCmd::InvalidPacket)
			{
				error = "Server rejected packet: ";
				error += packet["text"].isString() ? packet["text"].getString() : packet.toString();
			}
			else if (cmd == IncomingCmd::Retrieved)
			{
				if (packet["keys"].isObject())
				{
					auto& object = packet["keys"].getObject();
					for (auto& pair : object)
					{
						// Let storage_changes monitor all of Get, Set, and SetNotify.
						if (storage_changes.find(pair.first) == storage_changes.end())
						{
							storage_changes[pair.first] = std::move(storage_[pair.first]);
						}
						storage_[pair.first] = std::move(pair.second);
					}
					storage_.insert(object.begin(), object.end());
				}
			}
			else if (cmd == IncomingCmd::SetReply)
			{
				if (packet["key"].isString())
				{
					const std::string& key = packet["key"].getString();
					storage_[key] = std::move(packet["value"]);
					// If we don't already know an old value, save one now.
					if (storage_changes.find(key) == storage_changes.end())
					{
						storage_changes[key] = std::move(packet["original_value"]);
					}
				}
			}
			else
			{
				debug_log("Server sent unknown \"cmd\": %s", cmd.c_str());
			}
		}
	}

	if (is_active() && !outgoing.empty())
	{
#ifndef NDEBUG
		for (const auto& packet : outgoing)
		{
			debug_log("--> %s", packet.toString().c_str());
		}
#endif

		jt::Json outgoingJ;
		auto& array = outgoingJ.setArray() = std::move(outgoing);
		socket->send_text(outgoingJ.toString().c_str());
		// Reuse vector
		outgoing = std::move(array);
		outgoing.clear();
	}
}

bool ArchipelagoClient::pop_connected()
{
	if (connected_pending)
	{
		connected_pending = false;
		return true;
	}
	return false;
}

// ------------------------------------------------------------------------
// Room info and data packages

int ArchipelagoClient::player_id() const
{
	return player_id_;
}

std::string_view ArchipelagoClient::seed_name() const
{
	auto iter = room_info_.find("seed_name");
	if (iter != room_info_.end() && iter->second.isString())
	{
		return iter->second.getString();
	}
	return "";
}

std::string_view ArchipelagoClient::slot_game_name(int slot)
{
	const auto& val = room_info_["games"][slot];
	return val.isString() ? val.getString() : "Unknown";
}

std::string_view ArchipelagoClient::slot_player_alias(int slot)
{
	if (slot == 0)
	{
		// Slot 0 is Archipelago regardless of team.
		return "Archipelago";
	}
	// For now, assume AP teams feature is not in use and slot N is player N.
	const jt::Json& val = room_info_["players"][slot - 1]["alias"];
	return val.isString() ? val.getString() : "Unknown";
}

std::string_view ArchipelagoClient::item_name(std::string_view game, int64_t item)
{
	return item_names[std::make_pair(game, item)];
}

std::string_view ArchipelagoClient::item_name(int player, int64_t item)
{
	return item_name(slot_game_name(player), item);
}

std::string_view ArchipelagoClient::item_name(const ScoutedItem& item)
{
	return item_name(item.player, item.item);
}

std::string_view ArchipelagoClient::item_name(const MessagePart& part)
{
	return item_name(part.player, atoi(part.text.c_str()));
}

std::string_view ArchipelagoClient::item_name(const Message& message)
{
	return item_name(message.receiving, message.item.item);
}

std::string_view ArchipelagoClient::location_name(std::string_view game, int64_t location)
{
	return location_names[std::make_pair(game, location)];
}

std::string_view ArchipelagoClient::location_name(int player, int64_t item)
{
	return location_name(slot_game_name(player), item);
}

std::string_view ArchipelagoClient::location_name(const Item& item)
{
	return location_name(item.player, item.location);
}

std::string_view ArchipelagoClient::location_name(const MessagePart& part)
{
	return location_name(part.player, atoi(part.text.c_str()));
}

std::string_view ArchipelagoClient::location_name(const Message& message)
{
	return location_name(message.item);
}

const std::map<std::string, jt::Json, std::less<>>& ArchipelagoClient::room_info() const
{
	return room_info_;
}

const jt::Json& ArchipelagoClient::room_info(std::string_view key) const
{
	auto iter = room_info_.find(key);
	if (iter != room_info_.end())
	{
		return iter->second;
	}
	return json_null;
}

void ArchipelagoClient::load_data_package(std::string game, jt::Json value)
{
	// Store original maps plus anything extra.
	auto [iter, _] = data_packages.emplace(std::move(game), std::move(value));
	const auto& [key, value2] = *iter;

	// From name->id maps, create inverted id->name maps.
	if (value2["item_name_to_id"].isObject())
	{
		for (const auto& pair : value2["item_name_to_id"].getObject())
		{
			item_names[std::make_pair(key, pair.second.getLong())] = pair.first;
		}
	}
	if (value2["location_name_to_id"].isObject())
	{
		for (const auto& pair : value2["location_name_to_id"].getObject())
		{
			location_names[std::make_pair(key, pair.second.getLong())] = pair.first;
		}
	}
}

const jt::Json& ArchipelagoClient::data_package(std::string_view game) const
{
	auto iter = data_packages.find(game);
	if (iter != data_packages.end())
	{
		return iter->second;
	}
	return json_null;
}

// ------------------------------------------------------------------------
// Receiving items

const ArchipelagoClient::Item* ArchipelagoClient::pop_received_item()
{
	if (handled_received_items < received_items.size())
	{
		return &received_items[handled_received_items++];
	}
	return nullptr;
}

const std::vector<ArchipelagoClient::Item>& ArchipelagoClient::all_received_items() const
{
	return received_items;
}

// ------------------------------------------------------------------------
// Locations

const std::set<int64_t>& ArchipelagoClient::checked_locations() const
{
	return checked_locations_;
}

const std::set<int64_t>& ArchipelagoClient::missing_locations() const
{
	return missing_locations_;
}

bool ArchipelagoClient::location_is_checked(int64_t location) const
{
	return checked_locations_.find(location) != checked_locations_.end();
}

bool ArchipelagoClient::check_location(int64_t location)
{
	auto [iter, inserted] = checked_locations_.insert(location);
	if (inserted)
	{
		jt::Json& packet = outgoing.emplace_back();
		packet["cmd"] = OutgoingCmd::LocationChecks;
		packet["locations"][0] = location;
	}
	return inserted;
}

void ArchipelagoClient::check_goal()
{
	jt::Json& packet = outgoing.emplace_back();
	packet["cmd"] = OutgoingCmd::StatusUpdate;
	packet["status"] = ClientStatus::Goal;
}

void ArchipelagoClient::scout_locations(Slice<int64_t> locations, bool create_as_hint)
{
	jt::Json& packet = outgoing.emplace_back();
	packet["cmd"] = OutgoingCmd::LocationScouts;
	auto& array = packet["locations"].setArray();
	for (int64_t location : locations)
	{
		array.emplace_back(location);
	}
	packet["create_as_hint"] = create_as_hint ? 2 : 0;
}

void ArchipelagoClient::scout_locations()
{
	jt::Json& packet = outgoing.emplace_back();
	packet["cmd"] = OutgoingCmd::LocationScouts;
	auto& array = packet["locations"].setArray();
	array.reserve(checked_locations_.size() + missing_locations_.size());
	for (int64_t location : checked_locations_)
	{
		array.emplace_back(location);
	}
	for (int64_t location : missing_locations_)
	{
		array.emplace_back(location);
	}
}

const ArchipelagoClient::ScoutedItem* ArchipelagoClient::item_at_location(int64_t location) const
{
	auto iter = scouted_locations.find(location);
	if (iter != scouted_locations.end())
	{
		return &iter->second;
	}
	return nullptr;
}

// ------------------------------------------------------------------------
// Messages

// Non-const so that the user can std::move() the strings out if desired.
ArchipelagoClient::Message* ArchipelagoClient::pop_message()
{
	if (handled_messages < messages_pending.size())
	{
		return &messages_pending[handled_messages++];
	}
	handled_messages = 0;
	messages_pending.clear();
	return nullptr;
}

void ArchipelagoClient::say(std::string_view text)
{
	jt::Json& packet = outgoing.emplace_back();
	packet["cmd"] = OutgoingCmd::Say;
	packet["text"] = text;
}

// ------------------------------------------------------------------------
// DeathLink

void ArchipelagoClient::death_link_enable(bool enable)
{
	set_tag("DeathLink", enable);
}

void ArchipelagoClient::death_link_send(std::string_view cause, std::string_view source)
{
	jt::Json& packet = outgoing.emplace_back();
	packet["cmd"] = OutgoingCmd::Bounce;
	packet["tags"][0] = "DeathLink";
	packet["data"]["time"] = static_cast<long long>(time(nullptr));
	if (!cause.empty())
	{
		packet["data"]["cause"] = cause;
	}
	if (!source.empty())
	{
		packet["data"]["source"] = source;
	}
}

// If a DeathLink is pending, returns true and clears the pending status.
bool ArchipelagoClient::pop_death_link()
{
	if (death_link_pending)
	{
		death_link_pending = false;
		return true;
	}
	return false;
}

// ------------------------------------------------------------------------
// Storage system

const jt::Json& ArchipelagoClient::storage(std::string_view key) const
{
	auto iter = storage_.find(key);
	if (iter != storage_.end())
	{
		return iter->second;
	}
	return json_null;
}

const ArchipelagoClient::StorageChange* ArchipelagoClient::pop_storage_change()
{
	// TODO: this is a little unwieldy. Should it just be a vector internally?
	auto iter_change = storage_changes.begin();
	if (iter_change != storage_changes.end())
	{
		auto iter_value = storage_.find(iter_change->first);
		if (iter_value != storage_.end())
		{
			storage_change.key = iter_value->first;
			storage_change.new_value = &iter_value->second;
			if (!storage_old_value)
			{
				storage_old_value = std::make_unique<jt::Json>();
			}
			*storage_old_value = std::move(iter_change->second);
			storage_change.old_value = storage_old_value.get();
			storage_changes.erase(iter_change);
			return &storage_change;
		}
		else
		{
			// Shouldn't happen, but just in case.
			storage_changes.erase(iter_change);
		}
	}
	return nullptr;
}

void ArchipelagoClient::storage_get(Slice<std::string_view> keys)
{
	jt::Json& packet = outgoing.emplace_back();
	packet["cmd"] = OutgoingCmd::Get;
	auto& keysJ = packet["keys"].setArray();
	for (auto key : keys)
	{
		keysJ.emplace_back(key);
	}
}

void ArchipelagoClient::storage_set(std::string_view key, jt::Json value, bool want_reply)
{
	jt::Json& packet = outgoing.emplace_back();
	packet["cmd"] = OutgoingCmd::Set;
	packet["key"] = key;
	packet["operations"][0]["operation"] = "replace";
	packet["operations"][0]["value"] = std::move(value);
	if (!want_reply) // True is the server-side default.
	{
		packet["want_reply"] = want_reply;
	}
}

void ArchipelagoClient::storage_set_notify(Slice<std::string_view> keys)
{
	jt::Json& packet = outgoing.emplace_back();
	packet["cmd"] = OutgoingCmd::SetNotify;
	auto& keysJ = packet["keys"].setArray();
	for (auto key : keys)
	{
		keysJ.emplace_back(key);
	}
}

std::string_view ArchipelagoClient::storage_private() const
{
	return storage_private_prefix;
}

std::string ArchipelagoClient::storage_private(std::string_view key) const
{
	std::string copy = storage_private_prefix;
	copy += key;
	return copy;
}

std::string_view ArchipelagoClient::starts_with_storage_private(std::string_view full_key, std::string_view prefix) const
{
	if (full_key.size() >= storage_private_prefix.size() + prefix.size()
		&& full_key.substr(0, storage_private_prefix.size()) == storage_private_prefix
		&& full_key.substr(storage_private_prefix.size(), prefix.size()) == prefix)
	{
		return full_key.substr(storage_private_prefix.size() + prefix.size());
	}
	return {};
}

bool ArchipelagoClient::is_storage_private(std::string_view full_key, std::string_view key) const
{
	return full_key.size() == storage_private_prefix.size() + key.size()
		&& full_key.substr(0, storage_private_prefix.size()) == storage_private_prefix
		&& full_key.substr(storage_private_prefix.size(), key.size()) == key;
}
