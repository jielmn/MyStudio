// prjProcBarCtrl.h : main header file for the PRJPROCBARCTRL application
//

#if !defined(AFX_PRJPROCBARCTRL_H__6A00A136_496D_46C5_BEE6_4312BEA3C94C__INCLUDED_)
#define AFX_PRJPROCBARCTRL_H__6A00A136_496D_46C5_BEE6_4312BEA3C94C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPrjProcBarCtrlApp:
// See prjProcBarCtrl.cpp for the implementation of this class
//

class CPrjProcBarCtrlApp : public CWinApp
{
public:
	CPrjProcBarCtrlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrjProcBarCtrlApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPrjProcBarCtrlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRJPROCBARCTRL_H__6A00A136_496D_46C5_BEE6_4312BEA3C94C__INCLUDED_)
