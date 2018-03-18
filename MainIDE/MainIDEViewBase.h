
// MainIDEView.h : CMainIDEViewBase 类的接口
//

#pragma once
#include "UIEditView.h"
#include "MainIDEDocBase.h"


class CMainIDEViewBase : public CUIEditView
{
protected: // 仅从序列化创建
	CMainIDEViewBase();
	DECLARE_DYNCREATE(CMainIDEViewBase)

	// 特性
public:
	CMainIDEDocBase* GetDocument() const;
	virtual CUITextBuffer * LocateTextBuffer();

	// 操作
public:
	void SetSelection(const CPoint &ptStart, const CPoint &ptEnd);
	void SelectLine( int line );

	// 重写
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// 实现
public:
	virtual ~CMainIDEViewBase();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL IsCKeyword(LPCTSTR pszChars, int nLength);
	BOOL IsCType(LPCTSTR pszChars, int nLength);
	BOOL IsCNumber(LPCTSTR pszChars, int nLength);

public:
	CMapStringToPtr m_mapKeywords;
	CMapStringToPtr m_mapTypes;

	// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();

	virtual DWORD ParseLine( DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems );
	void InitHashMap();
};

#ifndef _DEBUG  // MainIDEView.cpp 中的调试版本
inline CMainIDEDocBase* CMainIDEViewBase::GetDocument() const
{ return reinterpret_cast<CMainIDEDocBase*>(m_pDocument); }
#endif

