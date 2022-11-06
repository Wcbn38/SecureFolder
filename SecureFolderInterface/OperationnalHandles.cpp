#include "OperationnalHandles.h"
#pragma warning(disable: 4996)

bool working = false;
long long totalBytes = 0;

bool readyToClose()
{
    return !working;
}

void getStorageFile(char* container, int size, HWND hwnd, HINSTANCE hInstance);
void getStorageFolder(char* container, int size, HWND hwnd);
SFC_CALLBACK statusUpdater();
long long getSize(char path[_MAX_PATH]);
DWORD CALLBACK scheduler(_In_ LPVOID params);

struct THREAD_INFO
{
	SFC::SECURE_FOLDER_PROPS* props;
	HWND hwnd;
};

DWORD WINAPI cypherThread(_In_ LPVOID params)
{
	THREAD_INFO* props = (THREAD_INFO*) params;

	bool stopVar = false;
	HANDLE timerTask = CreateThread(NULL, 0, scheduler, &stopVar, 0, 0);

	if (SFC::cypher(*props->props, statusUpdater) != 0)
	{
		char message[250];
		sprintf(message, "Error: 0x%X", SFC::getLastError());
		MessageBoxA(props->hwnd, message, NULL, MB_OK | MB_ICONSTOP | MB_APPLMODAL | MB_SETFOREGROUND);
	}

	stopVar = true;
	while (stopVar);

	fields.pathPos = 0;
	fields.targetPath[0] = '\0';

	InvalidateRect(props->hwnd, NULL, true);
	
	delete props->props->passwd;
	delete props->props->cypherPath;
	delete props->props->decypherPath;
	delete props->props;
	delete props;

	working = false;

	closeProgressBar();
	totalBytes = 1;

	return 0;
}

DWORD WINAPI deCypherThread(_In_ LPVOID params)
{
	THREAD_INFO* props = (THREAD_INFO*)params;

	bool stopVar = false;
	HANDLE timerTask = CreateThread(NULL, 0, scheduler, &stopVar, 0, 0);

	if (SFC::decypher(props->props, statusUpdater) == NULL)
	{
		char message[250];
		sprintf(message, "Error: 0x%X", SFC::getLastError());
		MessageBoxA(props->hwnd, message, NULL, MB_OK | MB_ICONSTOP | MB_APPLMODAL | MB_SETFOREGROUND);
	}

	stopVar = true;
	while (stopVar);

	fields.pathPos = 0;
	fields.targetPath[0] = '\0';

	InvalidateRect(props->hwnd, NULL, true);

	delete props->props->passwd;
	delete props->props->cypherPath;
	delete props->props->decypherPath;
	delete props->props;
	delete props;

	working = false;

	closeProgressBar();
	totalBytes = 1;

	return 0;
}

void startOP(HWND hwnd, HINSTANCE hInstance)
{
	switch (fields.currentMode)
	{
	case MODE::MODE_CYPHER: 
	{
		SFC::SECURE_FOLDER_PROPS* props = new SFC::SECURE_FOLDER_PROPS;
		props->cypherMethod = SFC::CYPHER_METHOD::SEQUENTIAL;
		props->keyMode = SFC::KEY_MODE::EVOLVING;
		props->decypherPath = new char[260];
		strncpy(props->decypherPath, fields.targetPath, 260);
		props->savingMethod = SFC::KEY_SAVING_METHOD::PASSWORD;

		props->cypherPath = new char[260];
		ZeroMemory(props->cypherPath, 260);
		getStorageFile(props->cypherPath, 260, hwnd, hInstance);

		props->passwd = new char[260];
		props->passwd[0] = '\0';
		getPassword(hwnd, props->passwd, 260);

		if (props->passwd[0] != '\0')
		{
			working = true;
			THREAD_INFO* params = new THREAD_INFO;
			params->props = props;
			params->hwnd = hwnd;

			createProgressBar(hwnd);
			{
				char path[_MAX_PATH];
				sprintf(path, "%s*", props->decypherPath);
				totalBytes = getSize(path);
			}
			CreateThread(NULL, 0, cypherThread, params, 0, NULL);
		}
		else
		{
			delete props->passwd;
			delete props->cypherPath;
			delete props->decypherPath;
			delete props;
			MessageBoxA(hwnd, "Invalid password", NULL, MB_OK | MB_ICONSTOP | MB_APPLMODAL | MB_SETFOREGROUND);
		}
	}
	break;
	case MODE::MODE_DECYPHER:
	{
		SFC::SECURE_FOLDER_PROPS* props = new SFC::SECURE_FOLDER_PROPS;
		props->cypherMethod = SFC::CYPHER_METHOD::SEQUENTIAL;
		props->keyMode = SFC::KEY_MODE::EVOLVING;
		props->cypherPath = new char[260];
		strncpy(props->cypherPath, fields.targetPath, 260);
		props->savingMethod = SFC::KEY_SAVING_METHOD::PASSWORD;
		props->decypherMethod = SFC::DECYPHER_METHOD::COPY;

		props->decypherPath = new char[260];
		ZeroMemory(props->decypherPath, 260);
		getStorageFolder(props->decypherPath, 260, hwnd);

		props->passwd = new char[260];
		props->passwd[0] = '\0';
		getPassword(hwnd, props->passwd, 260, true);

		if (props->passwd[0] != '\0')
		{
			working = true;
			THREAD_INFO* params = new THREAD_INFO;
			params->props = props;
			params->hwnd = hwnd;

			createProgressBar(hwnd);
			{
				char path[_MAX_PATH];
				strncpy(path, props->cypherPath, _MAX_PATH);
				totalBytes = getSize(path);
			}
			CreateThread(NULL, 0, deCypherThread, params, 0, NULL);
		}
		else
		{
			delete props->passwd;
			delete props->cypherPath;
			delete props->decypherPath;
			delete props;
			MessageBoxA(hwnd, "Invalid password", NULL, MB_OK | MB_ICONSTOP | MB_APPLMODAL | MB_SETFOREGROUND);
		}
	}
	break;
	default:
		MessageBoxA(hwnd, "This file cannot be treated", NULL, MB_OK | MB_ICONSTOP | MB_APPLMODAL | MB_SETFOREGROUND);
		break;
	}
}

