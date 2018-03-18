#include "StdAfx.h"
#include "Resource.h"
#include "Logic.h"
#include "TreeNode.h"
#include "Lexer.h"
#include "Parser.h"

// 文法分析是编译器的核心
// 采用自顶向下的递归下降分析方法

/* 文法生成

	主入口		program				->		declaration_list
	声明列表	declaration_list	->		declaration_list declaration
										  | declaration
	声明		declaration			->		var_declaration
										  | fun_declaration
	变量声明	var_declaration		->		type_specifier @ID(, ...) `;`
										  | type_specifier @ID `[` NUM `]` (, ...) `;`
	变量类型	type_specifier		->		`int`
										  | `void`
										  | `char`
	函数声明	fun_declaration		->		type_specifier @ID `(` params `)` compound_stmt
	参数		params				->		param_list
										  | `void`
										  | empty
	参数		param_list			->		param_list `,` param
										  | param
	参数		param				->		type_specifier ID
										  | type_specifier ID `[` `]`
	函数体		compound_stmt		->		`{` local_declarations statement_list `}`
										  | expression_stmt
	函数内声明	local_declarations	->		local_declarations var_declaration
										  | var_declaration
	表达式语句	expression_stmt		->		expression `;`
										  | `;`
	表达式		expression			->		var `=` expression
										  | logic1_expression
				logic1_expression	->		logic1_expression `||` logic2_expression
										  | logic2_expression
				logic2_expression	->		logic2_expression `&&` simple_expression
										  | simple_expression
	简单表达式	simple_expression	->		additive_expression relop additive_expression
										  | additive_expression
	子表达式	additive_expression ->		additive_expression addop term
										  | term
				term				->		term mulop logic3_expression
										  | logic3_expression
				logic3_expression	->		`!` logic3_expression
										  | factor
				factor				->		`(` expression `)`
										  | var
										  | call
										  | NUM
				var					->		ID
										  | ID `[` expression `]`
				call				->		ID `(` args `)`
				args				->		args_list
										  | empty
				args_list			->		args_list `,` expression
										  | expression
				sub_compoundstmt	->		ID `:`
										  | call `;`
										  | expression_stmt

	逻辑语句

	if			if_stmt				->		`if` `(` expression `)` compound_stmt
										  | `if` `(` expression `)` compound_stmt `else` compound_stmt
	while		while_stmt			->		`while` `(` expression `)` compound_stmt
	for			for_stmt			->		`for` `(` var `=` expression `;` expression `;` var `=` expression `)` compound_stmt
	goto		goto_stmt			->		`goto` ID `;`
	break		break_stmt			->		`break` `;`
	continue	continue_stmt		->		`continue` `;`
	return		return_stmt			->		`return` `;`
										  | `return` expression `;`

	终结符：

	一元操作符	addop	->	`+` | `-`
	一元操作符	mulop	->	`*` | `/` | `%`
	二元操作符	relop	->	`<=` | `<` | `>` | `>=` | `==` | `!=`
*/

//////////////////////////////////////////////////////////////////////////

using namespace Logic_Global;

// Grammar:
// 1. program->declaration_list
CTreeNode* CParser::program()
{
	return declaration_list();
}

// Grammar:
// 2. declaration_list->declaration_list declaration | declaration
CTreeNode* CParser::declaration_list()
{
	CTreeNode *first = NULL, *last = NULL, *temp = NULL;
	m_token = m_pLexer->NextToken();
	while( m_token.type != _EOF ) {
		if( m_token.type != _CHAR && m_token.type != _INT &&
			m_token.type != _VOID && m_token.type != _FLOAT ) {
			Logic_OutputMsg( IDS_E__INVALID_TYPE, m_pLexer->LineNo(), m_token.str );
			ConsumeUntil( SEMI /* ';' */, RBRACE /* '}' */ );
		} else if( (temp = declaration()) != NULL ) {
			// link all declarations together
			if( !first ) { first = temp; last = temp->LastSibling(); }
			else { last->sibling = temp; last = temp->LastSibling(); }
		}
		// read the next token
		m_token = m_pLexer->NextToken();
	}
	return first;
}

