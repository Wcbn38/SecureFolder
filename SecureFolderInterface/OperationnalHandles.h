#pragma once
#include "../SecureFolderCore/SecureFolderCore.h"
#include "InputHandles.h"
#include <shlobj_core.h>
#include <Windows.h>

bool readyToClose();
void startOP(HWND hwnd, HINSTANCE hInstance);