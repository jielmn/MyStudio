// HttpTestDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CHttpTestDlg 对话框
class CHttpTestDlg : public CDialog
{
// 构造
public:
	CHttpTestDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HTTPTEST_DIALOG };

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
    CEdit m_edit1;
    CListBox m_list1;
    afx_msg void OnBnClickedButton1();
    LRESULT OnHttpResponse(WPARAM wParam, LPARAM lParam);

    CButton m_opt1;
    CButton m_opt2;
    CButton m_opt3;
    CButton m_opt4;
    CButton m_opt5;
    CButton m_opt6;
    CButton m_opt7;
    CButton m_opt8;
    CButton m_opt9;
    CEdit m_proxy;
    CEdit m_proxy_port;
    CEdit m_proxy2;
    CEdit m_proxy_port2;
    CEdit m_proxy3;
    CEdit m_proxy_port3;
    CEdit m_edit2;
    CEdit m_edit3;
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnBnClickedButton4();
};
