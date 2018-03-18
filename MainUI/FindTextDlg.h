#pragma once
#include "EDIT_CMD.h"


class CUITextView;

// CFindTextDlg 对话框

class CFindTextDlg : public CDialog
{
	DECLARE_DYNAMIC(CFindTextDlg)

public:
	CFindTextDlg(CUITextView* pBuddy, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFindTextDlg();

// 对话框数据
	enum { IDD = IDD_EDIT_FIND };

	CPoint m_ptCurrentPos;

private:
	CUITextView *m_pBuddy;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual void OnOK();
	afx_msg void OnChangeEditText();
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()

public:
	int			m_nDirection;
	BOOL		m_bMatchCase;
	CString		m_sText;
	BOOL		m_bWholeWord;
	CButton		m_btnOK;
	CButton		m_btnCancel;
};
