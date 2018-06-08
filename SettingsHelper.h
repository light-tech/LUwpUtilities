/**
 * Various classes to encapsulate various type of settings such as
 * CheckBox (bool)
 * ToggleSwitch (bool)
 * ComboBox (string/int from a selection list)
 * TextBox (string/int)
 *
 * The easiest way to add settings to your application is to make a class,
 * say AppUserSettings, to manage your app settings. Then add the various
 * settings as property.
 * Alternatively, add the settings as member of the object where the settings
 * are applied. For instance, add setting members to App class for settings
 * that are global to the app; to MainPage class for setting related to the page.
 * See our App.cpp for an example.
 */

#ifndef _LUWPUTILITIES_SETTINGS_HELPER_
#define _LUWPUTILITIES_SETTINGS_HELPER_

// Declare a property whose value is of type T, whose name is PN and default value DV
#define DECLARE_PROPERTY(T, PN, DV) \
	public: property T PN { \
		T get() { return _ ## PN; } \
		void set(T value) { _ ## PN = value; PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(#PN)); } \
	} \
	void load ## PN() {\
		auto settings = Windows::Storage::ApplicationData::Current->LocalSettings->Values; \
		PN = settings->HasKey(#PN) ? static_cast<T>(settings->Lookup(#PN)) : DV; \
	} \
	void save ## PN() {\
		auto settings = Windows::Storage::ApplicationData::Current->LocalSettings->Values;\
		settings->Insert(#PN, _ ## PN);\
	}\
	private: T _ ## PN;

// Make a new ToggleSwitch setting PN with default value DV
// This will declare a bool property PN inside the current class
// and methods load##PN() to load the property from LocalSettings
// save##PN() to save the settings from the ToggleSwitch to
// HR = Header of the ToggleSwitch
// PN = Property name
// DV = Default value
#define DECLARE_TOGGLESWITCH_PROPERTY(HR, PN, DV) \
	public:\
	property Windows::UI::Xaml::Controls::ToggleSwitch^ PN ## UI {\
		Windows::UI::Xaml::Controls::ToggleSwitch^ get() {\
			auto ui = ref new Windows::UI::Xaml::Controls::ToggleSwitch(); \
			ui->Header = HR;\
			ui->IsOn = PN;\
			ui->Toggled += ref new Windows::UI::Xaml::RoutedEventHandler([this, ui](Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)\
			{\
				PN = ui->IsOn;\
			});\
			return ui;\
		}\
	}\
	DECLARE_PROPERTY(bool, PN, DV)

// Make a new CheckBox setting PN with default value DV
// This will declare a bool property PN inside the current class
// and methods load##PN() to load the property from LocalSettings
// save##PN() to save the settings from the CheckBox to
// HR = Header of the CheckBox
// PN = Property name
// DV = Default value
#define DECLARE_CHECKBOX_PROPERTY(HR, PN, DV) \
	public:\
	property Windows::UI::Xaml::Controls::CheckBox^ PN ## UI {\
		Windows::UI::Xaml::Controls::CheckBox^ get() {\
			auto ui = ref new Windows::UI::Xaml::Controls::CheckBox(); \
			ui->IsChecked = PN;\
			ui->Content = HR; \
			ui->Click += ref new Windows::UI::Xaml::RoutedEventHandler([this, ui](Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)\
			{\
				PN = ui->IsChecked->Value;\
			});\
			return ui;\
		}\
	}\
	DECLARE_PROPERTY(bool, PN, DV)

// Make a new ComboBox setting PN with default value DV
// This will declare a bool property PN inside the current class
// and methods load##PN() to load the property from LocalSettings
// save##PN() to save the settings from the ComboBox to
// HR = Header of the ComboBox
// T = Type of the selection item (typically int or String)
// PN = Property name
// DV = Default value
// IS = ItemsSource of the ComboBox, a list whose items are of given type T
#define DECLARE_COMBOBOX_PROPERTY(HR, T, PN, DV, IS) \
	public:\
	property Windows::UI::Xaml::Controls::ComboBox^ PN ## UI {\
		Windows::UI::Xaml::Controls::ComboBox^ get() { \
			auto ui = ref new Windows::UI::Xaml::Controls::ComboBox();\
			ui->Header = HR;\
			ui->ItemsSource = IS;\
			ui->SelectedItem = PN;\
			ui->SelectionChanged += ref new Windows::UI::Xaml::Controls::SelectionChangedEventHandler([this, ui](Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ args)\
			{\
				if (ui->SelectedItem == nullptr)\
					PN = DV;\
				else\
					PN = (T)ui->SelectedItem;\
			});\
			return ui; \
		}\
	}\
	DECLARE_PROPERTY(T, PN, DV)

