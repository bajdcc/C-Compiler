
// MainIDEView.cpp : CMainIDEView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MainIDE.h"
#endif

#include "MainIDEDoc.h"
#include "MainIDEView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainIDEView

IMPLEMENT_DYNCREATE(CMainIDEView, CMainIDEViewBase)

BEGIN_MESSAGE_MAP(CMainIDEView, CMainIDEViewBase)
END_MESSAGE_MAP()

// CMainIDEView 构造/析构

CMainIDEView::CMainIDEView()
{
}

CMainIDEView::~CMainIDEView()
{
}

