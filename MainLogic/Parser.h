#pragma once
#include "Lexer.h"

class AFX_EXT_CLASS CParser
{
public:
	CParser( CString& str, BOOL traces = TRUE );
	virtual ~CParser();

	CTreeNode*		BuildSyntaxTree();

	// Attributes
public:
	CTreeNode*		m_pProgram;

	// Operations
private:
	// help routines
	CTreeNode*		newNode( NodeKind kind, LTokenType type, CString& ID );
	CTreeNode*		newStmtNode( StmtKind kind, CString& ID );
	CTreeNode*		newExpNode( ExpKind kind, LTokenType type, CString& ID );

	BOOL			match( LTokenType type );
	void			ConsumeUntil( LTokenType type );
	void			ConsumeUntil( LTokenType type1, LTokenType type2 );

	// Grammar functions
	CTreeNode*		program();
	CTreeNode*		declaration_list();
	CTreeNode*		declaration();
	CTreeNode*		var_declaration();
	CTreeNode*		fun_declaration();
	CTreeNode*		params();
	CTreeNode*		compound_stmt();
	CTreeNode*		local_declarations();
	CTreeNode*		expression_stmt();
	CTreeNode*		subcompound_stmt();
	CTreeNode*		if_stmt();
	CTreeNode*		while_stmt();
	CTreeNode*		for_stmt();
	CTreeNode*		goto_stmt();
	CTreeNode*		break_stmt();
	CTreeNode*		continue_stmt();
	CTreeNode*		return_stmt();
	CTreeNode*		expression();
	CTreeNode*		logic1_expression();
	CTreeNode*		logic2_expression();
	CTreeNode*		simple_expression();
	CTreeNode*		additive_expression();
	CTreeNode*		term();
	CTreeNode*		logic3_expression();
	CTreeNode*		factor();
	CTreeNode*		var();
	CTreeNode*		call();
	CTreeNode*		args();

private:
	CLexer*			m_pLexer;
	TOKEN			m_token, TypeToken, IDToken; // the latter two are for temporary use
	CString			m_szScope;	// ”Ú(eg. global)
};
