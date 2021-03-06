﻿
// ChildFrm.cpp : CChildFrame 类的实现
//

#include "stdafx.h"
#include "LogCC.h"

#include "ChildFrm.h"
#include "LogCCDoc.h"
#include "LogCtrlView.h"
#include "LogMainView.h"
#include "LogTextView.h"
#include "LogCtrlController.h"
#include "LogMainController.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
END_MESSAGE_MAP()

// CChildFrame 构造/析构

CChildFrame::CChildFrame()
	: m_bSplitterCreated(false)
{
}

CChildFrame::~CChildFrame()
{
}

// UNDONE: 封装起来
static hash_map<HWND, HWND> ControllerMap;

static hash_map<HWND, WNDPROC> OriginWndProcMap;

static LRESULT CALLBACK ControllerRouteProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	HWND controller = ControllerMap[hwnd];
	WNDPROC originProc = OriginWndProcMap[hwnd];
	LRESULT result = ::CallWindowProc(originProc, hwnd, msg, wparam, lparam);
	if (controller) {
		::SendMessage(controller, msg, wparam, lparam);
	}
	return result;
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
#define LOGCC_WINUI_USE_SPLIT_VIEW
#ifndef LOGCC_WINUI_USE_SPLIT_VIEW
	m_wndSplitter.Create(this,
		2, 2,			// TODO: 调整行数和列数
		CSize(10, 10),	// TODO: 调整最小窗格大小
		pContext);
#else
	m_wndSplitter.CreateStatic(this, 3, 1);
	m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLogCtrlView), CSize(10, 10), pContext);
	m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CLogMainView), CSize(10, 10), pContext);
	m_wndSplitter.CreateView(2, 0, RUNTIME_CLASS(CLogTextView), CSize(10, 10), pContext);
	m_bSplitterCreated = true;

	CWnd* ctrlView = m_wndSplitter.GetPane(0, 0);
	LogCtrlController* ctrlController = new LogCtrlController(this);
	ControllerMap[ctrlView->GetSafeHwnd()] = ctrlController->GetSafeHwnd();
	OriginWndProcMap[ctrlView->GetSafeHwnd()] = reinterpret_cast<WNDPROC>(::SetWindowLong(
		ctrlView->GetSafeHwnd(), GWL_WNDPROC, reinterpret_cast<long>(ControllerRouteProc)));
	ctrlController->setViewData(pContext->m_pCurrentDoc);

	CWnd* mainView = m_wndSplitter.GetPane(1, 0);
	LogMainController* mainController = new LogMainController(this);
	ControllerMap[mainView->GetSafeHwnd()] = mainController->GetSafeHwnd();
	OriginWndProcMap[mainView->GetSafeHwnd()] = reinterpret_cast<WNDPROC>(::SetWindowLong(
		mainView->GetSafeHwnd(), GWL_WNDPROC, reinterpret_cast<long>(ControllerRouteProc)));
	mainController->setViewData(pContext->m_pCurrentDoc);

	// UNDONE: 没有释放内存和回设指针
#endif
	return TRUE;
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}


// CChildFrame 消息处理程序

void CChildFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWndEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	GetWindowRect(&rect);
	if(m_bSplitterCreated)  // m_bSplitterCreated set in OnCreateClient
	{
		m_wndSplitter.SetColumnInfo(0, rect.Width(), 10);
		const int ctrlViewHeight = 32;
		const int textViewHeight = 100;
		m_wndSplitter.SetRowInfo(0, ctrlViewHeight, 10);
		m_wndSplitter.SetRowInfo(1, max(rect.Height() - ctrlViewHeight - textViewHeight, 10), 10); 
		m_wndSplitter.SetRowInfo(2, textViewHeight, 10);
		m_wndSplitter.RecalcLayout();
	}
}

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIChildWndEx::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	if (bActivate && pActivateWnd == this) {
		// UNDONE: 特殊代码，不好
		GetActiveView()->SendMessage(LogCtrlController::WM_COMMIT);
	}
}
