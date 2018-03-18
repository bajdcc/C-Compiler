#include "StdAfx.h"
#include "Resource.h"
#include "Lexer.h"
#include "Logic.h"


CLexer::CLexer( CString& str, BOOL traces /*= TRUE*/ ): CToken(str)
{
	m_bPushedBack = FALSE;
	m_token.type = Logic_Global::_ID;
	m_pList = traces ? Logic_GetDataPtr() : NULL;

	MapKeyword();
}

CLexer::~CLexer()
{

}

//////////////////////////////////////////////////////////////////////////

void CLexer::MapKeyword()
{
	m_KeyIndex.RemoveAll();
	m_KeyIndex.InitHashTable( MAX_KEYWORDS );

	LPTSTR* s_apszCReservedKeywords = Logic_GetRevKeywords();
	for( int L = 0; s_apszCReservedKeywords[L] != NULL; L++ ) {
		m_KeyIndex[s_apszCReservedKeywords[L]] = (void*)L;
	}
}

BOOL CLexer::Run()
{
	Logic_TraceMsg( IDS_T_LEXER_BEGIN );
	TOKEN token;
	do{
		token = NextToken();
	} while( token.type != Logic_Global::_EOF );

	if (Logic_GetErrorFlag())
	{
		Logic_OutputMsg( IDS_E_LEXER );
		Logic_SetDataPtr(NULL);
		return FALSE;
	}

	Logic_TraceMsg( IDS_T_LEXER );
	return TRUE;
}

void CLexer::PushBack()
{
	m_bPushedBack = TRUE;
}

TOKEN& CLexer::NextToken()
{
	using namespace Logic_Global;

	if( m_bPushedBack ) {
		m_bPushedBack = FALSE;
		return m_token;
	}
	if( m_token.type == _EOF ) return m_token;

	LTokenType type;
	int val = CToken::NextToken(); // get token
	int	lineno = LineNo(); // the actual line the token is got from

	m_token.type = _ERROR;
	m_token.str = _T("Error");

	if( val == TT_EOF ) {
		m_token.type = _EOF;
		m_token.str = _T("EOF");
		return m_token;
	}

	if( val == TT_WORD ) {
		if( m_KeyIndex.Lookup( m_sVal, (void*&)type ) )
			m_token.type = type;
		else 
			m_token.type = _ID;
		m_token.str = m_sVal;
	} else if( val == TT_INTEGER || val == TT_REAL ) {
		m_token.type = _NUM;
		m_token.str = GetStrValue();
	} else if( val == TT_STRING ) {
		m_token.type = _STRING;
		m_token.str = m_sVal;
	} else if( val == TT_CHAR ) {
		m_token.type = _CHARACTER;
		m_token.str = (m_sVal.IsEmpty()) ? _T(" ") : CString( m_sVal[0] );
	} else if( val == TT_EOL ) {
		return NextToken();
	} else if( _tcschr( _T("=+-*/&|~!<>(){}[],.;:"), val ) ) {
		m_token.str = (TCHAR)val;
		m_token.type = (LTokenType)(UINT)m_KeyIndex[ m_token.str ];

		// complex operations
		switch( val ) {
		case _T('='):
			if( CToken::NextToken() == _T('=') ) {
				m_token.str = _T("==");
				m_token.type = EQ;
			} else
				CToken::PushBack();
			break;
		case _T('!'):
			if( CToken::NextToken() == _T('=') ) {
				m_token.str = _T("!=");
				m_token.type = NEQ;
			} else
				CToken::PushBack();
			break;
		case _T('+'):
			if( CToken::NextToken() == _T('=') ) {
				m_token.str = _T("+=");
				m_token.type = PLUS_ASSIGN;
			} else {
				CToken::PushBack();
				if( CToken::NextToken() == _T('+') ) {
					m_token.str = _T("++");
					m_token.type = PLUS_PLUS;
				} else
					CToken::PushBack();
			}
			break;
		case _T('-'):
			if( CToken::NextToken() == _T('=') ) {
				m_token.str = _T("-=");
				m_token.type = MINUS_ASSIGN;
			} else {
				CToken::PushBack();
				if( CToken::NextToken() == _T('-') ) {
					m_token.str = _T("--");
					m_token.type = MINUS_MINUS;
				} else
					CToken::PushBack();
			}
			break;
		case _T('*'):
			if( CToken::NextToken() == _T('=') ) {
				m_token.str = _T("*=");
				m_token.type = TIMES_ASSIGN;
			} else
				CToken::PushBack();
			break;
		case _T('/'):
			if( CToken::NextToken() == _T('=') ) {
				m_token.str = _T("/=");
				m_token.type = DIV_ASSIGN;
			} else
				CToken::PushBack();
			break;
		case _T('<'):
			if( CToken::NextToken() == _T('>') ) {
				m_token.str = _T("<>");
				m_token.type = NEQ;
			} else {
				CToken::PushBack();
				if( CToken::NextToken() == _T('=') ) {
					m_token.str = _T("<=");
					m_token.type = NGT;
				} else
					CToken::PushBack();
			}
			break;
		case _T('>'):
			if( CToken::NextToken() == _T('=') ) {
				m_token.str = _T(">=");
				m_token.type = NLT;
			} else
				CToken::PushBack();
			break;
		case _T('&'):
			if( CToken::NextToken() == _T('&') ) {
				m_token.str = _T("&&");
				m_token.type = LOGICAL_AND;
			} else
				CToken::PushBack();
			break;
		case _T('|'):
			if( CToken::NextToken() == _T('|') ) {
				m_token.str = _T("||");
				m_token.type = LOGICAL_OR;
			} else
				CToken::PushBack();
			break;
		}
	} else {
		Logic_OutputMsg( IDS_E__SYNTAX_ERROR, LineNo(), (TCHAR)val );
	}

	// into data
	if (m_pList)
	{
		CString tmp;
		CStringArray* sa = new CStringArray;
		sa->SetSize(3);
		tmp.Format(IDS_FORMAT_D, lineno);
		sa->SetAt(0, tmp);
		tmp.Format(_T("%02d"), m_token.type);
		sa->SetAt(1, tmp);
		sa->SetAt(2, m_token.str);
		m_pList->AddTail(sa);
	}

	return m_token;
}

