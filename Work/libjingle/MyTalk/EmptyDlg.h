#pragma once


// CEmptyDlg �Ի���

class CEmptyDlg : public CDialog
{
	DECLARE_DYNAMIC(CEmptyDlg)

public:
	CEmptyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CEmptyDlg();

// �Ի�������
	enum { IDD = IDD_EMPTY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
