#include "IppDib.h"

void CBmpShowview::OnLButtonDown(UINT nFlags, CPoint point) {
	IppDib dib;
	dib.Load("defect1.bmp");

	CClientDC dc(this);
	dib.Draw(dc.m_hDC, point.x, point.y);

	CView::OnLButtonDown(nFlags, point);
}