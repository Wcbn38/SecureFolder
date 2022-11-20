#include "SecureFolderCore.h"
#pragma warning(disable: 4996)

using namespace std;

int writeHeader(fstream* dest, SFC::KEY256 key, SFC::KEY_MODE::KEY_MODE mode); //0: success

int SFC::cypherFile(SFC::KEY256 key, fstream* srcFile, fstream* outFile, SFC::KEY_MODE::KEY_MODE mode, long long* byteCount)
{
	char buffer[256];

	if (!srcFile->is_open() || !outFile->is_open())
	{
		return -ERR_CANT_WRITE;
	}

	writeHeader(outFile, SFC::createKeyChecker(key), mode);

	do
	{
		srcFile->read(buffer, 256);
		int count = srcFile->gcount();

		if (mode == SFC::KEY_MODE::EVOLVING)
		{
			SFC::cypherRawDataEvolve((unsigned char*)buffer, count, &key);
		}
		else
		{
			SFC::cypherRawData((unsigned char*)buffer, count, &key);
		}

		outFile->write(buffer, count);
		if(byteCount != NULL)
			*byteCount += count;
	} while (!srcFile->eof());

	return -ERR_NO_ERR;
}

int writeHeader(fstream* dest, SFC::KEY256 key, SFC::KEY_MODE::KEY_MODE mode)
{
	char buffer[17];

	//hash

	for (int i = 0; i < 8; i++)
	{
		sprintf_s<17>(buffer, "%08lX", key.key[i]);
		dest->write(buffer, 8);
	}

	dest->put('\0');

	sprintf_s<17>(buffer, "%04hX", mode);	//cypher method
	dest->write(buffer, 4);

	dest->put('\n');

	return -ERR_NO_ERR;
}