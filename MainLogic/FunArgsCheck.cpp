#include "StdAfx.h"
#include "FunArgsCheck.h"


CFunArgsCheck::CFunArgsCheck()
{
	first = last = NULL;
}

CFunArgsCheck::~CFunArgsCheck()
{
	deleteList();
}

void CFunArgsCheck::deleteList()
{
	if( first ) { delete first; first = last = NULL; }
}

// insert a kFunDec node into the list, plus its parameters
void CFunArgsCheck::fa_insert( CTreeNode* pNode )
{
	ASSERT( pNode->nodekind == kFunDec );

	FunDecListRec* temp = new FunDecListRec( pNode->szName, pNode->type );
	CTreeNode* p = pNode->child[0];
	if( p ) {
		temp->params = new ParamListRec( p->type, p->bArray );
		temp->count++;

		ParamListRec* l = temp->params;
		while( p->sibling ) {
			p = p->sibling;
			l->next = new ParamListRec( p->type, p->bArray );
			l = l->next;
			temp->count++;
		}
	}
	if( first == NULL ) first = last = temp;
	else { last->next = temp; last = last->next; }
}

// check if a function call's arguments match its declaration parameters
// -1:	not found
// -2:  type not match
// -3:  match
// else count not match, return the declaration parameter count
int CFunArgsCheck::fa_check( CTreeNode* pNode )
{
	ASSERT( pNode->nodekind == kStmt && pNode->kind.stmt == kCall );

	FunDecListRec* l = first;
	while( l && l->name != pNode->szName ) l = l->next;
	if( l == NULL ) return -1;
	ParamListRec* p = l->params;
	CTreeNode* t = pNode->child[0];
	while( p && t ) 
		if( (p->type == t->type && p->bArray == t->bArray) ||
			(t->nodekind == kExp && t->kind.exp == kConst &&
			(t->type == _NUM || t->type == _CHARACTER))) {
				p = p->next; t = t->sibling;
		} else /* type not match */ return -2;
	if( p || t ) // count not match
		return l->count;
	return -3;
}