// Grammar:
// 3. declaration->var_declaration | fun_declaration
// m_token is a supported type-identifier token
CTreeNode* CParser::declaration()
{
	m_szScope.LoadString(IDS_A_GLOBAL);// global function or variable declaration
	CTreeNode* temp = NULL;

	TypeToken = m_token;
	IDToken = m_token = m_pLexer->NextToken();
	if( IDToken.type != _ID ) {
		Logic_OutputMsg( IDS_E__RESERVED_KEYWORD, m_pLexer->LineNo(), IDToken.str );
		ConsumeUntil( SEMI /* ';' */, RBRACE /* '}' */ );
	} else {
		m_token = m_pLexer->NextToken(); // '(', ';', '[', ',' or error
		if( m_token.type == LPARAN ) temp = fun_declaration();
		else if( m_token.type == SEMI || m_token.type == LSQUARE || m_token.type == COMMA )
			temp = var_declaration();
		else {
			Logic_OutputMsg( IDS_E__MISSING_SEMI_AFTER, m_pLexer->LineNo(), IDToken.str );
			ConsumeUntil( SEMI /* ';' */, RBRACE /* '}' */ );
		}
	}

	return temp;
}

// Grammar:
// 4. var_declaration->type_specifier ID(, ...)`;` | type_specifier ID `[` NUM `]`(, ...)`;`
// 5. type_specifier->`int` | `void` | `char`, actually this step is in declaration_list()
// m_token.str == ";" "," or "["
CTreeNode* CParser::var_declaration()
{
	CTreeNode* temp = newNode( kVarDec, TypeToken.type, IDToken.str );

	if( m_token.type == LSQUARE ) { // '['
		m_token = m_pLexer->NextToken(); // NUM
		if( m_token.type != _NUM ) {
			Logic_OutputMsg( IDS_E__MISSING_ARRAY_SIZE, m_pLexer->LineNo(), IDToken.str );
			delete temp;
			ConsumeUntil( SEMI /* ';' */, RBRACE /* '}' */ );
			return NULL;
		}

		temp->bArray = TRUE;
		temp->iArraySize = m_pLexer->GetIntNumValue();
		
		if( !match(RSQUARE) ) { // `]`
			Logic_OutputMsg( IDS_E__MISSING_ARRAY_SIZE,	m_pLexer->LineNo(), IDToken.str );
			m_pLexer->PushBack();
		}
		m_token = m_pLexer->NextToken(); // should be ';' or ','
	}
	if( m_token.type == COMMA ) {
		IDToken = m_token = m_pLexer->NextToken(); // ID or error
		if( IDToken.type != _ID ) {
			Logic_OutputMsg( IDS_E__RESERVED_KEYWORD, m_pLexer->LineNo(), IDToken.str );
			ConsumeUntil( SEMI /* ';' */, RBRACE /* '}' */ );
			return temp;
		}
		m_token = m_pLexer->NextToken(); // ';', '[', ',' or error
		if( m_token.type == SEMI || m_token.type == LSQUARE || m_token.type == COMMA )
			temp->sibling = var_declaration(); // link following variable declarations
		else {
			Logic_OutputMsg( IDS_E__MISSING_SEMI_AFTER, m_pLexer->LineNo(), IDToken.str );
			m_pLexer->PushBack();
			return temp;
		}
	} else if( m_token.type != SEMI ) { // m_token should be ';' now
		Logic_OutputMsg( IDS_E__MISSING_SEMI, m_pLexer->LineNo() );
		ConsumeUntil( SEMI /* ';' */, RBRACE /* '}' */ );
	}
	
	return temp;
}

