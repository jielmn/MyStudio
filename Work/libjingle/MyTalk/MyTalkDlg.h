// MyTalkDlg.h : 头文件
//

#pragma once

#include "AccountDlg.h"
#include "afxwin.h"
#include "afxcmn.h"

// CMyTalkDlg 对话框
class CMyTalkDlg : public CDialog
{
// 构造
public:
	CMyTalkDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MYTALK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

    CAccountDlg     m_AccountDlg;
    CMenu           m_SubMenu;

    CMenu           m_SperatorMenu;
    CMenu           m_AccountMenu[MAX_ACCOUNT_NUMBER];

    typedef std::vector<CChatDlg *> vChatDlg;
    vChatDlg m_vChatDlgs[MAX_ACCOUNT_NUMBER];

    typedef std::vector<CPictureDlg *> vPictureDlg;
    vPictureDlg m_vPictureDlgs[MAX_ACCOUNT_NUMBER];
    

    typedef std::vector<CFileTranferDlg *> vFileTranferDlg;
    vFileTranferDlg    m_vFileTranferDlgs[MAX_ACCOUNT_NUMBER];
      

    COnSubscribeDlg    m_OnSubscribeDlg;

    vOnSubscribeMsg    m_vOnSubscribeMsg;

    BOOL OnCommand( WPARAM wParam, LPARAM lParam );
    void OnTest();
    void OnTestMisc();

    void OnStartAccount( int nIndex );
    void OnStopAccount(  int nIndex );


    CToolTipCtrl   m_tipRoster;
    void OnTvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult);

    BOOL PreTranslateMessage(MSG* pMsg) ;


    int   m_nStarting[MAX_ACCOUNT_NUMBER];


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnAccount();
    afx_msg void OnXmppConsole();
    afx_msg void OnAddFriend();
    afx_msg void OnDelFriend();
    afx_msg void OnDebugInfo();
    afx_msg void OnSetNick();
    afx_msg void OnSendSubscribe();
    afx_msg void OnSendUnsubscribe();
    afx_msg void OnSendUnsubscribed();
    afx_msg void OnCreateChatRoom();
    afx_msg void OnFileTransfer();

    afx_msg void OnVCardSet();
    afx_msg void OnVCardGet();
    afx_msg void OnSetPassword();
    afx_msg void OnSendPicture();

    afx_msg void OnActivate( UINT nState, CWnd* pWndOther,BOOL bMinimized );

    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);

    afx_msg LRESULT OnState( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnRoster( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnChatMsg( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnPresence( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnRosterAdd( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnRosterDel( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnRosterChange( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnSubscribe(WPARAM wParam, LPARAM lParam );

    afx_msg LRESULT OnSubscribeClose(WPARAM wParam, LPARAM lParam );

    afx_msg LRESULT OnConferenceServer(WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnCreateChatRoomSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnInvited(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnChatRoomPresence(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnChatRoomMessage(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnChatRoomDestroy(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnChatRoomRosterAdd(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnChatRoomRosterDel(WPARAM wParam, LPARAM lParam);


    afx_msg LRESULT OnFileOffer(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnFileProgress(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnFileComplete(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnFileCancel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnFileFailure(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnFileTransfer(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnVCardInfo(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnPictureMsg(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnNotifyPersistentRooms(WPARAM wParam, LPARAM lParam);

    afx_msg LRESULT OnNotifyCharState(WPARAM wParam, LPARAM lParam);

    void OnTimer(UINT_PTR nIDEvent);


	DECLARE_MESSAGE_MAP()
public:
   
    CComboBox m_comboStatus;
    CTreeCtrl m_treeRoster;

    CImageList m_ImgList;
    afx_msg void OnCbnSelchangeComboStatus();
    afx_msg void OnNMDblclkTreeRoster(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMRClickTreeRoster(NMHDR *pNMHDR, LRESULT *pResult);
};
