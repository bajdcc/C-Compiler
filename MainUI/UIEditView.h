#pragma once
#include "UITextBuffer.h"
#include "UITextView.h"

class CEditDropTargetImpl;

// CUIEditView йсм╪

class AFX_EXT_CLASS CUIEditView : public CUITextView
{
	DECLARE_DYNCREATE(CUIEditView)

private:
	BOOL	m_bOvrMode;
	BOOL	m_bDropPosVisible;
	CPoint	m_ptSavedCaretPos;
	CPoint	m_ptDropPos;
	BOOL	m_bSelectionPushed;
	CPoint	m_ptSavedSelStart, m_ptSavedSelEnd;
	BOOL	m_bAutoIndent;

	//	[JRT]
	BOOL	m_bDisableBSAtSOL;								// Disable BS At Start Of Line

	BOOL   DeleteCurrentSelection();

protected:
	CEditDropTargetImpl *m_pDropTarget;
	virtual DROPEFFECT GetDropEffect();
	virtual void OnDropSource(DROPEFFECT de);
	void Paste();
	void Cut();
	virtual void ResetView();

	// Attributes
public:
	BOOL GetAutoIndent() const; 
	void SetAutoIndent(BOOL bAutoIndent);

	//	[JRT]
	void SetDisableBSAtSOL(BOOL bDisableBSAtSOL);
	BOOL GetDisableBSAtSOL() const;

	// Operations
public:
	// I add this function for ease
	void SelLast();

	CUIEditView();
	~CUIEditView();

	BOOL GetOverwriteMode() const;
	void SetOverwriteMode(BOOL bOvrMode = TRUE);

	void ShowDropIndicator(const CPoint &point);
	void HideDropIndicator();

	BOOL DoDropText(COleDataObject *pDataObject, const CPoint &ptClient);
	void DoDragScroll(const CPoint &point);

	virtual BOOL QueryEditable();
	virtual void UpdateView(CUITextView *pSource, IUpdateContext *pContext, DWORD dwFlags, int nLineIndex = -1);

	BOOL ReplaceSelection(LPCTSTR pszNewText);

	virtual void OnEditOperation(int nAction, LPCTSTR pszText);

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	// Implementation
protected:

	// Generated message map functions
protected:
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditDelete();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditDeleteBack();
	afx_msg void OnEditUntab();
	afx_msg void OnEditTab();
	afx_msg void OnEditSwitchOvrmode();
	afx_msg void OnUpdateEditSwitchOvrmode(CCmdUI* pCmdUI);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnEditReplace();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnUpdateIndicatorCol(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorOvr(CCmdUI* pCmdUI);
	afx_msg void OnUpdateIndicatorRead(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};


// CEditDropTargetImpl

class CEditDropTargetImpl : public COleDropTarget
{
private:
	CUIEditView *m_pOwner;
public:
	CEditDropTargetImpl(CUIEditView *pOwner);

	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);
};