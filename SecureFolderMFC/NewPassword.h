#pragma once
#include "afxdialogex.h"


// NewPassword dialog

class NewPassword : public CDialogEx
{
	DECLARE_DYNAMIC(NewPassword)
private:
	char* passwdContainer;

public:
	NewPassword(CWnd* pParent = nullptr);   // standard constructor
	virtual ~NewPassword();

	void getPassword(char* passwd);

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NEW_PASSWORD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPasswordConfirmed();
private:
	CString Pass;
	CString PassConf;
public:
	afx_msg void OnEdit1EnterPressed();
//	afx_msg void OnEdit2EnterPressed();
};
