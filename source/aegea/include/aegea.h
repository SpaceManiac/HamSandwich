#ifndef AEGEA_H
#define AEGEA_H

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace jt
{
	class Json;
}

class WebSocket;

class ArchipelagoClient
{
	enum {
		WaitingForRoomInfo,
		WaitingForDataPackage,
		WaitingForConnected,
		Active,
	} status = WaitingForRoomInfo;

	std::string game, address, slot, password;
	std::string error;
	std::unique_ptr<WebSocket> socket;
	std::vector<jt::Json> outgoing;

	std::map<std::string, jt::Json> storage;
	std::map<std::string, jt::Json> storageChanges;
public:
	ArchipelagoClient(std::string_view game, std::string_view address, std::string_view slot, std::string_view password);
	~ArchipelagoClient();

	std::string_view error_message() const;
	std::string_view connection_status() const;
	bool is_active() const;

	// Call this on a regular basis (ex: once per frame) to handle networking.
	void update();

	void storage_set(std::string_view key, jt::Json value, bool want_reply = true);
};

#endif
