// NewPassword.cpp : implementation file
//

#include "pch.h"
#include "SecureFolderMFC.h"
#include "afxdialogex.h"
#include "NewPassword.h"


// NewPassword dialog

IMPLEMENT_DYNAMIC(NewPassword, CDialogEx)

NewPassword::NewPassword(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NEW_PASSWORD, pParent)
	, Pass(_T(""))
	, PassConf(_T(""))
{

}

NewPassword::~NewPassword()
{
}

void NewPassword::getPassword(char* passwd)
{
	this->passwdContainer = passwd;
	this->DoModal();
}

void NewPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, Pass);
	DDX_Text(pDX, IDC_EDIT2, PassConf);
}


BEGIN_MESSAGE_MAP(NewPassword, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &NewPassword::OnPasswordConfirmed)
//	ON_EN_KILLFOCUS(IDC_EDIT1, &NewPassword::OnEdit1EnterPressed)
//	ON_EN_KILLFOCUS(IDC_EDIT2, &NewPassword::OnEdit2EnterPressed)
END_MESSAGE_MAP()


// NewPassword message handlers


void NewPassword::OnPasswordConfirmed()
{
	UpdateData(TRUE);
	if (!StrCmpCW(Pass, PassConf))
	{
		strcpy_s(passwdContainer, 256, (char*) CStringA(Pass).GetBuffer());
		this->EndDialog(0);
	}
	else
	{
		MessageBox(L"Please enter the same password.", L"Password error", MB_OK | MB_ICONERROR | MB_TOPMOST);
	}
}