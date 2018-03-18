#pragma once
#include "Interfaces.h"

class COutputCtrl : public CRichEditCtrl, public ILogic_OutputMsg
{
public:
	COutputCtrl();
	~COutputCtrl();

private:
	CFont				m_fontOutput;

public:
	void Clear();
	void AddColorText(LPCTSTR pstrText, COLORREF color = RGB(0, 0, 0));
	virtual void Output( LPCTSTR );
	virtual void Trace( LPCTSTR );

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnViewOutput();
	DECLARE_MESSAGE_MAP()
};


class COutputWnd : public CDockablePane
{
// 构造
public:
	COutputWnd();

// 特性
protected:
	CMFCTabCtrl			m_wndTabs;

public:
	COutputCtrl			m_wndRichEdit;

protected:
	void AdjustHorzScroll(CListBox& wndListBox);

// 实现
public:
	virtual ~COutputWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};