#ifdef LUU_EXPORT
namespace LUwpUtilities
{
	// A macro to declare common field and implementation of the base setting class
	// Ideally, we want to use an abstract base class like this
	template <class T, class E>
	ref class SettingWrapperBase abstract : Windows::UI::Xaml::Data::INotifyPropertyChanged
	{
	public:
		virtual E GetUI() = 0;

		virtual void Load()
		{
			auto settings = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
			SetValue(settings->HasKey(Name) ? static_cast<T>(settings->Lookup(Name)) : DefaultValue);
		}

		virtual void Save()
		{
			auto settings = Windows::Storage::ApplicationData::Current->LocalSettings->Values;
			settings->Insert(Name, Value);
		}

		T GetValue()
		{
			return Value;
		}

		void SetValue(T new_Value)
		{
			Value = new_Value;
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(Name));
		}

		property Platform::String^ Header;
		property Platform::String^ Name;
		property T Value;
		property T DefaultValue;
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;

	internal:
		SettingWrapperBase(
			Platform::String^ header,
			Platform::String^ name,
			T defaultValue,
			Windows::UI::Xaml::Data::PropertyChangedEventHandler^ handler
		)
		{
			this->Header = header;
			this->Name = name;
			this->DefaultValue = defaultValue;
			if (handler != nullptr)
				PropertyChanged += handler;
		}
	};
	// But this approach won't work in a WinRT DLL due to limitation of the UWP platform.
	// (It should works in case this header file is used directly.) And the reason being
	// C++ template are not truly generics in the sense of Java/C#.
	// Thus, we make a macro that will generate the above implementation if one plugs in
	// the C = class name, T = data type of the setting (bool/string/int)
	// and E = class of the UI element (CheckBox, ToggleSwitch, ComboBox, TextBox).
	// See ToggleSwitchSetting, CheckBoxSetting for examples.
