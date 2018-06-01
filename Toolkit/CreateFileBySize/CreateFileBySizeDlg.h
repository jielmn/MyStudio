// CreateFileBySizeDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CCreateFileBySizeDlg �Ի���
class CCreateFileBySizeDlg : public CDialog
{
// ����
public:
	CCreateFileBySizeDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CREATEFILEBYSIZE_DIALOG };

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
    afx_msg void OnBnClickedButtonGenerate();
    CButton m_rdRand;
    CButton m_rdDuplicate;
    CString m_duplicate;
    DWORD m_dwSize;
    CComboBox m_cmbUnit;
    CButton m_btnBrowse;
    CButton m_btnGenerate;
    CString m_filename;
    afx_msg void OnBnClickedButtonBrowse();
};
