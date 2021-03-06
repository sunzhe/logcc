﻿
// LogMainView.cpp : CLogMainView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "LogCC.h"
#endif

#include "LogCCDoc.h"
#include "LogMainView.h"
#include "ILogQuery.h"
#include "LogQueryResult.h"
#include "LogItem.h"
#include "ILogItemPainter.h"
#include "LogPainterFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const unsigned LineHeight = 15;

// CLogMainView

IMPLEMENT_DYNCREATE(CLogMainView, CScrollView)

BEGIN_MESSAGE_MAP(CLogMainView, CScrollView)
	ON_WM_ERASEBKGND()
	ON_WM_VSCROLL()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CLogMainView 构造/析构

CLogMainView::CLogMainView()
{
}

CLogMainView::~CLogMainView()
{
}

// CLogMainView 绘制

void CLogMainView::OnDraw(CDC* pDC)
{
	CLogCCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect clientRect;
	GetClientRect(clientRect);
	DEBUG_INFO(_T("客户区区域：") << clientRect.left << ", " << clientRect.top << ", "
		<< clientRect.right << ", " << clientRect.bottom);

	HDC memDC = ::CreateCompatibleDC(pDC->GetSafeHdc());

	HBITMAP memBmp = ::CreateCompatibleBitmap(pDC->GetSafeHdc(), clientRect.Width(), clientRect.Height());
	HGDIOBJ oldBmp = ::SelectObject(memDC, memBmp);

	HBRUSH bkgdBrush = reinterpret_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	::FillRect(memDC, clientRect, bkgdBrush);

	HFONT font = ::CreateFont(LineHeight - 2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, FIXED_PITCH, _T("新宋体"));
	HGDIOBJ oldFont = ::SelectObject(memDC, font);

	DEBUG_INFO(_T("重绘"));

	CPoint scrollPosition = GetScrollPosition();
	DEBUG_INFO(_T("滚动条位置：") << scrollPosition.x << ", " << scrollPosition.y);

	// 顶部可以显示半行
	int yLogLineStart = scrollPosition.y % LineHeight == 0 ? 0 : scrollPosition.y % LineHeight - LineHeight;
	unsigned beginLine = scrollPosition.y / LineHeight;
	// +1是为了底部能显示半行
	unsigned endLine = (scrollPosition.y + clientRect.Height()) / LineHeight + 1;
	endLine = min(endLine, GetDocument()->logQuery->getCurQueryResult()->getCount());
	DEBUG_INFO(_T("行号区间：") << beginLine << ", " << endLine);

	vector<LogItem*> vecLines = GetDocument()->logQuery->getCurQueryResult()->getRange(beginLine, endLine);
	for (unsigned i = 0; i < vecLines.size(); i++) {
		LogItem* item = vecLines[i];
		CRect rect = clientRect;
		rect.top = yLogLineStart + i * LineHeight;
		rect.bottom = rect.top + LineHeight;
		LogPainterFactory::GetInstance()->GetSingleLinePainter()->Draw(memDC, rect, *item);
	}

	::BitBlt(pDC->GetSafeHdc(), scrollPosition.x, scrollPosition.y, clientRect.Width(), clientRect.Height(),
		memDC, 0, 0, SRCCOPY);

	::SelectObject(memDC, oldFont);
	::DeleteObject(font);
	::SelectObject(memDC, oldBmp);
	::DeleteObject(memBmp);
	::DeleteDC(memDC);
}

void CLogMainView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	GetDocument()->logQuery->registerObserver(this);

	UpdateScroll();
	SetFocus();
}

void CLogMainView::PostNcDestroy()
{
	GetDocument()->logQuery->unregisterObserver(this);
	__super::PostNcDestroy();
}

void CLogMainView::UpdateScroll()
{
	CRect clientRect;
	GetClientRect(clientRect);
	CSize totalSize;
	totalSize.cx = 0; //clientRect.Width();
	// 加1是为了最后一行一定可见
	GetDocument()->length = totalSize.cy = (GetDocument()->logQuery->getCurQueryResult()->getCount() + 1) * LineHeight;
#define LOGCC_WINUI_CUSTOMIZE_PAGE_SIZE_LINE_SIZE
#ifdef LOGCC_WINUI_CUSTOMIZE_PAGE_SIZE_LINE_SIZE
	CSize pageSize(clientRect.Width(), clientRect.Height() / LineHeight * LineHeight);
	CSize lineSize(clientRect.Width(), LineHeight);
	SetScrollSizes(MM_TEXT, totalSize, pageSize, lineSize);
#else
	SetScrollSizes(MM_TEXT, totalSize);
#endif
#define LOGCC_WINUI_SCROLL_TO_END_ON_UPDATE
#ifdef LOGCC_WINUI_SCROLL_TO_END_ON_UPDATE
	int y = totalSize.cy - clientRect.Height();
	ScrollToPosition(CPoint(0, max(y, 0)));
#endif
}

void CLogMainView::onGeneralDataChanged() {
	Invalidate();
}

void CLogMainView::onQueryResultChanged() {
	UpdateScroll();
	Invalidate();
}

void CLogMainView::onScrollPositionChanged(int yPosition) {
	CPoint position = GetScrollPosition();
	position.y = yPosition;
	ScrollToPosition(position);
}

#ifdef _DEBUG
CLogCCDoc* CLogMainView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLogCCDoc)));
	return (CLogCCDoc*)m_pDocument;
}
#endif //_DEBUG


// CLogMainView 消息处理程序

BOOL CLogMainView::OnEraseBkgnd(CDC* pDC)
{
#ifdef LOGCC_WINUI_USE_DEFAULT_ERASE_BACKGROUND
	return CScrollView::OnEraseBkgnd(pDC);
#else
	return TRUE;
#endif
}

void CLogMainView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (nSBCode == SB_ENDSCROLL)
	{
		Invalidate();
	}
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CLogMainView::PreTranslateMessage(MSG* pMsg)
{
	// 更新UI数据到ViewData
	CPoint scrollPos = GetScrollPosition();
	GetDocument()->yScrollPos = scrollPos.y;
	
	GetDocument()->lineHeight = LineHeight;

	GetClientRect(GetDocument()->clientRect);

	return __super::PreTranslateMessage(pMsg);
}

void CLogMainView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetForegroundWindow() == AfxGetMainWnd()) {
		SetFocus();
	}
	__super::OnMouseMove(nFlags, point);
}
