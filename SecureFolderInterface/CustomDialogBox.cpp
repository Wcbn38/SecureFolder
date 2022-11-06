#include "CustomDialogBox.h"
#pragma warning(disable: 4996)

#ifndef WM_PERSONAL
#define WM_PERSONAL WM_USER + 0x0001
#endif // !WM_PERSONAL

int returnMsg = NULL;

BOOL CALLBACK callback(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		returnMsg = LOWORD(wParam);
		EndDialog(hwndDlg, wParam);
	}
	return TRUE;
	default:
		break;
	}
	return FALSE;
}

int customDiagBox(HINSTANCE hInstance, HWND hwnd)
{
	returnMsg = NULL;
	DialogBoxA(hInstance, MAKEINTRESOURCEA(IDD_OLE_PROPPAGE_SMALL), hwnd, (DLGPROC)callback);
	return returnMsg;
}

char* container;
int cSize;

BOOL CALLBACK pswdCallback(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDOK)
		{
			char pass1[260], pass2[260];

			GetDlgItemTextA(hwndDlg, IDC_EDIT1, pass1, sizeof(pass1));
			GetDlgItemTextA(hwndDlg, IDC_EDIT2, pass2, sizeof(pass2));

			if (strncmp(pass1, pass2, 260) == 0)
			{
				strncpy(container, pass1, cSize);
				EndDialog(hwndDlg, wParam);
			}
			else
			{
				SetDlgItemTextA(hwndDlg, IDC_INVALID_PASS, "Invalid password");
			}
		}
		else if(LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hwndDlg, wParam);
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
	default:
		break;
	}
	return FALSE;

	//TODO: check for file drops
}

BOOL CALLBACK pswdCallbackNoValid(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDOK)
		{
			GetDlgItemTextA(hwndDlg, IDC_EDIT1, container, cSize);
			EndDialog(hwndDlg, wParam);
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hwndDlg, wParam);
		}
		else
		{
			return FALSE;
		}
	}
	return TRUE;
	default:
		break;
	}
	return FALSE;

	//TODO: check for file drops
}

HWND gHwndDlg = NULL;
bool flush = true;

void getPassword(HWND hwnd, char* passwContainer, int size, bool noValidating)
{
	container = passwContainer;
	cSize = size;
	if (!noValidating)
	{
		DialogBoxA(NULL, MAKEINTRESOURCEA(IDD_DIALOG1), hwnd, (DLGPROC)pswdCallback);
	}
	else
	{
		DialogBoxA(NULL, MAKEINTRESOURCEA(IDD_DIALOG3), hwnd, (DLGPROC)pswdCallbackNoValid);
	}
}

void update(int pos, const char* status)
{
	if (flush)
	{
		return;
	}

	while (gHwndDlg == NULL)
	{
		Sleep(1000);
	}

	SendDlgItemMessageA(gHwndDlg, IDC_PROGRESS2, PBM_SETPOS, pos, 0);
	SetDlgItemTextA(gHwndDlg, IDC_STATUS, status);
}

BOOL CALLBACK progressBarCallback(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		gHwndDlg = hwndDlg;
		SendDlgItemMessageA(hwndDlg, IDC_PROGRESS2, PBM_SETRANGE, 0, MAKELPARAM(0, 1000));
		SendDlgItemMessageA(hwndDlg, IDC_PROGRESS2, PBM_SETPOS, 0, 0);
		SetDlgItemTextA(hwndDlg, IDC_STATUS, "Status: Starting");
		return TRUE;
	}
	case WM_CLOSE:
	{
		gHwndDlg = NULL;
		return FALSE;
	}
	default:
		break;
	}

	return FALSE;
}

DWORD CALLBACK progressBar(_In_ LPVOID param)
{
	DialogBoxA(NULL, MAKEINTRESOURCEA(IDD_PROGRESS_BAR_DIALOG), (HWND)param, (DLGPROC)progressBarCallback);
	return 0;
}

void createProgressBar(HWND hwnd)
{
	flush = false;
	CreateThread(NULL, 0, progressBar, hwnd, NULL, 0);
}

void closeProgressBar()
{
	flush = true;
	EndDialog(gHwndDlg, 0);
}