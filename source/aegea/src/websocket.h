#ifndef AEGEA_WEBSOCKET_H
#define AEGEA_WEBSOCKET_H

#include <memory>
#include <string>
#include <string_view>

class WebSocket
{
public:
	enum MessageType
	{
		Text,
		Binary,
		Close,
	};

	struct Message
	{
		MessageType type;
		std::string data;
	};

	static bool is_supported();
	static std::unique_ptr<WebSocket> connect(const char* url);
	virtual ~WebSocket() {}

	virtual bool is_connected() const = 0;
	// Empty string means no error yet.
	virtual std::string_view error_message() const = 0;
	// nullptr means call again later.
	// Return value valid until next call to recv().
	virtual const Message* recv() = 0;
	virtual void send_text(const char* text) = 0;
};

#endif
