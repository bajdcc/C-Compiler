
// MainIDEDoc.h : CMainIDEDoc 类的接口
//


#pragma once
#include "MainIDEDocBase.h"


class CMainIDEDoc : public CMainIDEDocBase
{
protected: // 仅从序列化创建
	CMainIDEDoc();
	DECLARE_DYNCREATE(CMainIDEDoc)

public:
	virtual ~CMainIDEDoc();

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCompileScan();
	afx_msg void OnCompileParse();
	afx_msg void OnCompileSymbol();
	afx_msg void OnCompilePcode();
	afx_msg void OnCompileAsm();
	afx_msg void OnCompileCheck();

protected:
	CMainIDEApp* m_pApp;
};
