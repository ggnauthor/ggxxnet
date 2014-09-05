// ggncfgDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ggncfg.h"
#include "ggncfgDlg.h"
#include "ColorEditDlg.h"
#include "EditMsgDlg.h"

#include "../zlib.h"
#include "../sharedMemory.h"

#include <shlwapi.h> // for PathFileExists

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IMG_X 214
#define IMG_Y 100
#define IMG_W 600
#define IMG_H 500

// 1cell 8 * 8
#define PAL_X 87
#define PAL_Y 100
#define PAL_W (64+1)
#define PAL_H (256+1)

enum
{
	CID_SOL = 0,
	CID_KY,
	CID_MAY,
	CID_MILLIA,
	CID_AXL,
	CID_POTEMKIN,
	CID_CHIPP,
	CID_EDDIE,
	CID_BAIKEN,
	CID_FAUST,
	CID_TESTAMENT,
	CID_JAM,
	CID_ANJI,
	CID_JOHNNY,
	CID_VENOM,
	CID_DIZZY,
	CID_SLAYER,
	CID_INO,
	CID_ZAPPA,
	CID_BRIDGET,
	CID_ROBOKY,
	CID_KLIFF,
	CID_JUSTICE,
	CHARACOUNT,
};

BYTE decode1a[0x00b3];
BYTE decode1b[0x0078];
BYTE decode5a[0x0dac];

// CggncfgDlg ダイアログ
CggncfgDlg::CggncfgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CggncfgDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_ctl_userName	= NULL;
	m_ctl_trip		= NULL;
	
	m_ctl_enableNet	= NULL;
	m_ctl_port		= NULL;
	m_ctl_delay		= NULL;
	m_ctl_useEx		= NULL;
	m_ctl_rounds	= NULL;

	m_ctl_enableChat	= NULL;
	m_ctl_ignoreSlow	= NULL;

	m_ctl_watch_enableBroadcast	= NULL;
	m_ctl_watch_allowIntrusion	= NULL;
	m_ctl_watch_saveReplay		= NULL;
	m_ctl_watch_maxRelayNode	= NULL;

	m_ctl_dispInvCombo	= NULL;
	m_ctl_showGGNVer	= NULL;

	m_ctl_editColor		= NULL;
	m_ctl_editChara		= NULL;
	m_ctl_editSprite	= NULL;
	m_ctl_saveColor		= NULL;

	m_ctl_editLobbyAddr = NULL;

	m_setting_wins			= 0;
	m_setting_rank			= 6;
	m_setting_score			= 0;
	m_setting_totalBattle	= 0;
	m_setting_totalWin		= 0;
	m_setting_totalLose		= 0;
	m_setting_totalDraw		= 0;
	m_setting_totalError	= 0;
	m_setting_slowRate		= 0;

///////////////////////////////////////
	m_imageW = 0;
	m_imageH = 0;
	m_palsize = 0;
	memset(m_palette, 0, 256);
	m_image = NULL;
	m_imgbits = NULL;
	m_palbits = NULL;

	memset(m_imageAddrList, 0, 4 * IMG_ADDR_LIST_SIZE);
	memset(m_imageFIDList, 0, IMG_ADDR_LIST_SIZE);
	m_imageAddrListCount = 0;

	memset(m_palAddrList, 0, 4 * PAL_ADDR_LIST_SIZE);
	m_palAddrListCount = 0;

	m_selectPalette = -1;

	m_paletteChanged = false;
	m_curCID		= 0;
	m_curPalette	= 0;
}

CggncfgDlg::~CggncfgDlg()
{
	if (m_image) delete m_image;
}

void CggncfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CggncfgDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SELECTCOLOR, OnBnClickedEditcolor)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_EN_KILLFOCUS(IDC_USERNAME, OnEnKillfocusUsername)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_EDITCHARA, &CggncfgDlg::OnCbnSelchangeEditchara)
	ON_CBN_SELCHANGE(IDC_EDITCOLOR, &CggncfgDlg::OnCbnSelchangeEditcolor)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_CBN_SELCHANGE(IDC_EDITSPRITE, OnCbnSelchangeEditsprite)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_SAVECOLOR, OnBnClickedSavecolor)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_ADDADDR, &CggncfgDlg::OnBnClickedAddaddr)
	ON_BN_CLICKED(IDC_DELETEADDR, &CggncfgDlg::OnBnClickedDeleteaddr)
	ON_BN_CLICKED(IDC_MESSAGE, &CggncfgDlg::OnBnClickedMessage)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CggncfgDlg メッセージ ハンドラ

