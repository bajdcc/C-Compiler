#pragma once
#include "Interfaces.h"

// CExSymbolDlg 对话框

class CExListDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CExListDlg)

public:
	CExListDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExListDlg();

// 对话框数据
	enum { IDD = IDD_LISTVIEW };

private:
	ILogic_ListInit*			m_pInterface;

public:
	void SetInterface(ILogic_ListInit* pInterface);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CListCtrl m_List;
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnInitDialog();
};