// Grammar:
// 6. fun_declaration->type_specifier ID `(` params `)` compound_stmt
// m_token.str == "(", TypeToken contains type_specifier, IDToken contains ID
CTreeNode* CParser::fun_declaration()
{
	CTreeNode* temp = newNode( kFunDec, TypeToken.type, IDToken.str );

	// update function scope
	m_szScope = IDToken.str;

	// params
	CTreeNode* p = temp->child[0] = params();
	if( p ) p->father = temp;
	while( p && p->sibling ) {
		p = p->sibling;	p->father = temp;
	}

	if( !match(RPARAN) ) {
		Logic_OutputMsg( IDS_E__MISSING_RPARAM, m_pLexer->LineNo(), m_token.str );
		m_pLexer->PushBack();
	}
	// compound statements
	p = temp->child[1] = compound_stmt();
	if( p ) p->father = temp;
	while( p && p->sibling ) {
		p = p->sibling; p->father = temp;
	}

	return temp;
}

// Grammar:
// 7. params->param_list | `void` | empty, `void` is thought as empty
// 8. param_list->param_list `,` param | param
// 9. param->type_specifier ID | type_specifier ID `[` `]`
// m_token.str == "("
CTreeNode* CParser::params()
{
	CTreeNode *first = NULL, *temp = NULL;

	TypeToken = m_token = m_pLexer->NextToken(); // type-specifier or ')'
	if( m_token.type == RPARAN ) {
		m_pLexer->PushBack();
		return NULL;
	}
	if( TypeToken.type == _VOID )
		if( match( RPARAN ) ) {
			m_pLexer->PushBack();
			return NULL;
		} else m_pLexer->PushBack(); // is not ')', push it back
	while( TypeToken.type == _INT || TypeToken.type == _CHAR ||
		   TypeToken.type == _VOID || TypeToken.type == _FLOAT ) {
		IDToken = m_token = m_pLexer->NextToken();
		if( IDToken.type != _ID ) {
			Logic_OutputMsg( IDS_E__INVALID_PARAMETER, m_pLexer->LineNo(), IDToken.str );
		} else {
			temp = newNode( kParam, TypeToken.type, IDToken.str );
			temp->sibling = first; // the FIRST parameter is the LAST sibling node
			first = temp;
		}
		m_token = m_pLexer->NextToken();
		if( m_token.type == LSQUARE ) { // '['
			temp->bArray = TRUE;
			if( !match( RSQUARE ) ) { //']'
				Logic_OutputMsg( IDS_E__MISSING_RSQUARE, m_pLexer->LineNo() );
				ConsumeUntil( COMMA /* ',' */, RPARAN /* ')' */ );
			} else
				m_token = m_pLexer->NextToken(); // should be ',' or ')'
		}
		if( m_token.type == RPARAN ) break; // ')'
		else if( m_token.type == COMMA ) // ','
			TypeToken = m_token = m_pLexer->NextToken();
		else {
			break;
		}
	}
	m_pLexer->PushBack(); // the next token should be ')'

	return first;
}

