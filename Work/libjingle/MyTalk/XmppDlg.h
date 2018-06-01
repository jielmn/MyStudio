#pragma once
#include "afxwin.h"


// CXmppDlg �Ի���

class CXmppDlg : public CDialog
{
	DECLARE_DYNAMIC(CXmppDlg)

public:
	CXmppDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CXmppDlg();

// �Ի�������
	enum { IDD = IDD_XMPP_MSG };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    BOOL OnInitDialog();
    HBRUSH   OnCtlColor(CDC*   pDC,   CWnd*   pWnd,   UINT   nCtlColor);
    void OnDestroy();
    void OnClose();
    void OnTimer(UINT_PTR nIDEvent);

    void OnOK();

    BOOL OnEraseBkgnd(CDC* pDC);
    void OnPaint();

    afx_msg LRESULT OnXmppMsg( WPARAM wParam, LPARAM lParam );
    afx_msg LRESULT OnDebugInfo( WPARAM wParam, LPARAM lParam );
    void OnSize(UINT nType, int cx, int cy);

    CString  m_content;
    BOOL     m_bChanged;

	DECLARE_MESSAGE_MAP()
public:
    CEdit m_editMsg;
    CEdit m_editCustome;
};
