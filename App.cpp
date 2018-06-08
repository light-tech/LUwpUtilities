#include <Windows.h>
#include <collection.h>
#include "App.h"
#include "XamlHelper.h"

using namespace LUwpUtilities;
using namespace LUwpUtilitiesTestApp;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Data;

App::App()
{
	// Set-up settings
	auto handler = ref new ::PropertyChangedEventHandler(this, &App::OnSettingChanged);
	AutoCheckForUpdate = ref new ToggleSwitchSetting("Auto check for updates", "AutoCheckForUpdate", false, handler);
	DarkTheme = ref new CheckBoxSetting("Use dark theme", "DarkTheme", false, handler);
	UserName = ref new AutoSuggestBoxSetting("User name", "UserName", nullptr, handler);
	RefreshFrequency = ref new ComboBoxIntSetting("Refresh frequency (seconds)", "RefreshFrequency", 60, ref new Platform::Collections::Vector<int>({ 10, 30, 60, 90, 120 }), handler);

	// Load the current setting
	LoadSettings();
}

void App::LoadSettings()
{
	AutoCheckForUpdate->Load();
	DarkTheme->Load();
	UserName->Load();
	RefreshFrequency->Load();
}

void App::SaveSettings()
{
	AutoCheckForUpdate->Save();
	DarkTheme->Save();
	UserName->Save();
	RefreshFrequency->Save();
}

void App::OnSettingChanged(Platform::Object^ sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs^ args)
{
	if (args->PropertyName == "AutoCheckForUpdate")
	{
		OutputDebugString(("AutoCheckForUpdate changed to " + AutoCheckForUpdate->GetValue() + "\n")->Data());
	}
	else if (args->PropertyName == "DarkTheme")
	{
		OutputDebugString(("DarkTheme changed to " + DarkTheme->GetValue() + "\n")->Data());
	}
	else if (args->PropertyName == "UserName")
	{
		OutputDebugString(("UserName changed to " + UserName->GetValue() + "\n")->Data());
	}
	else if (args->PropertyName == "RefreshFrequency")
	{
		OutputDebugString(("RefreshFrequency changed to once every " + RefreshFrequency->GetValue() + " seconds\n")->Data());
	}
	else
		OutputDebugString(L"Unknown setting changed!\n");
}

void App::ShowSettingsDialog(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	(ref new SettingDialog(this))->ShowAsync();
}

Grid^ App::MakeMainPage()
{
	auto grid = XH::MakeGrid(3, 1);
	XH::AddToGrid(grid, XH::MakeTextBlock("Hello world!"), 0, 0);
	XH::AddToGrid(grid, XH::MakeButton(L"\uE713", "Settings", ref new RoutedEventHandler(this, &App::ShowSettingsDialog)), 1, 0);
	return grid;
}

void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{
	if (Window::Current->Content == nullptr)
		Window::Current->Content = MakeMainPage();

	Window::Current->Activate();
}

SettingDialog::SettingDialog(App^ app)
{
	Title = "Settings";
	PrimaryButtonText = "Save";
	SecondaryButtonText = "Cancel";
	PrimaryButtonClick += ref new TypedEventHandler<ContentDialog^, ContentDialogButtonClickEventArgs^>([=](ContentDialog^ sender, ContentDialogButtonClickEventArgs^ args)
	{
		app->SaveSettings();
	});
	SecondaryButtonClick += ref new TypedEventHandler<ContentDialog^, ContentDialogButtonClickEventArgs^>([=](ContentDialog^ sender, ContentDialogButtonClickEventArgs^ args)
	{
		app->LoadSettings();
	});

	auto settingsPane = ref new StackPanel();
	settingsPane->Children->Append(app->AutoCheckForUpdate->GetUI());
	settingsPane->Children->Append(app->DarkTheme->GetUI());
	settingsPane->Children->Append(app->UserName->GetUI());
	settingsPane->Children->Append(app->RefreshFrequency->GetUI());

	// Wrap everything in a ScrollViewer
	auto panel = ref new ScrollViewer();
	panel->Content = settingsPane;
	this->Content = panel;
}

int __cdecl main(::Platform::Array<::Platform::String^>^ args)
{
	(void)args; // Unused parameter
	::Windows::UI::Xaml::Application::Start(ref new ::Windows::UI::Xaml::ApplicationInitializationCallback(
		[](::Windows::UI::Xaml::ApplicationInitializationCallbackParams^ p) {
		(void)p; // Unused parameter
		auto app = ref new App();
	}));
}
