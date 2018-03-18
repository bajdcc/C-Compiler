#include "stdafx.h"
#include "Resource.h"
#include "Token.h"
#include "Logic.h"


CToken::CToken(CString& string)
{
	m_bPushedBack = FALSE;
	m_bEolIsSignificant = TRUE;
	m_bSlSlComments = TRUE;
	m_bSlStComments = TRUE;
	m_bForceLower = FALSE;				// case sensitive
	m_iLineNo = 1;						// the first line
	m_iChar = 0;
	m_nToken = string.GetLength();
	m_peekc = ' ';
	m_sString = string;
	m_sString += (TCHAR) TT_EOF;
}

CToken::~CToken()
{
}

inline TCHAR CToken::GetChar()
{
	if (m_iChar >= m_nToken)
		return TT_EOF;
	return m_sString[m_iChar++];
}

void CToken::PushBack()
{
	m_bPushedBack = TRUE;
}

// Specifies that EOL is significant or not
void CToken::EolIsSignificant(BOOL bFlag)
{
	m_bEolIsSignificant = bFlag;
}

// Enable C Style comments
void CToken::SlStComments(BOOL bFlag)
{
	m_bSlStComments = bFlag;
}

// Enable C++ Style comments
void CToken::SlSlComments(BOOL bFlag)
{
	m_bSlSlComments = bFlag;
}

// Enable / disable case sensitivity
void CToken::LowerCaseMode(BOOL bFlag)
{
	m_bForceLower = bFlag;
}

// gives the next Token, returns the token type
int CToken::NextToken()
{
	m_tType = InternalNextToken();
	return m_tType;
}

