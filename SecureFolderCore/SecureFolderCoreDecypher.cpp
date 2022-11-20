#include "SecureFolderCore.h"
#include <filesystem>

#pragma warning(disable: 4996)

using namespace std;

int SFC::decypherFile(SFC::KEY256 key, std::fstream* srcFile, std::fstream* outFile, long long* byteCount)
{
	//securities
	unsigned char buffer[256];
	SFC::KEY256 fileKey; SFC::KEY_MODE::KEY_MODE mode;

	int iResult = SFC::readHeader(srcFile, &fileKey, &mode);
	switch (iResult)
	{
	case -ERR_NO_ERR:
		break;
	default:
		return iResult;
	}

	if (!(fileKey == (SFC::KEY256)SFC::createKeyChecker(key)))
		return -ERR_INVALID_PASSWORD;

	do
	{
		srcFile->read((char*)buffer, sizeof(buffer));
		int count = srcFile->gcount();

		if (mode == SFC::KEY_MODE::EVOLVING)
		{
			SFC::cypherRawDataEvolve(buffer, count, &key);
		}
		else
		{
			SFC::cypherRawData(buffer, count, &key);
		}

		outFile->write((char*) buffer, count);
		if(byteCount != NULL)
			*byteCount += count;
	} while (!srcFile->eof());

	return -ERR_NO_ERR;
}

int SFC::readHeader(fstream* src, SFC::KEY256* key, SFC::KEY_MODE::KEY_MODE* mode)
{
	char buffer[17];

	//hash

	for (int i = 0; i < 8; i++)
	{
		src->read((char*)buffer, 8);
		key->key[i] = strtoul((char*)buffer, NULL, 16);
	}

	if (src->get() != '\0')
		return -ERR_CANT_DECYPH;

	buffer[4] = '\0';
	src->read((char*)buffer, 4);
	*mode = static_cast<SFC::KEY_MODE::KEY_MODE>(strtoul((char*)buffer, NULL, 16));

	if (src->get() != '\n')
		return -ERR_CANT_DECYPH;

	return -ERR_NO_ERR;
}