BOOL CggncfgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	m_ctl_userName	= (CEdit*)GetDlgItem(IDC_USERNAME);
	m_ctl_trip		= (CEdit*)GetDlgItem(IDC_TRIP);
	
	m_ctl_enableNet	= (CButton*)GetDlgItem(IDC_ENABLE_NET);
	m_ctl_port		= (CEdit*)GetDlgItem(IDC_PORT);
	m_ctl_delay		= (CComboBox*)GetDlgItem(IDC_DELAY);
	m_ctl_useEx		= (CButton*)GetDlgItem(IDC_USE_EX);
	m_ctl_rounds	= (CComboBox*)GetDlgItem(IDC_ROUNDS);

	m_ctl_enableChat	= (CButton*)GetDlgItem(IDC_ENABLECHAT);
	m_ctl_ignoreSlow	= (CButton*)GetDlgItem(IDC_IGNORESLOW);

	m_ctl_watch_enableBroadcast	= (CButton*)GetDlgItem(IDC_WATCH_ENABLEBC);
	m_ctl_watch_allowIntrusion	= (CButton*)GetDlgItem(IDC_WATCH_INTRUSION);
	m_ctl_watch_saveReplay		= (CButton*)GetDlgItem(IDC_WATCH_SAVEREP);
	m_ctl_watch_maxRelayNode	= (CComboBox*)GetDlgItem(IDC_MAXRELAYNODE);

	m_ctl_dispInvCombo	= (CButton*)GetDlgItem(IDC_DISP_INV_COMBO);
	m_ctl_showGGNVer	= (CButton*)GetDlgItem(IDC_SYNCCHK);

	m_ctl_editChara		= (CComboBox*)GetDlgItem(IDC_EDITCHARA);
	m_ctl_editColor		= (CComboBox*)GetDlgItem(IDC_EDITCOLOR);
	m_ctl_editSprite	= (CComboBox*)GetDlgItem(IDC_EDITSPRITE);
	m_ctl_saveColor		= (CButton*)GetDlgItem(IDC_SAVECOLOR);

	m_ctl_editLobbyAddr	= (CComboBox*)GetDlgItem(IDC_LOBBYADDR);

	// デフォルト設定
	m_ctl_editLobbyAddr->ResetContent();

	m_ctl_userName->SetWindowText("NONAME");
	m_ctl_trip->SetWindowText("");

	m_ctl_enableNet->SetCheck(1);
	m_ctl_port->SetWindowText("10000");

	m_ctl_delay->ResetContent();
	m_ctl_delay->AddString("1");
	m_ctl_delay->AddString("2");
	m_ctl_delay->AddString("3");
	m_ctl_delay->AddString("4");
	m_ctl_delay->AddString("5");
	m_ctl_delay->AddString("6");
	m_ctl_delay->SetCurSel(1);

	m_ctl_useEx->SetCheck(0);
	
	m_ctl_rounds->ResetContent();
	m_ctl_rounds->AddString("3");
	m_ctl_rounds->AddString("5");
	m_ctl_rounds->SetCurSel(0);

	m_ctl_enableChat->SetCheck(1);
	m_ctl_ignoreSlow->SetCheck(1);

	m_ctl_watch_enableBroadcast->SetCheck(1);
	m_ctl_watch_allowIntrusion->SetCheck(0);
	m_ctl_watch_saveReplay->SetCheck(0);

	m_ctl_watch_maxRelayNode->ResetContent();
	m_ctl_watch_maxRelayNode->AddString("0");
	m_ctl_watch_maxRelayNode->AddString("1");
	m_ctl_watch_maxRelayNode->AddString("2");
	m_ctl_watch_maxRelayNode->AddString("3");
	m_ctl_watch_maxRelayNode->SetCurSel(1);

	m_ctl_dispInvCombo->SetCheck(1);
	m_ctl_showGGNVer->SetCheck(0);
	//m_ctl_showGGNVer->ShowWindow(SW_HIDE);

	m_setting_msg			= "No Comment";

	CString str;
	SetDlgItemText(IDC_STR0, "rank");		SetDlgItemText(IDC_STRB0, ": F");
	SetDlgItemText(IDC_STR1, "win");		SetDlgItemText(IDC_STRB1, ": 0");
	SetDlgItemText(IDC_STR2, "game count");	SetDlgItemText(IDC_STRB2, ": 0");
	SetDlgItemText(IDC_STR3, "total win");	SetDlgItemText(IDC_STRB3, ": 0");
	SetDlgItemText(IDC_STR4, "total lose");	SetDlgItemText(IDC_STRB4, ": 0");
	SetDlgItemText(IDC_STR5, "total error");SetDlgItemText(IDC_STRB5, ": 0");
	SetDlgItemText(IDC_STR6, "");			SetDlgItemText(IDC_STRB6, "");
	SetDlgItemText(IDC_STR7, "");			SetDlgItemText(IDC_STRB7, "");
	GetDlgItem(IDC_STR3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STRB3)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STR4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STRB4)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STR5)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STRB5)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STR6)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STRB6)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STR7)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STRB7)->ShowWindow(SW_HIDE);

	m_ctl_editColor->ResetContent();
	m_ctl_editColor->AddString("P(Default)");
	m_ctl_editColor->AddString("K");
	m_ctl_editColor->AddString("S");
	m_ctl_editColor->AddString("HS");
	m_ctl_editColor->AddString("Start");
	m_ctl_editColor->AddString("D");
	m_ctl_editColor->AddString("P(Sp)");
	m_ctl_editColor->AddString("K(Sp)");
	m_ctl_editColor->AddString("S(Sp)");
	m_ctl_editColor->AddString("HS(Sp)");
	m_ctl_editColor->AddString("Start(Sp)");
	m_ctl_editColor->AddString("D(Sp)");
	m_ctl_editColor->SetCurSel(0);

	m_ctl_editChara->ResetContent();
	m_ctl_editChara->AddString("Sol");
	m_ctl_editChara->AddString("Ky");
	m_ctl_editChara->AddString("May");
	m_ctl_editChara->AddString("Millia");
	m_ctl_editChara->AddString("Axl");
	m_ctl_editChara->AddString("Potemkin");
	m_ctl_editChara->AddString("Chipp");
	m_ctl_editChara->AddString("Eddie");
	m_ctl_editChara->AddString("Baiken");
	m_ctl_editChara->AddString("Faust");
	m_ctl_editChara->AddString("Testament");
	m_ctl_editChara->AddString("Jam");
	m_ctl_editChara->AddString("Anji");
	m_ctl_editChara->AddString("Johnny");
	m_ctl_editChara->AddString("Venom");
	m_ctl_editChara->AddString("Dizzy");
	m_ctl_editChara->AddString("Slayer");
	m_ctl_editChara->AddString("I-No");
	m_ctl_editChara->AddString("Zappa");
	m_ctl_editChara->AddString("Bridget");
	m_ctl_editChara->AddString("Robo-Ky");
	m_ctl_editChara->AddString("Kliff");
	m_ctl_editChara->AddString("Justice");
	m_ctl_editChara->SetCurSel(0);
	
	readSettingFile();
	
	// 画像展開コード抽出
	FILE *fp = fopen(getGGXXPath(), "rb");
	if (fp)
	{
		// 一応サイズだけチェック
		fpos_t size;
		fseek(fp, 0, SEEK_END);
		fgetpos(fp, &size);
		if ((int)size == 1822720)
		{
			fseek(fp, 0x0c6660, SEEK_SET);
			fread(&decode1a, 1, 0x00b3, fp);
			*((DWORD*)&decode1a[0x81]) = ((int)decode1b - (int)&decode1a[0x80] - 5);

			fseek(fp, 0x001800, SEEK_SET);
			fread(&decode1b, 1, 0x0078, fp);

			fseek(fp, 0x001b20, SEEK_SET);
			fread(&decode5a, 1, 0x0dac, fp);
		}
		fclose(fp);
	}

	changeColorEditChara(CID_SOL);
	readImageData(0, 0);

	m_windc = ::GetDC(m_hWnd);

	// BITMAPINFO設定
	BITMAPINFO	bmInfo;
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = IMG_W;
	bmInfo.bmiHeader.biHeight = -IMG_H;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biCompression = BI_RGB;
	
	// スプライトバックバッファ作成
	m_imgdc     = CreateCompatibleDC(m_windc);
	m_imgbmp    = CreateDIBSection(NULL, &bmInfo, DIB_RGB_COLORS, (void**)&m_imgbits, NULL, 0);
	m_imgoldbmp = (HBITMAP)SelectObject(m_imgdc, m_imgbmp);
	// パレットバックバッファ作成
	bmInfo.bmiHeader.biWidth  = PAL_W;
	bmInfo.bmiHeader.biHeight = -PAL_H;
	m_paldc     = CreateCompatibleDC(m_windc);
	m_palbmp    = CreateDIBSection(NULL, &bmInfo, DIB_RGB_COLORS, (void**)&m_palbits, NULL, 0);
	m_paloldbmp = (HBITMAP)SelectObject(m_paldc, m_palbmp);

	arrangeControls();
	m_ctl_userName->SetFocus();
	
	// 20msに１回描画
	SetTimer(1000, 20, NULL); //???

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。
void CggncfgDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CggncfgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CggncfgDlg::OnBnClickedEditcolor()
{
	COLORREF result;
	CHOOSECOLOR cc;
	memset(&cc, 0, sizeof(CHOOSECOLOR));
	cc.lStructSize	= sizeof(CHOOSECOLOR);
	cc.rgbResult	= 0x000000FF;
	cc.lpCustColors	= &result;
	cc.Flags		= CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc))
	{
	}
}

