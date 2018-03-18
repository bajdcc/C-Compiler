// FindTextDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FindTextDlg.h"
#include "UITextView.h"
#include "afxdialogex.h"


// CFindTextDlg 对话框

IMPLEMENT_DYNAMIC(CFindTextDlg, CDialog)

CFindTextDlg::CFindTextDlg(CUITextView* pBuddy, CWnd* pParent /*=NULL*/)
	: CDialog(CFindTextDlg::IDD, pParent)
{
	ASSERT(pBuddy != NULL);
	m_pBuddy = pBuddy;
	m_nDirection = 1;
	m_bMatchCase = FALSE;
	m_bWholeWord = FALSE;
}

CFindTextDlg::~CFindTextDlg()
{
}

void CFindTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_EDIT_DIRECTION_UP, m_nDirection);
	DDX_Check(pDX, IDC_EDIT_MATCH_CASE, m_bMatchCase);
	DDX_Text(pDX, IDC_EDIT_TEXT, m_sText);
	DDX_Check(pDX, IDC_EDIT_WHOLE_WORD, m_bWholeWord);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CFindTextDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT_TEXT, OnChangeEditText)
END_MESSAGE_MAP()


// CFindTextDlg 消息处理程序
void CFindTextDlg::OnOK() 
{
	if (UpdateData())
	{
		ASSERT(m_pBuddy != NULL);
		DWORD dwSearchFlags = 0;
		if (m_bMatchCase)
			dwSearchFlags |= FIND_MATCH_CASE;
		if (m_bWholeWord)
			dwSearchFlags |= FIND_WHOLE_WORD;
		if (m_nDirection == 0)
			dwSearchFlags |= FIND_DIRECTION_UP;

		CPoint ptTextPos;
		if (! m_pBuddy->FindText(m_sText, m_ptCurrentPos, dwSearchFlags, TRUE, &ptTextPos))
		{
			CString prompt;
			prompt.Format(IDS_EDIT_TEXT_NOT_FOUND, m_sText);
			AfxMessageBox(prompt, MB_OK | MB_ICONINFORMATION);
			m_ptCurrentPos = CPoint(0, 0);
			return;
		}

		m_pBuddy->HighlightText(ptTextPos, m_sText.GetLength());

		CDialog::OnOK();
	}
}

void CFindTextDlg::OnChangeEditText() 
{
	CString text;
	GetDlgItem(IDC_EDIT_TEXT)->GetWindowText(text);
	GetDlgItem(IDOK)->EnableWindow(text != _T(""));
}

BOOL CFindTextDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDOK)->EnableWindow(m_sText != _T(""));

	return TRUE;
}

void CFindTextDlg::OnCancel() 
{
	VERIFY(UpdateData());

	CDialog::OnCancel();
}
