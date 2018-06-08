/**
 * Static method to run code in background thread via callbacks; intended to avoid inclusion of <ppltasks.h>
 */

#ifndef _LUWPUTILITIES_TASK_HELPER_
#define _LUWPUTILITIES_TASK_HELPER_

#ifdef LUU_EXPORT

#include "LUwpUtilities.h"
#include <ppltasks.h>

namespace LUwpUtilities
{
	LUU_EXPORT delegate void ExecutionCallback(Platform::Object^ params);
	LUU_EXPORT delegate Platform::Object^ ExecutionCallbackWithValue(Platform::Object^ params);

using namespace Concurrency;

	/// STATIC_INLINE method to run background task to avoid #include <ppltasks.h>
	[Windows::Foundation::Metadata::WebHostHidden]
	LUU_EXPORT ref class TH sealed
	{
	public:
		STATIC_INLINE void RunAsync(
			ExecutionCallback^ execution,
			Platform::Object^ param
		)
		{
			create_task([=]()
			{
				execution(param);
			});
		}

		STATIC_INLINE void RunAsync(
			Windows::Foundation::IAsyncOperation<Platform::String^>^ operation,
			ExecutionCallback^ continuation
		)
		{
			create_task(operation).then([=](Platform::String^ result)
			{
				continuation(result);
			}, task_continuation_context::use_current());
		}

		STATIC_INLINE void RunAsync(
			ExecutionCallbackWithValue^ execution,
			Platform::Object^ param,
			ExecutionCallback^ continuation
		)
		{
			create_task([=]()
			{
				return execution(param);
			}).then([=](task<Platform::Object^> previous_task)
			{
				try
				{
					auto result = previous_task.get();
					continuation(result);
				}
				catch (Platform::Exception^ e)
				{
				}
			}, task_continuation_context::use_current());
		}

		STATIC_INLINE void RunAsync(
			ExecutionCallbackWithValue^ execution,
			Platform::Object^ param,
			ExecutionCallback^ continuation,
			ExceptionHandler^ on_error
		)
		{
			create_task([=]()
			{
				return execution(param);
			}).then([=](task<Platform::Object^> previous_task)
			{
				try
				{
					auto result = previous_task.get();
					continuation(result);
				}
				catch (Platform::Exception^ e)
				{
					if (on_error != nullptr)
						on_error(e);
				}
			}, task_continuation_context::use_current());
		}

		STATIC_INLINE void RunAsync(
			Windows::UI::Core::CoreDispatcher^ dispatcher,
			Windows::UI::Core::DispatchedHandler^ handler,
			ExecutionCallback^ continuation
		)
		{
			create_task(dispatcher->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal, handler)).then([=]()
			{
				if (continuation != nullptr)
					continuation(nullptr);
			}, task_continuation_context::use_current());
		}

		STATIC_INLINE void NotifyUser(
			Platform::String^ title,
			Platform::String^ message,
			Windows::UI::Popups::UICommand ^command,
			Windows::UI::Popups::UICommand ^secondary_command
		)
		{
			auto dialog = (title == nullptr ? ref new Windows::UI::Popups::MessageDialog(message) : ref new Windows::UI::Popups::MessageDialog(message, title));
			if (command != nullptr)
				dialog->Commands->Append(command);
			if (secondary_command != nullptr)
				dialog->Commands->Append(secondary_command);
			dialog->ShowAsync();
		}
	}; // class TH
} // namespace LUwpUtilities
#endif

#endif // #ifndef _TASK_HELPER