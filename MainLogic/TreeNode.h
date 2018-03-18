#pragma once
#include "Lexer.h"

#define MAX_CHILDREN 3

using namespace Logic_Global;

// 语法树结点类型
enum NodeKind
{
	kVarDec,		// 变量声明
	kFunDec,		// 函数声明
	kParam,			// 参数
	kStmt,			// 语句
	kExp			// 表达式
};

// 语句类型
enum StmtKind
{
	kIf,
	kWhile,
	kGoto,
	kBreak,
	kContinue,
	kReturn,
	kLabel,
	kCall
};

// 表达式类型
enum ExpKind
{
	kOp,
	kConst,
	kID
};

// 联合体
union UndefKind
{
	StmtKind	stmt;
	ExpKind		exp;
};

/*
*	三分支树结点(3-Tree)
*/
class AFX_EXT_CLASS CTreeNode
{
public:
	CTreeNode();
	~CTreeNode();

	CTreeNode* LastSibling();

public:
	CTreeNode*		father;					// point to father node
	CTreeNode*		child[ MAX_CHILDREN ];	// point to child node
	CTreeNode*		sibling;				// point to sibling node
	int				lineno;
	NodeKind		nodekind;
	UndefKind		kind;
	LTokenType		type;
	CString			szName;
	CString			szScope;				// node function scope
	BOOL			bArray;					// is this an array declaration
	int				iArraySize;				// array size
};
