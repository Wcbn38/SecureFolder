#pragma once
#include "afxdialogex.h"


// AskPassword dialog

class AskPassword : public CDialogEx
{
	DECLARE_DYNAMIC(AskPassword)
private:
	char* passwdContainer;

public:
	AskPassword(CWnd* pParent = nullptr);   // standard constructor
	virtual ~AskPassword();

	void getPassword(char* passwd);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ASK_PASSWORD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnOkPassword();
private:
	CString Pass;
};