CString CggncfgDlg::getSettingPath(void)
{
	char str[1024];
	::GetModuleFileName(NULL, str, 1024);
	CString	sPath = str;
	int pos = sPath.ReverseFind('\\');
	sPath = sPath.Left(pos) + "\\ggn_setting.dat";
	return sPath;
}

void CggncfgDlg::readSettingFile(void)
{
	FILE *fp = fopen(getSettingPath(), "rb");
	if (fp)
	{
		char	tmp[256];
		char	data[4096];
		char*	ptr = data;
		int datasize = zfsize(fp);
		if (datasize <= 4096)
		{
			zfread(data, 4096, fp);
			
			if (datasize == 310)		m_datVersion = 100;	// ver1.00-1.01
			else if (datasize == 311)	m_datVersion = 102;	// ver1.02-1.04
			else if (datasize == 376)	m_datVersion = 105;	// ver1.05
			else if (datasize == 2936)	m_datVersion = 106;	// ver1.06-1.09
			else
			{
				m_datVersion = *((DWORD*)ptr);	ptr += 4;	// ver1.10以降はバージョンを持っている
			}

			m_ctl_editLobbyAddr->SetWindowText(ptr);	ptr += 256;
			
			if (m_datVersion >= 106)
			{
				for (int i = 0; i < 10; i++)
				{
					if (strlen(ptr) > 0) m_ctl_editLobbyAddr->AddString(ptr);
					ptr += 256;
				}
			}

			if (m_datVersion >= 110){ m_ctl_userName->SetWindowText(ptr);	ptr += 41; }
			else					{ m_ctl_userName->SetWindowText(ptr);	ptr += 11; }

			m_ctl_trip->SetWindowText(ptr);				ptr += 11;
			m_ctl_enableNet->SetCheck(*ptr);			ptr += 1;

			_itoa(*((WORD*)ptr), tmp, 10);
			m_ctl_port->SetWindowText(tmp);				ptr += 2;

			_itoa(*ptr, tmp, 10);
			m_ctl_delay->SelectString(0, tmp);			ptr += 1;
			
			if (m_datVersion == 100)
			{
				m_ctl_enableChat->SetCheck(0);
				m_ctl_ignoreSlow->SetCheck(0);
			}
			else if (m_datVersion == 102)
			{
				m_ctl_enableChat->SetCheck(*ptr);		ptr += 1;
				m_ctl_ignoreSlow->SetCheck(0);
			}
			else if (m_datVersion == 105 ||
					 m_datVersion == 106 ||
					 m_datVersion >= 110)
			{
				m_ctl_enableChat->SetCheck(*ptr);	ptr += 1;
				m_ctl_ignoreSlow->SetCheck(*ptr);	ptr += 1;
			}

			ptr += 2; // auto connect wait 分

			m_ctl_useEx->SetCheck(*ptr);			ptr += 1;

			m_ctl_dispInvCombo->SetCheck(*ptr);		ptr += 1;
			m_ctl_showGGNVer->SetCheck(*ptr);			ptr += 1;

			m_setting_wins = *((WORD*)ptr);			ptr += 2;
			m_setting_rank = *ptr;					ptr += 1;

			if (m_datVersion >= 110)
			{
				m_setting_score			= *((int*)ptr);	ptr += 4;
				m_setting_totalBattle	= *((int*)ptr);	ptr += 4;
				m_setting_totalWin		= *((int*)ptr);	ptr += 4;
				m_setting_totalLose		= *((int*)ptr);	ptr += 4;
				m_setting_totalDraw		= *((int*)ptr);	ptr += 4;
				m_setting_totalError	= *((int*)ptr);	ptr += 4;
				m_setting_slowRate		= *((int*)ptr);	ptr += 4;

				_itoa(*ptr, tmp, 10);
				m_ctl_rounds->SelectString(0, tmp);		ptr += 1;
				
				ptr[255] = '\0';
				m_setting_msg			= ptr;			ptr += 256;
			}
			else
			{
				m_setting_msg			= "No Comment";
				m_setting_score			= 0;
				m_setting_totalBattle	= 0;
				m_setting_totalWin		= 0;
				m_setting_totalLose		= 0;
				m_setting_totalDraw		= 0;
				m_setting_totalError	= 0;
				m_setting_slowRate		= 0;
				m_ctl_rounds->SelectString(0, "3");
			}

			if (m_datVersion >= 120)
			{
				m_ctl_watch_enableBroadcast->SetCheck(*ptr);ptr += 1;
				m_ctl_watch_allowIntrusion->SetCheck(*ptr); ptr += 1;
				m_ctl_watch_saveReplay->SetCheck(*ptr);		ptr += 1;
				m_ctl_watch_maxRelayNode->SetCurSel(*ptr);	ptr += 1;
			}
			else
			{
				m_ctl_watch_enableBroadcast->SetCheck(1);
				m_ctl_watch_allowIntrusion->SetCheck(0);
				m_ctl_watch_saveReplay->SetCheck(0);
				m_ctl_watch_maxRelayNode->SetCurSel(1);
			}
			
			CString str;
			SetDlgItemText(IDC_STR0, "rank"); char rankchr[]="SABCDEF";
			str.Format(": %c", rankchr[m_setting_rank]);	SetDlgItemText(IDC_STRB0, str);
			SetDlgItemText(IDC_STR1, "win");
			str.Format(": %d", m_setting_wins);				SetDlgItemText(IDC_STRB1, str);
			SetDlgItemText(IDC_STR2, "game count");
			str.Format(": %d", m_setting_totalBattle);		SetDlgItemText(IDC_STRB2, str);
			SetDlgItemText(IDC_STR3, "total win");
			str.Format(": %d", m_setting_totalWin);			SetDlgItemText(IDC_STRB3, str);
			SetDlgItemText(IDC_STR4, "total lose");
			str.Format(": %d", m_setting_totalLose);		SetDlgItemText(IDC_STRB4, str);
			SetDlgItemText(IDC_STR5, "total error");
			str.Format(": %d", m_setting_totalError);		SetDlgItemText(IDC_STRB5, str);
			SetDlgItemText(IDC_STR6, "");					SetDlgItemText(IDC_STRB6, "");
			SetDlgItemText(IDC_STR7, "");					SetDlgItemText(IDC_STRB7, "");
			GetDlgItem(IDC_STR6)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STRB6)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STR7)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_STRB7)->ShowWindow(SW_HIDE);
		}
		fclose(fp);
	}
	else
	{
		m_datVersion = DATVERSION;
	}
}

