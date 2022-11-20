#include "SecureFolderCore.h"
#pragma warning(disable: 4996)

using namespace std;

SFC::KEY256 SFC::createKeyChecker(SFC::KEY256 key)
{
	SFC::KEY256 checker;

	for(int i = 0; i < 8; i++)
		checker.key[i] = key.key[7 - i];

	return SFC::getSHA256((char*) & checker, sizeof(checker));
}

int SFC::mapFolder(fstream* dest, char* path, char* entry, long long* byteCount, SFC_CALLBACK callback(char* fileFound, bool status))
{
	_finddatai64_t searchData;
	intptr_t searchHandle = _findfirsti64(path, &searchData);

	if (searchHandle == -1)
	{
		_findclose(searchHandle);
		return -ERR_CANT_OPEN_DIR;
	}

	//remove filters
	char* pathEnd = path;
	while (*pathEnd != '\0')
		pathEnd++;

	while (pathEnd > path && (*pathEnd != '\\'))
		pathEnd--;

	*pathEnd = '\0';

	do
	{
		if (strcmp(searchData.name, ".") != 0 && strcmp(searchData.name, "..") != 0) //infinite loop
		{
			char currentObject[_MAX_PATH];
			sprintf_s<_MAX_PATH>(currentObject, "%s\\%s", path, searchData.name);

			char myEntry[_MAX_PATH];
			sprintf_s<_MAX_PATH>(myEntry, "%s\\%s", entry, searchData.name);

			//map next

			if (searchData.attrib & _A_SUBDIR) //folder
			{
				{
					//write object
					char buffer[256];
					int length = strlen(myEntry);
					dest->write(myEntry, length + 1);			//object name, +1 for null terminator

					sprintf(buffer, "%llX", searchData.size);			//size
					length = strlen(buffer);
					dest->write(buffer, length + 1);

					sprintf(buffer, "%X", searchData.attrib);			//attrib
					length = strlen(buffer);
					dest->write(buffer, length + 1);

					sprintf(buffer, "%llX", searchData.time_access);	//time access
					length = strlen(buffer);
					dest->write(buffer, length + 1);

					sprintf(buffer, "%llX", searchData.time_write);	//time write
					length = strlen(buffer);
					dest->write(buffer, length + 1);

					sprintf(buffer, "%llX", searchData.time_create);	//time create
					length = strlen(buffer);
					dest->write(buffer, length + 1);

					dest->put('\n'); //end of header
				}

				sprintf_s<_MAX_PATH>(currentObject, "%s\\*", currentObject);
				SFC::mapFolder(dest, currentObject, myEntry, byteCount, callback);
			}
			else //file
			{
				fstream targetFile(currentObject, ios_base::binary | ios_base::in);
				if (!targetFile.is_open())
				{
					if (callback != NULL)
						callback(currentObject, false);
				}
				else
				{
					char* buffer = new char[256];
					if (buffer == NULL)
						return -ERR_HEAP_ERROR;

					if (callback != NULL)
						callback(currentObject, true);

					{
						//write object
						char buffer[256];
						int length = strlen(myEntry);
						dest->write(myEntry, length + 1);			//object name, +1 for null terminator

						sprintf(buffer, "%llX", searchData.size);			//size
						length = strlen(buffer);
						dest->write(buffer, length + 1);

						sprintf(buffer, "%X", searchData.attrib);			//attrib
						length = strlen(buffer);
						dest->write(buffer, length + 1);

						sprintf(buffer, "%llX", searchData.time_access);	//time access
						length = strlen(buffer);
						dest->write(buffer, length + 1);

						sprintf(buffer, "%llX", searchData.time_write);	//time write
						length = strlen(buffer);
						dest->write(buffer, length + 1);

						sprintf(buffer, "%llX", searchData.time_create);	//time create
						length = strlen(buffer);
						dest->write(buffer, length + 1);

						dest->put('\n'); //end of header
					}

					do
					{
						targetFile.read(buffer, 256);
						int count = targetFile.gcount();
						dest->write(buffer, count);
						if (byteCount != NULL)
							*byteCount += count;
					} while (!targetFile.eof());

					delete[] buffer;
				}
			}
		}
	} while (_findnexti64(searchHandle, &searchData) == 0);

	_findclose(searchHandle);
	return -ERR_NO_ERR;
}

struct fileData
{
	char name[_MAX_PATH];
	unsigned long long size;
	unsigned int attribs;
	long long t_access, t_write, t_create;
};

int SFC::unmapFolder(std::fstream* src, char* folderPath, long long* byteCount, SFC_CALLBACK callback(char* fileFound, bool status))
{
	while (!src->eof())
	{
		fileData data;
		char buffer[256];

		src->getline(buffer, 256, '\0');

		if (src->eof()) //eof non detecte si aucune tentative de lecture
		{
			return -ERR_NO_ERR;
		}

		strncpy(data.name, buffer, 256);

		src->getline(buffer, 256, '\0');
		data.size = strtoull(buffer, NULL, 16);

		src->getline(buffer, 256, '\0');
		data.attribs = strtoul(buffer, NULL, 16);

		src->getline(buffer, 256, '\0');
		data.t_access = strtoull(buffer, NULL, 16);

		src->getline(buffer, 256, '\0');
		data.t_write = strtoull(buffer, NULL, 16);

		src->getline(buffer, 256, '\0');
		data.t_create = strtoull(buffer, NULL, 16);

		if (src->get() != '\n')
		{
			return -ERR_CANT_DECYPH;
		}

		if ((data.attribs & _A_SUBDIR) == 0) //file
		{
			char fileName[_MAX_PATH];
			sprintf_s(fileName, "%s\\%s", folderPath, data.name);

			fstream writer(fileName, ios_base::out | ios_base::trunc | ios_base::binary);

			long long dataLeft = data.size;

			if (!writer.is_open())
			{
				if (callback != NULL)
					callback(data.name, false);

				do //flush file
				{
					src->read(buffer, dataLeft < 256 ? dataLeft : 256);
					int count = src->gcount();
					dataLeft -= count;

					if (byteCount != NULL)
						*byteCount += count;
				} while (dataLeft > 0);
			}
			else
			{
				if (callback != NULL)
					callback(data.name, true);

				do
				{
					src->read(buffer, dataLeft < 256 ? dataLeft : 256);
					int count = src->gcount();
					writer.write(buffer, count);
					dataLeft -= count;

					if (byteCount != NULL)
						*byteCount += count;
				} while (dataLeft > 0);

				writer.close();
			}
		}
		else //dir
		{
			char folderName[_MAX_PATH];
			sprintf_s(folderName, "%s\\%s", folderPath, data.name);
			int iResult = _mkdir(folderName);
		}
	}

	return -ERR_NO_ERR;
}