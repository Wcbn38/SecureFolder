
// SecureFolderMFCDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "SecureFolderMFC.h"
#include "SecureFolderMFCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSecureFolderMFCDlg dialog



CSecureFolderMFCDlg::CSecureFolderMFCDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SECUREFOLDERMFC_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSecureFolderMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, MainTab);
	DDX_Control(pDX, IDC_MFCEDITBROWSE2, MainBrowser);
	DDX_Control(pDX, IDC_COMBO1, KeySavingMethod);
	DDX_Control(pDX, IDC_PROGRESS1, MainProgressBar);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, MainBrowseResult);
	DDX_Control(pDX, IDC_BUTTON1, ButtonCtrl);
	DDX_Control(pDX, IDC_EDIT1, StatusTextBox);
	DDX_Control(pDX, IDC_EDIT3, CurrentProgressText);
	DDX_Control(pDX, IDC_EDIT4, MaxProgressText);
}

BEGIN_MESSAGE_MAP(CSecureFolderMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CSecureFolderMFCDlg::OnGoBtnClicked)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CSecureFolderMFCDlg::OnTabChange)
END_MESSAGE_MAP()


// CSecureFolderMFCDlg message handlers

BOOL CSecureFolderMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	ShowWindow(SW_NORMAL);

	ShowWindow(SW_NORMAL);

	MainTab.InsertItem(0, TAB_ITEM_CYPHER);
	MainTab.InsertItem(1, TAB_ITEM_DECYPHER);
	MainTab.SetCurSel(0);

	KeySavingMethod.InsertString(0, KEY_SAVE_PASSWD);
	KeySavingMethod.InsertString(1, KEY_SAVE_USBKEY);

	MainProgressBar.SetRange(0, 100);
	MainProgressBar.SetPos(0);

	SetBrowserMode();

	StatusTextBox.SetWindowTextW(L"Status: Idle");
	CurrentProgressText.SetWindowTextW(L"0 kB");
	MaxProgressText.SetWindowTextW(L"0 kB");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSecureFolderMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSecureFolderMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSecureFolderMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSecureFolderMFCDlg::SetBrowserMode()
{
	switch (MainTab.GetCurSel())
	{
	case 0:
		MainBrowser.EnableFolderBrowseButton();
		MainBrowseResult.EnableFileBrowseButton(L"sfp", L"Secure Folder Package|*.sfp| All |*.*||");
		break;
	case 1:
		MainBrowser.EnableFileBrowseButton(L"sfp", L"Secure Folder Package|*.sfp| All |*.*||");
		MainBrowseResult.EnableFolderBrowseButton();
		break;
	default:
		MainBrowser.EnableBrowseButton(FALSE);
		MainBrowseResult.EnableBrowseButton(FALSE);
		break;
	}
}

