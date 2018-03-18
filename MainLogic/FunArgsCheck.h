#pragma once
#include "Lexer.h"
#include "TreeNode.h"

using namespace Logic_Global;

class ParamListRec
{
public:
	LTokenType		type;
	BOOL			bArray;
	ParamListRec*	next;
public:
	ParamListRec() : bArray( FALSE ), next( NULL ) {}
	ParamListRec( LTokenType t, BOOL b ) : type( t ), bArray( b ), next( NULL ) {}
	~ParamListRec() { if( next ) delete next; }
};

class FunDecListRec
{
public:
	CString			name;
	LTokenType		type;
	int				count;
	ParamListRec*	params;
	FunDecListRec* 	next;
public:
	FunDecListRec() : count( 0 ), next( NULL ) {}
	FunDecListRec( CString& s, LTokenType t ) : name( s ), type( t ), count( 0 ), next( NULL ) {}
	~FunDecListRec() { if( next ) delete next; }
};

class CFunArgsCheck
{
public:
	CFunArgsCheck();
	~CFunArgsCheck();

public:
	void	deleteList();

	void	fa_insert( CTreeNode* pNode );
	int		fa_check( CTreeNode* pNode );

private:
	FunDecListRec	*first, *last;
};