void CggncfgDlg::writeSettingFile(void)
{
	// バックアップ取る
	if (DATVERSION != m_datVersion)
	{
		CString str = getSettingPath();
		remove(str + "_old");
		rename(str, str + "_old");
	}

	FILE *fp = fopen(getSettingPath(), "wb");
	if (fp)
	{
		char	tmp[256];
		char	data[4096];
		char*	ptr = data;
		
		/* cfgバージョン */
		*((DWORD*)ptr) = DATVERSION;					ptr += 4;

		m_ctl_editLobbyAddr->GetWindowText(ptr, 256);	ptr += 256;

		for (int i = 0; i < 10; i++)
		{
			if (i < m_ctl_editLobbyAddr->GetCount())
			{
				CString	str;
				m_ctl_editLobbyAddr->GetLBText(i, str);
				strncpy(ptr, str.GetBuffer(), 256);
				ptr += 256;
			}
			else
			{
				memset(ptr, 0, 256);
				ptr += 256;
			}
		}

		m_ctl_userName->GetWindowText(ptr, 41);		ptr += 41;

		m_ctl_trip->GetWindowText(ptr, 11);			ptr += 11;
		*ptr = m_ctl_enableNet->GetCheck();			ptr += 1;

		m_ctl_port->GetWindowText(tmp, 256);	
		*((WORD*)ptr) = atoi(tmp);					ptr += 2;

		m_ctl_delay->GetWindowText(tmp, 256);
		*ptr = atoi(tmp);							ptr += 1;
		
		*ptr = m_ctl_enableChat->GetCheck();		ptr += 1;
		*ptr = 1/*m_ctl_ignoreSlow->GetCheck()*/;	ptr += 1;	// オプションの必要性を感じないので、強制的にONとする

		*((WORD*)ptr) = 0;							ptr += 2;

		*ptr = m_ctl_useEx->GetCheck();				ptr += 1;

		*ptr = m_ctl_dispInvCombo->GetCheck();		ptr += 1;
		*ptr = m_ctl_showGGNVer->GetCheck();			ptr += 1;
		
		*((WORD*)ptr) = m_setting_wins;				ptr += 2;
		*ptr = m_setting_rank;						ptr += 1;

		*((int*)ptr) = m_setting_score;				ptr += 4;
		*((int*)ptr) = m_setting_totalBattle;		ptr += 4;
		*((int*)ptr) = m_setting_totalWin;			ptr += 4;
		*((int*)ptr) = m_setting_totalLose;			ptr += 4;
		*((int*)ptr) = m_setting_totalDraw;			ptr += 4;
		*((int*)ptr) = m_setting_totalError;		ptr += 4;
		*((int*)ptr) = m_setting_slowRate;			ptr += 4;

		m_ctl_rounds->GetWindowText(tmp, 256);
		*ptr = atoi(tmp);							ptr += 1;

		strncpy(ptr, m_setting_msg.GetBuffer(), 255);	ptr += 256;

		// ver1.16
		*ptr = m_ctl_watch_enableBroadcast->GetCheck();	ptr += 1;
		*ptr = m_ctl_watch_allowIntrusion->GetCheck();	ptr += 1;
		*ptr = m_ctl_watch_saveReplay->GetCheck();		ptr += 1;
		*ptr = m_ctl_watch_maxRelayNode->GetCurSel();		ptr += 1;

		zfwrite(data, (int)(ptr-data), fp, 0);

		fclose(fp);
	}
}

bool CggncfgDlg::valueCheck(void)
{
	int		value;
	char	data[128];
	
	m_ctl_userName->GetWindowText(data, 128);
	if (_mbschr((unsigned char*)data, '◆'))
	{
		AfxMessageBox("Username contain the invalid character \'◆\'.");
		m_ctl_userName->SetFocus();
		m_ctl_userName->SetSel(0, -1);
		return false;
	}
	if (_mbschr((unsigned char*)data, '@'))
	{
		AfxMessageBox("Username contain the invalid character \'@\'.");
		m_ctl_userName->SetFocus();
		m_ctl_userName->SetSel(0, -1);
		return false;
	}
	if ((int)strlen(data) > 20)
	{
		AfxMessageBox("Please input a user name under 20 characters.");
		m_ctl_userName->SetFocus();
		m_ctl_userName->SetSel(0, -1);
		return false;
	}
	
	m_ctl_trip->GetWindowText(data, 128);
	if (strlen(data) > 10)
	{
		AfxMessageBox("Please input a tripkey under 10 characters.");
		m_ctl_trip->SetFocus();
		m_ctl_trip->SetSel(0, -1);
		return false;
	}

	m_ctl_port->GetWindowText(data, 128);
	value = atoi(data);
	if (value < 1024 || value > 65535)
	{
		AfxMessageBox("Please input port number between 1024-65535");
		m_ctl_port->SetFocus();
		m_ctl_port->SetSel(0, -1);
		return false;
	}

	if (m_setting_msg.GetLength() > 255)
	{
		AfxMessageBox("Please input a message under 256 characters.");
		m_ctl_port->SetFocus();
		m_ctl_port->SetSel(0, -1);
		return false;
	}
	return true;
}

