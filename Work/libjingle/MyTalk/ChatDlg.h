#pragma once
#include "afxwin.h"


// CChatDlg �Ի���

class CChatDlg : public CDialog
{
	DECLARE_DYNAMIC(CChatDlg)

public:
	CChatDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CChatDlg();

// �Ի�������
	enum { IDD = IDD_CHAT };

    LibJingleEx::TJid   m_tJid;
    LibJingleEx::ECharStatus  m_eCharState;
    DWORD                     m_dwComposingTick;
    CString             m_sNick;

    int                 m_nIndex;

    void OnChatMessage( const char * szMsg );
    void OnChatState( LibJingleEx::ECharStatus e );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    BOOL OnInitDialog();
    BOOL PreTranslateMessage( MSG * pMsg );
    void OnSize(UINT nType, int cx, int cy);
    void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
public:
    CListBox m_lstMsg;
    CEdit m_editMsg;
    afx_msg void OnBnClickedBtnSend();
    CButton m_btnSend;
    CStatic m_CharState;
};
