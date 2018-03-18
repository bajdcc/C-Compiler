#include "stdafx.h"
#include "Interfaces.h"
#include "TreeNode.h"

namespace Logic_Class
{
	// Declaration
	class AFX_EXT_CLASS CExInitScanner : public ILogic_ListInit
	{
	public:
		virtual void Init( CString &, CListCtrl & );
	};

	class AFX_EXT_CLASS CExInitSymbol : public ILogic_ListInit
	{
	public:
		virtual void Init( CString &, CListCtrl & );
	};

	class AFX_EXT_CLASS CExInitTree : public ILogic_TreeInit
	{
	public:
		virtual void Init( CString &, CTreeCtrl & );
		void SetNode( CTreeNode * );
	private:
		void print_tree( CTreeNode *, HTREEITEM );
		CTreeNode* m_pNode;
		CTreeCtrl* m_pCtrl;
		LPTSTR* m_lpszRev;
	};

	// Global instances
	CExInitScanner AFX_EXT_CLASS exInitScanner;
	CExInitSymbol AFX_EXT_CLASS exInitSymbol;
	CExInitTree AFX_EXT_CLASS exInitTree;
}
