#ifndef __PROCESSING_BAR_CONTROL__
#define __PROCESSING_BAR_CONTROL__

typedef DWORD ( CALLBACK * run_callback )( LPVOID lpParam );

class CProcBarCtrl : public CStatic
{
public:
	enum SPEED
	{
		SPEED_LOW,
		SPEED_NORMAL,
		SPEED_HIGH
	};

	enum LIGHT_LEN
	{
		LIGHT_LEN0,
		LIGHT_LEN1,
		LIGHT_LEN2
	};

	CProcBarCtrl();
	~CProcBarCtrl();
	DWORD run();
	DWORD stop();
	DWORD run( run_callback pfunCallback, LPVOID lpParam );

	VOID SetSpeed( SPEED speed );
	VOID SetLightingLen( LIGHT_LEN len );

private:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd( CDC * );
	afx_msg LRESULT OnNotifyEnd( WPARAM, LPARAM );
	
	DECLARE_MESSAGE_MAP()

private:
	DWORD m_dwPos;
	DWORD m_dwSpeed;
	DWORD m_dwLen;
	
	CBrush m_brush;

	HANDLE m_hRun;
	HANDLE m_hCallback;

	BOOL   m_bRunFlag;

	LPVOID m_lpParam;
	run_callback m_runcb;

	DWORD GetBlockNum();
	DWORD GetRealSpeed();

	static DWORD WINAPI run_proc(LPVOID lpParam);
	static DWORD WINAPI runcb_proc(LPVOID lpParam );
	CRITICAL_SECTION m_cri;
};
#endif