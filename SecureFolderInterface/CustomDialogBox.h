#pragma once
#include <Windows.h>
#include "resource.h"
#include <CommCtrl.h>
#include <stdio.h>

int customDiagBox(HINSTANCE hInstance, HWND hwnd);
void getPassword(HWND hwnd, char* passwContainer, int size, bool noValidating = false);
void createProgressBar(HWND hwnd);
void update(int pos, const char* status);
void closeProgressBar();