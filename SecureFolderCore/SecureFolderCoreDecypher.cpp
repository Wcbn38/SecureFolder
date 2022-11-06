#include "SecureFolderCore.h"
#include <filesystem>

#pragma warning(disable: 4996)

using namespace std;

extern SFC::STATUS::STATUS status;
extern long long bytesRead;

struct fileData
{
	char name[_MAX_PATH];
	unsigned long long size;
	unsigned int attribs;
	long long t_access, t_write, t_create;
};

int writeFiles(SFC::SECURE_FOLDER_PROPS* props, fstream* reader);

SFC::SECURE_FOLDER_PROPS* SFC::decypher(SFC::SECURE_FOLDER_PROPS* props, SFC_CALLBACK callback(void))
{
	//securities

	if (props->decypherMethod == SFC::DECYPHER_METHOD::UNSET)
	{
		SFC::sfcErrno = ERR_DECYPH_METH_NOT_SUPPORTED;
		return NULL;
	}

	if (props->keyMode == SFC::KEY_MODE::UNSET)
	{
		SFC::sfcErrno = ERR_UNHANDLED_KEY_MODE;
		return NULL;
	}

	if (props->decypherMethod = SFC::DECYPHER_METHOD::COPY)
	{
		char decypherTemp[_MAX_PATH];
		{
			unsigned char buffer[256];
			
			std::tmpnam(decypherTemp);
			if (decypherTemp == NULL)
			{
				SFC::sfcErrno = ERR_CREAT_TEMP;
				return NULL;
			}

			fstream decypherOStream(decypherTemp, ios_base::binary | ios_base::out | ios_base::trunc);
			fstream decypherIStream(props->cypherPath, ios_base::in | ios_base::binary);

			if (!decypherIStream.is_open() || !decypherOStream.is_open())
			{
				SFC::sfcErrno = ERR_CANT_DECYPH;
				return NULL;
			}

			//checking if right password
			status = SFC::STATUS::GET_HASH;
			if (callback != NULL)
			{
				callback();
			}

			SFC::KEY256 passHash;

			for (int i = 0; i < 8; i++)
			{
				decypherIStream.read((char*)buffer, 8);
				passHash.key[i] = strtoul((char*)buffer, NULL, 16);
			}

			if (decypherIStream.get() != '\0')
			{
				SFC::sfcErrno = ERR_INVALID_HEADER;
				return NULL;
			}

			SFC::KEY256 entryHash = SFC::getSHA256(props->passwd, strlen(props->passwd));

			for (char i = 0; i < 8; i++)
			{
				if (entryHash.key[i] != passHash.key[i])
				{
					SFC::sfcErrno = ERR_INVALID_PASSWORD;
					return NULL;
				}
			}

			status = SFC::STATUS::WRITING_HEADER;
			if (callback != NULL)
			{
				callback();
			}

			//reading the rest of the header
			buffer[4] = '\0';
			decypherIStream.read((char*) buffer, 4);
			props->cypherMethod = static_cast<SFC::CYPHER_METHOD::CYPHER_METHOD>(strtoul((char*)buffer, NULL, 16));

			if (decypherIStream.get() != '\0')
			{
				SFC::sfcErrno = ERR_INVALID_HEADER;
				return NULL;
			}

			decypherIStream.read((char*) buffer, 4);
			props->savingMethod = static_cast<SFC::KEY_SAVING_METHOD::KEY_SAVING_METHOD>(strtoul((char* )buffer, NULL, 16));

			if (decypherIStream.get() != '\0')
			{
				SFC::sfcErrno = ERR_INVALID_HEADER;
				return NULL;
			}

			if (decypherIStream.get() != '\n')
			{
				SFC::sfcErrno = ERR_INVALID_HEADER;
				return NULL;
			}

			//decyphering
			status = SFC::STATUS::CYPHERING;
			bytesRead = 0;
			if (callback != NULL)
			{
				callback();
			}

			SFC::KEY256 cypherKey;
			memset(&cypherKey, 0, sizeof(cypherKey));

			for (int i = 0; props->passwd[i] != '\0'; i++)
			{
				cypherKey.key[7 - (i % 8)] += props->passwd[i];
			}

			cypherKey = SFC::getSHA256((char*)&cypherKey, sizeof(SFC::KEY256));

			do
			{
				decypherIStream.read((char*)buffer, sizeof(buffer));
				int count = decypherIStream.gcount();

				if (props->keyMode == SFC::KEY_MODE::EVOLVING)
				{
					SFC::cypherRawDataEvolve(buffer, count, &cypherKey);
				}
				else
				{
					SFC::cypherRawData(buffer, count, &cypherKey);
				}

				decypherOStream.write((char*) buffer, count);
				bytesRead += count;
			} while (!decypherIStream.eof());

			decypherOStream.close();
			decypherIStream.close();
		}

		//new readers
		status = SFC::STATUS::MAPPING_FILES;
		bytesRead = 0;
		if (callback != NULL)
		{
			callback();
		}

		fstream decypherIStream(decypherTemp, ios_base::in | ios_base::binary);

		writeFiles(props, &decypherIStream);

		decypherIStream.close();
		remove(decypherTemp);
	}
	else
	{
		SFC::sfcErrno = ERR_DECYPH_METH_NOT_SUPPORTED;
		return NULL;
	}

	status = SFC::STATUS::FINISHED;
	if (callback != NULL)
	{
		callback();
	}

	return props;
}

int writeFiles(SFC::SECURE_FOLDER_PROPS* props, fstream* reader)
{
	while (!reader->eof())
	{
		fileData data;
		char buffer[256];

		reader->getline(buffer, 256, '\0');
		strncpy(data.name, buffer, 256);

		if (reader->eof()) //eof non detecte si aucune tentative de lecture
		{
			return 0;
		}

		reader->getline(buffer, 256, '\0');
		data.size = strtoull(buffer, NULL, 16);

		reader->getline(buffer, 256, '\0');
		data.attribs = strtoul(buffer, NULL, 16);

		reader->getline(buffer, 256, '\0');
		data.t_access = strtoull(buffer, NULL, 16);

		reader->getline(buffer, 256, '\0');
		data.t_write = strtoull(buffer, NULL, 16);

		reader->getline(buffer, 256, '\0');
		data.t_create = strtoull(buffer, NULL, 16);

		if (reader->get() != '\n')
		{
			return -1;
		}

		sprintf_s<256>(buffer, "%s%s", props->decypherPath, data.name);

		if ((data.attribs & _A_SUBDIR) == 0) //file
		{

			fstream writer(buffer, ios_base::out | ios_base::trunc | ios_base::binary);

			if (!writer.is_open())
			{
				return -1;
			}

			long long dataLeft = data.size;

			do
			{
				reader->read(buffer, dataLeft < 256 ? dataLeft : 256);
				int count = reader->gcount();
				writer.write(buffer, count);
				dataLeft -= count;
				bytesRead += count;
			} while (dataLeft > 0);

			writer.close();
		}
		else //dir
		{
			_mkdir(buffer);
		}
	}

	return 0;
}