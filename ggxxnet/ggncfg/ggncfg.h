// ggncfg.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// メイン シンボル


// CggncfgApp:
// このクラスの実装については、ggncfg.cpp を参照してください。
//

class CggncfgApp : public CWinApp
{
public:
	CggncfgApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CggncfgApp theApp;