void CggncfgDlg::OnBnClickedOk()
{
	int ret = checkSavePalette();
	if (ret == 1)
	{
		// yes
		writePalette();
		m_paletteChanged = false;
	}
	else if (ret == 0)
	{
		// cancel
		m_ctl_editColor->SetCurSel(m_curPalette);
		return;
	}

	if (valueCheck())
	{
		writeSettingFile();
		OnOK();
	}
}

void CggncfgDlg::OnBnClickedCancel()
{
	int ret = checkSavePalette();
	if (ret == 1)
	{
		// yes
		writePalette();
		m_paletteChanged = false;
	}
	else if (ret == 0)
	{
		// cancel
		m_ctl_editColor->SetCurSel(m_curPalette);
		return;
	}
	
	KillTimer(1000);
	Sleep(100);// 念のため

	SelectObject(m_imgdc, m_imgoldbmp);
	DeleteDC(m_imgdc);
	DeleteObject(m_imgbmp);

	::ReleaseDC(m_hWnd, m_windc);

	OnCancel();
}

void CggncfgDlg::OnEnKillfocusUsername()
{
	//if (m_ctl_userName)
	//{
	//	CString str;
	//	m_ctl_userName->GetWindowText(str);
	//	str.MakeUpper();
	//	m_ctl_userName->SetWindowText(str);
	//}
}

CString CggncfgDlg::getPalDirPath(void)
{
	char str[1024];
	::GetModuleFileName(NULL, str, 1024);
	CString	sExePath = str;
	int pos = sExePath.ReverseFind('\\');
	CString sPath = sExePath.Left(pos) + "\\pal";
	
	return sPath;
}

CString CggncfgDlg::getPalettePath(int p_cid, int p_palidx)
{
	CString palDir = getPalDirPath();
	CreateDirectory(palDir, NULL);

	CString cName;
	m_ctl_editChara->GetLBText(p_cid, cName);
	
	CString	pName;
	m_ctl_editColor->GetLBText(p_palidx, pName);

	CString palFile;
	palFile.Format("%s\\%s_%s.pal", palDir, cName, pName);

	return palFile;
}

CString CggncfgDlg::getGGXXPath(void)
{
	char str[1024];
	::GetModuleFileName(NULL, str, 1024);
	CString	sExePath = str;
	int pos = sExePath.ReverseFind('\\');
	CString sPath = sExePath.Left(pos) + "\\ggxx.exe";
	
	return sPath;
}

CString CggncfgDlg::getDataPath(int p_id)
{
	char str[1024];
	::GetModuleFileName(NULL, str, 1024);
	CString	sExePath = str;
	int pos = sExePath.ReverseFind('\\');

	CString	sPath;
	sPath.Format("cddata%.2d.bin", p_id);

	sPath = sExePath.Left(pos) + "\\" + sPath;
	
	return sPath;
}

void CggncfgDlg::clearImageOffset(void)
{
	m_imageAddrListCount = 0;
	m_palAddrListCount = 0;
}

void CggncfgDlg::addImageOffset(int p_fid, int p_offset)
{
	FILE *fp = fopen(getDataPath(p_fid), "rb");
	if (fp)
	{
		DWORD	data1, data2;

		fseek(fp, p_offset, SEEK_SET);
		fread(&data1, 1, 4, fp);
// image data
		{
			fseek(fp, p_offset + data1 + 4, SEEK_SET);
			fread(&data2, 1, 4, fp);
			fseek(fp, p_offset + data1 + data2, SEEK_SET);
			int actualsize = fread(&m_imageAddrList[m_imageAddrListCount], 1, 4 * (IMG_ADDR_LIST_SIZE - m_imageAddrListCount), fp);
			
			for (int i = 0; i < actualsize / 4; i++)
			{
				if (m_imageAddrList[m_imageAddrListCount] == 0xFFFFFFFF) break;
				m_imageAddrList[m_imageAddrListCount] += p_offset + data1 + data2;
				m_imageFIDList[m_imageAddrListCount] = p_fid;
				m_imageAddrListCount++;
			}
		}
// pal data
		{
			fseek(fp, p_offset + data1 + 12, SEEK_SET);
			fread(&data2, 1, 4, fp);
			fseek(fp, p_offset + data1 + data2, SEEK_SET);
			int actualsize = fread(m_palAddrList, 1, 4 * PAL_ADDR_LIST_SIZE, fp);

			m_palAddrListCount = 0;
			for (int i = 0; i < actualsize / 4; i++)
			{
				if (m_palAddrList[m_palAddrListCount] == 0xFFFFFFFF) break;
				m_palAddrList[m_palAddrListCount] += p_offset + data1 + data2;
				m_palAddrListCount++;
			}
		}
		fclose(fp);
	}
}

