/**
 * Some method to deal with IBuffer and char* conversion.
 */

STATIC_INLINE char * BufferToByteArray(Windows::Storage::Streams::IBuffer^ data)
{
	size_t len = data->Length;
	char * result = new char[len];
	auto reader = Windows::Storage::Streams::DataReader::FromBuffer(data);
	for (size_t i = 0; i < len; i++)
		result[i] = reader->ReadByte();

	return result;
}

STATIC_INLINE void BufferToByteArray(Windows::Storage::Streams::IBuffer^ data, char *dest)
{
	auto len = data->Length;
	auto reader = Windows::Storage::Streams::DataReader::FromBuffer(data);
	for (size_t i = 0; i < len; i++)
		dest[i] = reader->ReadByte();
}

STATIC_INLINE char * BufferToCString(Windows::Storage::Streams::IBuffer^ data)
{
	size_t len = data->Length;
	char * result = new char[len + 1];
	auto reader = Windows::Storage::Streams::DataReader::FromBuffer(data);
	for (size_t i = 0; i < len; i++)
		result[i] = reader->ReadByte();
	result[len] = '\0';

	return result;
}