// Grammar:
// 10. compound_stmt->`{` loal_declarations statement_list `}` | expression_stmt
// the next token should be '{'
CTreeNode* CParser::compound_stmt()
{
	CTreeNode *first = NULL, *last = NULL, *temp = NULL;
	BOOL bHasNoBraces = FALSE;

	if( !match(LBRACE) ) { // match'{'
		bHasNoBraces = TRUE;
		m_pLexer->PushBack();
	}

	// local_declarations
	while( true ) {
		TypeToken = m_token = m_pLexer->NextToken();
		if( m_token.type == _CHAR || m_token.type == _INT ||
			m_token.type == _VOID || m_token.type == _FLOAT )
			temp = local_declarations();
		else { m_pLexer->PushBack(); break; }
		if( bHasNoBraces ) return temp; // has no braces, return when reach the first ';'
		if( temp ) {
			// link all local_declarations together
			if( !first ) { first = temp; last = temp->LastSibling(); }
			else { last->sibling = temp; last = temp->LastSibling(); }
		}
	}

	// statement_list
	// m_token contains the first token of statement_list
	m_token = m_pLexer->NextToken();
	while( 1 ) {
		temp = NULL;
		if( m_token.type == RBRACE ) {
			if( bHasNoBraces ) Logic_OutputMsg( IDS_E__UNPAIRED_RBRACE, m_pLexer->LineNo() );
			break; // '}'
		}
		if( m_token.type == _EOF ) {
			Logic_OutputMsg( IDS_E__MISSING_RBRACE, m_pLexer->LineNo() );
			m_pLexer->PushBack();
			break;
		}

		/* Main Logic Switch */
		switch( m_token.type ) {
		case SEMI: // ';'
		case _NUM:
		case _CHARACTER:
		case LOGICAL_NOT:
		case LPARAN:
			temp = expression_stmt();	break;
		case _ID:
			temp = subcompound_stmt();	break;
		case _IF:
			temp = if_stmt();			break;
		case _WHILE:
			temp = while_stmt();		break;
		case _FOR:
			temp = for_stmt();			break;
		case _GOTO:
			temp = goto_stmt();			break;
		case _BREAK:
			temp = break_stmt();		break;
		case _CONTINUE:
			temp = continue_stmt();		break;
		case _RETURN:
			temp = return_stmt();		break;
		case _ELSE:
			Logic_OutputMsg( IDS_E__UNPAIRED_ELSE, m_pLexer->LineNo() );
			ConsumeUntil( SEMI /* ';' */, RBRACE /* '}' */ );
			break;
		default:
			Logic_OutputMsg( IDS_E__INVALID_SYMBOL, m_pLexer->LineNo(), m_token.str );
			ConsumeUntil( SEMI /* ';' */, RBRACE /* '}' */ );
		}
		if( bHasNoBraces ) return temp; // has no braces, return when reach the first ';'
		if( temp ) {
			// link all statements together
			if( !first ) { first = temp; last = temp->LastSibling(); }
			else { last->sibling = temp; last = temp->LastSibling(); }
		}
		m_token = m_pLexer->NextToken();
	}

	return first;
}

// Grammar:
// 11. local_declarations->local_declarations var_declaration | var_declaration
// m_token is a supported type-specifier token
CTreeNode* CParser::local_declarations()
{
	CTreeNode* temp = NULL;

	IDToken = m_token = m_pLexer->NextToken(); // ID or error
	if( IDToken.type != _ID ) {
		Logic_OutputMsg( IDS_E__RESERVED_KEYWORD, m_pLexer->LineNo(), IDToken.str );
		ConsumeUntil( SEMI /* ';' */ );
		return NULL;
	}
	m_token = m_pLexer->NextToken(); // ';', '[', ',' or error
	if( m_token.type == SEMI || m_token.type == LSQUARE || m_token.type == COMMA )
		temp = var_declaration();
	else {
		Logic_OutputMsg( IDS_E__MISSING_SEMI_AFTER, m_pLexer->LineNo(), IDToken.str );
		m_pLexer->PushBack();
	}
	return temp;
}

// Grammar:
// 15. expression_stmt->expression `;` | `;`
// m_token is '!', '(', ID, NUM, CHARACTER or ';'
CTreeNode* CParser::expression_stmt()
{
	if( m_token.type == SEMI ) return NULL;
	CTreeNode* temp = expression();
	if( !match(SEMI) ) {
		Logic_OutputMsg( IDS_E__MISSING_SEMI, m_pLexer->LineNo() );
		m_pLexer->PushBack();
	}

	return temp;
}

// Grammar:
// 16. expression->var `=` expression | logic1_expression
// FIRST( expression ) = { `!`, `(`, ID, NUM, CHARACTER }
// m_token contains the first token of expression
CTreeNode* CParser::expression()
{
	CTreeNode *temp = logic1_expression(), *p = temp;
	m_token = m_pLexer->NextToken();
	if( m_token.type == ASSIGN ) {
		if( temp->type != _ID && temp->type != ASSIGN ) { // left of '=' should be a ID
			Logic_OutputMsg( IDS_E__INVALID_ASSIGN, m_pLexer->LineNo() );
			ConsumeUntil( SEMI /* ';' */, RPARAN /* ')' */ );
			delete temp;
			return NULL;
		}
		p = newExpNode( kOp, m_token.type, m_token.str );
		p->child[0] = temp;
		if( p->child[0] ) p->child[0]->father = p;
		m_token = m_pLexer->NextToken();
		p->child[1] = expression();
		if( p->child[1] ) p->child[1]->father = p;
	} else {
		m_pLexer->PushBack();
	}

	return p;
}

