#pragma once
#include "afxwin.h"


// CVCardSetDlg �Ի���

class CVCardSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CVCardSetDlg)

public:
	CVCardSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CVCardSetDlg();

// �Ի�������
	enum { IDD = IDD_VCARD_SET };

    int                    m_nIndex;
    LibJingleEx::TJid      m_tJid;
    LibJingleEx::TVCard    m_tVCard;
    BOOL                   m_bSelf;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
