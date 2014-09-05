// ggncfg.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "ggncfg.h"
#include "ggncfgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CggncfgApp

BEGIN_MESSAGE_MAP(CggncfgApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CggncfgApp コンストラクション

CggncfgApp::CggncfgApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CggncfgApp オブジェクトです。

CggncfgApp theApp;


// CggncfgApp 初期化

BOOL CggncfgApp::InitInstance()
{
	// アプリケーション　マニフェストが　visual スタイルを有効にするために、
	// ComCtl32.dll バージョン 6　以降の使用を指定する場合は、
	// Windows XP に　InitCommonControls() が必要です。さもなければ、ウィンドウ作成はすべて失敗します。
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();


	CggncfgDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ダイアログが <OK> で消された時のコードを
		//       記述してください。
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ダイアログが <キャンセル> で消された時のコードを
		//       記述してください。
	}

	// ダイアログは閉じられました。アプリケーションのメッセージ ポンプを開始しないで
	// アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}
