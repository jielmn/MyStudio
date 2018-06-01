// TestMergeFolder.h : main header file for the TESTMERGEFOLDER application
//

#if !defined(AFX_TESTMERGEFOLDER_H__40E00357_B018_4F4F_8A39_DA495D4CDD3C__INCLUDED_)
#define AFX_TESTMERGEFOLDER_H__40E00357_B018_4F4F_8A39_DA495D4CDD3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestMergeFolderApp:
// See TestMergeFolder.cpp for the implementation of this class
//

class CTestMergeFolderApp : public CWinApp
{
public:
	CTestMergeFolderApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestMergeFolderApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestMergeFolderApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTMERGEFOLDER_H__40E00357_B018_4F4F_8A39_DA495D4CDD3C__INCLUDED_)
