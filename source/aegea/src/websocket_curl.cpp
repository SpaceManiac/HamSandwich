#ifdef AEGEA_WEBSOCKET_CURL
#include "websocket.h"
#include <curl/curl.h>
#include <curl/websockets.h>
#include "owned_curl.h"

namespace
{

struct CurlWebSocket : public WebSocket
{
	owned::CURLM curlm;
	owned::CURL curl;
	bool connect_done = false;
	Message buffer;
	size_t buffer_offset = 0;
	std::string error;

	CurlWebSocket(const char* url)
		: curlm(owned::curl_multi_init())
		, curl(owned::curl_easy_init())
	{
		if (!curlm)
		{
			error = "curl_multi_init failed";
			return;
		}
		else if (!curl)
		{
			error = "curl_easy_init failed";
			return;
		}

		curl_easy_setopt(curl.get(), CURLOPT_URL, url);
		curl_easy_setopt(curl.get(), CURLOPT_CONNECT_ONLY, 2L);
		CURLMcode mret = curl_multi_add_handle(curlm.get(), curl.get());
		if (mret != CURLM_OK)
		{
			error = curl_multi_strerror(mret);
			return;
		}
	}

	bool is_connected() const
	{
		return connect_done;
	}

	std::string_view error_message() const
	{
		return error;
	}

	const Message* recv()
	{
		CURLMcode mret = curl_multi_perform(curlm.get(), nullptr);
		if (mret != CURLM_OK)
		{
			error = curl_multi_strerror(mret);
			return nullptr;
		}

		int queued;
		while (CURLMsg* msg = curl_multi_info_read(curlm.get(), &queued))
		{
			if (msg->msg == CURLMSG_DONE && msg->easy_handle == curl.get())
			{
				if (msg->data.result == CURLE_HTTP_RETURNED_ERROR)
				{
					error = "HTTP ";
					long http_code = 0;
					curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &http_code);
					error += std::to_string(http_code);
					return nullptr;
				}
				else if (msg->data.result != CURLE_OK)
				{
					error = curl_easy_strerror(msg->data.result);
					return nullptr;
				}
				connect_done = true;
			}
		}

		if (!connect_done)
		{
			return nullptr;
		}

		size_t got = 0;
		const curl_ws_frame* meta = nullptr;
		do
		{
			buffer.data.resize(buffer.data.capacity());
			CURLcode ret = curl_ws_recv(
				curl.get(),
				buffer.data.data() + buffer_offset,
				buffer.data.size() - buffer_offset,
				&got,
				&meta
			);
			if (ret == CURLE_AGAIN)
			{
				// No real error, but no more data. Call again later.
				return nullptr;
			}
			else if (ret != CURLE_OK)
			{
				error = curl_easy_strerror(ret);
				return nullptr;
			}
			buffer_offset += got;
			buffer.data.resize(buffer.data.size() + meta->bytesleft);
		} while (meta->bytesleft > 0);

		buffer.data.resize(buffer_offset);
		buffer_offset = 0;

		if (meta->flags & CURLWS_CLOSE)
		{
			buffer.type = Close;
		}
		else if (meta->flags & CURLWS_BINARY)
		{
			buffer.type = Binary;
		}
		else if (meta->flags & CURLWS_TEXT)
		{
			buffer.type = Text;
		}
		else
		{
			error = "unknown message type";
			return nullptr;
		}

		return &buffer;
	}

	void send_text(const char* text)
	{
		std::string_view text2 = text;
		do
		{
			size_t sent = 0;
			CURLcode ret = curl_ws_send(curl.get(), text2.data(), text2.size(), &sent, 0, CURLWS_TEXT);
			if (ret != CURLE_OK)
			{
				error = curl_easy_strerror(ret);
				return;
			}
			text2 = text2.substr(sent);
		} while (!text2.empty());
	}
};

}

bool WebSocket::is_supported()
{
	return true;
}

std::unique_ptr<WebSocket> WebSocket::connect(const char* url)
{
	return std::make_unique<CurlWebSocket>(url);
}
#endif
