#include "StdAfx.h"
#include "Resource.h"
#include "Analyzer.h"
#include "Logic.h"

// static member variables initiation
CSymbolTable	CAnalyzer::m_SymbolTable;
CFunArgsCheck	CAnalyzer::m_FunArgs;
CString			CAnalyzer::m_global;
int				CAnalyzer::location;

//////////////////////////////////////////////////////////////////////////

CAnalyzer::CAnalyzer( CString& str )
{
	m_pParser = new CParser( str, FALSE );
	m_pProgram = NULL;
	m_SymbolTable.deleteHashTable();
	m_FunArgs.deleteList();
	m_global.LoadString(IDS_A_GLOBAL);

	location = 0;
}

CAnalyzer::~CAnalyzer()
{
	if( m_pParser ) delete m_pParser;
}

// build the symbol table
void CAnalyzer::BuildSymbolTable( CTreeNode* pNode )
{
	Logic_TraceMsg( IDS_T_SYMBOL_BUILDING );
	traverse( pNode, insertNode, nullProc );
}

// Procedure typeCheck performs type checking by a postorder syntax tree traversal
void CAnalyzer::typeCheck( CTreeNode* pNode )
{
	Logic_TraceMsg( IDS_T_BEGIN_TYPE );
	traverse( pNode, nullProc, checkNode );
	// after semantic analysis, check if main() exists
	CString _main; _main.LoadString( IDS_A_MAIN );
	if( m_SymbolTable.st_lookup( _main, m_global ) == -1 )
		Logic_OutputMsg( IDS_E__UNRESOLVED_MAIN );
}

// trace the symbol table
BOOL CAnalyzer::Run()
{
	Logic_SetErrorFlag( FALSE );
	Logic_TraceMsg( IDS_T_BEGIN_BUILDING );
	m_pProgram = m_pParser->BuildSyntaxTree();
	if( Logic_GetErrorFlag() ) {
		Logic_OutputMsg( IDS_E_STOP_BUILDING );
		return FALSE;
	} else {
		Logic_TraceMsg( IDS_T_SUCCESS_BUILDING );
	}
	BuildSymbolTable( m_pProgram );
	if( Logic_GetErrorFlag() ) {
		Logic_OutputMsg(IDS_E_STOP_SYMBOL);
		return FALSE;
	}
	if (!m_SymbolTable.print())
	{
		Logic_OutputMsg(IDS_T_NO_SYMBOL);
		return FALSE;
	}
	return TRUE;
}

// trace type checking
BOOL CAnalyzer::TypeCheck()
{
	Logic_SetErrorFlag(FALSE);
	m_pProgram = m_pParser->BuildSyntaxTree();
	if( Logic_GetErrorFlag() ) {
		Logic_OutputMsg( IDS_E_STOP_BUILDING );
		return FALSE;
	}
	BuildSymbolTable( m_pProgram );
	if( Logic_GetErrorFlag() ) {
		Logic_OutputMsg( IDS_E_STOP_SYMBOL );
		return FALSE;
	}
	typeCheck( m_pProgram );
	if( Logic_GetErrorFlag() ) {
		Logic_OutputMsg( IDS_E_STOP_TYPE );
		return FALSE;
	}
	Logic_TraceMsg( IDS_T_SUCCESS_TYPE );
	return TRUE;
}
 
// Procedure traverse is a generic recursive syntax tree traversal routine:
// it applies preProc in preorder and postProc in postorder to tree pointed to by t
void CAnalyzer::traverse( CTreeNode* t, pProc preProc, pProc postProc )
{
	if( t ) {
		preProc( t );
		for( int i = 0; i < MAX_CHILDREN; i++ )
			traverse( t->child[i], preProc, postProc );
		postProc( t );
		traverse( t->sibling, preProc, postProc );
	}
}

// nullProc is a do-nothing procedure to generate preorder-only or
// postorder-only traversals from traverse
void CAnalyzer::nullProc( CTreeNode* t )
{
	return;
}

