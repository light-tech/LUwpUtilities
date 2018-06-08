/**
 * Some simple method to perform string conversion (C string to Platform::String).
 */

#include "LVitaCpp\StringHelper.h"

STATIC_INLINE Platform::String^ ToPlatformString(const char* str)
{
	wchar_t * wstr = ToWideString(str);
	auto res = ref new Platform::String(wstr);
	delete[] wstr;
	return res;
}

STATIC_INLINE Platform::String^ ToPlatformString(const char* str, int length)
{
	wchar_t * wstr = ToWideString(str, length);
	auto res = ref new Platform::String(wstr);
	delete[] wstr;
	return res;
}
