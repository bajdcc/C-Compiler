#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MainIDE.h"
#endif

#include "MainIDEDocBase.h"
#include "ExAsmView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CExAsmView

IMPLEMENT_DYNCREATE(CExAsmView, CMainIDEViewBase)

BEGIN_MESSAGE_MAP(CExAsmView, CMainIDEViewBase)
END_MESSAGE_MAP()

// CExAsmView 构造/析构

CExAsmView::CExAsmView()
{
}

CExAsmView::~CExAsmView()
{
}

void CExAsmView::OnInitialUpdate()
{
	CMainIDEViewBase::OnInitialUpdate();
	InitHashMap();
}

void CExAsmView::InitHashMap()
{
	LPTSTR s_apszCKeywordList[] =
	{
		NULL
	};

	for( int L = 0; s_apszCKeywordList[L] != NULL; L++ ) {
		m_mapKeywords[s_apszCKeywordList[L]] = NULL;
	}
}
