#pragma once
#include "Interfaces.h"

// CExTreeDlg 对话框

class CExTreeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CExTreeDlg)

public:
	CExTreeDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CExTreeDlg();

// 对话框数据
	enum { IDD = IDD_TREEVIEW };

private:
	ILogic_TreeInit*			m_pInterface;

public:
	void SetInterface(ILogic_TreeInit* pInterface);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnInitDialog();
	CTreeCtrl m_Tree;
};
