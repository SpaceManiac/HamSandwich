#include "aegea.h"
#include <json.h>
#include "websocket.h"
#include <SDL2/SDL_log.h>

namespace
{
	const jt::Json null;
}

// https://github.com/ArchipelagoMW/Archipelago/blob/main/docs/network%20protocol.md#items_handling-flags
namespace ItemsHandling
{
	constexpr int None = 0;
	constexpr int OtherWorlds = 0b001;
	constexpr int OwnWorld = 0b010 | OtherWorlds;
	constexpr int StartingInventory = 0b110 | OtherWorlds;
	constexpr int All = OtherWorlds | OwnWorld | StartingInventory;
}

ArchipelagoClient::ArchipelagoClient(std::string_view game, std::string_view address, std::string_view slot, std::string_view password)
	: game(game)
	, address(address)
	, slot(slot)
	, password(password)
{
	// TODO: if address has no protocol, first try wss:// then fall back to ws://
	std::string fullAddress = "ws://";
	fullAddress += address;
	socket = WebSocket::connect(fullAddress.c_str());
}

ArchipelagoClient::~ArchipelagoClient()
{
	// Declaring and defining this lets good things happen.
}

std::string_view ArchipelagoClient::error_message() const
{
	return !error.empty() ? error : socket ? socket->error_message() : "no socket";
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
		packet["cmd"] = "ConnectUpdate";
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
	connect["cmd"] = "Connect";
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
#ifndef NDEBUG
	fprintf(stderr, "--> %s\n", connect.toString().c_str());
#endif
	socket->send_text(outgoingJ.toString().c_str());
	status = WaitingForConnected;
}

