#include "SecureFolderCore.h"
#pragma warning(disable: 4996)

using namespace std;

extern SFC::STATUS::STATUS status;
extern long long bytesRead;

int writeSeq(fstream* dest, char* filePath, _finddata_t* data, char buffer[256], char* entry = NULL); //0: success
int writeHeader(fstream* dest, SFC::SECURE_FOLDER_PROPS* props); //0: success

int SFC::cypher(SFC::SECURE_FOLDER_PROPS props, SFC_CALLBACK callback(void))
{
	//securities
	if (props.cypherMethod == SFC::CYPHER_METHOD::UNSET)
	{
		SFC::sfcErrno = ERR_CYPH_METH_NOT_SUPPORTED;
		return -1;
	}

	if (props.savingMethod == SFC::KEY_SAVING_METHOD::UNSET)
	{
		SFC::sfcErrno = ERR_NO_SAVING_METH;
		return -1;
	}

	if (props.keyMode == SFC::KEY_MODE::UNSET)
	{
		SFC::sfcErrno = ERR_UNHANDLED_KEY_MODE;
		return NULL;
	}

	//generates the cypher key
	status = SFC::STATUS::GET_HASH;
	if (callback != NULL)
	{
		callback();
	}

	SFC::KEY256 cypherKey;
	memset(&cypherKey, 0, sizeof(cypherKey));

	for (int i = 0; props.passwd[i] != '\0'; i++)
	{
		cypherKey.key[7 - (i % 8)] += props.passwd[i];
	}

	cypherKey = getSHA256((char*) &cypherKey, sizeof(SFC::KEY256));

	char mappingFileName[_MAX_PATH];
	std::tmpnam(mappingFileName);

	if (mappingFileName == NULL)
	{
		SFC::sfcErrno = ERR_CREAT_TEMP;
		return -1;
	}

	//open the output stream
	fstream destination(mappingFileName, ios_base::binary | ios_base::out | ios_base::trunc);
	if (!destination.is_open())
	{
		SFC::sfcErrno = ERR_CANT_OPEN_FILE;
		return -1;
	}

	//writes header
	status = SFC::STATUS::WRITING_HEADER;
	if (callback != NULL)
	{
		callback();
	}

	if (writeHeader(&destination, &props) != 0)
	{
		SFC::sfcErrno = ERR_HEADER_NO_PWRD;
		return -1;
	}

	char buffer[256];

	//writes data
	status = SFC::STATUS::MAPPING_FILES;
	bytesRead = 0;
	if (callback != NULL)
	{
		callback();
	}

	if (props.cypherMethod == SFC::CYPHER_METHOD::SEQUENTIAL)
	{
		intptr_t dirHandle;
		_finddata_t dirData;

		{
			char searchPath[_MAX_PATH];
			sprintf(searchPath, "%s*", props.decypherPath);
			dirHandle = _findfirst(searchPath, &dirData);
		}

		if (dirHandle == -1)
		{
			destination.close();
			_findclose(dirHandle);
			SFC::sfcErrno = ERR_CANT_OPEN_DIR;
			return -1;
		}

		do
		{
			char path[_MAX_PATH];
			sprintf(path, "%s%s", props.decypherPath, dirData.name);

			if (writeSeq(&destination, path, &dirData, buffer) != 0)
			{
				destination.close();
				_findclose(dirHandle);
				SFC::sfcErrno = ERR_CANT_WRITE;
				return -1;
			}
		} while (_findnext(dirHandle, &dirData) == 0);

		_findclose(dirHandle);
	}
	else
	{
		SFC::sfcErrno = ERR_CYPH_METH_NOT_SUPPORTED;
		return -1;
	}

	destination.close();

	//cypher the file
	status = SFC::STATUS::CYPHERING;
	bytesRead = 0;
	if (callback != NULL)
	{
		callback();
	}

	fstream cypherIStream(mappingFileName, ios_base::binary | ios_base::in);
	fstream cypherOStream(props.cypherPath, ios_base::binary | ios_base::out | ios_base::trunc);

	if (!cypherIStream.is_open() || !cypherOStream.is_open())
	{
		SFC::sfcErrno = ERR_CANT_CYPH;
		return -1;
	}

	cypherIStream.getline(buffer, 256, '\n');
	cypherOStream.write(buffer, cypherIStream.gcount() - 1);
	cypherOStream.put('\n');

	do
	{
		cypherIStream.read(buffer, 256);
		int count = cypherIStream.gcount();

		if (props.keyMode == SFC::KEY_MODE::EVOLVING)
		{
			SFC::cypherRawDataEvolve((unsigned char*)buffer, count, &cypherKey);
		}
		else
		{
			SFC::cypherRawData((unsigned char*)buffer, count, &cypherKey);
		}

		cypherOStream.write(buffer, count);
		bytesRead += count;
	} while (!cypherIStream.eof());

	cypherIStream.close();
	cypherOStream.close();

	remove(mappingFileName);

	status = SFC::STATUS::FINISHED;
	if (callback != NULL)
	{
		callback();
	}

	return 0;
}

