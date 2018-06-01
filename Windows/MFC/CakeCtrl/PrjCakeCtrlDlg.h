// PrjCakeCtrlDlg.h : header file
//

#if !defined(AFX_PRJCAKECTRLDLG_H__BFC499BE_C8F5_4E4A_B26A_A58F817488E2__INCLUDED_)
#define AFX_PRJCAKECTRLDLG_H__BFC499BE_C8F5_4E4A_B26A_A58F817488E2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPrjCakeCtrlDlg dialog

#include "CakeChartCtrl.h"

class CPrjCakeCtrlDlg : public CDialog
{
// Construction
public:
	CPrjCakeCtrlDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPrjCakeCtrlDlg)
	enum { IDD = IDD_PRJCAKECTRL_DIALOG };
	CCakeChartCtrl	m_cake;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrjCakeCtrlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPrjCakeCtrlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRJCAKECTRLDLG_H__BFC499BE_C8F5_4E4A_B26A_A58F817488E2__INCLUDED_)
