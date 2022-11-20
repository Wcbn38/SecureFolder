#include "pch.h"
#include "Toolbox.h"

long long getFolderSize(char path[_MAX_PATH])
{
	_finddatai64_t data;
	intptr_t handle = _findfirsti64(path, &data);

	if (handle == -1)
	{
		return 0;
	}

	long long size = data.size;

	char* pathEnd = path + strlen(path);
	while (*pathEnd != '\\')
		pathEnd--;

	*pathEnd = '\0';

	do
	{
		if (data.attrib & _A_SUBDIR)
		{
			if ((strncmp(data.name, ".", _MAX_PATH) != 0) && (strncmp(data.name, "..", _MAX_PATH) != 0))
			{
				char nextPath[_MAX_PATH];

				sprintf_s(nextPath, "%s\\%s\\*", path, data.name);
				size += getFolderSize(nextPath);
			}
		}
		else
		{
			size += data.size;
		}
	} while (_findnexti64(handle, &data) == 0);

	return size;
}

long long getSize(char path[_MAX_PATH])
{
	_finddatai64_t data;
	intptr_t handle = _findfirsti64(path, &data);

	if (handle == -1)
	{
		return 0;
	}

	long long size = 0;

	do
	{
		if (data.attrib & _A_SUBDIR)
		{
			if ((strncmp(data.name, ".", _MAX_PATH) != 0) && (strncmp(data.name, "..", _MAX_PATH) != 0))
			{
				char nextPath[_MAX_PATH];

				sprintf_s(nextPath, "%s\\*", path);
				size += getFolderSize(nextPath);
			}
		}
		else
		{
			size += data.size;
		}
	} while (_findnexti64(handle, &data) == 0);

	return size;
}