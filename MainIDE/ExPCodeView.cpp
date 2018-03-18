#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MainIDE.h"
#endif

#include "MainIDEDocBase.h"
#include "ExPCodeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CExPCodeView

IMPLEMENT_DYNCREATE(CExPCodeView, CMainIDEViewBase)

BEGIN_MESSAGE_MAP(CExPCodeView, CMainIDEViewBase)
END_MESSAGE_MAP()

// CExPCodeView 构造/析构

CExPCodeView::CExPCodeView()
{
}

CExPCodeView::~CExPCodeView()
{
}

void CExPCodeView::OnInitialUpdate()
{
	CMainIDEViewBase::OnInitialUpdate();
	InitHashMap();
}

void CExPCodeView::InitHashMap()
{
	LPTSTR s_apszCKeywordList[] =
	{
		NULL
	};

	for( int L = 0; s_apszCKeywordList[L] != NULL; L++ ) {
		m_mapKeywords[s_apszCKeywordList[L]] = NULL;
	}
}