DWORD WINAPI BtnClickThread(LPVOID params)
{
	CSecureFolderMFCDlg* window = (CSecureFolderMFCDlg*) params;

	switch (window->MainTab.GetCurSel())
	{
	case 0: //cypher
	{
		SFC::KEY256 key;
		char tmpFilePath[_MAX_PATH];
		char sourceFolder[_MAX_PATH];
		long long progress;
		loadingBarStruct loadBarStruct;

		CString cstringBin;
		std::fstream finalFile;
		std::fstream mappedFile;

		CHANGE_STATUS("Computing key 1/4");
		switch (window->KeySavingMethod.GetCurSel())
		{
		case 0: //password
		{
			char passwd[256] = { '\0' };

			NewPassword passwdGetter(window->GetOwner());
			passwdGetter.getPassword(passwd);

			if (passwd[0] == '\0')
			{
				MessageBox(window->GetSafeHwnd(), L"No password entred.", L"Critical error", MB_OK | MB_ICONERROR | MB_TOPMOST);
				return 0;
			}

			key = SFC::getSHA256(passwd, strlen(passwd));
		}
		break;
		case 1: //usb
			MessageBox(window->GetSafeHwnd(), L"Not handled.", L"Critical error", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto clean_basic_cyph;
		default:
			MessageBox(window->GetSafeHwnd(), L"No key saving selected.", L"Critical error", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto clean_basic_cyph;
		}

		//cypher procedure
		CHANGE_STATUS("Scanning files 2/4");
		window->ButtonCtrl.ShowWindow(SW_HIDE);
		tmpnam_s<sizeof(tmpFilePath)>(tmpFilePath);
		mappedFile.open(tmpFilePath, std::ios_base::binary | std::ios_base::out | std::ios_base::in | std::ios_base::trunc);

		window->MainBrowser.GetWindowTextW(cstringBin);
		long long maxRange = getSize((char*)CStringA(cstringBin).GetBuffer());
		window->MainProgressBar.SetRange32(0, maxRange / 1024);
		wchar_t progressText[128]; wsprintf(progressText, L"%li kB", maxRange / 1024);
		window->MaxProgressText.SetWindowTextW(progressText);
		progress = 0;

		CHANGE_STATUS("Mapping files 3/4");

		loadBarStruct = { &window->MainProgressBar, &window->CurrentProgressText, &progress };
		TSKHNDL tskhndl = startTask(100, updateLoadingBar, (void*)&loadBarStruct);

		int iResult = SFC::mapFolder(&mappedFile, (char*)CStringA(cstringBin).GetBuffer(), ".", &progress);

		stopTask(tskhndl);

		wsprintf(progressText, L"%li kB", maxRange / 1024);
		window->CurrentProgressText.SetWindowTextW(progressText);
		window->MainProgressBar.SetPos(progress / 1024);

		switch (iResult)
		{
		case -ERR_CANT_OPEN_DIR:
			MessageBox(window->GetSafeHwnd(), L"Failed to find folder.", L"Critical error", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto clean_cyph;
		case -ERR_NO_ERR:
			break;
		default:
			MessageBox(window->GetSafeHwnd(), L"Unknown error.", L"Critical error", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto clean_cyph;
		}

		CHANGE_STATUS("Cyphering 4/4");
		window->MainBrowseResult.GetWindowTextW(cstringBin);
		finalFile.open((char*)CStringA(cstringBin).GetBuffer(), std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);

		maxRange = mappedFile.tellp();
		window->MainProgressBar.SetRange32(0, maxRange / 1024);
		progressText[128]; wsprintf(progressText, L"%li kB", maxRange / 1024);
		window->MaxProgressText.SetWindowTextW(progressText);
		mappedFile.seekg(0);
		progress = 0;
		loadBarStruct = { &window->MainProgressBar, &window->CurrentProgressText, &progress };
		tskhndl = startTask(100, updateLoadingBar, (void*)&loadBarStruct);

		iResult = SFC::cypherFile(key, &mappedFile, &finalFile, SFC::KEY_MODE::EVOLVING, &progress);

		stopTask(tskhndl);

		wsprintf(progressText, L"%li kB", maxRange / 1024);
		window->CurrentProgressText.SetWindowTextW(progressText);
		window->MainProgressBar.SetPos(progress / 1024);

		window->ButtonCtrl.ShowWindow(SW_SHOW);

		switch (iResult)
		{
		case -ERR_NO_ERR: break;
		case -ERR_CANT_WRITE:
			MessageBox(window->GetSafeHwnd(), L"Failed to write to final file.", L"Critical error", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto clean_cyph;
		default:
			break;
		}

		window->MainBrowser.SetWindowTextW(L"");
		window->MainBrowseResult.SetWindowTextW(L"");

	clean_cyph:
		window->ButtonCtrl.ShowWindow(SW_SHOW);
		mappedFile.close();
		remove(tmpFilePath);
	clean_basic_cyph:
		CHANGE_STATUS("Idle");
	}
	break;
	case 1: //decypher
	{
		SFC::KEY256 key;
		char tmpFilePath[_MAX_PATH];
		long long progress = 0;
		loadingBarStruct loadBarStruct;
		CString cstringBin;
		std::fstream uncypheredFile;
		std::fstream cypheredFile;

		CHANGE_STATUS("Computing key 1/4");

		switch (window->KeySavingMethod.GetCurSel())
		{
		case 0: //password
		{
			char passwd[256] = { '\0' };

			AskPassword passwdGetter(window->GetOwner());
			passwdGetter.getPassword(passwd);

			if (passwd[0] == '\0')
			{
				MessageBox(window->GetSafeHwnd(), L"No password entred.", L"Critical error", MB_OK | MB_ICONERROR | MB_TOPMOST);
				goto clean_basic_decyph;
			}

			key = SFC::getSHA256(passwd, strlen(passwd));
		}
		break;
		case 1: //usb
			MessageBox(window->GetSafeHwnd(), L"Not handled.", L"Critical error", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto clean_basic_decyph;
		default:
			MessageBox(window->GetSafeHwnd(), L"No key saving selected.", L"Critical error", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto clean_basic_decyph;
		}

		//decypher procedure
		CHANGE_STATUS("Scanning files 2/4");

		window->MainBrowser.GetWindowTextW(cstringBin);

		window->ButtonCtrl.ShowWindow(SW_HIDE);
		tmpnam_s<sizeof(tmpFilePath)>(tmpFilePath);
		uncypheredFile.open(tmpFilePath, std::ios_base::binary | std::ios_base::out | std::ios_base::in | std::ios_base::trunc);
		cypheredFile.open((char*)CStringA(cstringBin).GetBuffer(), std::ios_base::binary | std::ios_base::in);

		if (!cypheredFile.is_open())
		{
			MessageBox(window->GetSafeHwnd(), L"Failed to open file.", L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto clean_decyph;
		}


		long long maxRange = getSize((char*)CStringA(cstringBin).GetBuffer());
		window->MainProgressBar.SetRange32(0, maxRange / 1024);
		wchar_t progressText[128]; wsprintf(progressText, L"%li kB", maxRange / 1024);
		window->MaxProgressText.SetWindowTextW(progressText);
		progress = 0;

		CHANGE_STATUS("Decyphering 3/4");

		loadBarStruct = { &window->MainProgressBar, &window->CurrentProgressText, &progress };
		TSKHNDL tskhndl = startTask(100, updateLoadingBar, (void*)&loadBarStruct);

		int iResult = SFC::decypherFile(key, &cypheredFile, &uncypheredFile, &progress);

		stopTask(tskhndl);

		wsprintf(progressText, L"%li kB", maxRange / 1024);
		window->CurrentProgressText.SetWindowTextW(progressText);
		window->MainProgressBar.SetPos(progress / 1024);

		switch (iResult)
		{
		case -ERR_INVALID_PASSWORD:
			MessageBox(window->GetSafeHwnd(), L"Wrong password.", L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto clean_decyph;
		case -ERR_NO_ERR:
			break;
		default:
			MessageBox(window->GetSafeHwnd(), L"Unknown error.", L"Critical error", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto clean_decyph;
		}

		maxRange = uncypheredFile.tellp();
		window->MainProgressBar.SetRange32(0, maxRange / 1024);
		wsprintf(progressText, L"%li kB", maxRange / 1024);
		window->MaxProgressText.SetWindowTextW(progressText);

		uncypheredFile.seekg(0);
		progress = 0;

		loadBarStruct = { &window->MainProgressBar, &window->CurrentProgressText, &progress };
		tskhndl = startTask(100, updateLoadingBar, (void*)&loadBarStruct);

		CHANGE_STATUS("Unmapping files 4/4");

		window->MainBrowseResult.GetWindowTextW(cstringBin);

		iResult = SFC::unmapFolder(&uncypheredFile, (char*)CStringA(cstringBin).GetBuffer(), &progress);

		stopTask(tskhndl);

		wsprintf(progressText, L"%li kB", maxRange / 1024);
		window->CurrentProgressText.SetWindowTextW(progressText);
		window->MainProgressBar.SetPos(progress / 1024);

		switch (iResult)
		{
		case -ERR_CANT_WRITE:
			MessageBox(window->GetSafeHwnd(), L"Can't write to target folder.", L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto clean_decyph;
		case -ERR_CANT_DECYPH:
			MessageBox(window->GetSafeHwnd(), L"Corrupted file.", L"Error", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto clean_decyph;
		case -ERR_NO_ERR:
			break;
		default:
			MessageBox(window->GetSafeHwnd(), L"Unknown error.", L"Critical error", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto clean_decyph;
		}

		window->ButtonCtrl.ShowWindow(SW_SHOW);

		window->MainBrowser.SetWindowTextW(L"");
		window->MainBrowseResult.SetWindowTextW(L"");

	clean_decyph:
		uncypheredFile.close();
		cypheredFile.close();
		remove(tmpFilePath);
	clean_basic_decyph:
		window->ButtonCtrl.ShowWindow(SW_SHOW);
	}
	break;
	default:
		MessageBox(window->GetSafeHwnd(), L"No mode selected.", L"Critical error", MB_OK | MB_ICONERROR | MB_TOPMOST);
		break;
	}

	window->StatusTextBox.SetWindowTextW(L"Status: Idle");

	return 0;
}

void CSecureFolderMFCDlg::OnGoBtnClicked()
{
	CreateThread(NULL, 0, BtnClickThread, this, NULL, 0);
}

void CSecureFolderMFCDlg::OnTabChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetBrowserMode();
	*pResult = 0;
}

void updateLoadingBar(void* prms)
{
	loadingBarStruct* loadBarStr = (loadingBarStruct*)prms;

	loadBarStr->bar->SetPos(*loadBarStr->progress / 1024);
	wchar_t progress[128];
	wsprintf(progress, L"%li kB", *loadBarStr->progress / 1024);
	loadBarStr->progressText->SetWindowTextW(progress);
}