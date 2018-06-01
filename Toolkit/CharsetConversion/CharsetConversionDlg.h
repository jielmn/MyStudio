// CharsetConversionDlg.h : ͷ�ļ�
//

#pragma once


// CCharsetConversionDlg �Ի���
class CCharsetConversionDlg : public CDialog
{
// ����
public:
	CCharsetConversionDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CHARSETCONVERSION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonGbkConvert();
    CString m_sGbkStatus;
    CString m_sUtf8Status;
    afx_msg void OnBnClickedButtonUtf8Convert();
    afx_msg void OnBnClickedButtonGbkSave();
    afx_msg void OnBnClickedButtonUtf8Save();
    afx_msg void OnBnClickedButtonGbkConvert2();
    afx_msg void OnBnClickedButtonUtf8Convert2();
};
