// ggncfgDlg.h : ヘッダー ファイル
//

#pragma once

#define IMG_ADDR_LIST_SIZE	2048
#define PAL_ADDR_LIST_SIZE	12

// CggncfgDlg ダイアログ
class CggncfgDlg : public CDialog
{
// コンストラクション
public:
	CggncfgDlg(CWnd* pParent = NULL);	// 標準コンストラクタ
	~CggncfgDlg(void);

// ダイアログ データ
	enum { IDD = IDD_GGNCFG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void	readSettingFile(void);
	void	writeSettingFile(void);
	bool	valueCheck(void);

	CString	getSettingPath(void);
	
	CString getPalDirPath(void);
	CString getPalettePath(int p_cid, int p_palidx);
	CString getGGXXPath(void);
	CString getDataPath(int p_id);

	void clearImageOffset(void);
	void addImageOffset(int p_fid, int p_offset);
	void readImageData(int p_imgidx, int p_palidx);

	void changeColorEditChara(int p_charaid);

	void drawImage(bool p_inv = false);
	void drawPalette(void);

	CRect getImageRect(void);
	CRect getPaletteRect(void);
	void arrangeControls(void);

	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	afx_msg void OnBnClickedEditcolor();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnEnKillfocusUsername();
	afx_msg void OnCbnSelchangeEditchara();
	afx_msg void OnCbnSelchangeEditcolor();
	afx_msg void OnCbnSelchangeEditsprite();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedSavecolor();
	afx_msg void OnTimer(UINT nIDEvent);

	int		getSelectPalette(void)						{ return m_selectPalette; }
	DWORD	getPaletteColor(int p_pidx)					{ return m_palette[p_pidx]; }
	void	setPaletteColor(int p_pidx, DWORD p_color)	{ m_palette[p_pidx] = p_color; }
	void	setPaletteChanged(bool p_value)				{ m_paletteChanged = p_value; }

	int  checkSavePalette(void);
	bool readPalette(CString p_palFilePath);
	void writePalette(void);

	void updateGGXXPalette(int p_flash);

private:
	CEdit*		m_ctl_userName;
	CEdit*		m_ctl_trip;
	
	CButton*	m_ctl_enableNet;
	CEdit*		m_ctl_port;
	CComboBox*	m_ctl_delay;
	CButton*	m_ctl_useEx;
	CComboBox*	m_ctl_rounds;

	CButton*	m_ctl_enableChat;
	CButton*	m_ctl_ignoreSlow;
	
	CButton*	m_ctl_watch_enableBroadcast;
	CButton*	m_ctl_watch_allowIntrusion;
	CButton*	m_ctl_watch_saveReplay;
	CComboBox*	m_ctl_watch_maxRelayNode;

	CButton*	m_ctl_dispInvCombo;
	CButton*	m_ctl_showGGNVer;

	CComboBox*	m_ctl_editColor;
	CComboBox*	m_ctl_editChara;
	CComboBox*	m_ctl_editSprite;
	CButton*	m_ctl_saveColor;
	
	CComboBox*	m_ctl_editLobbyAddr;

//////////////////////////////////
	HDC		m_windc;

	HDC		m_imgdc;
	HBITMAP	m_imgbmp;
	HBITMAP	m_imgoldbmp;
	DWORD*	m_imgbits;
	
	HDC		m_paldc;
	HBITMAP	m_palbmp;
	HBITMAP	m_paloldbmp;
	DWORD*	m_palbits;

	int		m_imageW;
	int		m_imageH;
	int		m_palsize;
	DWORD	m_palette[256];
	DWORD	m_palHeader[16];
	BYTE*	m_image;
	DWORD	m_imageAddrList[IMG_ADDR_LIST_SIZE];
	char	m_imageFIDList[IMG_ADDR_LIST_SIZE];
	int		m_imageAddrListCount;
	DWORD	m_palAddrList[PAL_ADDR_LIST_SIZE];
	int		m_palAddrListCount;

	int		m_selectPalette;
	bool	m_paletteChanged;
	int		m_curCID;
	int		m_curPalette;

	WORD	m_setting_wins;
	char	m_setting_rank;
	int		m_setting_score;
	int		m_setting_totalBattle;
	int		m_setting_totalWin;
	int		m_setting_totalLose;
	int		m_setting_totalDraw;
	int		m_setting_totalError;
	int		m_setting_slowRate;

	int		m_datVersion;	// 起動時に開いたdatのバージョン　フォーマット変更する時はバックアップ取る
public:
	enum { DATVERSION = 120 };

public:
	CString	m_setting_msg;

	afx_msg void OnBnClickedAddaddr();
	afx_msg void OnBnClickedDeleteaddr();
	afx_msg void OnBnClickedMessage();
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
