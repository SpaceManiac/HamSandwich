#ifndef AEGEA_H
#define AEGEA_H

#include <string>
#include <memory>
#include <string_view>

class WebSocket;

class ArchipelagoClient
{
	std::string game, address, slot, password;
	std::string error;
	std::unique_ptr<WebSocket> socket;
public:
	ArchipelagoClient(std::string_view game, std::string_view address, std::string_view slot, std::string_view password);
	~ArchipelagoClient();

	std::string_view error_message();

	void update();
};

#endif
