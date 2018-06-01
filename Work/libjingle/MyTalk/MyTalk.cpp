// MyTalk.cpp : 定义应用程序的类行为。
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


// CMyTalkApp 构造

CMyTalkApp::CMyTalkApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMyTalkApp 对象

CMyTalkApp theApp;


// CMyTalkApp 初始化

BOOL CMyTalkApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

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
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

    delete g_pMainDlg;

    fclose( g_fp );
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。

    LibJingleEx::Deinit();
	return FALSE;
}