void CggncfgDlg::readImageData(int p_imgidx, int p_palidx)
{
	if (p_palidx != -1)
	{
		m_paletteChanged = false;
		m_curPalette = p_palidx;
	}
	if (p_imgidx < 0 || p_imgidx >= m_imageAddrListCount) return;
	if (p_palidx < -1 || p_palidx >= 12) return;

	FILE *fp = fopen(getDataPath(m_imageFIDList[p_imgidx]), "rb");
	if (fp)
	{
		char	header[16];

		fseek(fp, m_imageAddrList[p_imgidx], SEEK_SET);

		fread(header, 1, 16, fp);
		m_palsize = 1 << header[4];
		m_imageW = *((WORD*)&header[6]);
		m_imageH = *((WORD*)&header[8]);
		
		if (p_palidx != -1)
		{
			if (PathFileExists(getPalettePath(m_curCID, m_curPalette)))
			{
				// fileからpalette読み込み
				readPalette(getPalettePath(m_curCID, m_curPalette));
			}
			else
			{
				// cddataからデフォルトを読み込み
				if (m_palsize == 16)
				{
					memset(m_palHeader, 0, 16);
					memset(m_palette, 0, 256 * 4);
					fread(m_palette, 4, m_palsize, fp);
				}
				else
				{
					fpos_t pos;
					fgetpos(fp, &pos);
					
					fseek(fp, m_palAddrList[p_palidx], SEEK_SET);
					fread(m_palHeader, 1, 16, fp);

					fseek(fp, m_palAddrList[p_palidx] + 16, SEEK_SET);
					fread(m_palette, 1, 256 * 4, fp);
					
					// convert
					DWORD temp[8];
					for (int i = 0; i < 8; i++)
					{
						memcpy(temp, &m_palette[8+32*i], 32);
						memcpy(&m_palette[8+32*i], &m_palette[16+32*i], 32);
						memcpy(&m_palette[16+32*i], temp, 32);
					}
					fseek(fp, (int)pos, SEEK_SET);
				}
			}
		}
		else if (m_palsize == 16)
		{
			fseek(fp, m_palsize * 4, SEEK_CUR);
		}

		int imgsize = m_imageW * m_imageH * 2;
		BYTE* srcbuf = new BYTE[imgsize];	// srcのサイズを求めるのが面倒なので、てきとーに十分なサイズを確保
		fread(srcbuf, 1, imgsize, fp);
		fclose(fp);

		if (m_image) delete m_image;
		m_image = new BYTE[imgsize];
		memset(m_image, 0, imgsize);

		BYTE* dstbuf = m_image;
		if (header[0] == 1)
		{
			__asm
			{
				lea eax, decode1a
				push dstbuf
				push srcbuf
				call eax
				add esp, 8
			}
		}
		else if (header[0] == 5)
		{
			__asm
			{
				lea eax, decode5a
				push srcbuf
				push dstbuf
				call eax
				add esp, 8
			}
		}
		else
		{
			CString err;
			err.Format("未知の形式 = %d\n", header[0]);
			TRACE(err);
		}
		delete srcbuf;
	}
}

void CggncfgDlg::changeColorEditChara(int p_charaid)
{
	m_paletteChanged = false;
	m_curCID = p_charaid;

	clearImageOffset();

	switch (p_charaid)
	{
	case CID_SOL:
		addImageOffset(3, 0x003c8800);
		//addImageOffset(4, 0x01daf800);
		break;
	case CID_KY:
		//addImageOffset(3, 0x00796000);
		addImageOffset(4, 0x0217d000);
		break;
	case CID_MAY:
		//addImageOffset(3, 0x00b53800);
		addImageOffset(4, 0x0253a800);
		break;
	case CID_MILLIA:
		//addImageOffset(3, 0x00f48000);
		addImageOffset(4, 0x0292f000);
		break;
	case CID_EDDIE:
		//addImageOffset(3, 0x01cb6800);
		addImageOffset(4, 0x0369d800);
		break;
	case CID_POTEMKIN:
		//addImageOffset(3, 0x0154d000);
		addImageOffset(4, 0x02f34000);
		break;
	case CID_CHIPP:
		//addImageOffset(3, 0x019ee000);
		addImageOffset(4, 0x033d5000);
		break;
	case CID_FAUST:
		addImageOffset(3, 0x022b6000);
		break;
	case CID_AXL:
		//addImageOffset(3, 0x011f0000);
		addImageOffset(4, 0x02bd7000);
		break;
	case CID_BAIKEN:
		//addImageOffset(3, 0x01fa6000);
		addImageOffset(4, 0x0398d000);
		break;
	case CID_ANJI:
		//addImageOffset(3, 0x02e6c000);
		addImageOffset(5, 0x00bb6000);
		break;
	case CID_VENOM:
		//addImageOffset(3, 0x0361d000);
		addImageOffset(5, 0x01367000);
		break;
	case CID_JOHNNY:
		//addImageOffset(3, 0x03257800);
		addImageOffset(5, 0x00fa1800);
		break;
	case CID_JAM:
		//addImageOffset(3, 0x02b02000);
		addImageOffset(5, 0x0084c000);
		break;
	case CID_TESTAMENT:
		//addImageOffset(3, 0x026dd000);
		addImageOffset(5, 0x00427000);
		break;
	case CID_DIZZY:
		//addImageOffset(3, 0x03896800);
		addImageOffset(5, 0x015e0800);
		break;
	case CID_BRIDGET:
		//addImageOffset(4, 0x00f0a800);
		addImageOffset(5, 0x02a20800);
		break;
	case CID_SLAYER:
		addImageOffset(5, 0x01b16000);
		break;
	case CID_ZAPPA:
		//addImageOffset(4, 0x00a12000);
		addImageOffset(5, 0x02528000);
		break;
	case CID_INO:
		//addImageOffset(4, 0x004ff800);
		addImageOffset(5, 0x02015800);
		break;
	case CID_ROBOKY:
		//addImageOffset(4, 0x012dd800);
		addImageOffset(5, 0x02df3800);
		break;
	case CID_KLIFF:
		//addImageOffset(4, 0x01719800);
		addImageOffset(5, 0x0322f800);
		break;
	case CID_JUSTICE:
		//addImageOffset(4, 0x019b9000);
		addImageOffset(5, 0x034cf000);
		break;
	}

	m_ctl_editSprite->ResetContent();
	for (int i = 0; i < m_imageAddrListCount; i++)
	{
		char c[10];
		_itoa(i, c, 10);
		m_ctl_editSprite->AddString(c);
	}
	m_ctl_editSprite->SetCurSel(0);
}

DWORD ConvertRGBA(DWORD p_src)
{
	BYTE*	s = (BYTE*)&p_src;
	return (s[0] << 16) | (s[1] << 8) | s[2] | 0x80000000;
}

void CggncfgDlg::drawImage(bool p_inv)
{
	memset(m_imgbits, 0, IMG_W * IMG_H * 4);

	if (m_palsize == 16)
	{
		int count = 0;
		for (int i = 0; i < m_imageH; i++)
		{
			if (i < IMG_H)
			{
				for (int j = 0; j < m_imageW; j += 2)
				{
					BYTE pixel = m_image[count++];

					if (j < IMG_W)
					{
						DWORD color;
						color = ConvertRGBA(m_palette[pixel & 0xF]);
						if (p_inv && m_selectPalette == pixel) color = ~color;
						m_imgbits[j + i * IMG_W]		= color;
						color = ConvertRGBA(m_palette[(pixel >> 4)& 0xF]);
						if (p_inv && m_selectPalette == pixel) color = ~color;
						m_imgbits[j + i * IMG_W + 1]	= color;
					}
				}
			}
		}
	}
	else
	{
		int count = 0;
		for (int i = 0; i < m_imageH; i++)
		{
			if (i < IMG_H)
			{
				for (int j = 0; j < m_imageW; j++)
				{
					BYTE pixel = m_image[count++];

					if (j < IMG_W)
					{
						DWORD color = ConvertRGBA(m_palette[pixel]);
						if (p_inv && m_selectPalette == pixel) color = ~color;
						m_imgbits[j + i * IMG_W] = color;
					}
				}
			}
		}
	}

	CRect r = getImageRect();
	BitBlt(m_windc, r.left, r.top, r.right - r.left, r.bottom - r.top, m_imgdc, 0, 0, SRCCOPY);
}

