#pragma once
#include "MainIDEDocBase.h"
#include "MainIDEViewBase.h"


class CExAsmView : public CMainIDEViewBase
{
protected: // 仅从序列化创建
	CExAsmView();
	DECLARE_DYNCREATE(CExAsmView)

public:
	virtual ~CExAsmView();

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnInitialUpdate();

	void InitHashMap();
};

#ifndef _DEBUG  // MainIDEView.cpp 中的调试版本
inline CMainIDEDocBase* CExAsmView::GetDocument() const
{ return reinterpret_cast<CMainIDEDocBase*>(m_pDocument); }
#endif

