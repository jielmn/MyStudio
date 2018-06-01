#pragma once
#include "afxwin.h"


// COnSubscribeDlg 对话框

class COnSubscribeDlg : public CDialog
{
	DECLARE_DYNAMIC(COnSubscribeDlg)

public:
	COnSubscribeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COnSubscribeDlg();

    LibJingleEx::TJid   m_tJid;
    LibJingleEx::HANDLE m_hApp;

    void UpdateMsg();

// 对话框数据
	enum { IDD = IDD_ON_SUBSCRIBE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
    CButton m_btnAgree;
    CButton m_btnReject;
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnBnClickedBtnAgree();

    afx_msg void OnPaint();
    CButton m_btnClose;
    CString m_sOnScribeMsg;
    afx_msg void OnBnClickedBtnClose();
    afx_msg void OnBnClickedBtnReject();
};
