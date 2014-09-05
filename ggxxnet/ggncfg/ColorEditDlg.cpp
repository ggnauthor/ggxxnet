// ColorEditDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "ggncfg.h"
#include "ggncfgDlg.h"
#include "ColorEditDlg.h"

// CColorEditDlg ダイアログ

IMPLEMENT_DYNAMIC(CColorEditDlg, CDialog)
CColorEditDlg::CColorEditDlg(int p_x, int p_y, CWnd* pParent /*=NULL*/)
	: CDialog(CColorEditDlg::IDD, pParent)
{
	m_posx = p_x;
	m_posy = p_y;
	m_ggncfgdlg = (CggncfgDlg*)pParent;
	m_selectPaletteIdx = m_ggncfgdlg->getSelectPalette();
}

CColorEditDlg::~CColorEditDlg()
{
}

void CColorEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CColorEditDlg, CDialog)
	ON_EN_CHANGE(IDC_BE, OnEnChangeBe)
	ON_EN_CHANGE(IDC_GE, OnEnChangeGe)
	ON_EN_CHANGE(IDC_RE, OnEnChangeRe)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


BOOL CColorEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&CWnd::wndTop, m_posx, m_posy, 0, 0, SWP_NOSIZE);

	DWORD color = m_ggncfgdlg->getPaletteColor(m_selectPaletteIdx);
	m_oldColor = color;

	((CSliderCtrl*)GetDlgItem(IDC_RS))->SetRange(0, 255);
	((CSliderCtrl*)GetDlgItem(IDC_GS))->SetRange(0, 255);
	((CSliderCtrl*)GetDlgItem(IDC_BS))->SetRange(0, 255);
	((CSliderCtrl*)GetDlgItem(IDC_RS))->SetPageSize(10);
	((CSliderCtrl*)GetDlgItem(IDC_GS))->SetPageSize(10);
	((CSliderCtrl*)GetDlgItem(IDC_BS))->SetPageSize(10);

	((CSliderCtrl*)GetDlgItem(IDC_RS))->SetPos(color & 0xFF);
	((CSliderCtrl*)GetDlgItem(IDC_GS))->SetPos((color >> 8)  & 0xFF);
	((CSliderCtrl*)GetDlgItem(IDC_BS))->SetPos((color >> 16)  & 0xFF);

	char c[32];
	_itoa(color & 0xFF, c, 10);
	GetDlgItem(IDC_RE)->SetWindowText(c);
	_itoa((color >> 8) & 0xFF, c, 10);
	GetDlgItem(IDC_GE)->SetWindowText(c);
	_itoa((color >> 16) & 0xFF, c, 10);
	GetDlgItem(IDC_BE)->SetWindowText(c);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CColorEditDlg::OnEnChangeBe()
{
	CString s;
	GetDlgItem(IDC_BE)->GetWindowText(s);
	((CSliderCtrl*)GetDlgItem(IDC_BS))->SetPos(atoi(s));

	updateColor();
}

void CColorEditDlg::OnEnChangeGe()
{
	CString s;
	GetDlgItem(IDC_GE)->GetWindowText(s);
	((CSliderCtrl*)GetDlgItem(IDC_GS))->SetPos(atoi(s));

	updateColor();
}

void CColorEditDlg::OnEnChangeRe()
{
	CString s;
	GetDlgItem(IDC_RE)->GetWindowText(s);
	((CSliderCtrl*)GetDlgItem(IDC_RS))->SetPos(atoi(s));

	updateColor();
}

void CColorEditDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* slider = (CSliderCtrl*)pScrollBar;
	
	int color = slider->GetPos();
	char c[32];
	_itoa(color & 0xFF, c, 10);

	switch (slider->GetDlgCtrlID())
	{
	case IDC_RS: GetDlgItem(IDC_RE)->SetWindowText(c); break;
	case IDC_GS: GetDlgItem(IDC_GE)->SetWindowText(c); break;
	case IDC_BS: GetDlgItem(IDC_BE)->SetWindowText(c); break;
	}

	updateColor();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CColorEditDlg::updateColor(void)
{
	DWORD r = ((CSliderCtrl*)GetDlgItem(IDC_RS))->GetPos();
	DWORD g = ((CSliderCtrl*)GetDlgItem(IDC_GS))->GetPos();
	DWORD b = ((CSliderCtrl*)GetDlgItem(IDC_BS))->GetPos();
	m_ggncfgdlg->setPaletteColor(m_selectPaletteIdx, RGB(r,g,b));

	m_ggncfgdlg->updateGGXXPalette(-1);
}

void CColorEditDlg::OnBnClickedCancel()
{
	((CSliderCtrl*)GetDlgItem(IDC_RS))->SetPos(m_oldColor & 0xFF);
	((CSliderCtrl*)GetDlgItem(IDC_GS))->SetPos((m_oldColor >> 8) & 0xFF);
	((CSliderCtrl*)GetDlgItem(IDC_BS))->SetPos((m_oldColor >> 16) & 0xFF);
	updateColor();

	OnCancel();
}

void CColorEditDlg::OnBnClickedOk()
{
	m_ggncfgdlg->setPaletteChanged(true);
	OnOK();
}
