#pragma once

// Token types
#define TT_EOL		_T('\n')
#define TT_EOF		((TCHAR)-1)
#define TT_INTEGER	((TCHAR)-2)
#define TT_REAL		((TCHAR)-3)
#define TT_WORD		((TCHAR)-4)
#define TT_STRING	_T('"')
#define TT_CHAR		_T('\'')


/*
 * CToken ´Ê·¨·ÖÎö
 *
 */
class AFX_EXT_CLASS CToken : public CObject
{
protected:
	CString					m_sString;						// the tokenized string

	BOOL					m_bSlSlComments;				// Slash slash comments enabled
	BOOL					m_bSlStComments;				// Slash star comments enabled
	BOOL					m_bEolIsSignificant;			// Specifies that EOL is significant or not
	BOOL					m_bForceLower;					// Enable / disable case sensitivity
	BOOL					m_bPushedBack;					// Enable Pushed Back

	int						m_peekc;
	int						m_iLineNo;
	int						m_tType;						// The last read token type

	CString					m_sVal;							// the value of the token
	double					m_dVal;
	int						m_iChar;						// the index of the current character
	int						m_nToken;						// length of text

public:
	CToken( CString& str );
	virtual ~CToken();

private:
	TCHAR					GetChar();
	int						InternalNextToken();

public:
	double					GetDoubleNumValue() const;
	int						GetIntNumValue() const;	
	virtual CString			GetStrValue() const;

	int						LineNo() const;
	void					PushBack();
	int						NextToken();					// gives the next Token, returns the token type

	void					LowerCaseMode(BOOL bFlag);		// Enable / disable case sensitivity
	void					SlSlComments(BOOL bFlag);		// Enable C++ Style comments
	void					SlStComments(BOOL bFlag);		// Enable C Style comments
	void					EolIsSignificant(BOOL bFlag);	// Specifies that EOL is significant or not
};


