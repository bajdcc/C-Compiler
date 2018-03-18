#include "StdAfx.h"
#include "Resource.h"
#include "Logic.h"
#include "TreeNode.h"
#include "Parser.h"


CParser::CParser( CString& str, BOOL traces /*= TRUE*/ )
{
	m_pLexer = new CLexer( str, traces );
	m_pProgram = NULL;
}

CParser::~CParser()
{
	delete m_pLexer; m_pLexer = NULL;
	if (m_pProgram) delete m_pProgram;
}

CTreeNode* CParser::BuildSyntaxTree()
{
	Logic_TraceMsg( IDS_T_PARSER_BEGIN );
	m_pProgram = program();
	if (Logic_GetErrorFlag())
	{
		Logic_OutputMsg( IDS_E_PARSER );
		return NULL;
	}
	Logic_TraceMsg( IDS_T_PARSER );
	return m_pProgram; // 语法树
}

CTreeNode* CParser::newNode( NodeKind kind, LTokenType type, CString& ID )
{
	CTreeNode* t		= new CTreeNode;
	t->lineno			= m_pLexer->LineNo();
	t->nodekind			= kind;
	t->type				= type;
	t->szName			= ID;
	t->szScope			= m_szScope;
	return t;
}

// 新的语句结点
CTreeNode* CParser::newStmtNode( StmtKind kind, CString& ID )
{
	CTreeNode* t		= new CTreeNode;
	t->lineno			= m_pLexer->LineNo();
	t->nodekind			= kStmt;
	t->kind.stmt		= kind;
	t->type				= _NONE;
	t->szName			= ID;
	t->szScope			= m_szScope;
	return t;
}

// 新的表达式结点
CTreeNode* CParser::newExpNode( ExpKind kind, LTokenType type, CString& ID )
{
	CTreeNode* t		= new CTreeNode;
	t->lineno			= m_pLexer->LineNo();
	t->nodekind			= kExp;
	t->kind.exp			= kind;
	t->type				= type;
	t->szName			= ID;
	t->szScope			= m_szScope;
	return t;
}

// 文法匹配
BOOL CParser::match( LTokenType type )
{
	m_token = m_pLexer->NextToken();
	return m_token.type == type;
}

void CParser::ConsumeUntil( LTokenType type )
{
	while( m_token.type != type && m_token.type != _EOF )
		m_token = m_pLexer->NextToken();
}

void CParser::ConsumeUntil( LTokenType type1, LTokenType type2 )
{
	while( m_token.type != type1 && m_token.type != type2 && m_token.type != _EOF )
		m_token = m_pLexer->NextToken();
}
