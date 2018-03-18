#pragma once
#include "MainIDEDocBase.h"
#include "MainIDEViewBase.h"


class CExPCodeView : public CMainIDEViewBase
{
protected: // 仅从序列化创建
	CExPCodeView();
	DECLARE_DYNCREATE(CExPCodeView)

public:
	virtual ~CExPCodeView();

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnInitialUpdate();

	void InitHashMap();
};

#ifndef _DEBUG  // MainIDEView.cpp 中的调试版本
inline CMainIDEDocBase* CExPCodeView::GetDocument() const
{ return reinterpret_cast<CMainIDEDocBase*>(m_pDocument); }
#endif

