#include "stdafx.h"
#include "ProcBarCtrl.h"

#define  BLOCK_INTERVAL  1
#define  BLOCK_WIDTH     8
#define  BRUSH_COLOR RGB(0, 255, 0)
#define  SLEEP_TIME      200

#define UM_NOTIFY_END    ( WM_USER + 1010 )

BEGIN_MESSAGE_MAP(CProcBarCtrl, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE( UM_NOTIFY_END, OnNotifyEnd )
END_MESSAGE_MAP()

CProcBarCtrl::CProcBarCtrl()
{
	m_dwPos   = 1;
	m_dwSpeed = SPEED_NORMAL;
	m_dwLen   = LIGHT_LEN1;

	m_brush.CreateSolidBrush( BRUSH_COLOR );

	m_hRun = FALSE;
	m_bRunFlag = FALSE;

	m_hCallback = NULL;
	m_runcb = NULL;
	m_lpParam = NULL;
	
	InitializeCriticalSection( &m_cri );
}

CProcBarCtrl::~CProcBarCtrl()
{
	DeleteCriticalSection( &m_cri );
}

DWORD CProcBarCtrl::GetBlockNum()
{
	switch( m_dwLen )
	{
		case LIGHT_LEN0:
			return 5;
			break;
		case LIGHT_LEN1:
			return 8;
			break;
		case LIGHT_LEN2:
			return 12;
			break;
		default:
			ASSERT(FALSE);
			break;
	}

	return -1;
}

DWORD CProcBarCtrl::GetRealSpeed()
{
	switch( m_dwSpeed )
	{
		case SPEED_LOW:
			return 5;
			break;
		case SPEED_NORMAL:
			return 8;
			break;
		case SPEED_HIGH:
			return 15;
			break;
		default:
			ASSERT(FALSE);
			break;
	}

	return -1;
}

VOID CProcBarCtrl::SetSpeed( SPEED speed )
{
	if ( speed > SPEED_HIGH )
	{
		return;
	}

	m_dwSpeed = speed;
}

VOID CProcBarCtrl::SetLightingLen( LIGHT_LEN len )
{
	if ( len > LIGHT_LEN2 )
	{
		return;
	}

	m_dwLen = len;
}

BOOL CProcBarCtrl::OnEraseBkgnd( CDC * )
{
	return TRUE;
}

void CProcBarCtrl::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	this->GetWindowRect(rect);

	dc.Rectangle( 0, 0, rect.Width(), rect.Height() );
	
	DWORD dwNum = GetBlockNum();
	DWORD dwSpeed = GetRealSpeed();

	DWORD dwBlockHeight = rect.Height() - 1;

	///////////////////////整个控件都不足以画全部的process条
	if ( ( BLOCK_WIDTH + BLOCK_INTERVAL ) * dwNum >= (DWORD)rect.Width() - 1 )
	{
		return;
	}

	DWORD i;

	EnterCriticalSection( &m_cri );
	for( i = 0; i < dwNum; i++ )
	{
		CRect r( m_dwPos + i * ( BLOCK_WIDTH + BLOCK_INTERVAL ), 
			     1, 
				 m_dwPos + i * ( BLOCK_WIDTH + BLOCK_INTERVAL ) + BLOCK_WIDTH,
				 dwBlockHeight );

		if ( r.left < rect.Width() )
		{
			///////////////////process block完全在控件左边
			if ( r.right < rect.Width() )
			{
				dc.FillRect( r, &m_brush );
			}
			///////////////////process block要分为两部分来画
			else
			{
				/////////////////////////被分割出来的部分r2
				CRect r_divided( 1, 1, r.right - ( rect.Width() - 2 ), r.bottom );
				r.right = rect.Width() - 1;

				dc.FillRect( r, &m_brush );
				dc.FillRect( r_divided, &m_brush );
			}
		}
		////////////////////// process block 完全在控件右边
		else
		{
			r.left  -= rect.Width() - 2;
			r.right -= rect.Width() - 2;

			dc.FillRect( r, &m_brush );
		}
	}
	LeaveCriticalSection( &m_cri );
}

DWORD WINAPI CProcBarCtrl::run_proc(LPVOID lpParam)
{
	CProcBarCtrl * pCtrl = (CProcBarCtrl *)lpParam;
	while( pCtrl->m_bRunFlag )
	{
		EnterCriticalSection( &pCtrl->m_cri );
		CRect rect;
		pCtrl->GetWindowRect( rect );
		pCtrl->m_dwPos += pCtrl->GetRealSpeed();
		if ( pCtrl->m_dwPos > (DWORD)rect.Width() )
		{
			pCtrl->m_dwPos = 1;
		}
		LeaveCriticalSection( &pCtrl->m_cri );
		pCtrl->Invalidate();
		Sleep( SLEEP_TIME );
	}
	return 0;
}

DWORD CProcBarCtrl::run()
{
	if ( NULL != m_hRun )
	{
		return 1;
	}

	ASSERT ( FALSE == m_bRunFlag);
	m_bRunFlag = TRUE;
	m_hRun = CreateThread( 0, 0, run_proc, this, 0, 0);

	return 0;
}

DWORD CProcBarCtrl::stop()
{
	if ( NULL == m_hRun )
	{
		return 1;
	}

	if ( NULL != m_hCallback )
	{
		return 2;
	}

	m_bRunFlag = FALSE;
	WaitForSingleObject( m_hRun, INFINITE );
	m_hRun = NULL;
	return 0;
}

DWORD WINAPI CProcBarCtrl::runcb_proc(LPVOID lpParam )
{
	CProcBarCtrl * pCtrl = (CProcBarCtrl *)lpParam;
	pCtrl->m_runcb(pCtrl->m_lpParam);

	pCtrl->m_bRunFlag = FALSE;
	WaitForSingleObject( pCtrl->m_hRun, INFINITE );
	pCtrl->m_hRun = NULL;

	pCtrl->PostMessage( UM_NOTIFY_END, 0, 0 );
	return 0;
}

LRESULT CProcBarCtrl::OnNotifyEnd( WPARAM, LPARAM )
{
	WaitForSingleObject( m_hCallback, INFINITE );
	m_hCallback = NULL;
	m_runcb     = NULL;
	m_lpParam   = NULL;
	return 0;
}
DWORD CProcBarCtrl::run( run_callback pfunCallback, LPVOID lpParam )
{
	if ( NULL != m_hRun )
	{
		return 1;
	}

	run();
	m_runcb = pfunCallback;
	m_lpParam = lpParam;
	m_hCallback = CreateThread( 0, 0, runcb_proc, this, 0, 0 );

	return 0;
}