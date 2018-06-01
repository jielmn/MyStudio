// prjScrollWnd.h : main header file for the PRJSCROLLWND application
//

#if !defined(AFX_PRJSCROLLWND_H__D618ACE8_8EA2_4495_8AE0_7D3E9636DFF4__INCLUDED_)
#define AFX_PRJSCROLLWND_H__D618ACE8_8EA2_4495_8AE0_7D3E9636DFF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPrjScrollWndApp:
// See prjScrollWnd.cpp for the implementation of this class
//

class CPrjScrollWndApp : public CWinApp
{
public:
	CPrjScrollWndApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrjScrollWndApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPrjScrollWndApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRJSCROLLWND_H__D618ACE8_8EA2_4495_8AE0_7D3E9636DFF4__INCLUDED_)
