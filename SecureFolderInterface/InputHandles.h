#pragma once
#include "ExitCodes.h"
#include "../SecureFolderCore/SecureFolderCore.h"
#include "DisplayHandles.h"
#include "CustomDialogBox.h"
#include "resource.h"
#include "OperationnalHandles.h"
#include <shlobj_core.h>

enum class MODE : char
{
	MODE_UNKNOWN, MODE_CYPHER, MODE_DECYPHER
};

enum class SELECTED : short
{
	NONE, TARGET_PATH, TARGET_BTN, CYPHER_BTN, OPTIONS
};

enum class OPTION : short
{
	NONE
};

struct INPUT_FIELDS
{
	char targetPath[_MAX_PATH] = { '\0' };
	int pathPos = 0;

	MODE currentMode = MODE::MODE_UNKNOWN;
	SELECTED selected = SELECTED::NONE;
	OPTION option = OPTION::NONE;
};

extern INPUT_FIELDS fields;

//input functions
void setTargetPath(char* input, HWND hwnd);

void onKeyPress(char key);
void onMousePress(HWND hwnd, HINSTANCE hInstance, short x, short y);

void onTargetSearchSelect(HWND hwnd, HINSTANCE hInstance);
void onCypherBtnSelect(HWND hwnd, HINSTANCE hInstance);
void onOptionsSelect(short x, short y);