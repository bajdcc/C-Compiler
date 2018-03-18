#include "stdafx.h"
#include "MainIDEViewBase.h"

//////////////////////////////////////////////////////////////////////////

#define COOKIE_COMMENT			0x0001
#define COOKIE_PREPROCESSOR		0x0002
#define COOKIE_EXT_COMMENT		0x0004
#define COOKIE_STRING			0x0008
#define COOKIE_CHAR				0x0010

#define MAX_WORD_LEN			20

#define DEFINE_BLOCK(pos, colorindex)	\
	ASSERT((pos) >= 0 && (pos) <= nLength);\
	if (pBuf != NULL)\
{\
	if (nActualItems == 0 || pBuf[nActualItems - 1].m_nCharPos <= (pos)){\
	pBuf[nActualItems].m_nCharPos = (pos);\
	pBuf[nActualItems].m_nColorIndex = (colorindex);\
	nActualItems ++;}\
}

BOOL CMainIDEViewBase::IsCKeyword(LPCTSTR pszChars, int nLength)
{
	static TCHAR pstr[MAX_WORD_LEN + 1]; static void* p;
	if (nLength > MAX_WORD_LEN) nLength = MAX_WORD_LEN;
	_tcsncpy_s(pstr, pszChars, nLength); pstr[nLength] = _T('\0');
	return m_mapKeywords.Lookup(pstr, p);
}

BOOL CMainIDEViewBase::IsCType(LPCTSTR pszChars, int nLength)
{
	static TCHAR pstr[MAX_WORD_LEN + 1]; static void* p;
	if (nLength > MAX_WORD_LEN) nLength = MAX_WORD_LEN;
	_tcsncpy_s(pstr, pszChars, nLength); pstr[nLength] = _T('\0');
	return m_mapTypes.Lookup(pstr, p);
}

BOOL CMainIDEViewBase::IsCNumber(LPCTSTR pszChars, int nLength)
{
	if( nLength > 2 && pszChars[0] == '0' && pszChars[1] == 'x' ) {
		for( int I = 2; I < nLength; I++ ) {
			if (_istdigit(pszChars[I]) || (pszChars[I] >= 'A' && pszChars[I] <= 'F') ||
				(pszChars[I] >= 'a' && pszChars[I] <= 'f'))
				continue;
			return FALSE;
		}
		return TRUE;
	}
	if( !_istdigit(pszChars[0]) ) return FALSE;
	for( int I = 1; I < nLength; I++ ) {
		if( !_istdigit(pszChars[I]) && pszChars[I] != '+' &&
			pszChars[I] != '-' && pszChars[I] != '.' && pszChars[I] != 'e' &&
			pszChars[I] != 'E' )
			return FALSE;
	}
	return TRUE;
}

