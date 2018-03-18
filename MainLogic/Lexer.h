#pragma once
#include "Token.h"
#include "Interfaces.h"

#define MAX_KEYWORDS 100

namespace Logic_Global
{
	enum LTokenType
	{
		// 保留关键字
		_EXTERN, _STATIC, _SIZEOF,
		_AUTO, _CONST, _TYPEDEF, _DEFAULT,
		_STRUCT, _ENUM, _CLASS, _UNION,
		_SIGNED, _UNSIGNED,
		_VOID,
		_CHAR, _INT, _DOUBLE, _FLOAT, _LONG,
		_VOLATILE, _MUTABLE, _REGISTER,
		_IF, _ELSE, _FOR, _BREAK, _WHILE, _DO, _CONTINUE, _SWITCH, _CASE, _GOTO,
		_RETURN,

		// 函数

		// 操作符
		ASSIGN, PLUS, MINUS, TIMES, DIV, MOD,
		BITWISE_AND, BITWISE_OR, BITWISE_NOT, LOGICAL_NOT, LT, GT,

		// 标点符号
		LPARAN, RPARAN, LBRACE, RBRACE, LSQUARE, RSQUARE, COMMA, DOT, SEMI, COLON,

		// 二元操作符
		EQ					/* == */,
		NEQ					/* != */,
		PLUS_PLUS			/* ++ */,
		MINUS_MINUS			/* -- */,
		PLUS_ASSIGN			/* += */,
		MINUS_ASSIGN		/* -= */,
		TIMES_ASSIGN		/* *= */,
		DIV_ASSIGN			/* /= */,
		NGT					/* <= */,
		NLT					/* >= */,
		LOGICAL_AND			/* && */,
		LOGICAL_OR			/* || */,

		// 其他
		_EOF, _ID, _NUM, _STRING, _CHARACTER, _LABEL, _ERROR, _NONE,
	};
}

struct TOKEN
{
	Logic_Global::LTokenType		type;
	CString							str;
};

class AFX_EXT_CLASS CLexer : public CToken
{
private:
	LIST_ARRAY*			m_pList;
	BOOL				m_bPushedBack;
	TOKEN				m_token;
	CMapStringToPtr		m_KeyIndex;

public:
	explicit CLexer( CString& str, BOOL traces = TRUE );
	virtual ~CLexer();

	void				MapKeyword(); // Generate keyword map
	void				PushBack();
	TOKEN&				NextToken();
	BOOL				Run();
};

