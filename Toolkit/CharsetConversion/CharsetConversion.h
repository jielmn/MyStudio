// CharsetConversion.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCharsetConversionApp:
// �йش����ʵ�֣������ CharsetConversion.cpp
//

class CCharsetConversionApp : public CWinApp
{
public:
	CCharsetConversionApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCharsetConversionApp theApp;