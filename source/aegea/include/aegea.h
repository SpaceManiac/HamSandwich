#ifndef AEGEA_H
#define AEGEA_H

#include <stdint.h>
#include <initializer_list>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <string>
#include <string_view>
#include <vector>

// Forward declarations are used here so games only need to #include <json.h>
// if they are actually using the arbitrary-value storage system.

namespace jt
{
	class Json;
}

class WebSocket;

class ArchipelagoClient
{
public:
	// ------------------------------------------------------------------------
	// Basics

	ArchipelagoClient(std::string_view game, std::string_view address, std::string_view slot, std::string_view password);
	~ArchipelagoClient();

	std::string_view error_message() const;
	std::string_view connection_status() const;
	bool is_active() const;

	// Call this on a regular basis (ex: once per frame) to handle networking.
	void update();

	// ------------------------------------------------------------------------
	// Data packages

	const jt::Json& get_data_package(std::string_view game);

	// ------------------------------------------------------------------------
	// Receiving items

	// https://github.com/ArchipelagoMW/Archipelago/blob/main/docs/network%20protocol.md#NetworkItem
	struct Item
	{
		int64_t item;
		int64_t location;
		int player;
		int flags;

		// flags
		static constexpr int Normal = 0;
		static constexpr int Advancement = 0b001;
		static constexpr int Important = 0b010;
		static constexpr int Trap = 0b100;
	};

	// Return the next unprocessed item, or nullptr if everything has been processed.
	const Item* pop_received_item();
	// Return all received items regardless of processed status.
	const std::vector<Item>& all_received_items() const;

	// ------------------------------------------------------------------------
	// Sending locations

	// Call when a location is checked (picked up, completed, achieved).
	void check_location(int64_t location);

	// ------------------------------------------------------------------------
	// Messages

	// https://github.com/ArchipelagoMW/Archipelago/blob/main/docs/network%20protocol.md#jsonmessagepart
	struct MessagePart
	{
		std::string type; // If blank, presume "text".
		std::string text;
		std::string color;
		int flags; // if item_id, item_name
		int player; // if item_id, item_name, location_id, location_name

		// type
		static constexpr std::string_view Text = "text";
		static constexpr std::string_view PlayerId = "player_id";
		static constexpr std::string_view PlayerName = "player_name";
		static constexpr std::string_view ItemId = "item_id";
		static constexpr std::string_view ItemName = "item_name";
		static constexpr std::string_view LocationId = "location_id";
		static constexpr std::string_view LocationName = "location_name";
		static constexpr std::string_view EntranceName = "entrance_name";
		static constexpr std::string_view Color = "color";

		// color
		static constexpr std::string_view Bold = "bold";
		static constexpr std::string_view Underline = "underline";
		static constexpr std::string_view Black = "black";
		static constexpr std::string_view Red = "red";
		static constexpr std::string_view Green = "green";
		static constexpr std::string_view Yellow = "yellow";
		static constexpr std::string_view Blue = "blue";
		static constexpr std::string_view Magenta = "magenta";
		static constexpr std::string_view Cyan = "cyan";
		static constexpr std::string_view White = "white";
		static constexpr std::string_view BlackBg = "black_bg";
		static constexpr std::string_view RedBg = "red_bg";
		static constexpr std::string_view GreenBg = "green_bg";
		static constexpr std::string_view YellowBg = "yellow_bg";
		static constexpr std::string_view BlueBg = "blue_bg";
		static constexpr std::string_view MagentaBg = "magenta_bg";
		static constexpr std::string_view CyanBg = "cyan_bg";
		static constexpr std::string_view WhiteBg = "white_bg";
	};

	// https://github.com/ArchipelagoMW/Archipelago/blob/main/docs/network%20protocol.md#printjson
	struct Message
	{
		std::vector<MessagePart> data;
		std::string type; // If blank or not one of the constants below, use `data` only.

		int receiving;				   // if ItemSend, ItemCheat, Hint
		Item item;					   // if ItemSend, ItemCheat, Hint
		bool found;					   // if Hint
		int team;					   // if Join, Part, Chat, TagsChanged, Goal, Release, Collect, or ItemCheat
		int slot;					   // if Join, Part, Chat, TagsChanged, Goal, Release, Collect
		std::string message;		   // if Chat, ServerChat
		std::vector<std::string> tags; // if Join, TagsChanged
		int countdown;				   // if Countdown

		// type
		static constexpr std::string_view ItemSend = "ItemSend";
		static constexpr std::string_view ItemCheat = "ItemCheat";
		static constexpr std::string_view Hint = "Hint";
		static constexpr std::string_view Join = "Join";
		static constexpr std::string_view Part = "Part";
		static constexpr std::string_view Chat = "Chat";
		static constexpr std::string_view ServerChat = "ServerChat";
		static constexpr std::string_view Tutorial = "Tutorial";
		static constexpr std::string_view TagsChanged = "TagsChanged";
		static constexpr std::string_view CommandResult = "CommandResult";
		static constexpr std::string_view AdminCommandResult = "AdminCommandResult";
		static constexpr std::string_view Goal = "Goal";
		static constexpr std::string_view Release = "Release";
		static constexpr std::string_view Collect = "Collect";
		static constexpr std::string_view Countdown = "Countdown";
	};

	// Pop from this queue when you have time to display a message.
	bool pop_message(Message* message);

	// ------------------------------------------------------------------------
	// DeathLink

	// Enable this client to send and receive DeathLink.
	void death_link_enable(bool enable = true);
	// Send a DeathLink to connected games.
	void death_link_send(std::string_view cause = {}, std::string_view player = {});
	// If a DeathLink is pending, returns true and clears the pending status.
	bool is_death_link_pending();

	// ------------------------------------------------------------------------
	// Storage system

	// Contains known storage values.
	std::map<std::string, jt::Json> storage;
	// Contains previous values for storage keys that have changed.
	// The new values are already in `storage`.
	// Clear it manually once you've processed it.
	std::map<std::string, jt::Json> storage_changes;

	void storage_get(std::initializer_list<std::string_view> keys);
	void storage_get(std::string_view key);
	void storage_set(std::string_view key, jt::Json value, bool want_reply = true);
	void storage_set_notify(std::initializer_list<std::string_view> keys);
	void storage_set_notify(std::string_view key);

private:
	enum
	{
		Connecting,
		WaitingForRoomInfo,
		WaitingForDataPackage,
		WaitingForConnected,
		Active,
	} status = Connecting;

	std::string game, address, slot, password;
	std::set<std::string, std::less<>> tags;
	std::string error;
	std::unique_ptr<WebSocket> socket;
	std::vector<jt::Json> outgoing;

	std::map<std::string, jt::Json, std::less<>> room_info;
	std::map<std::string, jt::Json, std::less<>> data_packages;

	bool death_link_pending = false;
	std::queue<Message> messages_pending;
	size_t handled_received_items = 0;
	std::vector<Item> received_items;
	std::set<int64_t> checked_locations;

	void set_tag(std::string_view tag, bool present);
	void send_connect();
};

#endif
