// AskPassword.cpp : implementation file
//

#include "pch.h"
#include "SecureFolderMFC.h"
#include "afxdialogex.h"
#include "AskPassword.h"


// AskPassword dialog

IMPLEMENT_DYNAMIC(AskPassword, CDialogEx)

AskPassword::AskPassword(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ASK_PASSWORD, pParent)
	, Pass(_T(""))
{

}

void AskPassword::getPassword(char* passwd)
{
	this->passwdContainer = passwd;
	this->DoModal();
}

AskPassword::~AskPassword()
{
}

void AskPassword::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, Pass);
}


BEGIN_MESSAGE_MAP(AskPassword, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &AskPassword::OnOkPassword)
END_MESSAGE_MAP()


// AskPassword message handlers


void AskPassword::OnOkPassword()
{
	UpdateData(TRUE);
	strcpy_s(passwdContainer, 256, (char*)CStringA(Pass).GetBuffer());
	this->EndDialog(0);
}
