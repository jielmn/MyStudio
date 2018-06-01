#pragma once
#include "afxwin.h"


// CVCardSetDlg 对话框

class CVCardSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CVCardSetDlg)

public:
	CVCardSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CVCardSetDlg();

// 对话框数据
	enum { IDD = IDD_VCARD_SET };

    int                    m_nIndex;
    LibJingleEx::TJid      m_tJid;
    LibJingleEx::TVCard    m_tVCard;
    BOOL                   m_bSelf;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    BOOL  OnInitDialog();

    afx_msg void OnPaint();

    IPicture *   m_pPicture;

	DECLARE_MESSAGE_MAP()
public:
    CString m_sName;
    CComboBox m_cmbSysPicIndex;
    afx_msg void OnBnClickedOk();
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedButtonBrownPic();
    afx_msg void OnCbnSelchangeComboSysPicIndex();
};
