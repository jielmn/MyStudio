#include "stdafx.h"
#include "CakeChartCtrl.h"
#include "stdarg.h"
#include <math.h>
#include <stdarg.h>

#define PI 3.14159262

int GetY1(int iX, LPVOID pParam)
{
	RECT rect = *(RECT *)pParam;

	DWORD dwA = ( rect.right -  rect.left ) / 2;
	DWORD dwB = ( rect.bottom - rect.top  ) / 2;

	return (int)( sqrt( 1.0 - (double)(iX * iX) / ( dwA * dwA ) ) * dwB );
}

int GetY2( int iX, LPVOID pParam )
{
	//RECT rect     = *(RECT *)pParam;
	INT  iVoffset = *(INT *)( (CHAR *)pParam + sizeof( RECT ) );

	return GetY1(iX, pParam) + iVoffset;
	
//	DWORD dwA = ( rect.right -  rect.left ) / 2;
//	DWORD dwB = ( rect.bottom - rect.top  ) / 2;
//
//	return (int)( sqrt( 1.0 - (double)(iX * iX) / ( dwA * dwA ) ) * dwB + iVoffset );
}

int GetX( DWORD dwPercentCnt, double a, double b )
{
	double angle_a = 2 * PI * ( dwPercentCnt / 100.0 );
	double ratio = b / a;
	
	double k = tan( angle_a) * ratio;
	if ( k >= 0 )
	{
		return (int)( sqrt( 1 / ( k * k * a * a + b * b) ) * a * b );
	}
	else
	{
		return -(int)( sqrt( 1 / ( k * k * a * a + b * b) ) * a * b );
	}
}

CCakeChartCtrl::CCakeChartCtrl()
{
	m_dwCnt = 0;
	m_pdwPercent = NULL;

	m_dwColors[0] = RGB( 0,   0,   255 );
	m_dwColors[1] = RGB( 255,   0, 255 );
	m_dwColors[2] = RGB( 255,   0,   0 );
	m_dwColors[3] = RGB( 255, 255, 0 );
	m_dwColors[4] = RGB( 0, 255,   255 );
	m_dwColors[5] = RGB( 200, 200, 50 );
	m_dwColors[6] = RGB( 100, 200, 200 );
	m_dwColors[7] = RGB( 200, 200, 200 );
}

CCakeChartCtrl::~CCakeChartCtrl()
{
	if ( NULL != m_pdwPercent )
	{
		delete[] m_pdwPercent;
		m_pdwPercent = NULL;
	}
}

