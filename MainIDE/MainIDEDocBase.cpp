
// MainIDEDocBase.cpp : CMainIDEDocBase 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MainIDE.h"
#endif

#include "MainIDEDocBase.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainIDEDocBase

IMPLEMENT_DYNCREATE(CMainIDEDocBase, CDocument)

BEGIN_MESSAGE_MAP(CMainIDEDocBase, CDocument)
END_MESSAGE_MAP()


// CMainIDEDocBase 构造/析构

CMainIDEDocBase::CMainIDEDocBase()
{
	// TODO: 在此添加一次性构造代码
	// GetObject(GetStockObject(SYSTEM_FIXED_FONT), sizeof(LOGFONT), &m_lf);
	memset(&m_lf, 0, sizeof(LOGFONT));
	m_lf.lfHeight = -14;
	m_lf.lfWeight = FW_LIGHT;
	_tcscpy( m_lf.lfFaceName, _T("新宋体") );
}

CMainIDEDocBase::~CMainIDEDocBase()
{
}

BOOL CMainIDEDocBase::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	m_TextBuffer.InitNew();

	return TRUE;
}




// CMainIDEDocBase 序列化

void CMainIDEDocBase::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CMainIDEDocBase::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CMainIDEDocBase::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:  strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CMainIDEDocBase::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CMainIDEDocBase 诊断

#pragma region DEBUG
#ifdef _DEBUG
void CMainIDEDocBase::AssertValid() const
{
	CDocument::AssertValid();
}

void CMainIDEDocBase::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
#pragma endregion DEBUG


// CMainIDEDocBase 命令


BOOL CMainIDEDocBase::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  在此添加您专用的创建代码

	return m_TextBuffer.LoadFromFile(lpszPathName);
}


BOOL CMainIDEDocBase::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 在此添加专用代码和/或调用基类

	return m_TextBuffer.SaveToFile(lpszPathName);
}


void CMainIDEDocBase::OnCloseDocument()
{
	// TODO: 在此添加专用代码和/或调用基类
	// 	if( m_TextBuffer.IsModified() )
	// 	{
	// 		int ret = AfxMessageBox( _T("文件已经更改，要保存吗？"), MB_YESNOCANCEL | MB_ICONQUESTION);
	// 		if( ret == IDCANCEL )
	// 			return;
	// 		else if( ret == IDYES )
	// 			this->OnFileSave();
	// 	}

	CDocument::OnCloseDocument();
}


void CMainIDEDocBase::DeleteContents()
{
	// TODO: 在此添加专用代码和/或调用基类
	m_TextBuffer.FreeAll();

	CDocument::DeleteContents();
}

LPCTSTR CMainIDEDocBase::GetTitleFromPathName( CString& strPathName )
{
	int index1 = strPathName.ReverseFind( _T('\\') );
	int index2 = strPathName.ReverseFind( _T('.') );

	if( index2 > -1 && index2 > index1 )
		strPathName.SetAt( index2, _T('\0') );

	return (LPCTSTR)strPathName;
}