// Procedure insertNode inserts identifiers stored in t into 
// the symbol table 
void CAnalyzer::insertNode( CTreeNode* t )
{
	ASSERT( t );
	switch( t->nodekind ) {
	case kFunDec:
		if( m_SymbolTable.st_lookup( t->szName, t->szScope ) == -1 ) {
			// not defined, so add it to the symbol table
			m_SymbolTable.st_insert( t->szName, t->szScope, t->type, t->lineno, location++ );
			// add it to function declaration list
			m_FunArgs.fa_insert( t );
		} else // redefinition
			Logic_OutputMsg( IDS_E__REDEF_FUNC, t->lineno, t->szName );
		break;
	case kVarDec:
	case kParam:
		if( m_SymbolTable.st_lookup( t->szName, t->szScope ) == -1 )
			// not defined, so add it to the symbol table
			m_SymbolTable.st_insert( t->szName, t->szScope, t->type, t->lineno, location++, t->bArray );
		else // redefinition
			Logic_OutputMsg( IDS_E__REDEF_VAR, t->lineno, t->szName );
		break;
	case kStmt:
		switch( t->kind.stmt ) {
		case kLabel:
			if( m_SymbolTable.st_lookup( t->szName, t->szScope ) == -1 )
				// first time encountered in the scope, add it to the symbol table
				m_SymbolTable.st_insert( t->szName, t->szScope, _LABEL, t->lineno, location++ );
			else // label redifition
				Logic_OutputMsg( IDS_E__REDEF_LABEL, t->lineno, t->szName, t->szScope );
			break;
		case kGoto:
			if( m_SymbolTable.st_lookup( t->szName, t->szScope ) == -1 )
				// label undeclared
				Logic_OutputMsg( IDS_E__UNDEC_LABEL, t->lineno, t->szName, t->szScope );
			else
				m_SymbolTable.st_insert( t->szName, t->szScope, _LABEL, t->lineno, 0 );
			break;
		case kCall:
			if( m_SymbolTable.st_lookup( t->szName, t->szScope ) == -1 )
				Logic_OutputMsg( IDS_E__UNRESOLVED_SYMBOL, t->lineno, t->szName );
			else
				m_SymbolTable.st_insert( t->szName, t->szScope, _ID, t->lineno, 0 );
		default:
			break;
		}
		break;
	case kExp:
		switch( t->kind.exp ) {
		case kID:
			if( m_SymbolTable.st_lookup( t->szName, t->szScope ) == -1 &&
				m_SymbolTable.st_lookup( t->szName, m_global ) == -1 )
				// undeclared
				Logic_OutputMsg( IDS_E__UNDEC_VAR, t->lineno, t->szName );
			else if( m_SymbolTable.st_lookup( t->szName, t->szScope ) != -1 ) {
				// local variable
				if( t->father && (t->father->nodekind != kStmt || t->father->kind.stmt != kCall)/* not in call statement */ &&
					t->bArray != m_SymbolTable.st_lookup_isarray(t->szName, t->szScope) ) {
					// one is array but the other is not
					Logic_OutputMsg( IDS_E__DECL_ARRAY, t->lineno, t->szName, t->bArray ? _T("²»ÊÇ") : _T("Îª") );
					break;
				}
				m_SymbolTable.st_insert( t->szName, t->szScope, t->type, t->lineno, 0 );
			} else { // global variable
				t->szScope = m_global;
				if( t->father && (t->father->nodekind != kStmt || t->father->kind.stmt != kCall)/* not in call statement */ &&
					t->bArray != m_SymbolTable.st_lookup_isarray(t->szName, t->szScope) ) {
					// one is array but the other is not
					Logic_OutputMsg( IDS_E__DECL_ARRAY, t->lineno, t->szName, t->bArray ? " not " : " " );
					break;
				}
				m_SymbolTable.st_insert( t->szName, t->szScope, t->type, t->lineno, 0 );
			}
			break;
		default:
			break;
		}
	default:
		break;
	}
}

// Procedure checkNode performs type checking at a single tree node
void CAnalyzer::checkNode( CTreeNode* t )
{
	ASSERT(t);
	CString m_global; m_global.LoadString(IDS_A_GLOBAL);

	CTreeNode* p = t;
	int ret;

	switch( t->nodekind ) {
	case kStmt:
		switch( t->kind.stmt ) {
		case kReturn:
			if( t->child[0] == NULL ) { // 'return' returns 'void'
				if( m_SymbolTable.st_lookup_type(t->szName, m_global) != _VOID )
					Logic_OutputMsg( IDS_E__FUNC_RETURN, t->lineno, t->szName );
			}
			break;
		case kBreak:
			while( p->father && (p->father->nodekind != kStmt || p->father->kind.stmt != kWhile) )
				p = p->father;
			if( p->father == NULL )
				// 'break' is not within a while statment
				Logic_OutputMsg( IDS_E__ILLEGAL_BREAK, t->lineno );
			break;
		case kContinue: // treat it like kBreak
			while( p->father && (p->father->nodekind != kStmt || p->father->kind.stmt != kWhile) )
				p = p->father;
			if( p->father == NULL )
				// 'continue' is not within a while statment
				Logic_OutputMsg( IDS_E__ILLEGAL_CONTINUE, t->lineno );
			break;
		case kCall:
			// check if its arguments match declaration
			ret = m_FunArgs.fa_check( t );
			if( ret != -3 ) {// errors 
				if( ret >= 0 )
					Logic_OutputMsg( IDS_E__FUNC_PARA, t->lineno, t->szName, ret );
				else if( ret == -1 )
					Logic_OutputMsg( IDS_E__UNDEC_FUNC, t->lineno, t->szName );
				else
					Logic_OutputMsg( IDS_E__FUNC_PARA_MISMATCH, t->lineno, t->szName );
				break;
			}
			t->type = m_SymbolTable.st_lookup_type( t->szName, t->szScope );
			break;
		default:
			break;
		}
		break;
	case kExp:
		switch( t->kind.exp ) {
		case kOp:// assign a type to this op node
			if( t->type == LOGICAL_NOT || t->type == ASSIGN )
				t->type = t->child[0]->type;
			else {
				// the others are binary operations, 
				// and the type should be the subexpression with higher precision
				if( t->child[0]->type == _VOID || t->child[1]->type == _VOID )
					Logic_OutputMsg( IDS_E__ILLEGAL_USE_VOID, t->lineno );
				else if( t->child[0]->type == _FLOAT || t->child[1]->type == _FLOAT )
					t->type = _FLOAT;
				else if( t->child[0]->type == _INT || t->child[1]->type == _INT )
					t->type = _INT;
				else
					t->type = _CHAR;
			}
			break;
		case kID:
			// find the type in the symbol table, assign it to the node
			t->type = m_SymbolTable.st_lookup_type( t->szName, t->szScope );
			t->bArray = m_SymbolTable.st_lookup_isarray( t->szName, t->szScope );
			break;
		default:
			break;
		}
	default:
		break;
	}
}