// Grammar:
// 17. logic1_expression->logic1_expression `||` logic2_expression | logic2_expression
// m_token contains the first token of logic1_expression
CTreeNode* CParser::logic1_expression()
{
	CTreeNode *p = logic2_expression();

	m_token = m_pLexer->NextToken();
	while( m_token.type == LOGICAL_OR ) {
		CTreeNode* temp = newExpNode( kOp, m_token.type, m_token.str );
		temp->child[0] = p;
		p = temp;
		if( p->child[0] ) p->child[0]->father = p;
		m_token = m_pLexer->NextToken();
		if( (p->child[1] = logic2_expression()) ) p->child[1]->father = p;
		m_token = m_pLexer->NextToken();
	}
	m_pLexer->PushBack();

	return p;
}

// Grammar:
// 18. logic2_expression-> logic2_expression `&&` simple_expression | simple_expression
// m_token contains the first token of logic2_expression
CTreeNode* CParser::logic2_expression()
{
	CTreeNode *p = simple_expression();

	m_token = m_pLexer->NextToken();
	while( m_token.type == LOGICAL_AND ) {
		CTreeNode* temp = newExpNode( kOp, m_token.type, m_token.str );
		temp->child[0] = p;
		p = temp;
		if( p->child[0] ) p->child[0]->father = p;
		m_token = m_pLexer->NextToken();
		if( (p->child[1] = simple_expression()) ) p->child[1]->father = p;
		m_token = m_pLexer->NextToken();
	}
	m_pLexer->PushBack();

	return p;
}

// Grammar:
// 19. simple_expression->additive_expression relop additive_expression | additive_expression
// 20. relop-> `<=` | `<` | `>` | `>=` | `==` | `!=`
// m_token contains the first token of simple_expression
CTreeNode* CParser::simple_expression()
{
	CTreeNode* p = additive_expression();
	
	m_token = m_pLexer->NextToken();
	if( m_token.type == NGT || m_token.type == LT || m_token.type == GT ||
		m_token.type == NLT || m_token.type == EQ || m_token.type == NEQ ) {
		CTreeNode* temp = newExpNode( kOp, m_token.type, m_token.str );
		temp->child[0] = p;
		p = temp;
		if( p->child[0] ) p->child[0]->father = p;
		m_token = m_pLexer->NextToken();
		if( (p->child[1] = additive_expression()) ) p->child[1]->father = p;
	} else
		m_pLexer->PushBack();

	return p;
}

// Grammar:
// 21. additive_expression -> additive_expression addop term | term
// 22. addop-> `+` | `-`
// m_token contains the first token of add_expression
CTreeNode* CParser::additive_expression()
{
	CTreeNode* p = term();

	m_token = m_pLexer->NextToken();
	while( m_token.type == PLUS || m_token.type == MINUS ) {
		CTreeNode* temp = newExpNode( kOp, m_token.type, m_token.str );
		temp->child[0] = p;
		p = temp;
		if( p->child[0] ) p->child[0]->father = p;
		m_token = m_pLexer->NextToken();
		if( (p->child[1] = term()) ) p->child[1]->father = p;
		m_token = m_pLexer->NextToken();
	}
	m_pLexer->PushBack();

	return p;
}

// Grammar:
// 23. term->term mulop logic3_expression | logic3_expression
// 24. mulop-> `*` | `/` | `%`
// m_token contains the first token of term
CTreeNode* CParser::term()
{
	CTreeNode* p = logic3_expression();

	m_token = m_pLexer->NextToken();
	while( m_token.type == TIMES || m_token.type == DIV || m_token.type == MOD ) {
		CTreeNode* temp = newExpNode( kOp, m_token.type, m_token.str );
		temp->child[0] = p;
		p = temp;
		if( p->child[0] ) p->child[0]->father = p;
		m_token = m_pLexer->NextToken();
		if( (p->child[1] = logic3_expression()) ) p->child[1]->father = p;
		m_token = m_pLexer->NextToken();
	}
	m_pLexer->PushBack();

	return p;
}

