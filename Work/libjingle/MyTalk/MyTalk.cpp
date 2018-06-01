// MyTalk.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
// #include "vld.h"
#include "MyTalk.h"
#include "ChatDlg.h"
#include "PictureDlg.h"
#include "OnSubscribeDlg.h"
#include "ChatRoomDlg.h"
#include "ProgressDlg.h"
#include "EmptyDlg.h"
#include "FileTranferDlg.h"
#include "MyTalkDlg.h"
#include "XmppDlg.h"

#pragma warning( disable: 4996 )

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CXmppDlg * g_pXmppDlg = 0;

CMyTalkDlg * g_pMainDlg = 0;

// CMyTalkApp

BEGIN_MESSAGE_MAP(CMyTalkApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMyTalkApp ����

CMyTalkApp::CMyTalkApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CMyTalkApp ����

CMyTalkApp theApp;


// CMyTalkApp ��ʼ��

BOOL CMyTalkApp::InitInstance()
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

    int  i;
    for ( i = 0; i < MAX_ACCOUNT_NUMBER; i++ )
    {
        g_bConferenceAvailable[i] = FALSE;
    }




    char path[512] = {0};
    GetModuleFileName( 0, path, 512 );

    char * p = strrchr( path, '\\' );
    if ( p )
    {
        *p = '\0';
    }
    std::string sPath = path;
    sPath += '\\';
    sPath += CONFIG_FILE;

    MyConfigFile cfg_file;

    try
    {
        cfg_file.LoadFile( sPath );

        std::string s = cfg_file["log flag"];
        if ( "true" == s )
        {
            LibJingleEx::SetDebugInfoHandler( OnXmppMessage, OnLogInfo );
        }
    }
    catch ( MyConfigFile::file_not_found_exception e )
    {
        
    }
    
    

	//CMyTalkDlg dlg;
	//m_pMainWnd = &dlg;

    g_fp = fopen("logabc.txt", "wb+");
    InitializeCriticalSection( &g_tLock );

    g_pMainDlg = new CMyTalkDlg;
    m_pMainWnd = g_pMainDlg;
	INT_PTR nResponse = g_pMainDlg->DoModal();
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

    delete g_pMainDlg;

    fclose( g_fp );
	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�

    LibJingleEx::Deinit();
	return FALSE;
}