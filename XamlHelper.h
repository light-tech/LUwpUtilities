/**
 * Static methods to build XAML user interface. For example,
 *  - MakeButton : make an icon button (with specified tooltip)
 *  - MakeTextBlock
 *  - MakeListView
 *  - MakeGrid
 *
 * Our intention is not to avoid XAML technology but to build platform-independent code.
 *
 * For example, the following piece of UI building code
 *
 *     auto grid = MakeGrid(2, 3);
 *     auto tblk = MakeTextBlock("Hello world");
 *     AddToGrid(grid, tblk, 0, 0);
 *
 * makes sense in other platform: Should Android implements these methods,
 * your existing UWP code above can be used to build UI on Android version of
 * your app as well!
 * The key here is that the code is FREE of C++ TYPES. Android's `MakeGrid` may
 * return something like `AndroidGridLayout*` instead but Android's `AddToGrid`
 * takes care of the correct input.
 *
 * At the moment, we want the code to be usable in both C++/CX and the new C++/WinRT.
 */

#ifndef _LUWPUTILITIES_XAML_HELPER_
#define _LUWPUTILITIES_XAML_HELPER_

#ifdef LUU_EXPORT

#include "LUwpUtilities.h"
#include <Windows.h>

namespace LUwpUtilities
{
	/// Class to contain public static methods to build XAML UI
	[Windows::Foundation::Metadata::WebHostHidden]
	LUU_EXPORT ref class XH sealed
	{
	public:
		STATIC_INLINE Windows::UI::Xaml::Controls::Button^ MakeButton(
			Platform::String^ content,
			Platform::String^ label,
			Windows::UI::Xaml::RoutedEventHandler^ clickHandler
		)
		{
			static auto fontfamily = ref new Windows::UI::Xaml::Media::FontFamily("Segoe MDL2 Assets");
			Windows::UI::Xaml::Thickness margin = { 1, 1, 1, 1 };
			auto button = ref new Windows::UI::Xaml::Controls::Button();
			button->FontFamily = fontfamily;
			button->MinWidth = 40;
			button->MinHeight = 40;
			button->Margin = margin;
			button->IsTabStop = false;
			Windows::UI::Xaml::Controls::ToolTipService::SetToolTip(button, label);
			button->Content = content;
			if (clickHandler != nullptr) button->Click += clickHandler;
			return button;
		}

		STATIC_INLINE Windows::UI::Xaml::Controls::TextBlock^ MakeTextBlock(
			Platform::String^ content
		)
		{
			auto block = ref new Windows::UI::Xaml::Controls::TextBlock();
			block->Text = content;
			return block;
		}

		STATIC_INLINE Windows::UI::Xaml::Controls::ListView^ MakeListView(
			Platform::String^ item_template,
			Windows::UI::Xaml::Controls::ItemClickEventHandler^ item_click
		)
		{
			auto listview = ref new Windows::UI::Xaml::Controls::ListView();
			if (item_template != nullptr)
			{
				listview->ItemTemplate = (Windows::UI::Xaml::DataTemplate^)Windows::UI::Xaml::Markup::XamlReader::Load(item_template);
			}
			if (item_click != nullptr)
			{
				listview->IsItemClickEnabled = true;
				listview->ItemClick += item_click;
			}
			return listview;
		}

		STATIC_INLINE Windows::UI::Xaml::Controls::Grid^ MakeGrid(
			int num_rows,
			int num_cols
		)
		{
			auto grid = ref new Windows::UI::Xaml::Controls::Grid();
			for (int i = 0; i < num_rows; i++)
			{
				grid->RowDefinitions->Append(ref new Windows::UI::Xaml::Controls::RowDefinition());
			}
			for (int i = 0; i < num_cols; i++)
			{
				grid->ColumnDefinitions->Append(ref new Windows::UI::Xaml::Controls::ColumnDefinition());
			}
			return grid;
		}

		STATIC_INLINE void AddToGrid(
			Windows::UI::Xaml::Controls::Grid^ grid,
			Windows::UI::Xaml::FrameworkElement^ element,
			int row, int column
		)
		{
			grid->Children->Append(element);
			grid->SetRow(element, row);
			grid->SetColumn(element, column);
		}

		STATIC_INLINE void AddToGridWithSpan(
			Windows::UI::Xaml::Controls::Grid^ grid,
			Windows::UI::Xaml::FrameworkElement^ element,
			int row, int column,
			int row_span, int column_span
		)
		{
			grid->Children->Append(element);
			grid->SetRow(element, row);
			grid->SetRowSpan(element, row_span);
			grid->SetColumn(element, column);
			grid->SetColumnSpan(element, column_span);
		}

		STATIC_INLINE void MakeVisible(
			Windows::UI::Xaml::FrameworkElement^ element
		)
		{
			element->Visibility = Windows::UI::Xaml::Visibility::Visible;
		}

		STATIC_INLINE bool IsVisible(
			Windows::UI::Xaml::FrameworkElement^ element
		)
		{
			return element->Visibility == Windows::UI::Xaml::Visibility::Visible;
		}

		STATIC_INLINE void Collapse(
			Windows::UI::Xaml::FrameworkElement^ element
		)
		{
			element->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}

		STATIC_INLINE void ToggleVisibility(
			Windows::UI::Xaml::FrameworkElement^ element
		)
		{
			if (element->Visibility == Windows::UI::Xaml::Visibility::Visible)
				element->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
			else
				element->Visibility = Windows::UI::Xaml::Visibility::Visible;
		}

		STATIC_INLINE Windows::UI::Xaml::VisualState^ MakeVisualState(
			int min_windows_width
		)
		{
			auto state = ref new Windows::UI::Xaml::VisualState();
			auto stateTrigger = ref new Windows::UI::Xaml::AdaptiveTrigger();
			stateTrigger->MinWindowWidth = min_windows_width;
			state->StateTriggers->Append(stateTrigger);
			return state;
		}

		STATIC_INLINE void Bind(
			Windows::UI::Xaml::FrameworkElement^ element,
			Windows::UI::Xaml::DependencyProperty^ property,
			Platform::String^ path
		)
		{
			auto binding = ref new Windows::UI::Xaml::Data::Binding();
			binding->Path = ref new Windows::UI::Xaml::PropertyPath(path);
			Windows::UI::Xaml::Data::BindingOperations::SetBinding(element, property, binding);
		}

		STATIC_INLINE void PrintVisualTree(
			Windows::UI::Xaml::DependencyObject^ s,
			int level
		)
		{
			auto c = Windows::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(s);
			for (int j = 0; j < level; j++)
				OutputDebugString(L"  ");
			OutputDebugString((s + "[" + ((Windows::UI::Xaml::FrameworkElement^)s)->Name + "]\n")->Data());
			for (int i = 0; i < c; i++)
			{
				auto ci = Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(s, i);
				PrintVisualTree(ci, level + 1);
			}
		}

		STATIC_INLINE Windows::UI::Xaml::FrameworkElement^ FindElementByName(
			Windows::UI::Xaml::DependencyObject^ s,
			Platform::String^ name
		)
		{
			auto c = Windows::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(s);
			for (int i = 0; i < c; i++)
			{
				auto ci = (Windows::UI::Xaml::FrameworkElement^)Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(s, i);
				if (ci->Name == name)
					return ci;
				return FindElementByName(ci, name);
			}
			return nullptr;
		}

		STATIC_INLINE Windows::UI::Xaml::Controls::Pivot^ MakeTabPivot()
		{
			auto pivot = ref new Windows::UI::Xaml::Controls::Pivot();
			pivot->Loaded += ref new Windows::UI::Xaml::RoutedEventHandler([=](Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
			{
				// PrintVisualTree(pivot, 0);
				auto header_clipper = FindHeaderClipper(pivot);
				if (header_clipper != nullptr)
					((Windows::UI::Xaml::Controls::ContentControl^)header_clipper)->IsTabStop = false;
			});

			return pivot;
		}

		STATIC_INLINE Windows::UI::Xaml::Controls::TreeView^ MakeTreeView(
			Platform::String^ item_template
		)
		{
			auto treeview = ref new Windows::UI::Xaml::Controls::TreeView();
			treeview->Loaded += ref new Windows::UI::Xaml::RoutedEventHandler([=](Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
			{
				// PrintVisualTree(treeview, 0);
				auto list_control = FindTreeViewListControl(treeview);
				if (list_control != nullptr)
					((Windows::UI::Xaml::Controls::TreeViewList^)list_control)->ItemTemplate = (Windows::UI::Xaml::DataTemplate^)Windows::UI::Xaml::Markup::XamlReader::Load(item_template);
			});

			return treeview;
		}

	private:
		static Windows::UI::Xaml::FrameworkElement^ FindTreeViewListControl(
			Windows::UI::Xaml::DependencyObject^ s
		)
		{
			auto c = Windows::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(s);
			for (int i = 0; i < c; i++)
			{
				auto ci = (Windows::UI::Xaml::FrameworkElement^)Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(s, i);
				if (ci->Name == "ListControl")
					return ci;
				auto hc = FindTreeViewListControl(ci);
				if (hc != nullptr)
					return hc;
			}
			return nullptr;
		}

		static Windows::UI::Xaml::FrameworkElement^ FindHeaderClipper(
			Windows::UI::Xaml::DependencyObject^ s
		)
		{
			auto c = Windows::UI::Xaml::Media::VisualTreeHelper::GetChildrenCount(s);
			for (int i = 0; i < c; i++)
			{
				auto ci = (Windows::UI::Xaml::FrameworkElement^)Windows::UI::Xaml::Media::VisualTreeHelper::GetChild(s, i);
				if (ci->Name == "HeaderClipper")
					return ci;
				auto hc = FindHeaderClipper(ci);
				if (hc != nullptr)
					return hc;
			}
			return nullptr;
		}
	}; // class XH
} // namespace LUwpUtilities
#endif

#endif // #ifndef _XAML_HELPER