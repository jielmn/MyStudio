// prjMyGridWndDlg.h : header file
//
//{{AFX_INCLUDES()
#include "richtext.h"
//}}AFX_INCLUDES

#if !defined(AFX_PRJMYGRIDWNDDLG_H__C648C517_E6B5_420D_8708_7579F730EBD0__INCLUDED_)
#define AFX_PRJMYGRIDWNDDLG_H__C648C517_E6B5_420D_8708_7579F730EBD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPrjMyGridWndDlg dialog

class CPrjMyGridWndDlg : public CDialog
{
// Construction
public:
	CPrjMyGridWndDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPrjMyGridWndDlg)
	enum { IDD = IDD_PRJMYGRIDWND_DIALOG };
	CRichText	m_rich;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrjMyGridWndDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPrjMyGridWndDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRJMYGRIDWNDDLG_H__C648C517_E6B5_420D_8708_7579F730EBD0__INCLUDED_)
