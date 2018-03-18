#pragma once
#include "Lexer.h"

using namespace Logic_Global;

// SHIFT is the power of two used as multiplier in hash function
#define SHIFT 4

// the list of line numbers of the source code in which a variable is referenced
class LineListRec
{
public:
	int					lineno;
	LineListRec*		next;

public:
	// initiation
	LineListRec() : lineno( 0 ), next( NULL ) {}
	// for convenient self-destruction
	~LineListRec() { if( next ) delete next; }
};

// The record in the bucket lists for each variable, including name, 
// assigned memory location, and the list of line numbers in which
// it appears in the source code
class BucketListRec
{
public:
	CString				name;	// variable name
	CString				scope;	// function scope
	LTokenType			type;
	int					memloc;	// memory location for variable
	BOOL				bArray; // for array checking
	LineListRec*		lineno;
	BucketListRec*		next;

public:
	// initiation
	BucketListRec() : memloc( 0 ), lineno( NULL ), next( NULL ) {}
	// for convenient self-destruction
	~BucketListRec()
	{
		if( lineno ) delete lineno;
		if( next ) delete next;
	}
};

/************************************************************************/
/* ·ûºÅ±í                                                               */
/************************************************************************/
class CSymbolTable
{
public:
	CSymbolTable();
	~CSymbolTable();

	// Operations
public:
	void		deleteHashTable();
	BOOL		print();

	void		st_insert( CString& name, CString& scope, LTokenType type, int lineno, int memloc, BOOL bArray = FALSE );
	int			st_lookup( CString& name, CString& scope );
	BOOL		st_lookup_isarray( CString& name, CString& scope );
	LTokenType	st_lookup_type( CString& name, CString& scope );

private:
	CMapStringToPtr		m_hashTable;
};
