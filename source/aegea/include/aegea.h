#ifndef AEGEA_H
#define AEGEA_H

#include <stdint.h>
#include <initializer_list>
#include <map>
#include <memory>
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
	// A convenience for passing vectors, initializer lists, or other containers.
	// Should only be used for temporaries.
	template <typename T>
	class Slice
	{
		const T* begin_;
		const T* end_;
	public:
		Slice(const Slice&) = delete;
		Slice(Slice&&) = delete;
		void operator=(const Slice&) = delete;
		void operator=(Slice&&) = delete;

		Slice(const T* begin, const T* end) : begin_(begin), end_(end) {}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Winit-list-lifetime"
#endif
		Slice(std::initializer_list<T> init) : begin_(init.begin()), end_(init.end()) {}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

		Slice(const std::vector<T>& container) : begin_(container.data()), end_(container.data() + container.size()) {}

		const T* begin() { return begin_; }
		const T* end() { return end_; }
	};

public:
	// ------------------------------------------------------------------------
	// Basics

	~ArchipelagoClient();
	ArchipelagoClient(std::string_view game, std::string_view address, std::string_view slot, std::string_view password);

	std::string_view error_message() const;
	std::string_view connection_status() const;
	bool is_active() const;

	// Call this on a regular basis (ex: once per frame) to handle networking.
	void update();

	// If a reconnect has occurred, returns true and clears the pending status.
	bool pop_connected();

	// ------------------------------------------------------------------------
	// Room info and data packages

	// Get our numeric player ID.
	int player_id() const;
	// Get Archipelago's unique ID for this generated world.
	std::string_view seed_name() const;

	std::string_view slot_game_name(int slot);
	std::string_view slot_player_alias(int slot);
	std::string_view item_name(int64_t item);
	std::string_view location_name(int64_t location);

	// Get the combined RoomInfo, Connected, and RoomUpdate data, except for
	// checked_locations and missing_locations.
	const std::map<std::string, jt::Json, std::less<>>& room_info() const;
	// Get a piece of room info by key, or null if absent.
	const jt::Json& room_info(std::string_view key) const;
	// Get the arbitrary JSON data package for the given game.
	const jt::Json& data_package(std::string_view game) const;

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

	// Pop unobserved received item.
	// Returns `nullptr` if queue is empty.
	// Pointer is invalidated on next call to `pop_received_item` or `update`.
	const Item* pop_received_item();
	// Get all items ever received, even already-observed items.
	const std::vector<Item>& all_received_items() const;

	// ------------------------------------------------------------------------
	// Locations

	// Get observed received items.
	const std::set<int64_t>& checked_locations() const;
	const std::set<int64_t>& missing_locations() const;

	// Call when a location is checked (picked up, completed, achieved).
	void check_location(int64_t location);
	// Call when the game's goal is completed.
	void check_goal();

	// Call to scout specific locations. Can optionally mark them as hinted.
	void scout_locations(Slice<int64_t> locations, bool create_as_hint = false);
	// Call to scout all of this game's locations, without creating hints.
	void scout_locations();
	// Return the item scouted at a particular location. Requires calling scout_locations first.
	const Item* item_at_location(int64_t location) const;

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

	// Pop unobserved chat message, including sending/receiving items.
	// Returns `nullptr` if queue is empty.
	// Pointer is invalidated on next call to `pop_message` or `update`.
	Message* pop_message();

	void say(std::string_view text);

	// ------------------------------------------------------------------------
	// DeathLink

	// Enable this client to send and receive DeathLink.
	void death_link_enable(bool enable = true);
	// Send a DeathLink to connected games.
	void death_link_send(std::string_view cause = {}, std::string_view player = {});
	// If a DeathLink is pending, returns true and clears the pending status.
	bool pop_death_link();

	// ------------------------------------------------------------------------
	// Storage system

	struct StorageChange
	{
		std::string_view key;
		const jt::Json* new_value;
		const jt::Json* old_value;
	};

	// Get cached storage value. Returns null if absent.
	const jt::Json& storage(std::string_view key) const;

	// Pop unobserved storage change.
	// Returns `nullptr` if queue is empty.
	// Pointer is invalidated on next call to `pop_storage_change` or `update`.
	const StorageChange* pop_storage_change();

	// Request storage values. They will appear in `storage` and `storage_changes` later.
	void storage_get(Slice<std::string_view> keys);
	// Set a storage value. If want_reply is true, it will appear in `storage` and `storage_changes` later.
	void storage_set(std::string_view key, jt::Json value, bool want_reply = true);
	// Request notification for future changes to storage values. They will appear in `storage` and `storage_changes` later.
	void storage_set_notify(Slice<std::string_view> keys);

	// Return a storage prefix unique to this slot to avoid conflicts.
	std::string_view storage_private();
	// Return storage_private() + key.
	std::string storage_private(std::string_view key);

	// ------------------------------------------------------------------------
private:
	enum
	{
		Idle,
		ConnectingExact,
		ConnectingAutoWss,
		WaitingForRoomInfo,
		WaitingForDataPackage,
		WaitingForConnected,
		Active,
	} status = Idle;

	std::string game, address, slot, password;
	std::set<std::string, std::less<>> tags;
	std::string error;
	std::unique_ptr<WebSocket> socket;
	std::vector<jt::Json> outgoing;

	int player_id_ = -1;
	std::map<std::string, jt::Json, std::less<>> room_info_;
	std::map<std::string, jt::Json, std::less<>> data_packages;
	std::map<int64_t, std::string> item_names;
	std::map<int64_t, std::string> location_names;

	bool connected_pending = false;
	bool death_link_pending = false;
	size_t handled_messages = 0;
	std::vector<Message> messages_pending;
	size_t handled_received_items = 0;
	std::vector<Item> received_items;
	std::set<int64_t> checked_locations_;
	std::set<int64_t> missing_locations_;
	std::map<int64_t, Item> scouted_locations;

	std::string storage_private_prefix;
	std::map<std::string, jt::Json, std::less<>> storage_;
	std::map<std::string, jt::Json> storage_changes;
	std::unique_ptr<jt::Json> storage_old_value;
	StorageChange storage_change;

	void set_tag(std::string_view tag, bool present);
	void send_connect();
	void load_data_package(std::string game, jt::Json value);
};

#endif
