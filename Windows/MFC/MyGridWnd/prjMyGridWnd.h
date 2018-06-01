// prjMyGridWnd.h : main header file for the PRJMYGRIDWND application
//

#if !defined(AFX_PRJMYGRIDWND_H__D94D0618_4ACD_49B2_9055_C78714F86149__INCLUDED_)
#define AFX_PRJMYGRIDWND_H__D94D0618_4ACD_49B2_9055_C78714F86149__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPrjMyGridWndApp:
// See prjMyGridWnd.cpp for the implementation of this class
//

class CPrjMyGridWndApp : public CWinApp
{
public:
	CPrjMyGridWndApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrjMyGridWndApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPrjMyGridWndApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRJMYGRIDWND_H__D94D0618_4ACD_49B2_9055_C78714F86149__INCLUDED_)
