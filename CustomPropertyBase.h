/**
 * Simple implementation of ICustomProperty to ease creation of class that support XAML {Binding}.
 * This facilitates data binding via DataTemplate since we can't use {x:Bind} in our no-XAML app development approach.
 */

#ifndef _LUWPUTILITIES_CUSTOM_PROPERTY_BASE_
#define _LUWPUTILITIES_CUSTOM_PROPERTY_BASE_

#ifdef LUU_EXPORT
namespace LUwpUtilities
{
	LUU_EXPORT delegate Platform::Object^ GetNamedValue(Platform::Object^ target, Platform::String^ name);
	LUU_EXPORT delegate void SetNamedValue(Platform::Object^ target, Platform::String^ name, Platform::Object^ value);

	[Windows::Foundation::Metadata::WebHostHidden]
	LUU_EXPORT ref class CustomPropertyBase sealed : Windows::UI::Xaml::Data::ICustomProperty
	{
	public:
		CustomPropertyBase(Windows::UI::Xaml::Interop::TypeName type, Platform::String ^name, GetNamedValue^ getter, SetNamedValue^ setter)
		{
			this->type = type;
			this->name = name;
			this->getter = getter;
			this->setter = setter;
		}

		property Windows::UI::Xaml::Interop::TypeName Type
		{
			virtual Windows::UI::Xaml::Interop::TypeName get()
			{
				//OutputDebugString(L"CustomPropertyBase->Type\n");
				return type;
			}
		}

		property Platform::String ^Name
		{
			virtual Platform::String ^get()
			{
				//OutputDebugString(L"CustomPropertyBase->Name\n");
				return name;
			}
		}

		virtual Platform::Object^ GetValue(Platform::Object^ target)
		{
			//OutputDebugString(("CustomPropertyBase->GetValue for " + target + "\n")->Data());
			return getter != nullptr ? getter(target, name) : nullptr;
		}

		virtual void SetValue(Platform::Object^ target, Platform::Object^ value)
		{
			if (setter != nullptr)
				setter(target, name, value);

			//OutputDebugString(("CustomPropertyBase->SetValue for " + target + " to " + value + "\n")->Data());
		}

		virtual Platform::Object^ GetIndexedValue(Platform::Object^ target, Platform::Object^ index)
		{
			//OutputDebugString(("CustomPropertyBase->GetIndexedValue for " + target + " at index " + index + "\n")->Data());
			return nullptr;
		}

		virtual void SetIndexedValue(Platform::Object^ target, Platform::Object^ value, Platform::Object^ index)
		{
			//OutputDebugString(("CustomPropertyBase->SetIndexedValue for " + target + " at index " + index + " to " + value + "\n")->Data());
		}

		property bool CanWrite
		{
			virtual bool get()
			{
				//OutputDebugString(L"CustomPropertyBase->CanWrite\n");
				return true; // setter != nullptr;
			}
		}
		property bool CanRead
		{
			virtual bool get()
			{
				//OutputDebugString(L"CustomPropertyBase->CanRead\n");
				return true; // getter != nullptr
			}
		}
	private:
		Windows::UI::Xaml::Interop::TypeName type;
		Platform::String ^name;
		GetNamedValue^ getter;
		SetNamedValue^ setter;
	}; // class CustomPropertyBase
} // namespace LUwpUtilities
#endif // #ifdef LUU_EXPORT

#endif // #ifndef __CUSTOM_PROPERTY_BASE