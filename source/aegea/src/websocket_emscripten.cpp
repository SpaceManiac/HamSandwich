#ifdef AEGEA_WEBSOCKET_EMSCRIPTEN
#include "websocket.h"
#include <queue>
#include <emscripten/websocket.h>
#include <SDL/SDL_log.h>

typedef EMSCRIPTEN_WEBSOCKET_T _EMSCRIPTEN_WEBSOCKET_T;
#undef EMSCRIPTEN_WEBSOCKET_T
typedef _EMSCRIPTEN_WEBSOCKET_T EMSCRIPTEN_WEBSOCKET_T;

namespace owned
{
	class EMSCRIPTEN_WEBSOCKET_T
	{
		static const ::EMSCRIPTEN_WEBSOCKET_T INVALID = -99;
		::EMSCRIPTEN_WEBSOCKET_T raw;

		explicit EMSCRIPTEN_WEBSOCKET_T(int raw) : raw(raw) {}
	public:
		EMSCRIPTEN_WEBSOCKET_T() : raw(-1) {}
		EMSCRIPTEN_WEBSOCKET_T(EMSCRIPTEN_WEBSOCKET_T&) = delete;
		EMSCRIPTEN_WEBSOCKET_T(EMSCRIPTEN_WEBSOCKET_T&& other) noexcept
			: raw(other.raw)
		{
			other.raw = INVALID;
		}

		EMSCRIPTEN_WEBSOCKET_T& operator=(EMSCRIPTEN_WEBSOCKET_T&) = delete;
		EMSCRIPTEN_WEBSOCKET_T& operator=(EMSCRIPTEN_WEBSOCKET_T&& other) noexcept
		{
			// swap and let other's destructor run
			std::swap(raw, other.raw);
			return *this;
		}

		friend EMSCRIPTEN_WEBSOCKET_T emscripten_websocket_new(EmscriptenWebSocketCreateAttributes *createAttributes);

		~EMSCRIPTEN_WEBSOCKET_T()
		{
			EMSCRIPTEN_RESULT ret;
			if (*this && (ret = ::emscripten_websocket_delete(raw)) != EMSCRIPTEN_RESULT_SUCCESS)
			{
				SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "emscripten_websocket_delete: %d", ret);
			}
		}

		int get() { return raw; }

		explicit operator bool() const noexcept { return raw >= 0; }
	};

	EMSCRIPTEN_WEBSOCKET_T emscripten_websocket_new(EmscriptenWebSocketCreateAttributes *createAttributes)
	{
		return EMSCRIPTEN_WEBSOCKET_T { ::emscripten_websocket_new(createAttributes) };
	}
}

namespace
{

struct EmscriptenWebSocket : public WebSocket
{
	owned::EMSCRIPTEN_WEBSOCKET_T socket;
	std::queue<Message> queue;
	Message buffer;
	std::string error;
	bool opened = false;

	EmscriptenWebSocket(const char* url)
	{
		EmscriptenWebSocketCreateAttributes create = { url, nullptr, false };
		socket = owned::emscripten_websocket_new(&create);
		if (!socket)
		{
			error = "emscripten_websocket_new: ";
			error += std::to_string(socket.get());
			return;
		}

		emscripten_websocket_set_onopen_callback(socket.get(), this, onopen);
		emscripten_websocket_set_onmessage_callback(socket.get(), this, onmessage);
		emscripten_websocket_set_onerror_callback(socket.get(), this, onerror);
		emscripten_websocket_set_onclose_callback(socket.get(), this, onclose);
	}

	static EM_BOOL onopen(int eventType, const EmscriptenWebSocketOpenEvent *websocketEvent, void *userData)
	{
		(void)eventType;
		(void)websocketEvent;

		auto self = static_cast<EmscriptenWebSocket*>(userData);
		self->opened = true;
		return EM_TRUE; // has no effect
	}

	static EM_BOOL onmessage(int eventType, const EmscriptenWebSocketMessageEvent *websocketEvent, void *userData)
	{
		(void)eventType;

		auto self = static_cast<EmscriptenWebSocket*>(userData);
		self->queue.push(Message
		{
			websocketEvent->isText ? Text : Binary,
			// if isText, Emscripten includes a trailing \0 that we need to omit
			{ websocketEvent->data, websocketEvent->data + websocketEvent->numBytes - (websocketEvent->isText ? 1 : 0) }
		});
		return EM_TRUE; // has no effect
	}

	static EM_BOOL onerror(int eventType, const EmscriptenWebSocketErrorEvent *websocketEvent, void *userData)
	{
		(void)eventType;
		(void)websocketEvent;

		auto self = static_cast<EmscriptenWebSocket*>(userData);
		self->error = "communication error";
		return EM_TRUE; // has no effect
	}

	static EM_BOOL onclose(int eventType, const EmscriptenWebSocketCloseEvent *websocketEvent, void *userData)
	{
		(void)eventType;

		auto self = static_cast<EmscriptenWebSocket*>(userData);
		self->queue.push(Message
		{
			Close,
			websocketEvent->reason
		});
		return EM_TRUE; // has no effect
	}

	bool is_connected() const
	{
		return opened;
	}

	std::string_view error_message() const
	{
		return error;
	}

	const Message* recv()
	{
		if (queue.empty())
		{
			return nullptr;
		}
		buffer = std::move(queue.front());
		queue.pop();
		return &buffer;
	}

	void send_text(const char* text)
	{
		EMSCRIPTEN_RESULT ret = emscripten_websocket_send_utf8_text(socket.get(), text);
		if (ret != EMSCRIPTEN_RESULT_SUCCESS)
		{
			error = "emscripten_websocket_send_utf8_text: ";
			error += std::to_string(ret);
			return;
		}
	}
};

}

bool WebSocket::is_supported()
{
	return emscripten_websocket_is_supported();
}

std::unique_ptr<WebSocket> WebSocket::connect(const char* url)
{
	return std::make_unique<EmscriptenWebSocket>(url);
}
#endif
