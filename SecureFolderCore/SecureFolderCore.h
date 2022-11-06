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

#define SFC_CALLBACK void _cdecl

namespace SFC
{
	//ENUMS

	namespace CYPHER_METHOD {
		enum CYPHER_METHOD : short
		{
			UNSET = NULL, SEQUENTIAL, MAPPED
		};
	}

	namespace KEY_SAVING_METHOD
	{
		enum KEY_SAVING_METHOD : short
		{
			UNSET = NULL, PASSWORD, USB, FILE
		};
	}

	namespace DECYPHER_METHOD
	{
		enum DECYPHER_METHOD : short
		{
			UNSET = NULL, VIRTUAL_DISK, COPY, VIRTUAL_FTP
		};
	}

	namespace KEY_MODE
	{
		enum KEY_MODE : short
		{
			UNSET = NULL, CLASSIC, EVOLVING
		};
	}

	namespace STATUS
	{
		enum STATUS : short
		{
			GET_HASH, WRITING_HEADER, MAPPING_FILES, CYPHERING, FINISHED
		};
	}

	//STRUCTS

	struct KEY256
	{
		unsigned long key[8];
	};

	struct SECURE_FOLDER_PROPS
	{
		CYPHER_METHOD::CYPHER_METHOD cypherMethod = CYPHER_METHOD::UNSET;
		KEY_MODE::KEY_MODE keyMode = KEY_MODE::UNSET;
		KEY_SAVING_METHOD::KEY_SAVING_METHOD savingMethod = KEY_SAVING_METHOD::UNSET;
		char* passwd;
		char* cypherPath = NULL;
		DECYPHER_METHOD::DECYPHER_METHOD decypherMethod = DECYPHER_METHOD::UNSET;
		char* decypherPath = NULL;
	};

	//LAST ERROR
	extern unsigned int sfcErrno;

	int getLastError();
	STATUS::STATUS getStatus();
	long long getRead();

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
		props: properties of the file to cypher
		callback: callback to run on each step of the cyphering (GET_HASH, WRITING_HEADER, MAPPING_FILES, CYPHERING, FINISHED)

	Returns:
		256 bit key
	*/
	int cypher(SECURE_FOLDER_PROPS props, SFC_CALLBACK callback(void) = NULL);
	SECURE_FOLDER_PROPS* decypher(SECURE_FOLDER_PROPS* props, SFC_CALLBACK callback(void) = NULL);

	void cypherRawData(unsigned char* data, long size, SFC::KEY256* key);
	void cypherRawDataEvolve(unsigned char* data, long size, SFC::KEY256* key);
	void cypherKeyEvolve(SFC::KEY256* key);
}