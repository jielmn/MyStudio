// CreateFileBySize.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCreateFileBySizeApp:
// �йش����ʵ�֣������ CreateFileBySize.cpp
//

class CCreateFileBySizeApp : public CWinApp
{
public:
	CCreateFileBySizeApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCreateFileBySizeApp theApp;