// Grammar:
// 25. logic3_expression-> `!` logic3_expression | factor
// m_token contains the first token of logic3_expression
CTreeNode* CParser::logic3_expression()
{
	CTreeNode *p = NULL, *temp = NULL;

	if( m_token.type == LOGICAL_NOT ) {
		p = newExpNode( kOp, m_token.type, m_token.str );
		m_token = m_pLexer->NextToken();
		if( (temp = factor()) ) {
			p->child[0] = temp;
			p->child[0]->father = p;
		}
	} else
		p = factor();

	return p;
}

// Grammar:
// 26. factor->`(` expression `)` | var | call | NUM
// m_token contains the first token of factor
CTreeNode* CParser::factor()
{
	CTreeNode* temp = NULL;

	switch( m_token.type ) {
	case _NUM:
	case _CHARACTER:
		temp = newExpNode( kConst, m_token.type, m_token.str );
		break;
	case _ID:
		IDToken = m_token;
		m_token = m_pLexer->NextToken();
		if( m_token.type == LPARAN ) temp = call();
		else {
			m_pLexer->PushBack();
			temp = var();
		}
		break;
	case LPARAN:
		m_token = m_pLexer->NextToken();
		temp = expression();
		if( !match(RPARAN) ) { // match ')'
			Logic_OutputMsg( IDS_E__MISSING_RPARAM, m_pLexer->LineNo() );
			m_pLexer->PushBack();
		}
		break;
	default:
		Logic_OutputMsg( IDS_E__SYNTAX_ERROR, m_pLexer->LineNo() );
		ConsumeUntil( SEMI /* ';' */, RBRACE /* '}' */ );
	}

	return temp;
}

// Grammar:
// 27. var->ID | ID `[` expression `]`
// IDToken contains ID
CTreeNode* CParser::var()
{
	CTreeNode* temp = newExpNode( kID, IDToken.type, IDToken.str );
	m_token = m_pLexer->NextToken(); // should be `[` or just push back
	if( m_token.type == LSQUARE ) {
		temp->bArray = TRUE;
		m_token = m_pLexer->NextToken();
		temp->child[0] = expression();
		if( !match(RSQUARE) ) {
			Logic_OutputMsg( IDS_E__MISSING_RSQUARE, m_pLexer->LineNo() );
			m_pLexer->PushBack();
		}
	} else 
		m_pLexer->PushBack();

	return temp;
}

// Grammar: 
// 28. call->ID `(` args `)`
// m_token.str == "(", IDToken contains ID
CTreeNode* CParser::call()
{
	CTreeNode *p = newStmtNode( kCall, IDToken.str ), *temp = NULL;
	p->szScope.LoadString(IDS_A_GLOBAL);
	if( (p->child[0] = args()) ) p->child[0]->father = p;
	temp = p->child[0];
	while( temp && temp->sibling ) {
		temp = temp->sibling;
		temp->father = p;
	}
	if( !match(RPARAN) ) { // match ')'
		Logic_OutputMsg( IDS_E__MISSING_RPARAM, m_pLexer->LineNo() );
		m_pLexer->PushBack();
	}

	return p;
}

// Grammar:
// 29. args->args_list | empty
// 30. args_list->args_list `,` expression | expression
// m_token.str == "("
CTreeNode* CParser::args()
{
	CTreeNode *first = NULL, *temp = NULL;

	m_token = m_pLexer->NextToken();
	if( m_token.type == RPARAN ) {
		m_pLexer->PushBack();
		return NULL;
	}
	while( 1 ) {
		if( (temp = expression()) != NULL )
			// link all args together, the LAST argument is the FIRST in the list
			if( !first ) first = temp;
			else { temp->sibling = first; first = temp; }
		m_token = m_pLexer->NextToken();
		if( m_token.type == COMMA ) m_token = m_pLexer->NextToken();
		else break;
	}
	m_pLexer->PushBack();

	return first;
}

