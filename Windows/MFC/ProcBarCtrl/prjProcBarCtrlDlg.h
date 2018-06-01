// prjProcBarCtrlDlg.h : header file
//

#if !defined(AFX_PRJPROCBARCTRLDLG_H__847CAC5C_C1B3_4D25_A260_0B0933BD0BC5__INCLUDED_)
#define AFX_PRJPROCBARCTRLDLG_H__847CAC5C_C1B3_4D25_A260_0B0933BD0BC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProcBarCtrl.h"
/////////////////////////////////////////////////////////////////////////////
// CPrjProcBarCtrlDlg dialog

class CPrjProcBarCtrlDlg : public CDialog
{
// Construction
public:
	CPrjProcBarCtrlDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPrjProcBarCtrlDlg)
	enum { IDD = IDD_PRJPROCBARCTRL_DIALOG };
	CProcBarCtrl	m_procbar;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrjProcBarCtrlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPrjProcBarCtrlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRJPROCBARCTRLDLG_H__847CAC5C_C1B3_4D25_A260_0B0933BD0BC5__INCLUDED_)
