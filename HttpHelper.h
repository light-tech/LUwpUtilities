/**
 * Various static method to deal with HTTP requests and responses.
 * For example,
 *  - Get(url) makes a GET request and return a response
 *  - HttpResponseToText(response) read the response content as string (must be in background thread)
 */

#ifndef _LUWPUTILITIES_HTTP_HELPER_
#define _LUWPUTILITIES_HTTP_HELPER_

#ifdef LUU_EXPORT

#include "LUwpUtilities.h"
#include <ppltasks.h>

namespace LUwpUtilities
{
	LUU_EXPORT delegate void HttpResponseHandler(Windows::Web::Http::HttpResponseMessage^ response);

using namespace Concurrency;
using namespace Windows::Foundation;
using namespace Windows::Storage::Streams;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;

	// Static methods to perform basic Http operations like cURL
	LUU_EXPORT ref class Http sealed
	{
	public:
		STATIC_INLINE Windows::Web::Http::HttpClient^ GetHttpClient()
		{
			static Windows::Web::Http::HttpClient^ httpClient = nullptr;

			if (httpClient == nullptr)
			{
				auto filter = ref new Windows::Web::Http::Filters::HttpBaseProtocolFilter();
				filter->CacheControl->ReadBehavior = Windows::Web::Http::Filters::HttpCacheReadBehavior::MostRecent;
				filter->CacheControl->WriteBehavior = Windows::Web::Http::Filters::HttpCacheWriteBehavior::NoCache;
				httpClient = ref new Windows::Web::Http::HttpClient(filter);
			}

			return httpClient;
		}

		STATIC_INLINE Platform::String^ DecodeUrl(
			Platform::String^ text
		)
		{
			// TODO Implement
			return nullptr;
		}

		STATIC_INLINE Platform::String^ EncodeUrl(
			Platform::String^ text
		)
		{
			// TODO Implement
			return nullptr;
		}

		STATIC_INLINE Platform::String^ HttpResponseToText(
			Windows::Web::Http::HttpResponseMessage^ response
		)
		{
			return create_task(response->Content->ReadAsStringAsync()).get();
		}

		STATIC_INLINE Windows::Storage::Streams::IBuffer^ HttpResponseToBuffer(
			Windows::Web::Http::HttpResponseMessage^ response
		)
		{
			return create_task(response->Content->ReadAsBufferAsync()).get();
		}

		STATIC_INLINE Windows::Web::Http::HttpResponseMessage^ Get(
			Platform::String^ url
		)
		{
			return create_task(GetHttpClient()->GetAsync(ref new Uri(url), HttpCompletionOption::ResponseContentRead)).get();
		}

		STATIC_INLINE void GetAsync(
			Platform::String^ url,
			HttpResponseHandler^ on_response
		)
		{
			create_task(GetHttpClient()->GetAsync(ref new Uri(url), HttpCompletionOption::ResponseContentRead))
				.then([=](task<HttpResponseMessage^> previous_task)
			{
				try
				{
					auto response = previous_task.get();
					on_response(response);
				}
				catch (Platform::Exception^ e)
				{
				}
			}, task_continuation_context::use_current());
		}

		STATIC_INLINE void GetAsync(
			Platform::String^ url,
			HttpResponseHandler^ on_response,
			ExceptionHandler^ on_error
		)
		{
			create_task(GetHttpClient()->GetAsync(ref new Uri(url), HttpCompletionOption::ResponseContentRead))
				.then([=](task<HttpResponseMessage^> previous_task)
			{
				try
				{
					auto response = previous_task.get();
					on_response(response);
				}
				catch (Platform::Exception^ e)
				{
					if (on_error != nullptr)
						on_error(e);
				}
			}, task_continuation_context::use_current());
		}

		STATIC_INLINE void PrintHttpResponse(HttpResponseMessage^ response)
		{
			if (response == nullptr)
				return;

			create_task(response->Content->ReadAsBufferAsync())
				.then([response](IBuffer^ responseBuffer)
			{
				OutputDebugString(response->ToString()->Data());
				auto responseReader = DataReader::FromBuffer(responseBuffer);
				auto size = responseReader->UnconsumedBufferLength;
				char *buf = new char[size + 1];
				for (int i = 0; i < size; i++)
					buf[i] = responseReader->ReadByte();
				buf[size] = '\0';
				OutputDebugStringA(buf);
				delete[] buf;
			});
		}
	}; // class Http
} // namespace LUwpUtilities
#endif

#endif // #ifndef _HTTP_HELPER