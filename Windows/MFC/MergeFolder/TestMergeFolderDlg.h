// TestMergeFolderDlg.h : header file
//

#if !defined(AFX_TESTMERGEFOLDERDLG_H__7D1323DE_C31F_4B93_944A_E0B624D7F8F9__INCLUDED_)
#define AFX_TESTMERGEFOLDERDLG_H__7D1323DE_C31F_4B93_944A_E0B624D7F8F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CTestMergeFolderDlg dialog
#include "MergeFolder.h"
#include "ProcBarCtrl.h"

class CTestMergeFolderDlg : public CDialog
{
// Construction
public:
	CTestMergeFolderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTestMergeFolderDlg)
	enum { IDD = IDD_TESTMERGEFOLDER_DIALOG };
	CButton	m_btnOpen;
	CButton	m_btnMerge;
	CProcBarCtrl	m_progCtrl;
	CListCtrl	m_lst;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestMergeFolderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	PMfe  m_pMfe;

	void ShowMfeFolder( PMfeFolder pFolder );

	// Generated message map functions
	//{{AFX_MSG(CTestMergeFolderDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnMerge();
	afx_msg void OnOpen();
	afx_msg void OnDblclkFolder(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG

    afx_msg LRESULT OnProgComplete(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnProgNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTMERGEFOLDERDLG_H__7D1323DE_C31F_4B93_944A_E0B624D7F8F9__INCLUDED_)