int CToken::InternalNextToken()
{
	if( m_bPushedBack ) {
		m_bPushedBack = FALSE;
		return m_tType;
	}

	int c = m_peekc;
	m_sVal = _T("");
	
	if ( c == TT_EOF ) return TT_EOF;
	
	// is this a space 
	while( _istspace( c ) ) {
		if( c == _T('\r') ) {
			m_iLineNo++;
			c = GetChar();
			if( c == _T('\n') ) c = GetChar();
			if( m_bEolIsSignificant ) {
				m_peekc = c;
				return TT_EOL;
			}
		} else {
			if( c == _T('\n') ) {
				m_iLineNo++;
				if( m_bEolIsSignificant ) {
					m_peekc = _T(' ');
					return TT_EOL;
				}
			}
			c = GetChar();
		}

		if ( c == TT_EOF ) return TT_EOF;
	}
	
	// is this a number
	if( _istdigit( c ) || c == _T('-') ) {
		BOOL neg = FALSE;
		if( c == _T('-') )	{
			c = GetChar();
			if( c != _T('.') && !_istdigit( c ) ) {
				m_peekc = c;
				return  _T('-');
			}
			neg = TRUE;
		}
		double v = 0;
		int decexp = 0;
		int seendot = 0;
		while( true ) {
			if (c == _T('.') && seendot == 0)
				seendot = 1;
			else if ( _istdigit( c ) ) {
				v = v * 10 + (c - _T('0'));
				decexp += seendot;
			} else
				break;
			c = GetChar();
		}
		m_peekc = c;
		if( decexp != 0 ) {
			double denom = 10;
			decexp --;
			while( decexp > 0 ) {
				denom *= 10;
				decexp --;
			}
			v = v / denom;
		} else if( seendot == 1 ) {
			m_iChar --;
			m_peekc = _T('.');
			seendot = 0;
		}
		m_dVal = neg ? -v : v;
		if( seendot == 0 )
			return TT_INTEGER;
		else
			return TT_REAL;
	}
	
	// is this a word
	if( _istalpha( c ) || c == _T('_') ) {
		int i = 0;
		m_sVal = _T("");
		do {
			m_sVal = m_sVal + (TCHAR)c;
			c = GetChar();
		} while ( _istalnum( c ) || c == _T('_') );
		m_peekc = c;
		if( m_bForceLower )
			m_sVal.MakeLower();
		return TT_WORD;
	}

	// now the char & string
	if( c == _T('\'') || c == _T('"') ) {
		m_sVal = _T("");
		m_tType = c; // enclosed tag
		m_peekc = ' ';
		int i = 0, c2;
		while( (c = GetChar()) != TT_EOF && c != m_tType && c != _T('\n') && c != _T('\r') ) {
			if( c == _T('\\') )// escape
				switch( c = GetChar() ) {
				case _T('a'): c = _T('\x7');	break;
				case _T('b'): c = _T('\b');		break;
				case _T('f'): c = _T('\xC');	break;
				case _T('n'): c = _T('\n');		break;
				case _T('r'): c = _T('\r');		break;
				case _T('t'): c = _T('\t');		break;
				case _T('v'): c = _T('\xB');	break;
				case _T('0'):
				case _T('1'):
				case _T('2'):
				case _T('3'):
				case _T('4'):
				case _T('5'):
				case _T('6'):
				case _T('7'):
					c = c - _T('0');
					c2 = GetChar();
					if( c2 == m_tType ) {
						m_sVal += (TCHAR)c;
						return m_tType;
					}
					if( _T('0') <= c2 && c2 <= _T('7') ) { // octal
						c = (c << 3) + (c2 - _T('0'));
						c2 = GetChar();
						if( c2 == m_tType ) {
							m_sVal += (TCHAR)c;
							return m_tType;
						}
						if( _T('0') <= c2 && c2 <= _T('7'))
							c = (c << 3) + (c2 - _T('0'));
						else {
							m_sVal += (TCHAR)c;
							c = c2;
						}
					} else {
						m_sVal += (TCHAR)c;
						c = c2;
					}
					break;
				default:
					// warning: 'c' : unrecognized character escape sequence
					Logic_OutputMsg( IDS_E__SYNTAX_ERROR, m_iLineNo );
				}
			m_sVal += (TCHAR)c;
		}
		if( c == TT_EOF ) {
			// error msg: syntax error in line %d: missing _T('"')
			Logic_OutputMsg( IDS_E__MISSING_QUOTATION, m_iLineNo );
		} else if( c == _T('\r') || c == _T('\n') ) {
			// error msg: syntax error in line %d: new line in constant
			Logic_OutputMsg( IDS_E__NEW_LINE_IN_CONSTANT, m_iLineNo );
		}

		return m_tType;
	}

	// and now the comment
	// "//" or "/*...*/"
	if( c == _T('/') && ( m_bSlSlComments || m_bSlStComments ) ) {
		c = GetChar();
		if( c == _T('*') && m_bSlStComments ) {
			int prevc = 0;
			while( (c = GetChar()) != _T('/') || prevc != _T('*') ) {
				if( c == _T('\n') )	m_iLineNo++;
				if( c == TT_EOF ) return TT_EOF;
				prevc = c;
			}
			m_peekc = _T(' ');
			return InternalNextToken();
		} else {
			if (c == _T('/') && m_bSlSlComments) {
				while ( (c = GetChar()) != _T('\n') && c != _T('\r') );
				m_peekc = c;
				return InternalNextToken();
			} else {
				m_peekc = c;
				return _T('/');
			}
		}
	}

	m_peekc = _T(' ');
	return c;
}

int CToken::LineNo() const
{
	return m_iLineNo;
}

CString CToken::GetStrValue() const
{
	CString ret;
	switch (m_tType)
	{
	case TT_EOF:
		ret = _T("EOF");
		break;
	case TT_EOL:
		ret = _T("EOL");
		break;
	case TT_WORD:
		ret = m_sVal;
		break;
	case TT_STRING:
		ret = m_sVal;
		break;
	case TT_INTEGER:
	case TT_REAL:
		ret.Format(_T("%g"), m_dVal);
		break;
	default:
		ret.Format(_T("\'%c\'"), (TCHAR) m_tType) ;
	}
	return ret;
}

double CToken::GetDoubleNumValue() const
{
	return m_dVal;
}

int CToken::GetIntNumValue() const
{
	return (int) m_dVal;
}

