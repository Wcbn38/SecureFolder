#include "InputHandles.h"
#pragma warning(disable: 4996)

INPUT_FIELDS fields;

#define BETWEEN(x, X1, X2) ((x > X1) && (x < X2))

void refreshCurrentMode();

void setTargetPath(char* input, HWND hwnd)
{
	strncpy(fields.targetPath, input, _MAX_PATH);

	fields.pathPos = 0;

	while (input[fields.pathPos] != '\0' && fields.pathPos < _MAX_PATH)
	{
		fields.targetPath[fields.pathPos] = input[fields.pathPos];
		fields.pathPos++;
	}

	int result = GetFileAttributesA(fields.targetPath);
	if (result & FILE_ATTRIBUTE_DIRECTORY)
	{
		fields.targetPath[fields.pathPos] = '\\';
		fields.pathPos++;
		fields.targetPath[fields.pathPos] = '\0';
	}

	refreshCurrentMode();
	InvalidateRect(hwnd, NULL, true);
}

void onMousePress(HWND hwnd, HINSTANCE hInstance, short x, short y)
{
	if (readyToClose) //no operation
	{
		if (BETWEEN(x, POS_TARGET_PATH_X, POS_TARGET_PATH_X + DIM_TARGET_PATH_WIDTH)
			&& BETWEEN(y, POS_TARGET_PATH_Y, POS_TARGET_PATH_Y + DIM_TARGET_PATH_HEIGHT)) //target box
		{
			fields.selected = SELECTED::TARGET_PATH;
		}
		else if (BETWEEN(x, POS_RESEARCH_BTN_X, POS_RESEARCH_BTN_X + DIM_RESEARCH_BTN_WIDTH)
			&& BETWEEN(y, POS_RESEARCH_BTN_Y, POS_RESEARCH_BTN_Y + DIM_RESEARCH_BTN_HEIGHT)) //target box
		{
			fields.selected = SELECTED::TARGET_BTN;
			onTargetSearchSelect(hwnd, hInstance);
		}
		else if (BETWEEN(x, POS_CYPHER_BTN_X, POS_CYPHER_BTN_X + DIM_CYPHER_BTN_WIDTH)
			&& BETWEEN(y, POS_CYPHER_BTN_Y, POS_CYPHER_BTN_Y + DIM_CYPHER_BTN_HEIGHT)) //cypher button box
		{
			fields.selected = SELECTED::CYPHER_BTN;
			onCypherBtnSelect(hwnd, hInstance);
		}
		else if (BETWEEN(x, POS_OPTIONS_RECT_X, POS_OPTIONS_RECT_X + DIM_OPTIONS_RECT_WIDTH)
			&& BETWEEN(y, POS_OPTIONS_RECT_Y, POS_OPTIONS_RECT_Y + DIM_OPTIONS_RECT_HEIGHT)) //option box
		{
			fields.selected = SELECTED::OPTIONS;
			onOptionsSelect(x, y);
		}
		else
		{
			fields.selected = SELECTED::NONE;
		}

		InvalidateRect(hwnd, NULL, true);
	}
}

void onTargetSearchSelect(HWND hwnd, HINSTANCE hInstance)
{
	
	char filePath[_MAX_PATH] = { NULL };
	char fileName[_MAX_PATH] = { NULL };

	switch (customDiagBox(hInstance, hwnd))
	{
	case IDC_FILE:
	{
		OPENFILENAMEA searchData;
		searchData.lStructSize = sizeof(searchData);
		searchData.hwndOwner = hwnd;
		searchData.hInstance = hInstance;
		searchData.lpstrFilter = "Raw data (*)\0*\0Secure Folder Package (*.sfp)\0*.sfp\0";
		searchData.lpstrCustomFilter = NULL;
		searchData.lpstrFile = filePath;
		searchData.nMaxFile = sizeof(filePath);
		searchData.lpstrFileTitle = fileName;
		searchData.nMaxFileTitle = sizeof(fileName);
		searchData.lpstrInitialDir = NULL;
		searchData.lpstrTitle = NULL;
		searchData.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		searchData.nFileOffset = 0;
		searchData.lpstrDefExt = NULL;

		if (GetOpenFileNameA(&searchData))
		{
			fields.pathPos = 0;

			while (filePath[fields.pathPos] != '\0' && fields.pathPos < _MAX_PATH)
			{
				fields.targetPath[fields.pathPos] = filePath[fields.pathPos];
				fields.pathPos++;
			}

			fields.targetPath[fields.pathPos] = '\0';
		}
	}
		break;
	case IDC_FOLDER:
	{
		BROWSEINFOA browseInfo;
		browseInfo.hwndOwner = hwnd;
		browseInfo.pidlRoot = NULL;
		browseInfo.pszDisplayName = fileName;
		browseInfo.lpszTitle = "Please select the folder you wan't to cypher.";
		browseInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON | BIF_BROWSEINCLUDEFILES;
		browseInfo.lpfn = NULL;

		LPITEMIDLIST id = SHBrowseForFolderA(&browseInfo);

		SHGetPathFromIDListA(id, filePath);

		fields.pathPos = 0;

		while (filePath[fields.pathPos] != '\0' && fields.pathPos < (_MAX_PATH - 1))
		{
			fields.targetPath[fields.pathPos] = filePath[fields.pathPos];
			fields.pathPos++;
		}

		if (fields.pathPos > 0)
		{
			fields.targetPath[fields.pathPos] = '\\';
			fields.pathPos++;
			fields.targetPath[fields.pathPos] = '\0';
		}
	}
		break;
	default:
		break;
	}

	refreshCurrentMode();
}

void onCypherBtnSelect(HWND hwnd, HINSTANCE hInstance)
{
	startOP(hwnd, hInstance);
}

void onOptionsSelect(short x, short y)
{
	
}

void onKeyPress(char key)
{
	if (fields.selected == SELECTED::TARGET_PATH)
	{
		if (key == '\b')
		{
			if (fields.pathPos > 0)
			{
				fields.pathPos--;
				fields.targetPath[fields.pathPos] = '\0';
			}
		}
		else if (fields.pathPos < (_MAX_PATH - 1))
		{
			fields.targetPath[fields.pathPos] = key;
			fields.pathPos++;
			fields.targetPath[fields.pathPos] = '\0';
		}

		refreshCurrentMode();
	}
}

void refreshCurrentMode()
{
	if (fields.pathPos <= 0)
	{
		fields.currentMode = MODE::MODE_UNKNOWN;
	}
	else if ((fields.targetPath[fields.pathPos - 1] == '\\') || (fields.targetPath[fields.pathPos - 1] == '/'))
	{
		fields.currentMode = MODE::MODE_CYPHER;
	}
	else
	{
		fields.currentMode = MODE::MODE_DECYPHER;
	}
}