int writeSeq(fstream* dest, char* filePath, _finddata_t* data, char buffer[256], char* entry)
{
	if(strcmp(data->name, ".") == 0 || strcmp(data->name, "..") == 0)
	{ 
		return 0;
	}
	//sets the header
	size_t length = 0;
	char* fileEntry;

	if (entry == NULL)
	{
		fileEntry = new char[strlen(data->name) + 1];
		sprintf(fileEntry, "%s", data->name);
	}
	else
	{
		fileEntry = new char[strlen(entry) + 1];
		sprintf(fileEntry, "%s", entry);
	}

	length = strlen(fileEntry);
	dest->write(fileEntry, length + 1); // x2 because writing wchar and + 2 for the null terminator

	sprintf(buffer, "%lX", data->size);			//size
	length = strlen(buffer);
	dest->write(buffer, length + 1);

	sprintf(buffer, "%X", data->attrib);			//attrib
	length = strlen(buffer);
	dest->write(buffer, length + 1);

	sprintf(buffer, "%llX", data->time_access);	//time access
	length = strlen(buffer);
	dest->write(buffer, length + 1);

	sprintf(buffer, "%llX", data->time_write);	//time write
	length = strlen(buffer);
	dest->write(buffer, length + 1);

	sprintf(buffer, "%llX", data->time_create);	//time create
	length = strlen(buffer);
	dest->write(buffer, length + 1);

	dest->put('\n'); //end of header

	//fills the data
	if ((data->attrib & _A_SUBDIR) == 0 )
	{
		fstream reader(filePath, ios_base::binary | ios_base::in);
		if (!reader.is_open())
		{
			return -1;
		}

		do
		{
			reader.read(buffer, 256);
			int count = reader.gcount();
			dest->write(buffer, count);
			bytesRead += count;
		} while (!reader.eof());

		delete[] fileEntry;
		reader.close();
		return 0;
	}
	else
	{
		char path[_MAX_PATH];
		char entryPath[_MAX_PATH];

		sprintf_s<_MAX_PATH>(path, "%s\\*", filePath);

		_finddata_t searchData;
		intptr_t searchHandle = _findfirst(path, &searchData);

		if (searchHandle == -1)
		{
			delete[] fileEntry;
			_findclose(searchHandle);
			return -1;
		}

		do
		{
			sprintf_s<_MAX_PATH>(path, "%s\\%s", filePath, searchData.name);
			sprintf_s<_MAX_PATH>(entryPath, "%s\\%s", fileEntry, searchData.name);

			if (writeSeq(dest, path, &searchData, buffer, entryPath) != 0)
			{
				_findclose(searchHandle);
				return -1;
			}
		} while (_findnext(searchHandle, &searchData) == 0);

		delete[] fileEntry;
		_findclose(searchHandle);
		return 0;
	}
}

int writeHeader(fstream* dest, SFC::SECURE_FOLDER_PROPS* props)
{
	char buffer[17];

	//hash
	SFC::KEY256 hash = SFC::getSHA256(props->passwd, strlen(props->passwd));

	for (int i = 0; i < 8; i++)
	{
		sprintf_s<17>(buffer, "%08lX", hash.key[i]);
		dest->write(buffer, 8);
	}

	dest->put('\0');

	sprintf_s<17>(buffer, "%04hX", props->cypherMethod);	//cypher method
	dest->write(buffer, 5);

	sprintf_s<17>(buffer, "%04hX", props->savingMethod);	//saving method method
	dest->write(buffer, 5);

	dest->put('\n');

	return 0;
}