#include "StdAfx.h"
#include "TreeNode.h"


CTreeNode::CTreeNode(): father( NULL ), sibling( NULL ), lineno( 0 ), bArray( FALSE )
{
	memset(child, 0, sizeof(child));
}

CTreeNode::~CTreeNode()
{
	for( int i = 0; i < MAX_CHILDREN; i++ ) if( child[i] ) delete child[i];
	if( sibling ) delete sibling;
}

CTreeNode* CTreeNode::LastSibling()
{
	CTreeNode* last = this;
	while( last->sibling ) last = last->sibling;
	return last;
}
