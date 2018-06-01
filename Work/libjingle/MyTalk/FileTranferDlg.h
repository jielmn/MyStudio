#pragma once
#include "afxwin.h"
#include "ScrollWnd.h"

// CFileTranferDlg 对话框

class CFileTranferDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileTranferDlg)

public:
	CFileTranferDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFileTranferDlg();

// 对话框数据
	enum { IDD = IDD_FILE_TRANSFER };

    void Add( LibJingleEx::HSession, LibJingleEx::PFileShareManifest pManifest, BOOL bSender );
    void Del( LibJingleEx::HSession );

    CProgressDlg * IsContain( LibJingleEx::HSession hSession, BOOL bStoped );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    BOOL OnInitDialog();

    typedef std::vector<CProgressDlg *>  vProgressDlg;
    vProgressDlg   m_vProgressDlgs;

    

	DECLARE_MESSAGE_MAP()
public:
    CListBox m_lstSend;

    LibJingleEx::TJid              m_tSelfJid;
    LibJingleEx::TJid              m_tPeerJid;

    int                            m_nIndex;

    afx_msg void OnBnClickedBtnAddFile();
    afx_msg void OnBnClickedBtnAddFolder();
    afx_msg void OnBnClickedBtnCancel();
    afx_msg void OnBnClickedBtnSend();

    afx_msg LRESULT OnCloseProgress( WPARAM wParam, LPARAM lParam );

    afx_msg void OnDestroy();

    CSCrollWnd                      m_ScrollWnd;
    CEmptyDlg                       m_EmptyDlg;
    afx_msg void OnBnClickedBtnDelete();
};
