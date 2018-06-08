/**
 * Generic implementation of the interface ISupportIncrementalLoading to provide infinite scrolling list
 *
 * To use, simply create an instance with the constructor
 *  - IncrementalLoadingList(LoadMoreItemsHandler^, HasMoreItemsHandler^)
 * supplying the two callbacks to "load more items" and "check if there are more items".
 * And set it as `ItemsSource` of a `ListView`.
 */

#ifndef _LUWPUTILITIES_INCREMENTAL_LOADING_LIST_
#define _LUWPUTILITIES_INCREMENTAL_LOADING_LIST_

#ifdef LUU_EXPORT

#include <collection.h>
#include <ppltasks.h>

namespace LUwpUtilities
{
	LUU_EXPORT delegate Windows::Foundation::Collections::IVector<Platform::Object^>^ LoadMoreItemsHandler(int count);
	LUU_EXPORT delegate bool HasMoreItemsHandler();

using namespace Concurrency;

	// This class can used as a jumpstart for implementing ISupportIncrementalLoading. 
	// Implementing the ISupportIncrementalLoading interfaces allows you to create a list that loads
	//  more data automatically when the user scrolls to the end of of a GridView or ListView.
	[Windows::Foundation::Metadata::WebHostHidden]
	LUU_EXPORT ref class IncrementalLoadingList sealed
		: Windows::UI::Xaml::Interop::IBindableObservableVector,
		Windows::UI::Xaml::Data::ISupportIncrementalLoading
	{
	public:
		IncrementalLoadingList(LoadMoreItemsHandler^ loadMore, HasMoreItemsHandler^ hasMore)
		{
			_storage = ref new Platform::Collections::Vector<Platform::Object ^>();
			_storage->VectorChanged += ref new Windows::Foundation::Collections::VectorChangedEventHandler<Platform::Object ^>(this, &IncrementalLoadingList::_storageVectorChanged);
			_busy = false;
			_isVectorChangedObserved = false;
			_LoadMore = loadMore;
			_HasMore = hasMore;
		}

		virtual Windows::Foundation::IAsyncOperation<Windows::UI::Xaml::Data::LoadMoreItemsResult>^ LoadMoreItemsAsync(unsigned int count)
		{
			if (_busy)
			{
				throw ref new Platform::FailureException("Only one operation in flight at a time");
			}

			_busy = true;

			return Concurrency::create_async([=](Concurrency::cancellation_token c) {
				return Concurrency::task<void>([=]() {})
					.then([=]() {
					return _LoadMore(count); // LoadMoreItemsOverride(c, count);
				})
					.then([=](Windows::Foundation::Collections::IVector<Platform::Object^>^ items) -> Windows::UI::Xaml::Data::LoadMoreItemsResult {
					auto baseIndex = _storage->Size;

					for (unsigned int i = 0; i < items->Size; i++)
					{
						_storage->Append(items->GetAt(i));
					}

					Windows::UI::Xaml::Data::LoadMoreItemsResult result;
					result.Count = items->Size;
					return result;
				}, Concurrency::task_continuation_context::use_current())
				.then([=](Windows::UI::Xaml::Data::LoadMoreItemsResult result) -> Windows::UI::Xaml::Data::LoadMoreItemsResult {
					_busy = false;
					return result;
				});
			});
		}

		property bool HasMoreItems
		{
			virtual bool get()
			{
				return _HasMore(); // HasMoreItemsOverride();
			}
		}

#pragma endregion 

#pragma region IBindableObservableVector

		virtual event Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler^ VectorChanged
		{
			Windows::Foundation::EventRegistrationToken add(Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler^ e)
			{
				_isVectorChangedObserved = true;
				return _privateVectorChanged += e;
			}

			void remove(Windows::Foundation::EventRegistrationToken t)
			{
				_privateVectorChanged -= t;
			}

		protected:
			void raise(Windows::UI::Xaml::Interop::IBindableObservableVector^ vector, Platform::Object^ e)
			{
				if (_isVectorChangedObserved)
				{
					_privateVectorChanged(vector, e);
				}
			}
		}

#pragma endregion 

#pragma region Windows::UI::Xaml::Interop::IBindableIterator

		virtual Windows::UI::Xaml::Interop::IBindableIterator^ First()
		{
			return dynamic_cast<Windows::UI::Xaml::Interop::IBindableIterator^>(_storage->First());
		}

#pragma endregion

#pragma region Windows::UI::Xaml::Interop::IBindableVector 

		virtual void Append(Platform::Object^ value)
		{
			_storage->Append(value);
		}

		virtual void Clear()
		{
			_storage->Clear();
		}

		virtual Platform::Object^ GetAt(unsigned int index)
		{
			return _storage->GetAt(index);
		}

		virtual Windows::UI::Xaml::Interop::IBindableVectorView^ GetView()
		{
			return safe_cast<Windows::UI::Xaml::Interop::IBindableVectorView^>(_storage->GetView());
		}

		virtual bool IndexOf(Platform::Object^ value, unsigned int* index)
		{
			return _storage->IndexOf(value, index);
		}

		virtual void InsertAt(unsigned int index, Platform::Object^ value)
		{
			_storage->InsertAt(index, value);
		}

		virtual void RemoveAt(unsigned int index)
		{
			_storage->RemoveAt(index);
		}

		virtual void RemoveAtEnd()
		{
			_storage->RemoveAtEnd();
		}

		virtual void SetAt(unsigned int index, Platform::Object^ value)
		{
			_storage->SetAt(index, value);
		}

		virtual property unsigned int Size
		{
			unsigned int get() { return _storage->Size; }
		}


#pragma endregion

#pragma region State

	private:
		Platform::Collections::Vector<Platform::Object^>^ _storage;
		bool _busy;
		bool _isVectorChangedObserved;
		event Windows::UI::Xaml::Interop::BindableVectorChangedEventHandler^ _privateVectorChanged;

		void _storageVectorChanged(Windows::Foundation::Collections::IObservableVector<Platform::Object^>^ sender, Windows::Foundation::Collections::IVectorChangedEventArgs^ e)
		{
			if (_isVectorChangedObserved)
			{
				VectorChanged(this, e);
			}
		}

		LoadMoreItemsHandler^ _LoadMore;
		HasMoreItemsHandler^ _HasMore;

#pragma endregion 
	};

}
#endif

#endif