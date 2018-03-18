
// MainIDEView.cpp : CMainIDEViewBase 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MainIDE.h"
#endif

#include "Logic.h"
#include "MainIDEViewBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainIDEViewBase

IMPLEMENT_DYNCREATE(CMainIDEViewBase, CUIEditView)

BEGIN_MESSAGE_MAP(CMainIDEViewBase, CUIEditView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CMainIDEViewBase 构造/析构

CMainIDEViewBase::CMainIDEViewBase()
{
	// TODO: 在此处添加构造代码
}

CMainIDEViewBase::~CMainIDEViewBase()
{
}

// CMainIDEViewBase 打印


void CMainIDEViewBase::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMainIDEViewBase::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return CUIEditView::OnPreparePrinting(pInfo);
}

void CMainIDEViewBase::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 添加额外的打印前进行的初始化过程
	CUIEditView::OnBeginPrinting(pDC, pInfo);
}

void CMainIDEViewBase::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: 添加打印后进行的清理过程
	CUIEditView::OnEndPrinting(pDC, pInfo);
}

void CMainIDEViewBase::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
	CUIEditView::OnRButtonUp(nFlags, point);
}

void CMainIDEViewBase::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMainIDEViewBase 诊断

#pragma region DEBUG
#ifdef _DEBUG
void CMainIDEViewBase::AssertValid() const
{
	CView::AssertValid();
}

void CMainIDEViewBase::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG
#pragma endregion DEBUG

CMainIDEDocBase* CMainIDEViewBase::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMainIDEDocBase)));
	return (CMainIDEDocBase*)m_pDocument;
}

void CMainIDEViewBase::SetSelection( const CPoint &ptStart, const CPoint &ptEnd )
{
	CUITextView::SetSelection(ptStart,ptEnd);
}

void CMainIDEViewBase::SelectLine( int line )
{
	CUITextBuffer* pBuffer = &((CMainIDEDocBase*)GetDocument())->m_TextBuffer;
	ASSERT( pBuffer );

	if( line < 1 ) line = 1;
	if( line > pBuffer->GetLineCount() ) line = pBuffer->GetLineCount();

	WINDOWPLACEMENT pl;
	GetWindowPlacement( &pl );
	pl.flags = WPF_RESTORETOMAXIMIZED;
	pl.showCmd = SW_SHOWMAXIMIZED;
	SetWindowPlacement( &pl );
	this->SetFocus();
	SetSelection( CPoint(0, line - 1),
		CPoint(pBuffer->GetLineLength(line-1), line - 1) );
	SetCursorPos( CPoint(pBuffer->GetLineLength(line - 1), line - 1) );
	EnsureVisible( CPoint(pBuffer->GetLineLength(line - 1), line - 1) );
}

CUITextBuffer * CMainIDEViewBase::LocateTextBuffer()
{
	return &((CMainIDEDocBase*)GetDocument())->m_TextBuffer;
}


// CMainIDEViewBase 消息处理程序


void CMainIDEViewBase::OnInitialUpdate()
{
	CUIEditView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	InitHashMap();
	SetFont(((CMainIDEDocBase*)GetDocument())->m_lf);
}

void CMainIDEViewBase::InitHashMap()
{
	using namespace Logic_Global;
	LPTSTR* s_apszCKeywordList = Logic_GetKeywords();
	LPTSTR* s_apszCTypeList = Logic_GetTypes();

	for( int L = 0; s_apszCKeywordList[L] != NULL; L++ ) {
		m_mapKeywords[s_apszCKeywordList[L]] = NULL;
	}
	for( int L = 0; s_apszCTypeList[L] != NULL; L++ ) {
		m_mapTypes[s_apszCTypeList[L]] = NULL;
	}
}
