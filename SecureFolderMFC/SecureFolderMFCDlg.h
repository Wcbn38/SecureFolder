
// SecureFolderMFCDlg.h : header file
//

#pragma once
#include "CtrlStrings.h"
#include "../SecureFolderCore/SecureFolderCore.h"
#include "NewPassword.h"
#include "AskPassword.h"
#include "schedule.h"
#include <filesystem>
#include "Toolbox.h"

#define CHANGE_STATUS(a) window->StatusTextBox.SetWindowTextW(L"Status: " a);

// CSecureFolderMFCDlg dialog
class CSecureFolderMFCDlg : public CDialogEx
{
// Construction
public:
	CSecureFolderMFCDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SECUREFOLDERMFC_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void SetBrowserMode();
public:
	CTabCtrl MainTab;
	CMFCEditBrowseCtrl MainBrowser;
	CComboBox KeySavingMethod;
	afx_msg void OnGoBtnClicked();
	CProgressCtrl MainProgressBar;
	CMFCEditBrowseCtrl MainBrowseResult;
	CButton ButtonCtrl;
	CEdit StatusTextBox;

public:
	afx_msg void OnTabChange(NMHDR* pNMHDR, LRESULT* pResult);
	CEdit CurrentProgressText;
	CEdit MaxProgressText;
};

struct loadingBarStruct
{
	CProgressCtrl* bar;
	CEdit* progressText;
	long long* progress;
};

void updateLoadingBar(void* prms);