#pragma once

#include "SettingsHelper.h"

namespace LUwpUtilitiesTestApp
{
	using namespace LUwpUtilities;

	ref class App sealed : public ::Windows::UI::Xaml::Application
	{
	public:
		property ToggleSwitchSetting^ AutoCheckForUpdate;
		property CheckBoxSetting^ DarkTheme;
		property AutoSuggestBoxSetting^ UserName;
		property ComboBoxIntSetting^ RefreshFrequency;
		void LoadSettings();
		void SaveSettings();

	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

	internal:
		App();
		void ShowSettingsDialog(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void OnSettingChanged(Platform::Object^ sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs^ args);
		Windows::UI::Xaml::Controls::Grid^ MakeMainPage();
	};

	[Windows::Foundation::Metadata::WebHostHidden]
	ref class SettingDialog sealed : public Windows::UI::Xaml::Controls::ContentDialog
	{
	internal:
		SettingDialog(App^ app);
	};
}
