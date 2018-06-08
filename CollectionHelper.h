/**
 * Various static helper method MakeIntVector, MakeStringVector and MakeObjectVector to avoid the heavy header #include <collection.h>
 */

#ifndef _LUWPUTILITIES_COLLECTION_HELPER_
#define _LUWPUTILITIES_COLLECTION_HELPER_

#ifdef LUU_EXPORT

#include "LUwpUtilities.h"
#include <collection.h>

namespace LUwpUtilities
{
	// Static methods to create IVector; to avoid the heavy #include <collection.h>
	LUU_EXPORT ref class CH sealed
	{
	public:
		STATIC_INLINE Windows::Foundation::Collections::IVector<int>^ MakeIntVector()
		{
			return ref new Platform::Collections::Vector<int>();
		}

		STATIC_INLINE Windows::Foundation::Collections::IVector<Platform::String^>^ MakeStringVector()
		{
			return ref new Platform::Collections::Vector<Platform::String^>();
		}

		STATIC_INLINE Windows::Foundation::Collections::IVector<Platform::Object^>^ MakeObjectVector()
		{
			return ref new Platform::Collections::Vector<Platform::Object^>();
		}
	}; // class SH
} // namespace LUwpUtilities
#endif

#endif // #ifndef _COLLECTION_HELPER