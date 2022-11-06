#include "SecureFolderCore.h"
#pragma warning(disable: 4996)

unsigned int SFC::sfcErrno = ERR_NO_ERR;

SFC::STATUS::STATUS status = SFC::STATUS::FINISHED;
long long bytesRead;

int SFC::getLastError()
{
	return SFC::sfcErrno;
}

SFC::STATUS::STATUS SFC::getStatus()
{
	return status;
}

long long SFC::getRead()
{
	return bytesRead;
}