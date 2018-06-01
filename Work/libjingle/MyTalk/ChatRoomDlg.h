#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CChatRoomDlg 对话框

class CChatRoomDlg : public CDialog
{
	DECLARE_DYNAMIC(CChatRoomDlg)

public:
	CChatRoomDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CChatRoomDlg();

// 对话框数据
	enum { IDD = IDD_CHATROOM };

    PTChatRoom         m_pChatRoom;
    LibJingleEx::TJid  m_tSelfJid;

    void UpdateParticipant( const LibJingleEx::PTJid ptJid, LibJingleEx::EPresence * pPres, 
                            LibJingleEx::EAffiliation * peAffi, BOOL bKick = FALSE );

    void UpdateRosterItem( LibJingleEx::PTRoomRosterItem  pItem, BOOL bAdded );

    DWORD  GetFirstRosterItemIndex();
    DWORD  GetRosterItemIndex( LibJingleEx::PTJid  ptJid );

    void AddMessage( const LibJingleEx::PTJid ptJid, const char * szMsg );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    BOOL OnInitDialog();
    BOOL m_bQuit;
    CMenu          m_menu;
    BOOL PreTranslateMessage( MSG * pMsg );

    void OnSize(UINT nType, int cx, int cy);

    const float   m_fWidthMsgPercent;

	DECLARE_MESSAGE_MAP()
public:
    CListBox m_lstMsg;
    CListCtrl m_lstParticipant;
    CString m_sMessage;
    afx_msg void OnBnClickedBtnSend();
    afx_msg void OnInvite();
    afx_msg void OnQuitChatRoom();
    afx_msg void OnDestroyChatRoom();
    afx_msg void OnKick();
    afx_msg void OnScreen();
    afx_msg void OnUnScreen();
    afx_msg void OnTest();
    void OnClose();
    CStatic m_staticParticipant;
    CStatic m_staticSendMsg;
    CEdit m_editSendMsg;
    CButton m_btnSend;
};