#define __BASE_SETTING(C, T, E) \
	public:\
		void Load()\
		{\
			auto settings = Windows::Storage::ApplicationData::Current->LocalSettings->Values;\
			SetValue(settings->HasKey(Name) ? static_cast<T>(settings->Lookup(Name)) : DefaultValue);\
		}\
		void Save()\
		{\
			auto settings = Windows::Storage::ApplicationData::Current->LocalSettings->Values;\
			settings->Insert(Name, Value);\
		}\
		T GetValue()\
		{\
			return Value;\
		}\
		void SetValue(T new_Value)\
		{\
			Value = new_Value;\
			PropertyChanged(this, ref new Windows::UI::Xaml::Data::PropertyChangedEventArgs(Name));\
		}\
		property Platform::String^ Header;\
		property Platform::String^ Name;\
		property T Value;\
		property T DefaultValue;\
		virtual event Windows::UI::Xaml::Data::PropertyChangedEventHandler^ PropertyChanged;\
	internal:\
		inline void Initialize(Platform::String^ header, Platform::String^ name, T defaultValue, Windows::UI::Xaml::Data::PropertyChangedEventHandler^ handler)\
		{\
			this->Header = header;\
			this->Name = name;\
			this->DefaultValue = defaultValue;\
			if (handler != nullptr)\
				PropertyChanged += handler;\
		}

	// A boolean settings via ToggleSwitch
	[Windows::Foundation::Metadata::WebHostHidden]
	LUU_EXPORT ref class ToggleSwitchSetting sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
		// : SettingWrapperBase<bool, Windows::UI::Xaml::Controls::ToggleSwitch^>
	{
		__BASE_SETTING(ToggleSwitchSetting, bool, Windows::UI::Xaml::Controls::ToggleSwitch^)

	public:
		ToggleSwitchSetting(
			Platform::String^ header,
			Platform::String^ name,
			bool defaultValue,
			Windows::UI::Xaml::Data::PropertyChangedEventHandler^ handler
		) // : SettingWrapperBase(Header, Name, def)
		{
			Initialize(header, name, defaultValue, handler);
		}

		Windows::UI::Xaml::Controls::ToggleSwitch^ ToggleSwitchSetting::GetUI()
		{
			auto ui = ref new Windows::UI::Xaml::Controls::ToggleSwitch();
			ui->Header = Header;
			ui->Toggled += ref new Windows::UI::Xaml::RoutedEventHandler([this, ui](Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
			{
				SetValue(ui->IsOn);
			});
			ui->IsOn = Value;
			return ui;
		}
	};

	// A boolean settings via CheckBox
	[Windows::Foundation::Metadata::WebHostHidden]
	LUU_EXPORT ref class CheckBoxSetting sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
		// : SettingWrapperBase<bool, Windows::UI::Xaml::Controls::CheckBox^>
	{
		__BASE_SETTING(CheckBoxSetting, bool, Windows::UI::Xaml::Controls::CheckBox^)

	public:
		CheckBoxSetting(
			Platform::String^ header,
			Platform::String^ name,
			bool defaultValue,
			Windows::UI::Xaml::Data::PropertyChangedEventHandler^ handler
		) // : SettingWrapperBase(Header, Name, def)
		{
			Initialize(header, name, defaultValue, handler);
		}

		Windows::UI::Xaml::Controls::CheckBox^ GetUI()
		{
			auto ui = ref new Windows::UI::Xaml::Controls::CheckBox();
			ui->Content = Header;
			ui->Click += ref new Windows::UI::Xaml::RoutedEventHandler([this, ui](Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
			{
				SetValue(ui->IsChecked->Value);
			});
			ui->IsChecked = Value;
			return ui;
		}
	};

	[Windows::Foundation::Metadata::WebHostHidden]
	LUU_EXPORT ref class AutoSuggestBoxSetting sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
		// : SettingWrapperBase<Platform::String^, Windows::UI::Xaml::Controls::TextBox^>
	{
		__BASE_SETTING(AutoSuggestBoxSetting, Platform::String^, Windows::UI::Xaml::Controls::AutoSuggestBox^)

	public:
		AutoSuggestBoxSetting(
			Platform::String^ header,
			Platform::String^ name,
			Platform::String^ defaultValue,
			Windows::UI::Xaml::Data::PropertyChangedEventHandler^ handler
		) // : SettingWrapperBase(Header, Name, def)
		{
			Initialize(header, name, defaultValue, handler);
		}

		Windows::UI::Xaml::Controls::AutoSuggestBox^ AutoSuggestBoxSetting::GetUI()
		{
			auto ui = ref new Windows::UI::Xaml::Controls::AutoSuggestBox();
			ui->QueryIcon = ref new Windows::UI::Xaml::Controls::SymbolIcon(Windows::UI::Xaml::Controls::Symbol::Go);
			ui->QuerySubmitted += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Controls::AutoSuggestBox^, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^>(
				[=](Windows::UI::Xaml::Controls::AutoSuggestBox^ sender, Windows::UI::Xaml::Controls::AutoSuggestBoxQuerySubmittedEventArgs^ args)
			{
				SetValue(args->QueryText);
			});
			ui->Header = Header;
			ui->Text = Value;
			return ui;
		}
	};

	// For ComboBox-based settings, again we wish to use a class template like this
	template <class T>
	ref class ComboBoxSetting sealed : SettingWrapperBase<T, Windows::UI::Xaml::Controls::ComboBox^>
	{
	public:
		ComboBoxSetting(Platform::String^ header, Platform::String^ name, T defaultValue, Windows::Foundation::Collections::IVector<T>^ options, Windows::UI::Xaml::Data::PropertyChangedEventHandler^ handler) : SettingWrapperBase(header, name, defaultValue, handler)
		{
			this->Options = options;
		}

		Windows::UI::Xaml::Controls::ComboBox^ GetUI() override
		{
			auto ui = ref new Windows::UI::Xaml::Controls::ComboBox();
			ui->Header = Header;
			ui->ItemsSource = Options;
			ui->SelectionChanged += ref new Windows::UI::Xaml::Controls::SelectionChangedEventHandler([this, ui](Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ args)
			{
				if (ui->SelectedItem == nullptr)
					SetValue(DefaultValue);
				else
					SetValue((T)ui->SelectedItem);
			});
			ui->SelectedItem = Value;
			return ui;
		}

	private:
		Windows::Foundation::Collections::IVector<T>^ Options;
	};
	// which translates to the following macro
#define __BASE_COMBOBOX_SETTING(C, T) \
	__BASE_SETTING(C, T, Windows::UI::Xaml::Controls::ComboBox^)\
	public:\
		C(Platform::String^ header, Platform::String^ name, T defaultValue, Windows::Foundation::Collections::IVector<T>^ options, Windows::UI::Xaml::Data::PropertyChangedEventHandler^ handler)\
		{\
			Initialize(header, name, defaultValue, handler);\
			this->Options = options;\
		}\
		Windows::UI::Xaml::Controls::ComboBox^ GetUI()\
		{\
			auto ui = ref new Windows::UI::Xaml::Controls::ComboBox();\
			ui->Header = Header;\
			ui->ItemsSource = Options;\
			ui->SelectionChanged += ref new Windows::UI::Xaml::Controls::SelectionChangedEventHandler([=](Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ args)\
			{\
				if (ui->SelectedItem == nullptr)\
					SetValue(DefaultValue);\
				else\
					SetValue((T)ui->SelectedItem);\
			});\
			ui->SelectedItem = Value;\
			return ui;\
		}\
	private:\
		Windows::Foundation::Collections::IVector<T>^ Options;

	// Now we make ComboBox-based settings for a string/int which is to be selected a list
	[Windows::Foundation::Metadata::WebHostHidden]
	LUU_EXPORT ref class ComboBoxStringSetting sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
		// : ComboBoxSetting<Platform::String^>
	{
		__BASE_COMBOBOX_SETTING(ComboBoxStringSetting, Platform::String^)
	};

	[Windows::Foundation::Metadata::WebHostHidden]
	LUU_EXPORT ref class ComboBoxIntSetting sealed : Windows::UI::Xaml::Data::INotifyPropertyChanged
		// : ComboBoxSetting<int>
	{
		__BASE_COMBOBOX_SETTING(ComboBoxIntSetting, int)
	};

	// Undefine macros for safety (in principle, one should push/pop)
#undef __BASE_SETTING
#undef __BASE_COMBOBOX_SETTING
}
#endif

#endif