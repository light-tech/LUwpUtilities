/* This header provide macros
 *  - to quickly declare interface methods of ICustomPropertyProvider to avoid tedious retyping;
 *  - to implement common interface methods of ICustomPropertyProvider.
 * Note that ICustomPropertyProvider should be implemented by class that provides XAML data binding.
 * 
 * This is to overcome UWP limitation that we cannot export unsealed public class in a WinRT DLL.
 */

#define DECLARE_ICUSTOMPROPERTYPROVIDER_VIRTUAL_METHODS \
		property Windows::UI::Xaml::Interop::TypeName Type { virtual Windows::UI::Xaml::Interop::TypeName get(); } \
		virtual Windows::UI::Xaml::Data::ICustomProperty ^GetCustomProperty(Platform::String ^name); \
		virtual Windows::UI::Xaml::Data::ICustomProperty ^GetIndexedProperty(Platform::String ^name, Windows::UI::Xaml::Interop::TypeName type); \
		virtual Platform::String ^GetStringRepresentation();

#define IMPLEMENT_ICUSTOMPROPERTYPROVIDER_VIRTUAL_METHODS(cn) \
		Windows::UI::Xaml::Interop::TypeName cn ## ::Type::get() { return Windows::UI::Xaml::Interop::TypeName(cn ## ::typeid); }; \
		Windows::UI::Xaml::Data::ICustomProperty^ cn ## ::GetIndexedProperty(Platform::String ^name, Windows::UI::Xaml::Interop::TypeName type) { return nullptr; } \
		Platform::String^ cn ## ::GetStringRepresentation() { return nullptr; }

// Declare a static LUwpUtilities::CustomPropertyBase^ for property PN of data type PT
// that essentially return D->PN and setting D->PN
#define DECLARE_CUSTOM_PROPERTY(D, PT, PN) \
static LUwpUtilities::CustomPropertyBase^ D ## PN ## Property =\
	ref new LUwpUtilities::CustomPropertyBase(\
		Windows::UI::Xaml::Interop::TypeName(PT::typeid),\
		#PN,\
		ref new LUwpUtilities::GetNamedValue([](Platform::Object^ target, Platform::String^ name)\
		{\
			return ((D^)target)->PN;\
		}),\
		ref new LUwpUtilities::SetNamedValue([](Platform::Object^ target, Platform::String^ name, Platform::Object^ value)\
		{\
			((D^)target)->PN = (PT^) value;\
		}));

#define DECLARE_READONLY_CUSTOM_PROPERTY(D, PT, PN) \
static LUwpUtilities::CustomPropertyBase^ D ## PN ## Property =\
	ref new LUwpUtilities::CustomPropertyBase(\
		Windows::UI::Xaml::Interop::TypeName(PT::typeid),\
		#PN,\
		ref new LUwpUtilities::GetNamedValue([](Platform::Object^ target, Platform::String^ name)\
		{\
			return ((D^)target)->PN;\
		}),\
		nullptr);

// Check if a property is PN and return the instance D ## PN ## Property
// declared using DECLARE_CUSTOM_PROPERTY macro above
#define CHECK_AND_RETURN(D, PN) if (name == #PN) return D ## PN ## Property;