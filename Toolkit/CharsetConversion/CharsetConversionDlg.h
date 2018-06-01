// CharsetConversionDlg.h : 头文件
//

#pragma once


// CCharsetConversionDlg 对话框
class CCharsetConversionDlg : public CDialog
{
// 构造
public:
	CCharsetConversionDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CHARSETCONVERSION_DIALOG };

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
    afx_msg void OnBnClickedButtonGbkConvert();
    CString m_sGbkStatus;
    CString m_sUtf8Status;
    afx_msg void OnBnClickedButtonUtf8Convert();
    afx_msg void OnBnClickedButtonGbkSave();
    afx_msg void OnBnClickedButtonUtf8Save();
    afx_msg void OnBnClickedButtonGbkConvert2();
    afx_msg void OnBnClickedButtonUtf8Convert2();
};
