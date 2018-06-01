// prjScrollWndDlg.h : header file
//

#if !defined(AFX_PRJSCROLLWNDDLG_H__86CB50E6_8F09_4886_9CF7_906567A5B72D__INCLUDED_)
#define AFX_PRJSCROLLWNDDLG_H__86CB50E6_8F09_4886_9CF7_906567A5B72D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPrjScrollWndDlg dialog

#include "ScrollWnd.h"
#include "TestDlg.h"

class CPrjScrollWndDlg : public CDialog
{
// Construction
public:
	CPrjScrollWndDlg(CWnd* pParent = NULL);	// standard constructor

	CSCrollWnd m_scroll;
	CTestDlg   m_dlg;
// Dialog Data
	//{{AFX_DATA(CPrjScrollWndDlg)
	enum { IDD = IDD_PRJSCROLLWND_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrjScrollWndDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL
	
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPrjScrollWndDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRJSCROLLWNDDLG_H__86CB50E6_8F09_4886_9CF7_906567A5B72D__INCLUDED_)