void CggncfgDlg::drawPalette(void)
{
	memset(m_palbits, 0xFFFFFFFF, PAL_W * PAL_H * 4);
	CRect	palRect = getPaletteRect();
	CRect	cellRect(0, 0, 8, 8);
	
	//HBRUSH	hBrush2 = CreateSolidBrush(RGB(0,0,255));
	//HPEN	pn = CreatePen(PS_SOLID, 3, RGB(255,0,0));
	//SelectObject(m_paldc, pn);
	//SelectObject(m_paldc, hBrush);

	// color cell
	for (int i = 0; i < m_palsize; i++)
	{
		HBRUSH	hBrush = CreateSolidBrush(m_palette[i]);
		
		CRect r = cellRect;
		r.OffsetRect((i % 8) * 8, (i / 8) * 8);
		FillRect(m_paldc, &r, hBrush);

		DeleteObject(hBrush);
	}
	
	// frame
	HPEN	hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
	SelectObject(m_paldc, hPen);
	for (int i = 0; i <= 8; i++)
	{
		for (int j = 0; j <= 32; j++)
		{
			MoveToEx(m_paldc, 0, j * 8, 0);
			LineTo(m_paldc, palRect.right - palRect.left, j * 8);
		}
		MoveToEx(m_paldc, i * 8, 0, 0);
		LineTo(m_paldc, i * 8, PAL_H);
	}
	DeleteObject(hPen);

	// highlight
	if (m_selectPalette != -1)
	{
		hPen = CreatePen(PS_SOLID, 2, RGB(255,0,0));
		SelectObject(m_paldc, hPen);
		SelectObject(m_paldc, GetStockObject(NULL_BRUSH));
		Rectangle(m_paldc,
			m_selectPalette % 8 * 8 + 1,
			m_selectPalette / 8 * 8 + 1,
			m_selectPalette % 8 * 8 + 9,
			m_selectPalette / 8 * 8 + 9);
		DeleteObject(hPen);
	}

	BitBlt(m_windc,
		palRect.left,
		palRect.top,
		palRect.right - palRect.left,
		palRect.bottom - palRect.top,
		m_paldc, 0, 0, SRCCOPY);
}

void CggncfgDlg::OnClose()
{
	//int ret = checkSavePalette();
	//if (ret == 1)
	//{
	//	// yes
	//	writePalette();
	//	m_paletteChanged = false;
	//}
	//else if (ret == 0)
	//{
	//	// cancel
	//	m_ctl_editColor->SetCurSel(m_curPalette);
	//	return;
	//}
	CDialog::OnClose();
}

void CggncfgDlg::OnCbnSelchangeEditchara()
{
	int cid = m_ctl_editChara->GetCurSel();
	int palidx = m_ctl_editColor->GetCurSel();

	if (cid != m_curCID)
	{
		int ret = checkSavePalette();
		if (ret == 1)
		{
			// yes
			writePalette();
			m_paletteChanged = false;
		}
		else if (ret == 0)
		{
			// cancel
			m_ctl_editChara->SetCurSel(m_curCID);
			return;
		}
		changeColorEditChara(cid);
		readImageData(0, palidx);
		updateGGXXPalette(false);
	}
}

void CggncfgDlg::OnCbnSelchangeEditcolor()
{
	int imgidx = m_ctl_editSprite->GetCurSel();
	int palidx = m_ctl_editColor->GetCurSel();
	
	if (palidx != m_curPalette)
	{
		int ret = checkSavePalette();
		if (ret == 1)
		{
			// yes
			writePalette();
			m_paletteChanged = false;
		}
		else if (ret == 0)
		{
			// cancel
			m_ctl_editColor->SetCurSel(m_curPalette);
			return;
		}
		readImageData(imgidx, palidx);
		updateGGXXPalette(false);
	}
}

void CggncfgDlg::OnCbnSelchangeEditsprite()
{
	int imgidx = m_ctl_editSprite->GetCurSel();
	readImageData(imgidx, -1);
}

void CggncfgDlg::arrangeControls()
{
	CRect r;
	GetClientRect(&r);

	int cx = r.right - r.left;
	int cy = r.bottom - r.top;

	CWnd* wnd;
	wnd = GetDlgItem(IDC_CEGROUP);
	if (wnd)
	{
		wnd->SetWindowPos(&CWnd::wndTop, 0, 0, cx - IMG_X - 3, cy - 93, SWP_NOMOVE);
		
		GetDlgItem(IDOK)->SetWindowPos(&CWnd::wndTop, cx - 176, cy - 26, 0, 0, SWP_NOSIZE);

		GetDlgItem(IDCANCEL)->SetWindowPos(&CWnd::wndTop, cx - 86, cy - 26, 0, 0, SWP_NOSIZE);

		GetDlgItem(IDC_SSAGROUP)->SetWindowPos(&CWnd::wndTop, 0, 0, cx - IMG_X - 3, 49, SWP_NOMOVE);
		GetDlgItem(IDC_LOBBYADDR)->SetWindowPos(&CWnd::wndTop, 0, 0, cx - IMG_X - 112, 19, SWP_NOMOVE);
		GetDlgItem(IDC_ADDADDR)->SetWindowPos(&CWnd::wndTop, cx - 103, 25, 0, 0, SWP_NOSIZE);
		GetDlgItem(IDC_DELETEADDR)->SetWindowPos(&CWnd::wndTop, cx - 63, 25, 0, 0, SWP_NOSIZE);
	}
}

void CggncfgDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	arrangeControls();

	CRect r = getImageRect();
	r.right  = cx;
	r.bottom = cy;
	InvalidateRect(&r);

	// Spriteの差分だけ
	//static int bx = r.right, by = r.bottom;
	//CRect r1;
	//r1.SetRect(r.right, r.top, r.right + bx - r.right, r.bottom + by - r.bottom);
	//InvalidateRect(&r1);
	//r1.SetRect(r.left, r.bottom, r.right + bx - r.right, r.bottom + by - r.bottom);
	//InvalidateRect(&r1);
	//bx = r.right;
	//by = r.bottom;
}

CRect CggncfgDlg::getImageRect(void)
{
	CRect r, sr;
	GetClientRect(&r);
	int w = r.right - r.left - IMG_X - 95;
	int h = r.bottom - r.top - IMG_Y - 44;
	if (w > IMG_W) w = IMG_W;
	if (h > IMG_H) h = IMG_H;
	
	sr.SetRect(IMG_X, IMG_Y, IMG_X + w, IMG_Y + h);
	return sr;
}

CRect CggncfgDlg::getPaletteRect(void)
{
	CRect r, sr;
	GetClientRect(&r);

	sr.SetRect(r.right - r.left - PAL_X, PAL_Y, r.right - r.left - PAL_X + PAL_W, PAL_Y + PAL_H);
	return sr;
}

void CggncfgDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 583;
	lpMMI->ptMinTrackSize.y = 462;
	lpMMI->ptMaxTrackSize.x = 900;
	lpMMI->ptMaxTrackSize.y = 600;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CggncfgDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_selectPalette != -1)
	{
		CPoint pt;
		GetCursorPos(&pt);
		CColorEditDlg	colorEditDlg(pt.x - 150, pt.y - 150, this);
		colorEditDlg.DoModal();
	}
	CDialog::OnLButtonDown(nFlags, point);
}

int CggncfgDlg::checkSavePalette(void)
{
	if (m_paletteChanged)
	{
		int ret = AfxMessageBox("Do you save changed palette?", MB_YESNOCANCEL);
		switch (ret)
		{
		case IDYES:		return 1; // save
		case IDNO:		return 2; // no save
		case IDCANCEL:	return 0; // cancel
			break;
		}
	}
	return 2;
}

bool CggncfgDlg::readPalette(CString p_palFilePath)
{
	bool result = false;

	FILE *fp = fopen(p_palFilePath, "rb");
	if (fp)
	{
		char buf[16+256*4];
		int size = zfsize(fp);
		if (size == 0x410)
		{
			zfread(buf, 16+256*4, fp);
			memcpy(m_palHeader, &buf[0], 16);
			memcpy(m_palette, &buf[16], 256 * 4);
			m_palsize = 256;
			result = true;
		}
		fclose(fp);
	}
	return result;
}

void CggncfgDlg::writePalette(void)
{
	FILE *fp = fopen(getPalettePath(m_curCID, m_curPalette), "wb");
	if (fp)
	{
		char buf[16+256*4];
		memcpy(&buf[0], m_palHeader, 16);
		memcpy(&buf[16], m_palette, 256 * 4);
		zfwrite(buf, 16+256*4, fp, 0);
		fclose(fp);
	}
}

void CggncfgDlg::OnBnClickedSavecolor()
{
	writePalette();
	m_paletteChanged = false;
}

void CggncfgDlg::OnTimer(UINT nIDEvent)
{
	static int pulse = 0;
	pulse = pulse++ % 10;
	static bool oldinv = 0;

	if (nIDEvent == 1000)
	{
		// 選択パレット変更
		if (GetActiveWindow() == this)
		{
			CRect	palRect = getPaletteRect();
			CPoint pt;
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			palRect.right -= 1;
			if (palRect.PtInRect(pt))
			{
				m_selectPalette = ((pt.x - palRect.left) / 8) + ((pt.y - palRect.top) / 8) * 8;
				if (m_selectPalette < 0 || m_selectPalette >= m_palsize) m_selectPalette = -1;
			}
			else m_selectPalette = -1;
		}

		bool inv = (pulse >= 8) &&
				(GetActiveWindow() == this) &&
				(m_selectPalette != -1);
		drawImage(inv);
		drawPalette();

		if (oldinv != inv) updateGGXXPalette(inv ? m_selectPalette : -1);
		oldinv = inv;
	}
	CDialog::OnTimer(nIDEvent);
}

void CggncfgDlg::OnBnClickedAddaddr()
{
	if (m_ctl_editLobbyAddr->GetCount() < 10)
	{
		CString str;
		m_ctl_editLobbyAddr->GetWindowText(str);
		int idx = m_ctl_editLobbyAddr->FindString(0, str);
		
		if (idx == CB_ERR) m_ctl_editLobbyAddr->AddString(str);
	}
	else
	{
		AfxMessageBox("Cannot Register Address.");
	}
}

void CggncfgDlg::OnBnClickedDeleteaddr()
{
	CString str;
	m_ctl_editLobbyAddr->GetWindowText(str);
	int idx = m_ctl_editLobbyAddr->FindString(0, str);

	if (idx != CB_ERR) m_ctl_editLobbyAddr->DeleteString(idx);
}

void CggncfgDlg::updateGGXXPalette(int p_flash)
{
	CSharedMemory	sm_pal("ggxxnet_pal", 2048);
	char val;
	/* キャラID */
	val = m_curCID;
	sm_pal.set(&m_curCID, 1, 1);
	/* キャラID */
	val = m_curPalette;
	sm_pal.set(&val, 2, 1);
	/* パレットヘッダー、データ */
	sm_pal.set(m_palHeader, 3, 16);
	sm_pal.set(m_palette, 16 + 3, 1024);
	/* 点滅カラー */
	if (p_flash != -1)
	{
		DWORD fpal = ~m_palette[p_flash];
		sm_pal.set(&fpal, 16 + 3 + 4 * p_flash, 4);
	}
	/* 更新フラグ立て */
	val = 1;
	sm_pal.set(&val, 0, 1);
}

void CggncfgDlg::OnBnClickedMessage()
{
	CEditMsgDlg dlg(this);

	dlg.DoModal();
}

void CggncfgDlg::OnDropFiles(HDROP hDropInfo)
{
	char dropFileName[1024] = "";
	DragQueryFile(hDropInfo, 0, dropFileName, 1024);
	if (readPalette(dropFileName))
	{
		setPaletteChanged(true);
	}

	CDialog::OnDropFiles(hDropInfo);
}
