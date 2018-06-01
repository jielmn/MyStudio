// HexStream2FileDlg.h : 头文件
//

#pragma once


// CHexStream2FileDlg 对话框
class CHexStream2FileDlg : public CDialog
{
// 构造
public:
	CHexStream2FileDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HEXSTREAM2FILE_DIALOG };

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
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    CString m_HexStream;
    CString m_Delimiter;
    afx_msg void OnBnClickedButtonSave();
};
