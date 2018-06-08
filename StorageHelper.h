/**
 * Various static helper methods to deal with files and folders to avoid the dreaded <ppltasks.h>
 */

#ifndef _LUWPUTILITIES_STORAGE_HELPER_
#define _LUWPUTILITIES_STORAGE_HELPER_

#ifdef LUU_EXPORT

#include "LUwpUtilities.h"
#include <ppltasks.h>

namespace LUwpUtilities
{
	LUU_EXPORT delegate void BufferHandler(Windows::Storage::Streams::IBuffer^ data);
	LUU_EXPORT delegate void StringHandler(Platform::String^ data);
	LUU_EXPORT delegate void FileHandler(Windows::Storage::StorageFile^ file);
	LUU_EXPORT delegate void FolderHandler(Windows::Storage::StorageFolder^ folder);
	LUU_EXPORT delegate void StorageItemsHandler(Windows::Foundation::Collections::IVectorView<Windows::Storage::IStorageItem^>^ items);

using namespace Concurrency;

	// Static synchronous methods to perform common I/O; basically wrap around
	// the Async counterpart in a create_task().get() so that client doesn't have
	// to #include <ppltasks.h> and reduce the *.obj file by 3MB.
	LUU_EXPORT ref class SH sealed
	{
	public:
		// Read/write/get files
		STATIC_INLINE Windows::Storage::Streams::IBuffer^ ReadFileBuffer(
			Windows::Storage::StorageFile^ file
		)
		{
			return create_task(Windows::Storage::FileIO::ReadBufferAsync(file)).get();
		}

		STATIC_INLINE void ReadFileBuffer(
			Windows::Storage::StorageFile^ file,
			BufferHandler^ handler
		)
		{
			create_task(Windows::Storage::FileIO::ReadBufferAsync(file)).then([=](Windows::Storage::Streams::IBuffer^ buffer)
			{
				handler(buffer);
			}, task_continuation_context::use_current());
		}

		STATIC_INLINE Platform::String^ ReadFileString(
			Windows::Storage::StorageFile^ file
		)
		{
			return create_task(Windows::Storage::FileIO::ReadTextAsync(file)).get();
		}

		STATIC_INLINE void ReadFileString(
			Windows::Storage::StorageFile^ file,
			StringHandler^ handler
		)
		{
			create_task(Windows::Storage::FileIO::ReadTextAsync(file)).then([=](Platform::String^ text)
			{
				handler(text);
			}, task_continuation_context::use_current());
		}

		STATIC_INLINE void WriteFile(
			Windows::Storage::StorageFile^ file,
			Platform::String^ data
		)
		{
			create_task(Windows::Storage::FileIO::WriteTextAsync(file, data)).get();
		}

		STATIC_INLINE void WriteFile(
			Windows::Storage::StorageFile^ file,
			Windows::Storage::Streams::IBuffer^ data
		)
		{
			create_task(Windows::Storage::FileIO::WriteBufferAsync(file, data)).get();
		}

		STATIC_INLINE Windows::Storage::StorageFile^ GetFile(
			Windows::Storage::StorageFolder^ folder,
			Platform::String^ name
		)
		{
			return (Windows::Storage::StorageFile^)create_task(folder->TryGetItemAsync(name)).get();
		}

		STATIC_INLINE Windows::Storage::StorageFile^ GetFile(
			Platform::String^ path
		)
		{
			return create_task(Windows::Storage::StorageFile::GetFileFromPathAsync(path)).get();
		}

		STATIC_INLINE Windows::Storage::StorageFolder^ GetFolder(
			Platform::String^ path
		)
		{
			return create_task(Windows::Storage::StorageFolder::GetFolderFromPathAsync(path)).get();
		}

		STATIC_INLINE Windows::Storage::StorageFolder^ GetFolder(
			Windows::Storage::StorageFolder^ folder,
			Platform::String^ name
		)
		{
			return (Windows::Storage::StorageFolder^)create_task(folder->TryGetItemAsync(name)).get();
		}

		STATIC_INLINE Windows::Storage::StorageFolder^ GetParent(
			Windows::Storage::IStorageItem2^ item
		)
		{
			return create_task(item->GetParentAsync()).get();
		}

		STATIC_INLINE Windows::Storage::StorageFile^ CreateFile(
			Windows::Storage::StorageFolder^ folder,
			Platform::String^ name
		)
		{
			return create_task(folder->CreateFileAsync(name, Windows::Storage::CreationCollisionOption::OpenIfExists)).get();
		}

		STATIC_INLINE Windows::Foundation::Collections::IVectorView<Windows::Storage::IStorageItem^>^ List(
			Windows::Storage::StorageFolder^ folder
		)
		{
			return create_task(folder->GetItemsAsync()).get();
		}

		STATIC_INLINE void List(
			Windows::Storage::StorageFolder^ folder,
			StorageItemsHandler^ handler
		)
		{
			create_task(folder->GetItemsAsync()).then([=](Windows::Foundation::Collections::IVectorView<Windows::Storage::IStorageItem^>^ items)
			{
				handler(items);
			}, task_continuation_context::use_current());
		}

		STATIC_INLINE void PickSaveFile(
			Windows::Storage::Pickers::FileSavePicker^ savePicker,
			FileHandler^ callback
		)
		{
			create_task(savePicker->PickSaveFileAsync())
				.then([=](Windows::Storage::StorageFile^ file)
			{
				// file is null if user cancels the file picker.
				// Stop work and clean up.
				if (file == nullptr)
					cancel_current_task();
				else
					callback(file);
			});
		}

		STATIC_INLINE void PickFiles(
			Windows::Storage::Pickers::FileOpenPicker^ openPicker,
			FileHandler^ callback
		)
		{
			create_task(openPicker->PickMultipleFilesAsync())
				.then([=](Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile^>^ files)
			{
				// file is null if user cancels the file picker.
				if (files == nullptr)
				{
					// Stop work and clean up.
					cancel_current_task();
				}

				for (auto iter = files->First(); iter->HasCurrent; iter->MoveNext())
					callback(iter->Current);
			}, task_continuation_context::use_current());
		}

		STATIC_INLINE void PickFolder(
			Windows::Storage::Pickers::FolderPicker^ folderPicker,
			FolderHandler^ callback
		)
		{
			create_task(folderPicker->PickSingleFolderAsync())
				.then([=](Windows::Storage::StorageFolder^ folder)
			{
				if (folder == nullptr)
					cancel_current_task();
				else
					callback(folder);
			});
		}
	}; // class SH
} // namespace LUwpUtilities
#endif

#endif // #ifndef _STORAGE_HELPER