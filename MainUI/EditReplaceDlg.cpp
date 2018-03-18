// EditReplaceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "EditReplaceDlg.h"
#include "UIEditView.h"
#include "afxdialogex.h"


// CEditReplaceDlg 对话框

IMPLEMENT_DYNAMIC(CEditReplaceDlg, CDialog)

CEditReplaceDlg::CEditReplaceDlg(CUIEditView* pBuddy, CWnd* pParent /*=NULL*/)
	: CDialog(CEditReplaceDlg::IDD, pParent)
{
	ASSERT(pBuddy != NULL);
	m_pBuddy = pBuddy;
	m_bMatchCase = FALSE;
	m_bWholeWord = FALSE;
	m_nScope = -1;
	m_bEnableScopeSelection = TRUE;
}

CEditReplaceDlg::~CEditReplaceDlg()
{
}

void CEditReplaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_EDIT_MATCH_CASE, m_bMatchCase);
	DDX_Check(pDX, IDC_EDIT_WHOLE_WORD, m_bWholeWord);
	DDX_Text(pDX, IDC_EDIT_TEXT, m_sText);
	DDX_Text(pDX, IDC_EDIT_REPLACE_WITH, m_sNewText);
	DDX_Radio(pDX, IDC_EDIT_SCOPE_SELECTION, m_nScope);
	DDX_Control(pDX, IDC_EDIT_SKIP, m_btnEditSkip);
	DDX_Control(pDX, IDC_EDIT_REPLACE, m_btnEditReplace);
	DDX_Control(pDX, IDC_EDIT_REPLACE_ALL, m_btnEditReplaceAll);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CEditReplaceDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT_TEXT, OnChangeEditText)
	ON_BN_CLICKED(IDC_EDIT_REPLACE, OnEditReplace)
	ON_BN_CLICKED(IDC_EDIT_REPLACE_ALL, OnEditReplaceAll)
	ON_BN_CLICKED(IDC_EDIT_SKIP, OnEditSkip)
END_MESSAGE_MAP()


// CEditReplaceDlg 消息处理程序
void CEditReplaceDlg::OnChangeEditText() 
{
	CString text;
	GetDlgItem(IDC_EDIT_TEXT)->GetWindowText(text);
	GetDlgItem(IDC_EDIT_SKIP)->EnableWindow(text != _T(""));
}

void CEditReplaceDlg::OnCancel() 
{
	VERIFY(UpdateData());

	CDialog::OnCancel();
}

BOOL CEditReplaceDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_EDIT_SKIP)->EnableWindow(m_sText != _T(""));
	GetDlgItem(IDC_EDIT_SCOPE_SELECTION)->EnableWindow(m_bEnableScopeSelection);
	m_bFound = FALSE;

	return TRUE;
}

BOOL CEditReplaceDlg::DoHighlightText()
{
	ASSERT(m_pBuddy != NULL);
	DWORD dwSearchFlags = 0;
	if (m_bMatchCase)
		dwSearchFlags |= FIND_MATCH_CASE;
	if (m_bWholeWord)
		dwSearchFlags |= FIND_WHOLE_WORD;

	BOOL bFound;
	if (m_nScope == 0)
	{
		//	Searching selection only
		bFound = m_pBuddy->FindTextInBlock(m_sText, m_ptFoundAt, m_ptBlockBegin, m_ptBlockEnd,
			dwSearchFlags, FALSE, &m_ptFoundAt);
	}
	else
	{
		//	Searching whole text
		bFound = m_pBuddy->FindText(m_sText, m_ptFoundAt, dwSearchFlags, FALSE, &m_ptFoundAt);
	}

	if (! bFound)
	{
		CString prompt;
		prompt.Format(IDS_EDIT_TEXT_NOT_FOUND, m_sText);
		AfxMessageBox(prompt, MB_OK | MB_ICONINFORMATION);
		m_ptCurrentPos = m_nScope == 0 ? m_ptBlockBegin : CPoint(0, 0);
		return FALSE;
	}

	m_pBuddy->HighlightText(m_ptFoundAt, m_sText.GetLength());
	return TRUE;
}

void CEditReplaceDlg::OnEditSkip() 
{
	if (! UpdateData())
		return;

	if (! m_bFound)
	{
		m_ptFoundAt = m_ptCurrentPos;
		m_bFound = DoHighlightText();
		return;
	}

	m_ptFoundAt.x += 1;
	m_bFound = DoHighlightText();
}

void CEditReplaceDlg::OnEditReplace() 
{
	if (! UpdateData())
		return;

	if (! m_bFound)
	{
		m_ptFoundAt = m_ptCurrentPos;
		m_bFound = DoHighlightText();
		return;
	}

	//	We have highlighted text
	VERIFY(m_pBuddy->ReplaceSelection(m_sNewText));

	//	Manually recalculate points
	if (m_bEnableScopeSelection)
	{
		if (m_ptBlockBegin.y == m_ptFoundAt.y && m_ptBlockBegin.x > m_ptFoundAt.x)
		{
			m_ptBlockBegin.x -= m_sText.GetLength();
			m_ptBlockBegin.x += m_sNewText.GetLength();
		}
		if (m_ptBlockEnd.y == m_ptFoundAt.y && m_ptBlockEnd.x > m_ptFoundAt.x)
		{
			m_ptBlockEnd.x -= m_sText.GetLength();
			m_ptBlockEnd.x += m_sNewText.GetLength();
		}
	}
	m_ptFoundAt.x += m_sNewText.GetLength();
	m_bFound = DoHighlightText();
}

void CEditReplaceDlg::OnEditReplaceAll() 
{
	if (! UpdateData())
		return;

	if (! m_bFound)
	{
		m_ptFoundAt = m_ptCurrentPos;
		m_bFound = DoHighlightText();
	}

	while (m_bFound)
	{
		//	We have highlighted text
		VERIFY(m_pBuddy->ReplaceSelection(m_sNewText));

		//	Manually recalculate points
		if (m_bEnableScopeSelection)
		{
			if (m_ptBlockBegin.y == m_ptFoundAt.y && m_ptBlockBegin.x > m_ptFoundAt.x)
			{
				m_ptBlockBegin.x -= m_sText.GetLength();
				m_ptBlockBegin.x += m_sNewText.GetLength();
			}
			if (m_ptBlockEnd.y == m_ptFoundAt.y && m_ptBlockEnd.x > m_ptFoundAt.x)
			{
				m_ptBlockEnd.x -= m_sText.GetLength();
				m_ptBlockEnd.x += m_sNewText.GetLength();
			}
		}
		m_ptFoundAt.x += m_sNewText.GetLength();
		m_bFound = DoHighlightText();
	}
}
