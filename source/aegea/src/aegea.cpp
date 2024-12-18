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

std::string_view ArchipelagoClient::error_message()
{
	return !error.empty() ? error : socket ? socket->error_message() : "no socket";
}

void ArchipelagoClient::update()
{
	if (!socket->error_message().empty())
	{
		return;
	}

	jt::Json outgoing;
	outgoing.setArray();

	while (const WebSocket::Message* message = socket->recv())
	{
		if (message->type == WebSocket::Close)
		{
			error = "WebSocket was closed by server";
			return;
		}

		auto [status, body] = jt::Json::parse(message->data);
		if (status != jt::Json::success)
		{
			error = "Server sent invalid JSON (";
			error += jt::Json::StatusToString(status);
			error += ")";
			continue;
		}
		if (!body.isArray())
		{
			error = "Server sent non-array JSON";
			continue;
		}

		fprintf(stderr, "<-- %s\n", message->data.c_str());
		for (const auto& packet : body.getArray())
		{
			const auto& cmdJ = packet["cmd"];
			if (!cmdJ.isString())
			{
				continue;
			}
			const auto& cmd = cmdJ.getString();
			if (cmd == "RoomInfo")
			{
				jt::Json connect;
				connect["cmd"] = "Connect";
				// mandatory
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
				outgoing.getArray().push_back(connect);
			}
			else if (cmd == "ConnectionRefused")
			{
				error = "Connection refused: ";
				error += packet["errors"].toString();
			}
		}
	}

	if (!outgoing.getArray().empty())
	{
		fprintf(stderr, "--> %s\n", outgoing.toString().c_str());
		socket->send_text(outgoing.toString().c_str());
	}
}
