#ifndef AEGEA_H
#define AEGEA_H

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
	enum {
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

	std::map<std::string, jt::Json, std::less<>> data_packages;

	bool death_link_pending;

	void set_tag(std::string_view tag, bool present);
	void send_connect();
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
};

#endif
