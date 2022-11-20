#pragma once

#include <fstream>
#include <io.h>
#include <iostream>
#include <direct.h>

#define NULL 0

#define ERR_NO_ERR 0x0000
#define ERR_CANT_OPEN_FILE 0x0001
#define ERR_CANT_OPEN_DIR 0x0002
#define ERR_CANT_WRITE 0x0003
#define ERR_CREAT_TEMP 0x0004

#define ERR_CYPH_METH_NOT_SUPPORTED 0x0010

#define ERR_DECYPH_METH_NOT_SUPPORTED 0x0020

#define ERR_NO_SAVING_METH 0x0030

#define ERR_HEADER_NO_PWRD 0x0040
#define ERR_INVALID_HEADER 0x0041
#define ERR_INVALID_PASSWORD 0x0042

#define ERR_CANT_CYPH 0x0050
#define ERR_CANT_DECYPH 0x0051
#define ERR_UNHANDLED_KEY_MODE 0x0052

#define ERR_HEAP_ERROR 0x0060

#define SFC_CALLBACK void _cdecl

namespace SFC
{
	//ENUMS

	namespace MAPPING_METHOD {
		enum MAPPING_METHOD : short
		{
			UNSET = NULL, SEQUENTIAL, MAPPED
		};
	}

	namespace KEY_MODE
	{
		enum KEY_MODE : short
		{
			UNSET = NULL,
			CLASSIC,
			EVOLVING
		};
	}

	//STRUCTS

	struct KEY256
	{
		unsigned long key[8];
		friend bool operator==(const SFC::KEY256& a, const SFC::KEY256& b);
	};

	//FUNCTIONS

	/**
	Generate SHA256 key.

	Parameters:
		input: data array for key generation
		size: size of the data array

	Returns:
		256 bit key
	*/
	KEY256 getSHA256(const char* input, long long size);

	/**
	Maps and cyphers folder to a file.

	Parameters:
		key: SHA256 key
		srcFile: file to cypher
		outFile: file to write to
		mode: cyphering mode

	Returns:
		256 bit key
	*/
	int cypherFile(SFC::KEY256 key, std::fstream* srcFile, std::fstream* outFile, SFC::KEY_MODE::KEY_MODE mode, long long* byteCount = NULL);
	int decypherFile(KEY256 key, std::fstream* srcFile, std::fstream* outFile, long long* byteCount = NULL);

	int mapFolder(std::fstream* dest, char* path, char* entry, long long* byteCount = NULL, SFC_CALLBACK callback(char* fileFound, bool status) = NULL);
	int unmapFolder(std::fstream* src, char* folderPath, long long* byteCount = NULL, SFC_CALLBACK callback(char* fileFound, bool status) = NULL);
	KEY256 createKeyChecker(KEY256 key);
	//TODO: int readMap()
	int readHeader(std::fstream* src, SFC::KEY256* key, SFC::KEY_MODE::KEY_MODE* mode);

	void cypherRawData(unsigned char* data, long size, SFC::KEY256* key);
	void cypherRawDataEvolve(unsigned char* data, long size, SFC::KEY256* key);
	void cypherKeyEvolve(SFC::KEY256* key);
}