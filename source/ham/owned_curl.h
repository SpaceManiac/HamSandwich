#ifndef OWNED_CURL_H
#define OWNED_CURL_H

#include <memory>
#include <curl/curl.h>
#include <SDL_log.h>

// ----------------------------------------------------------------------------
// CURL

namespace owned
{
	namespace _deleter
	{
		struct CURL
		{
			void operator()(::CURL* ptr)
			{
				return curl_easy_cleanup(ptr);
			}
		};
	}

	typedef std::unique_ptr<::CURL, _deleter::CURL> CURL;

	inline CURL curl_easy_init()
	{
		return CURL { ::curl_easy_init() };
	}

	inline CURL curl_easy_duphandle(::CURL* d)
	{
		return CURL { ::curl_easy_duphandle(d) };
	}
}

// ----------------------------------------------------------------------------
// CURLM

namespace owned
{
	namespace _deleter
	{
		struct CURLM
		{
			void operator()(::CURLM* ptr)
			{
				CURLMcode err;
				if (ptr && (err = curl_multi_cleanup(ptr)) != CURLM_OK)
				{
					SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "curl_multi_cleanup: %s", curl_multi_strerror(err));
				}
			}
		};
	}

	typedef std::unique_ptr<::CURLM, _deleter::CURLM> CURLM;

	inline CURLM curl_multi_init()
	{
		return CURLM { ::curl_multi_init() };
	}
}

[[nodiscard]] // If you don't care about the return value, use .reset() instead.
inline CURLMcode curl_multi_cleanup(owned::CURLM ptr)
{
	return curl_multi_cleanup(ptr.release());
}

#endif
