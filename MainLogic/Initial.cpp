#include "stdafx.h"
#include "Resource.h"
#include "Logic.h"
#include "Initial.h"

namespace Logic_Class
{
	/************************************************************************/
	/*  CExInitScanner                                                      */
	/************************************************************************/
	void CExInitScanner::Init( CString & title, CListCtrl & ctrl )
	{
		title.LoadString(IDS_INIT_LEXER_TITLE);

		// col
		ctrl.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 50);
		ctrl.InsertColumn(1, _T("行号"), LVCFMT_CENTER, 100);
		ctrl.InsertColumn(2, _T("索引"), LVCFMT_CENTER, 100);
		ctrl.InsertColumn(3, _T("值"), LVCFMT_CENTER, 400);

		// data
		LIST_ARRAY* pList = Logic_GetDataPtr();

		POSITION pos = pList->GetHeadPosition();
		int i = 0; CString line;
		while(pos)
		{
			// Get data to show and delete node
			CStringArray* pArray = pList->GetNext(pos);

			line.Format(IDS_FORMAT_D, ++i);
			int index = ctrl.InsertItem(i, line);
			ctrl.SetItemText(index, 1, pArray->GetAt(0));
			ctrl.SetItemText(index, 2, pArray->GetAt(1));
			ctrl.SetItemText(index, 3, pArray->GetAt(2));

			delete pArray;
		}

		// clear
		Logic_SetDataPtr(NULL);
	}

	/************************************************************************/
	/*  CExInitSymbol                                                       */
	/************************************************************************/
	void CExInitSymbol::Init( CString & title, CListCtrl & ctrl )
	{
		title.LoadString(IDS_INIT_SYMBOL_TITLE);

		ctrl.InsertColumn(0, _T("序号"), LVCFMT_LEFT, 50);
		ctrl.InsertColumn(1, _T("变量名"), LVCFMT_CENTER, 160);
		ctrl.InsertColumn(2, _T("类型"), LVCFMT_CENTER, 120);
		ctrl.InsertColumn(3, _T("作用域"), LVCFMT_CENTER, 120);
		ctrl.InsertColumn(4, _T("索引"), LVCFMT_CENTER, 80);
		ctrl.InsertColumn(5, _T("行号"), LVCFMT_LEFT, 160);

		// data
		LIST_ARRAY* pList = Logic_GetDataPtr();

		POSITION pos = pList->GetHeadPosition();
		int i = 0; CString line;
		while(pos)
		{
			// Get data to show and delete node
			CStringArray* pArray = pList->GetNext(pos);

			line.Format(IDS_FORMAT_D, ++i);
			int index = ctrl.InsertItem(i, line);
			ctrl.SetItemText(index, 1, pArray->GetAt(0));
			ctrl.SetItemText(index, 2, pArray->GetAt(1));
			ctrl.SetItemText(index, 3, pArray->GetAt(2));
			ctrl.SetItemText(index, 4, pArray->GetAt(3));
			ctrl.SetItemText(index, 5, pArray->GetAt(4));

			delete pArray;
		}

		// clear
		Logic_SetDataPtr(NULL);
	}

	/************************************************************************/
	/*  CExInitTree                                                         */
	/************************************************************************/
	void CExInitTree::Init( CString & title, CTreeCtrl & ctrl )
	{
		ASSERT(m_pNode);

		title.LoadString(IDS_INIT_TREE_TITLE);
		CString _global; _global.LoadString(IDS_A_GLOBAL);
		m_lpszRev = Logic_GetRevKeywords();
		ASSERT(m_lpszRev);

		m_pCtrl = &ctrl;
		HTREEITEM root = m_pCtrl->InsertItem(_global, TVI_ROOT, TVI_FIRST);
		print_tree(m_pNode, root);
		m_pCtrl->SelectItem(root);
	}

	void CExInitTree::SetNode( CTreeNode * node )
	{
		m_pNode = node;
	}

	void CExInitTree::print_tree( CTreeNode * node, HTREEITEM item )
	{
		HTREEITEM save_item = item;
		while( node != NULL ) {
			CString tmp;
			switch( node->nodekind ) {
			case kFunDec:
				tmp.Format(_T("函数声明：%s %s"), m_lpszRev[(int)node->type], node->szName);
				item = m_pCtrl->InsertItem(tmp, item);
				break;
			case kVarDec:
				tmp.Format(_T("变量声明：%s %s"), m_lpszRev[(int)node->type], node->szName);
				if (node->bArray) tmp.AppendFormat(_T("[%d]"), node->iArraySize);
				m_pCtrl->InsertItem(tmp, item);
				break;
			case kParam:
				tmp.Format(_T("形参：%s %s"), m_lpszRev[(int)node->type], node->szName);
				if (node->bArray) tmp.Append(_T("[]"));
				m_pCtrl->InsertItem(tmp, item);
				break;
			case kStmt:
				switch( node->kind.stmt ) {
				case kLabel:
					tmp.Format(_T("标签：%s"), node->szName);
					item = m_pCtrl->InsertItem(tmp, item);
					break;
				case kGoto:
					tmp.Format(_T("跳转：%s"), node->szName);
					item = m_pCtrl->InsertItem(tmp, item);
					break;
				case kCall:
					tmp.Format(_T("调用函数：%s  实参："), node->szName);
					item = m_pCtrl->InsertItem(tmp, item);
					break;
				case kIf:		item = m_pCtrl->InsertItem(_T("if"), item);				break;
				case kWhile:	item = m_pCtrl->InsertItem(_T("while"), item);			break;
				case kBreak:	item = m_pCtrl->InsertItem(_T("break"), item);			break;
				case kContinue:	item = m_pCtrl->InsertItem(_T("continue"), item);		break;
				case kReturn:	item = m_pCtrl->InsertItem(_T("return"), item);			break;
				default:		item = m_pCtrl->InsertItem(_T("未知类型"), item);
				}
				break;
			case kExp:
				switch( node->kind.exp ) {
				case kOp:
					tmp.Format(_T("操作符：%s"), node->szName);
					item = m_pCtrl->InsertItem(tmp, item);
					break;
				case kConst:
					tmp.Format(_T("常量：%s"), node->szName);
					m_pCtrl->InsertItem(tmp, item);
					break;
				case kID:
					tmp.Format(_T("ID：%s"), node->szName);
					if( node->bArray ) {
						node = node->child[0];
						tmp.AppendFormat(_T("[%s]"), node->szName);
					}
					m_pCtrl->InsertItem(tmp, item);
					break;
				default: item = m_pCtrl->InsertItem(_T("未知类型"), item);
				}
				break;
			default: item = m_pCtrl->InsertItem(_T("未知类型"), item);
			}

			for( int i = 0; i < MAX_CHILDREN; i++ ) print_tree( node->child[i], item );
			node = node->sibling;

			item = save_item;
		}

		m_pCtrl->Expand(item, TVE_EXPAND);
	}
}
