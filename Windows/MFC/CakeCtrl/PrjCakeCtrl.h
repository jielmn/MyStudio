// PrjCakeCtrl.h : main header file for the PRJCAKECTRL application
//

#if !defined(AFX_PRJCAKECTRL_H__191F0029_0DF0_43F7_A479_5FFAB6541A6B__INCLUDED_)
#define AFX_PRJCAKECTRL_H__191F0029_0DF0_43F7_A479_5FFAB6541A6B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPrjCakeCtrlApp:
// See PrjCakeCtrl.cpp for the implementation of this class
//

class CPrjCakeCtrlApp : public CWinApp
{
public:
	CPrjCakeCtrlApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrjCakeCtrlApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPrjCakeCtrlApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRJCAKECTRL_H__191F0029_0DF0_43F7_A479_5FFAB6541A6B__INCLUDED_)
