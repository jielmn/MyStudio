// HexStream2FileDlg.h : ͷ�ļ�
//

#pragma once


// CHexStream2FileDlg �Ի���
class CHexStream2FileDlg : public CDialog
{
// ����
public:
	CHexStream2FileDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_HEXSTREAM2FILE_DIALOG };

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
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    CString m_HexStream;
    CString m_Delimiter;
    afx_msg void OnBnClickedButtonSave();
};