// Grammar:
// 31: sub_compoundstmt->ID `:` | call `;` | expression_stmt
// m_token contains the first token of sub_compoundstmt
CTreeNode* CParser::subcompound_stmt()
{
	CTreeNode* temp = NULL;

	IDToken = m_token;
	m_token = m_pLexer->NextToken();
	if( m_token.type == COLON ) { // label
		temp = newStmtNode( kLabel, IDToken.str );
	} else if( m_token.type == LPARAN ) { // call statement
		temp = call();
		if( !match(SEMI) ) {
			Logic_OutputMsg( IDS_E__MISSING_SEMI, m_pLexer->LineNo() );
			m_pLexer->PushBack();
		}
	} else {
		m_pLexer->PushBack();
		m_token = IDToken;
		temp = expression_stmt();
	}

	return temp;
}

// Grammar:
// 32: if_stmt->`if` `(` expression `)` compound_stmt
//              | `if` `(` expression `)` compound_stmt `else` compound_stmt
// m_token.str == "if"
CTreeNode* CParser::if_stmt()
{
	CTreeNode *temp = newStmtNode( kIf, m_token.str ), *p = NULL;

	if( !match(LPARAN) ) // match '('
		Logic_OutputMsg( IDS_E__MISSING_LPARAM, m_pLexer->LineNo() );
	else m_token = m_pLexer->NextToken();
	// m_token should be the first token of expression
	temp->child[0] = expression();
	if( temp->child[0] ) temp->child[0]->father = temp;
	if( !match(RPARAN) ) { // match ')'
		Logic_OutputMsg( IDS_E__MISSING_RPARAM, m_pLexer->LineNo() );
		m_pLexer->PushBack();
	}
	p = temp->child[1] = compound_stmt();
	if( p ) p->father = temp;
	while( p && p->sibling ) { p = p->sibling; p->father = temp; }
	m_token = m_pLexer->NextToken();
	if( m_token.type == _ELSE ) {
		p = temp->child[2] = compound_stmt();
		if( p ) p->father = temp;
		while( p && p->sibling ) { p = p->sibling; p->father = temp; }
	} else 
		m_pLexer->PushBack();

	return temp;
}

// Grammar:
// 33. while_stmt->`while` `(` expression `)` compound_stmt
// m_token.str == "while"
CTreeNode* CParser::while_stmt()
{
	CTreeNode *temp = newStmtNode( kWhile, m_token.str ), *p = NULL;

	if( !match(LPARAN) ) // match '('
		Logic_OutputMsg( IDS_E__MISSING_LPARAM, m_pLexer->LineNo() );
	else m_token = m_pLexer->NextToken();
	// m_token should be the first token of expression
	temp->child[0] = expression();
	if( temp->child[0] ) temp->child[0]->father = temp;
	if( !match(RPARAN) ) { // match ')'
		Logic_OutputMsg( IDS_E__MISSING_RPARAM, m_pLexer->LineNo() );
		m_pLexer->PushBack();
	}
	// the next token should be '{'
	p = temp->child[1] = compound_stmt();
	if( p ) p->father = temp;
	while( p && p->sibling ) { p = p->sibling; p->father = temp; }

	return temp;
}