DWORD CMainIDEViewBase::ParseLine( DWORD dwCookie, int nLineIndex, TEXTBLOCK *pBuf, int &nActualItems )
{
	int nLength = GetLineLength( nLineIndex );
	if( nLength <= 0 )
		return dwCookie & COOKIE_EXT_COMMENT;

	LPCTSTR pszChars    = GetLineChars(nLineIndex);
	BOOL bFirstChar     = (dwCookie & ~COOKIE_EXT_COMMENT) == 0;
	BOOL bRedefineBlock = TRUE;
	BOOL bDecIndex  = FALSE;
	int I, nIdentBegin = -1;
	for( I = 0; ; I++ ) {
		if( bRedefineBlock ) {
			int nPos = I;
			if( bDecIndex )	nPos--;
			if( dwCookie & (COOKIE_COMMENT | COOKIE_EXT_COMMENT) ) {
				DEFINE_BLOCK(nPos, COLORINDEX_COMMENT);
			} else if( dwCookie & (COOKIE_CHAR | COOKIE_STRING) ) {
				DEFINE_BLOCK(nPos, COLORINDEX_STRING);
			} else if( dwCookie & COOKIE_PREPROCESSOR )	{
				DEFINE_BLOCK(nPos, COLORINDEX_PREPROCESSOR);
			} else {
				DEFINE_BLOCK(nPos, COLORINDEX_NORMALTEXT);
			}
			bRedefineBlock = FALSE;
			bDecIndex      = FALSE;
		}

		if( I == nLength ) break;

		if( dwCookie & COOKIE_COMMENT ) {
			DEFINE_BLOCK(I, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			break;
		}

		// String constant "....", end
		if( dwCookie & COOKIE_STRING ) {
			if( pszChars[I] == _T('"') && (I == 0 || pszChars[I - 1] != _T('\\')) )	{
				dwCookie &= ~COOKIE_STRING;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		// Char constant '..', end
		if( dwCookie & COOKIE_CHAR ) {
			if( pszChars[I] == _T('\'') && (I == 0 ||
				(I >= 2 && pszChars[I - 1] == _T('\\') && pszChars[I - 2] == _T('\\')) ||
				pszChars[I - 1] != _T('\\')) ) {
				dwCookie &= ~COOKIE_CHAR;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		// Extended comment /*....*/, end
		if( dwCookie & COOKIE_EXT_COMMENT ) {
			if( I > 0 && pszChars[I] == _T('/') && pszChars[I - 1] == _T('*') ) {
				dwCookie &= ~COOKIE_EXT_COMMENT;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		// Comment double slash //
		if( I > 0 && pszChars[I] == _T('/') && pszChars[I - 1] == _T('/') ) {
			DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_COMMENT;
			break;
		}

		// Preprocessor directive #....
		if( dwCookie & COOKIE_PREPROCESSOR ) {
			if( !_istalpha(pszChars[I]) ) {
				dwCookie &= ~COOKIE_PREPROCESSOR;
				bRedefineBlock = TRUE;
			}
			continue;
		}

		// String constant "...."
		if( pszChars[I] == _T('"') ) {
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_STRING;
			continue;
		}

		// Char constant '..'
		if( pszChars[I] == _T('\'') ) {
			DEFINE_BLOCK(I, COLORINDEX_STRING);
			dwCookie |= COOKIE_CHAR;
			continue;
		}

		// Extended comment /*....*/
		if( I > 0 && pszChars[I] == _T('*') && pszChars[I - 1] == _T('/') ) {
			DEFINE_BLOCK(I - 1, COLORINDEX_COMMENT);
			dwCookie |= COOKIE_EXT_COMMENT;
			continue;
		}

		if( bFirstChar ) {
			if( pszChars[I] == _T('#') ) {
				DEFINE_BLOCK(I, COLORINDEX_PREPROCESSOR);
				dwCookie |= COOKIE_PREPROCESSOR;
				continue;
			}
			if( !_istspace(pszChars[I]) )
				bFirstChar = FALSE;
		}

		if( pBuf == NULL )
			continue;	//	We don't need to extract keywords,
						//	for faster parsing skip the rest of loop

		// Operators
		if( pszChars[I] == _T('+') || pszChars[I] == _T('-') || pszChars[I] == _T('=') || 
			pszChars[I] == _T('<') || pszChars[I] == _T('>') || pszChars[I] == _T('!') ||
			pszChars[I] == _T('|') || pszChars[I] == _T('&') || pszChars[I] == _T('^') ) {
			DEFINE_BLOCK(I, COLORINDEX_OPERATOR);
			bRedefineBlock = TRUE;
			continue;
		}

		if( pszChars[I] == _T('*') ||
			(pszChars[I] == _T('/') && (I < (nLength - 1) && pszChars[I + 1] != _T('/') && pszChars[I + 1] != _T('*'))) ) {
			DEFINE_BLOCK(I, COLORINDEX_OPERATOR);
			bRedefineBlock = TRUE;
			continue;
		}

		if( _istalnum(pszChars[I]) || pszChars[I] == _T('_') || pszChars[I] == _T('.') ) {
			if (nIdentBegin == -1) nIdentBegin = I;
		} else {
			if( nIdentBegin >= 0 ) {
				if( IsCKeyword(pszChars + nIdentBegin, I - nIdentBegin) ) {
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
				} else if( IsCNumber(pszChars + nIdentBegin, I - nIdentBegin) ) {
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
				} else if( IsCType(pszChars + nIdentBegin, I - nIdentBegin) ) {
					DEFINE_BLOCK(nIdentBegin, COLORINDEX_TYPE);
				}
				bRedefineBlock = TRUE;
				bDecIndex = TRUE;
				nIdentBegin = -1;
			}
		}
	}

	if( nIdentBegin >= 0 ) {
		if( IsCKeyword(pszChars + nIdentBegin, I - nIdentBegin)) {
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_KEYWORD);
		} else if( IsCNumber(pszChars + nIdentBegin, I - nIdentBegin) ) {
			DEFINE_BLOCK(nIdentBegin, COLORINDEX_NUMBER);
		}
	}

	if( pszChars[nLength - 1] != _T('\\') )	dwCookie &= COOKIE_EXT_COMMENT;

	return dwCookie;
}
