#include "aegea.h"
#include <json.h>
#include "websocket.h"
#include <SDL2/SDL_log.h>

namespace ItemsHandling
{
	// https://github.com/ArchipelagoMW/Archipelago/blob/main/docs/network%20protocol.md#items_handling-flags
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

void ArchipelagoClient::update()
{
	if (!socket || !socket->error_message().empty())
	{
		return;
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

		for (const auto& packet : body.getArray())
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
				// Bypass outgoing queue during connection setup.
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
				connect["tags"][0] = "TextOnly";
				socket->send_text(outgoingJ.toString().c_str());
				status = WaitingForConnected;
			}
			else if (cmd == "ConnectionRefused")
			{
				error = "Server refused connection: ";
				error += packet["errors"].toString();
			}
			else if (cmd == "Connected")
			{
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
			}
			else if (cmd == "PrintJSON")
			{
			}
			else if (cmd == "DataPackage")
			{
			}
			else if (cmd == "Bounced")
			{
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
					const auto& object = packet["keys"].getObject();
					for (const auto& pair : object)
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

// https://github.com/ArchipelagoMW/Archipelago/blob/main/docs/network%20protocol.md#client---server

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
	packet["operations"][0]["value"] = value;
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