void ArchipelagoClient::update()
{
	if (!socket || !socket->error_message().empty())
	{
		return;
	}

	if (socket->is_connected() && status == Connecting)
	{
		status = WaitingForRoomInfo;
	}

	while (const WebSocket::Message* message = socket->recv())
	{
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
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Server sent invalid JSON: %s in %s", jt::Json::StatusToString(parseStatus), message->data.c_str());
			continue;
		}
		if (!body.isArray())
		{
			error = "Server sent non-array JSON";
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Server sent non-array JSON: %s", message->data.c_str());
			continue;
		}

		for (auto& packet : body.getArray())
		{
#ifndef NDEBUG
			fprintf(stderr, "<-- %s\n", packet.toString().c_str());
#endif

			const auto& cmdJ = packet["cmd"];
			if (!cmdJ.isString())
			{
				error = "Server sent non-string \"cmd\"";
				continue;
			}
			const auto& cmd = cmdJ.getString();
			SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "Archipelago: Handling %s", cmd.c_str());

			// https://github.com/ArchipelagoMW/Archipelago/blob/main/docs/network%20protocol.md#server---client
			if (cmd == "RoomInfo")
			{
				jt::Json outgoingJ;
				jt::Json& getDataPackage = outgoingJ.setArray().emplace_back();
				getDataPackage["cmd"] = "GetDataPackage";
				auto& games = getDataPackage["games"].setArray();
				for (const auto& pair : packet["datapackage_checksums"].getObject())
				{
					// TODO: Check for cache validitity here.
					games.emplace_back(pair.first);
				}

				if (games.empty())
				{
					send_connect();
				}
				else
				{
					// Bypass outgoing queue during connection setup.
#ifndef NDEBUG
					fprintf(stderr, "--> %s\n", getDataPackage.toString().c_str());
#endif
					socket->send_text(outgoingJ.toString().c_str());
					status = WaitingForDataPackage;
				}

				room_info.clear();
				for (auto& pair : packet.getObject())
				{
					room_info.insert(std::move(pair));
				}
			}
			else if (cmd == "DataPackage")
			{
				for (auto& pair : packet["data"]["games"].getObject())
				{
					data_packages[pair.first] = std::move(pair.second);
				}
				send_connect();
			}
			else if (cmd == "ConnectionRefused")
			{
				error = "Server refused connection: ";
				error += packet["errors"].toString();
			}
			else if (cmd == "Connected")
			{
				// Just dump both RoomInfo and Connected into the same object.
				// There are no specified overlapping fields.
				for (auto& pair : packet.getObject())
				{
					room_info.insert(std::move(pair));
				}
				status = Active;
			}
			else if (cmd == "ReceivedItems")
			{
			}
			else if (cmd == "LocationInfo")
			{
			}
			else if (cmd == "RoomUpdate")
			{
				// Can update anything, but stock server sends:
				// - From RoomInfo: permissions, hint_cost, location_check_points
				// - From Connected: hint_points, players, checked_locations
				for (auto& pair : packet.getObject())
				{
					if (pair.first == "checked_locations" && pair.second.isArray())
					{
						// checked_locations is special in that it includes only
						// new locations and must be merged in.
						auto iter = room_info.find("checked_locations");
						if (iter != room_info.end())
						{
							auto& array = iter->second.isArray() ? iter->second.getArray() : iter->second.setArray();
							for (auto& element : pair.second.getArray())
							{
								array.emplace_back(std::move(element));
							}
						}
						else
						{
							room_info.insert(std::move(pair));
						}
					}
					else
					{
						room_info.insert(std::move(pair));
					}
				}
			}
			else if (cmd == "PrintJSON")
			{
			}
			else if (cmd == "Bounced")
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
			else if (cmd == "InvalidPacket")
			{
				error = "Server rejected packet: ";
				error += packet["text"].isString() ? packet["text"].getString() : packet.toString();
			}
			else if (cmd == "Retrieved")
			{
				if (packet["keys"].isObject())
				{
					auto& object = packet["keys"].getObject();
					for (auto& pair : object)
					{
						// Let storage_changes monitor all of Get, Set, and SetNotify.
						if (storage_changes.find(pair.first) == storage_changes.end())
						{
							storage_changes[pair.first] = std::move(storage[pair.first]);
						}
						storage[pair.first] = std::move(pair.second);
					}
					storage.insert(object.begin(), object.end());
				}
			}
			else if (cmd == "SetReply")
			{
				if (packet["key"].isString())
				{
					const std::string& key = packet["key"].getString();
					storage[key] = std::move(packet["value"]);
					// If we don't already know an old value, save one now.
					if (storage_changes.find(key) == storage_changes.end())
					{
						storage_changes[key] = std::move(packet["original_value"]);
					}
				}
			}
			else
			{
				SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Archipelago: Server sent unknown \"cmd\": %s", cmd.c_str());
			}
		}
	}

	if (is_active() && !outgoing.empty())
	{
#ifndef NDEBUG
		for (const auto& packet : outgoing)
		{
			fprintf(stderr, "--> %s\n", packet.toString().c_str());
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

// ------------------------------------------------------------------------
// Data packages

const jt::Json& ArchipelagoClient::get_data_package(std::string_view game)
{
	auto iter = data_packages.find(game);
	if (iter == data_packages.end())
	{
		return null;
	}
	return iter->second;
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
	packet["cmd"] = "Bounce";
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
bool ArchipelagoClient::is_death_link_pending()
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

void ArchipelagoClient::storage_get(std::initializer_list<std::string_view> keys)
{
	jt::Json& packet = outgoing.emplace_back();
	packet["cmd"] = "Get";
	auto& keysJ = packet["keys"].setArray();
	for (auto key : keys)
	{
		keysJ.emplace_back(key);
	}
}

void ArchipelagoClient::storage_get(std::string_view key)
{
	storage_get({ key });
}

void ArchipelagoClient::storage_set(std::string_view key, jt::Json value, bool want_reply)
{
	jt::Json& packet = outgoing.emplace_back();
	packet["cmd"] = "Set";
	packet["key"] = key;
	packet["operations"][0]["operation"] = "replace";
	packet["operations"][0]["value"] = std::move(value);
	if (!want_reply) // True is the server-side default.
	{
		packet["want_reply"] = want_reply;
	}
}

void ArchipelagoClient::storage_set_notify(std::initializer_list<std::string_view> keys)
{
	jt::Json& packet = outgoing.emplace_back();
	packet["cmd"] = "SetNotify";
	auto& keysJ = packet["keys"].setArray();
	for (auto key : keys)
	{
		keysJ.emplace_back(key);
	}
}

void ArchipelagoClient::storage_set_notify(std::string_view key)
{
	storage_set_notify({ key });
}
