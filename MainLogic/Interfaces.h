#ifndef _INTERFACE_H
#define _INTERFACE_H

typedef CTypedPtrList<CObList, CStringArray*> LIST_ARRAY;

// Output Interface
class ILogic_OutputMsg
{
public:
	virtual void Output( LPCTSTR ) = 0;
	virtual void Trace( LPCTSTR ) = 0;
};

// ListCtrl Initialization Interface
class ILogic_ListInit
{
public:
	virtual void Init( CString &, CListCtrl & ) = 0;
};

// TreeCtrl Initialization Interface
class ILogic_TreeInit
{
public:
	virtual void Init( CString &, CTreeCtrl & ) = 0;
};

#endif