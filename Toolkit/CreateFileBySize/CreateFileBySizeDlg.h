// CreateFileBySizeDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CCreateFileBySizeDlg 对话框
class CCreateFileBySizeDlg : public CDialog
{
// 构造
public:
	CCreateFileBySizeDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CREATEFILEBYSIZE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
