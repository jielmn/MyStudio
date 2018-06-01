// HttpTest.cpp : ����Ӧ�ó��������Ϊ��
//
#include "vld.h"
#include "stdafx.h"
#include "HttpTest.h"
#include "HttpTestDlg.h"
#include "httpstack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




CHttpTestDlg * g_pDlg = 0;

CHttp * g_http = CHttp::GetInstance();

void _http_callback( int nError, DWORD dwCode, const char * szData, DWORD dwDataLen, const char * szHeader, DWORD dwHeadLen, void * context )
{
    char * pData = 0;
    pData = new char[dwDataLen + 1];
    if ( szData )
    {
        memcpy( pData, szData, dwDataLen );
    }
    pData[dwDataLen] = '\0';

    ::PostMessage( g_pDlg->m_hWnd, UM_HTTP_RESPONSE, (WPARAM)dwCode, (LPARAM)pData );
}


// CHttpTestApp

BEGIN_MESSAGE_MAP(CHttpTestApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CHttpTestApp ����

CHttpTestApp::CHttpTestApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CHttpTestApp ����

CHttpTestApp theApp;


// CHttpTestApp ��ʼ��

BOOL CHttpTestApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

    ::InitHttpStack( _http_callback );

    g_pDlg = new CHttpTestDlg;
	CHttpTestDlg & dlg = *g_pDlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}