BEGIN_MESSAGE_MAP(CCakeChartCtrl, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

VOID CCakeChartCtrl::SetPercent(DWORD dwCnt, ...)
{
	if ( dwCnt >= 8 )
	{
		return;
	}

	m_dwCnt = dwCnt + 1;
	m_pdwPercent = new DWORD[m_dwCnt];

	va_list ap;
	va_start( ap, dwCnt );
	DWORD dwPercentCnt = 0;
	for ( DWORD i = 0; i < dwCnt; i++ )
	{
		DWORD dwPercent = va_arg(ap, DWORD);
		m_pdwPercent[i] = dwPercent;
		dwPercentCnt += dwPercent;
	}
	m_pdwPercent[i] = 100 - dwPercentCnt;
	va_end(ap);
}

VOID CCakeChartCtrl::OnPaint()
{
	CPaintDC dc(this);
	CRect rcClient;
	GetClientRect( rcClient );
	dc.SetViewportOrg( rcClient.Width() / 2, rcClient.Height() / 2 );
	
	//dc.Ellipse( - rcClient.Width() / 2, - rcClient.Height() / 2, rcClient.Width() / 2, rcClient.Height() / 2 );
	CRect rcSurface( - rcClient.Width() / 2, - rcClient.Height() * 2 / 5, rcClient.Width() / 2, rcClient.Height() * 2 / 5 );
	//dc.Ellipse( rcSurface );

	CRect r1 = rcSurface;
	INT iVOffset = rcClient.Height() / 10;
	r1.top += iVOffset;
	r1.bottom += iVOffset;

	//dc.Pie( r1, CPoint(-0xFFFF, iVOffset), CPoint(0xFFFF, iVOffset)  );
	//dc.Ellipse( rcSurface );
	dc.MoveTo( CPoint(-rcClient.Width() / 2, 0) );
	dc.LineTo( CPoint(-rcClient.Width() / 2, iVOffset) );
	dc.MoveTo( CPoint( rcClient.Width() / 2, 0) );
	dc.LineTo( CPoint( rcClient.Width() / 2, iVOffset) );
	
	CHAR * pParam = new CHAR[sizeof(RECT) + sizeof(int)];
	*(RECT *)pParam = rcSurface;
	*(int *)((RECT *)pParam + 1) = iVOffset;


	CPoint ptStart( 0, 0 );
	CPoint ptEnd( -0xFFFF, 0 );
	DWORD  dwPercentCnt = 0;

	int iStartX = rcClient.Width() / 2;
	int iStartY = 0;

	for ( DWORD i = 0; i < m_dwCnt; i++ )
	{
		CBrush  brush(m_dwColors[i]);	
		HGDIOBJ hOld = dc.SelectObject( brush );

		dwPercentCnt += m_pdwPercent[i];

		double angle_a = 2 * PI * ( dwPercentCnt / 100.0 );
		double ratio = (double) rcSurface.Height() / rcSurface.Width();

		double angle_b = atan( tan( angle_a) * ratio );

		if ( dwPercentCnt > 25 && dwPercentCnt <=50 )
		{
			angle_b += PI;
		}
		else if ( dwPercentCnt > 50 && dwPercentCnt <= 75 )
		{
			angle_b += PI;
		}
		else if ( dwPercentCnt > 75 )
		{
			angle_b += 2 * PI;
		}

		ptStart.x = (long)(-0xFFFF * cos( angle_b ));
		ptStart.y = (long)(-0xFFFF * sin( angle_b ));

		if ( m_pdwPercent[i] > 0 )
		{
			dc.Pie( rcSurface, ptStart, ptEnd );
		}
		
		ptEnd = ptStart;
		
		if ( dwPercentCnt > 50 )
		{
			POINT * pPts1 = NULL;
			POINT * pPts2 = NULL;
			POINT pt1, pt2;
			
			pt1.x = iStartX;
			pt1.y = iStartY;

			int iEndX = GetX( dwPercentCnt, rcSurface.Width() / 2, rcSurface.Height() / 2 );
		
			DWORD dwPtCnt = CreatePoints( &pPts1, iStartX, iEndX, -1, GetY1, pParam, 0, NULL );
			//ASSERT( dwPtCnt >= 2 );
			pt2.x = iEndX;
			pt2.y = pPts1[dwPtCnt - 1].y;
			//int aaa = GetY1(pt2.x, pParam);

			dwPtCnt = CreatePoints( &pPts2, iEndX, iStartX, 1, GetY2, pParam, dwPtCnt, pPts1 );
			delete[] pPts1;

			HRGN hRgn = CreatePolygonRgn( pPts2, dwPtCnt, ALTERNATE );
			delete[] pPts2;

			FillRgn( dc.m_hDC, hRgn, brush );
			DeleteObject( hRgn );

			dc.MoveTo( pt1 );
			dc.LineTo( pt1.x, pt1.y + iVOffset );

			dc.MoveTo( pt2 );
			dc.LineTo( pt2.x, pt2.y + iVOffset );

			iStartX = pt2.x;
			iStartY = pt2.y;
		}
		dc.SelectObject(hOld);
	}

	delete[] pParam;

	HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	HGDIOBJ hOld = dc.SelectObject(hNullBrush);

	dc.Arc( r1, CPoint(-0xFFFF, iVOffset), CPoint(0xFFFF, iVOffset)  );
	dc.Ellipse( rcSurface );

	dc.SelectObject( hOld );
}

DWORD CCakeChartCtrl::CreatePoints( POINT ** ppPoint, int iStartX, int iEndX, int iInterval, GetY pFunc, 
								    LPVOID pParam , int iCnt, POINT * arPts  )
{
	int iTmpCnt = ( iEndX - iStartX ) / iInterval + 1;
	iTmpCnt = iTmpCnt > 0 ? iTmpCnt : -iTmpCnt;
	int iCount = iTmpCnt + iCnt;
	POINT * pstPoints = new POINT[iCount];

	pstPoints[0].x = iStartX;
	pstPoints[iTmpCnt - 1].x = iEndX;

	for ( int i = 1; i < iTmpCnt - 1 ; i++ )
	{
		pstPoints[i].x = pstPoints[i - 1].x + iInterval;
	}

	for ( i = 0; i < iTmpCnt; i++ )
	{
		pstPoints[i].y = pFunc( pstPoints[i].x, pParam );
	}

	for ( i = 0; i < iCnt; i++ )
	{
		POINT pt = arPts[i];
		pstPoints[ iTmpCnt + i ] = pt;
	}

	* ppPoint = pstPoints;
	return iCount;
}