void getStorageFile(char * container, int size, HWND hwnd, HINSTANCE hInstance)
{
	OPENFILENAMEA searchData;
	searchData.lStructSize = sizeof(searchData);
	searchData.hwndOwner = NULL;
	searchData.hInstance = hInstance;
	searchData.lpstrFilter = "Raw data (*)\0*\0Secure Folder Package (*.sfp)\0*.sfp\0";
	searchData.lpstrCustomFilter = NULL;
	searchData.lpstrFile = container;
	searchData.nMaxFile = size;
	searchData.lpstrFileTitle = NULL;
	searchData.nMaxFileTitle = NULL;
	searchData.lpstrInitialDir = NULL;
	searchData.lpstrTitle = NULL;
	searchData.Flags = OFN_PATHMUSTEXIST | OFN_CREATEPROMPT;
	searchData.nFileOffset = 0;
	searchData.lpstrDefExt = "sfp";

	GetSaveFileNameA(&searchData);
}

void getStorageFolder(char* container, int size, HWND hwnd)
{
	char fileName[_MAX_PATH];

	BROWSEINFOA browseInfo;
	browseInfo.hwndOwner = hwnd;
	browseInfo.pidlRoot = NULL;
	browseInfo.pszDisplayName = fileName;
	browseInfo.lpszTitle = "Please select the folder you wan't to decypher.";
	browseInfo.ulFlags = BIF_NEWDIALOGSTYLE;
	browseInfo.lpfn = NULL;

	LPITEMIDLIST id = SHBrowseForFolderA(&browseInfo);

	SHGetPathFromIDListA(id, container);

	int i = 0;
	while ((container[i] != '\0') && (i < (size - 1)))
	{
		i++;
	}

	if (i >= (size - 1))
	{
		container[0] = '\0';
	}
	else
	{
		container[i] = '\\';
		container[i + 1] = '\0';
	}
}

SFC_CALLBACK statusUpdater()
{
	long long pos = SFC::getRead();
	SFC::STATUS::STATUS status = SFC::getStatus();
	pos = pos * 1000 / totalBytes;

	if (fields.currentMode == MODE::MODE_CYPHER)
	{
		switch (status)
		{
		case SFC::STATUS::GET_HASH: update(pos, "Status: Getting hash");
			break;
		case SFC::STATUS::WRITING_HEADER: update(pos, "Status: Writing header");
			break;
		case SFC::STATUS::MAPPING_FILES: update(pos, "Status: Mapping files and directories");
			break;
		case SFC::STATUS::CYPHERING: update(pos, "Status: Cyphering");
			break;
		case SFC::STATUS::FINISHED: update(pos, "Status: Finished");
			break;
		default: update(pos, "Status: UNKNOWN");
			break;
		}
	}
	else
	{
		switch (status)
		{
		case SFC::STATUS::GET_HASH: update(pos, "Status: Getting hash");
			break;
		case SFC::STATUS::WRITING_HEADER: update(pos, "Status: Reading header");
			break;
		case SFC::STATUS::MAPPING_FILES: update(pos, "Status: Writing files and directories");
			break;
		case SFC::STATUS::CYPHERING: update(pos, "Status: Decyphering");
			break;
		case SFC::STATUS::FINISHED: update(pos, "Status: Finished");
			break;
		default: update(pos, "Status: UNKNOWN");
			break;
		}
	}
}

long long getSize(char path[_MAX_PATH])
{
	_finddata_t data;
	intptr_t handle = _findfirst(path, &data);
	path[strnlen(path, _MAX_PATH) - 1] = '\0';

	if (handle == -1)
	{
		return 0;
	}

	long long size = data.size;

	while (_findnext(handle, &data) == 0)
	{
		if (data.attrib & _A_SUBDIR)
		{
			if ((strncmp(data.name, ".", _MAX_PATH) != 0) && (strncmp(data.name, "..", _MAX_PATH) != 0))
			{
				char nextPath[_MAX_PATH];

				sprintf(nextPath, "%s%s\\*", path, data.name);
				size += getSize(data.name);
			}
		}
		else
		{
			size += data.size;
		}
	}
	
	return size;
}

DWORD CALLBACK scheduler(_In_ LPVOID params)
{
	bool* stop = (bool*) params;

	while (!(*stop))
	{
		Sleep(500);
		statusUpdater();
	}

	*stop = false;

	return false;
}