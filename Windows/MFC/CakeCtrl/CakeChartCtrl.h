#ifndef __CAKE_CHART_2008_12_19__
#define __CAKE_CHART_2008_12_19__

typedef int (*GetY)(int iX, LPVOID pParam );

class CCakeChartCtrl : public CStatic
{
public:
	CCakeChartCtrl();
	~CCakeChartCtrl();
	VOID SetPercent(DWORD dwCnt, ...);

private:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

private:
	DWORD   m_dwCnt;
	DWORD * m_pdwPercent;
	DWORD   m_dwColors[8];
	

	DWORD CreatePoints( POINT ** ppPoint, int iStartX, int iEndX, int iInterval, GetY pFunc, 
		                LPVOID pParam , int iCnt, POINT * arPts );
};

#endif