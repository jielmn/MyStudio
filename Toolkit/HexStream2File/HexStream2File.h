// HexStream2File.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CHexStream2FileApp:
// �йش����ʵ�֣������ HexStream2File.cpp
//

class CHexStream2FileApp : public CWinApp
{
public:
	CHexStream2FileApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CHexStream2FileApp theApp;