// Grammar:
// 34. for_stmt->`for` `(` var `=` expression `;` expression `;` var `=` expression `)` compound_stmt
// m_token.str == "for"
CTreeNode* CParser::for_stmt()
{
	CTreeNode *temp = NULL, *p1 = NULL, *p2 = NULL, *p3 = NULL;
	
	if( !match(LPARAN) ) // match '('
		Logic_OutputMsg( IDS_E__MISSING_LPARAM, m_pLexer->LineNo() );
	else m_token = m_pLexer->NextToken();
	// m_token should be var or ';'
	if( m_token.type == SEMI )  {
		p1 = temp = newStmtNode( kWhile, CString("while") );
		m_token = m_pLexer->NextToken();
	} else {
		if( (temp = expression()) == NULL ) {
			Logic_OutputMsg( IDS_E__SYNTAX_ERROR, m_pLexer->LineNo() );
			ConsumeUntil( RBRACE /* '}' */ );
			return NULL;
		} else
			p1 = temp->sibling = newStmtNode( kWhile, CString("while") );

		if( !match(SEMI) ) // match ';'
			Logic_OutputMsg( IDS_E__MISSING_SEMI, m_pLexer->LineNo() );
		else m_token = m_pLexer->NextToken();
	}
	// m_token should be the first token of expression
	p1->child[0] = expression();
	if( !p1->child[0] ) {
		Logic_OutputMsg( IDS_E__SYNTAX_ERROR, m_pLexer->LineNo() );
		ConsumeUntil( RBRACE /* '}' */ );
		if( temp ) delete temp;
		return NULL;
	}
	p1->child[0]->father = p1;
	if( !match(SEMI) ) // match ';'
		Logic_OutputMsg( IDS_E__MISSING_SEMI, m_pLexer->LineNo() );
	else m_token = m_pLexer->NextToken();
	// m_token should be var
	p2 = expression();
	if( p2 ) p2->father = p1;
	if( !match(RPARAN) ) { // match ')'
		Logic_OutputMsg( IDS_E__MISSING_RPARAM, m_pLexer->LineNo() );
		m_pLexer->PushBack();
	}
	// the next token should be '{'
	p3 = p1->child[1] = compound_stmt();
	if( p3 ) p3->father = p1;
	if( p3 && p3->sibling ) {
		p3 = p3->sibling; p3->father = p1;
	}
	if( p3 ) p3->sibling = p2;
	else p1->child[1] = p2;

	return temp;
}

// Grammar:
// 35. goto_stmt->`goto` ID `;`
// m_token.str == "goto"
CTreeNode* CParser::goto_stmt()
{
	if( !match(_ID) ) {
		Logic_OutputMsg( IDS_E__MISSING_LABEL, m_pLexer->LineNo() );
		ConsumeUntil( SEMI /* ';' */, RBRACE /* '}' */ );
		return NULL;
	}
	CTreeNode* temp = newStmtNode( kGoto, m_token.str );
	if( !match(SEMI) ) {
		Logic_OutputMsg( IDS_E__MISSING_SEMI, m_pLexer->LineNo() );
		m_pLexer->PushBack();
	}
	return temp;
}

// Grammar:
// 36. break_stmt->`break` `;`
// m_token.str == "break"
CTreeNode* CParser::break_stmt()
{
	CTreeNode* temp = newStmtNode( kBreak, m_token.str );
	if( !match(SEMI) ) { // match ';'
		Logic_OutputMsg( IDS_E__MISSING_SEMI, m_pLexer->LineNo() );
		m_pLexer->PushBack();
	}
	return temp;
}

// Grammar:
// 37. continue_stmt->`continue` `;`
// m_token.str = "continue"
CTreeNode* CParser::continue_stmt()
{
	CTreeNode* temp = newStmtNode( kContinue, m_token.str );
	if( !match(SEMI) ) {
		Logic_OutputMsg( IDS_E__MISSING_SEMI, m_pLexer->LineNo() );
		m_pLexer->PushBack();
	}
	return temp;
}

// Grammar:
// 38. return_stmt->`return` `;` | `return` expression `;`
// m_token.str = "return"
CTreeNode* CParser::return_stmt()
{
	CTreeNode* temp = newStmtNode( kReturn, m_token.str );
	m_token = m_pLexer->NextToken();
	if( m_token.type != SEMI ) {
		temp->child[0] = expression();
		if( !match(SEMI) ) {
			Logic_OutputMsg( IDS_E__MISSING_SEMI, m_pLexer->LineNo() );
			m_pLexer->PushBack();
		}
	}
	return temp;
}
