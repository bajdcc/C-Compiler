#pragma once
#include "TreeNode.h"
#include "Parser.h"
#include "SymbolTable.h"
#include "FunArgsCheck.h"

class AFX_EXT_CLASS CAnalyzer
{
public:
	CAnalyzer( CString& str );
	~CAnalyzer();

	// Attributes
public:
	CTreeNode*				m_pProgram;
	CParser*				m_pParser;
	static CSymbolTable		m_SymbolTable;
	static CFunArgsCheck	m_FunArgs;
	static CString			m_global;

	// Operations
public:
	void					BuildSymbolTable( CTreeNode* pNode );
	void					typeCheck( CTreeNode* pNode );

	BOOL					Run();
	BOOL					TypeCheck();

	// help routines
private:
	typedef void			pProc(CTreeNode*);
	void					traverse( CTreeNode* t, pProc, pProc );
	static void				nullProc( CTreeNode* t );
	static void				insertNode( CTreeNode* t );
	static void				checkNode( CTreeNode* t );

private:
	static int				location;
};

