#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CProgressDlg �Ի���

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProgressDlg();

// �Ի�������
	enum { IDD = IDD_PROGRESS };
    
    LibJingleEx::HSession    m_hSession;
    LibJingleEx::FileShareManifest m_manifest;
    BOOL                           m_bSender;

    int                            m_nIndex;

    BOOL                           m_bStoped;

    void UpdateProgress( DWORD dwProgress, const char * szCurFileName, DWORD dwTransferSize, DWORD dwRate );

    void OnComplete();
    void OnTransfer();
    void OnFileCancel();
    void OnFailure();

    CToolTipCtrl   m_tip;

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
    BOOL OnInitDialog();

    CString FormatSize( DWORD dwSize );

    BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnTimer( UINT_PTR nIDEvent );

	DECLARE_MESSAGE_MAP()
public:
    CProgressCtrl m_ctrProgress;
    CButton m_btnCancel;
    afx_msg void OnBnClickedBtnCancel();
    CButton m_btnAccept;
    CString m_strCurrent;
    CString m_strNum;
    afx_msg void OnBnClickedBtnAccept();
    afx_msg void OnBnClickedBtnDecline();
    CButton m_btnDecline;
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
