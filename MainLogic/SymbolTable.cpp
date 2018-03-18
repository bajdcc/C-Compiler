#include "StdAfx.h"
#include "Resource.h"
#include "Logic.h"
#include "SymbolTable.h"
#include "Lexer.h"


CSymbolTable::CSymbolTable()
{
	m_hashTable.InitHashTable(100);
}

CSymbolTable::~CSymbolTable()
{
	deleteHashTable();
}

void CSymbolTable::deleteHashTable()
{
	POSITION pos = m_hashTable.GetStartPosition();
	CString key; void* ptr = NULL;
	while (pos)
	{
		m_hashTable.GetNextAssoc(pos, key, ptr);
		delete ptr;
	}
	m_hashTable.RemoveAll();
}

// insert a node into the hash table,
// memloc is inserted only the first time, otherwise ignored
void CSymbolTable::st_insert( CString& name, CString& scope, LTokenType type,
	int lineno, int memloc, BOOL bArray )
{
	void* ptr = NULL;
	m_hashTable.Lookup(name, ptr);

	BucketListRec* l = (BucketListRec*) ptr;
	while( l && ((l->name != name) || (l->scope != scope)) ) l = l->next;
	if( l == NULL ) {
		// variable not yet in table
		l = new BucketListRec;
		l->name = name;
		l->scope = scope;
		l->type = type;
		l->memloc = memloc;
		l->bArray = bArray;
		l->lineno = new LineListRec;
		l->lineno->lineno = lineno;
		l->next = NULL;
		m_hashTable.SetAt(name, l);
	} else {
		// found in table, so just add line number
		LineListRec* t = l->lineno;
		while( t->next ) t = t->next;
		t->next = new LineListRec;
		t->next->lineno = lineno;
	}
}

// lookup a node with specified name and scope from the hash table,
// return the memloc
int CSymbolTable::st_lookup( CString& name, CString& scope )
{
	void* ptr = NULL;
	if (!m_hashTable.Lookup(name, ptr)) return -1;
	BucketListRec* l = (BucketListRec*) ptr;

	while( l && ((l->name != name) || (l->scope != scope)) ) l = l->next;
	return (l == NULL) ? -1 : l->memloc;
}

// check if it is array
BOOL CSymbolTable::st_lookup_isarray( CString& name, CString& scope )
{
	void* ptr = NULL;
	if (!m_hashTable.Lookup(name, ptr)) return FALSE;
	BucketListRec* l = (BucketListRec*) ptr;

	while( l && ((l->name != name) || (l->scope != scope)) ) l = l->next;
	return (l == NULL) ? FALSE : l->bArray;
}

// return the type of the specified table node
LTokenType CSymbolTable::st_lookup_type( CString& name, CString& scope )
{
	void* ptr = NULL;
	m_hashTable.Lookup(name, ptr);
	BucketListRec* l = (BucketListRec*) ptr;

	while( l && ((l->name != name) || (l->scope != scope)) ) l = l->next;
	return (l == NULL ) ? _ERROR : l->type;
}

BOOL CSymbolTable::print()
{
	if (m_hashTable.IsEmpty()) return FALSE;

	LIST_ARRAY* list = Logic_GetDataPtr();
	if (!list) return FALSE;
	LPTSTR* pstrKeywords = Logic_GetRevKeywords();
	ASSERT(pstrKeywords);

	POSITION pos = m_hashTable.GetStartPosition();
	CString key; void* ptr = NULL;
	while (pos)
	{
		m_hashTable.GetNextAssoc(pos, key, ptr);
		BucketListRec* l = (BucketListRec*) ptr;

		CString tmp;
		while( l ) {
			CStringArray* sa = new CStringArray;
			sa->SetSize(5);
			sa->SetAt(2, l->scope);
			sa->SetAt(0, l->name);
			if( l->type == _LABEL )
				sa->SetAt(1, _T("Label") );
			else {
				tmp.Format(_T("%s%s"), pstrKeywords[l->type], ((l->bArray) ? _T("[]") : _T("")));
				sa->SetAt(1, tmp );
			}
			tmp.Format(IDS_FORMAT_D, l->memloc);
			sa->SetAt(3, tmp);
			LineListRec* t = l->lineno;
			tmp.Empty();
			while( t ) {
				tmp.AppendFormat(IDS_FORMAT_D, t->lineno);
				tmp.AppendChar(_T(' '));
				t = t->next;
			}
			sa->SetAt(4, tmp);
			list->AddTail(sa);
			l = l->next;
		}
	}

	return TRUE;
}
