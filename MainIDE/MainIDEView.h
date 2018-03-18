
// MainIDEView.h : CMainIDEView 类的接口
//

#pragma once
#include "MainIDEDoc.h"
#include "MainIDEViewBase.h"


class CMainIDEView : public CMainIDEViewBase
{
protected: // 仅从序列化创建
	CMainIDEView();
	DECLARE_DYNCREATE(CMainIDEView)

public:
	virtual ~CMainIDEView();

	// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // MainIDEView.cpp 中的调试版本
inline CMainIDEDoc* CMainIDEView::GetDocument() const
   { return reinterpret_cast<CMainIDEDoc*>(m_pDocument); }
#endif

