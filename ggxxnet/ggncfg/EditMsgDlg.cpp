// EditMsgDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "EditMsgDlg.h"
#include "ggncfgDlg.h"

// CEditMsgDlg ダイアログ

IMPLEMENT_DYNAMIC(CEditMsgDlg, CDialog)

CEditMsgDlg::CEditMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditMsgDlg::IDD, pParent)
{
	m_ggncfgdlg = (CggncfgDlg*)pParent;
}

CEditMsgDlg::~CEditMsgDlg()
{
}

void CEditMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	SetDlgItemText(IDC_EDIT1, m_ggncfgdlg->m_setting_msg);
}


BEGIN_MESSAGE_MAP(CEditMsgDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CEditMsgDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT1, &CEditMsgDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CEditMsgDlg メッセージ ハンドラ

void CEditMsgDlg::OnBnClickedOk()
{
	CString str;
	GetDlgItemText(IDC_EDIT1, str);
	if (str.GetLength() > 255)
	{
		AfxMessageBox("Please input a message under 256 characters.");
		//((CEdit*)GetDlgItem(IDC_EDIT1))->SetFocus();
		//((CEdit*)GetDlgItem(IDC_EDIT1))->SetSel(0, -1);
		return;
	}

	GetDlgItemText(IDC_EDIT1, m_ggncfgdlg->m_setting_msg);
	OnOK();
}

void CEditMsgDlg::OnEnChangeEdit1()
{
	CString str;
	GetDlgItemText(IDC_EDIT1, str);
	
	str.Format("%d", str.GetLength());
	SetDlgItemText(IDC_CHARCOUNT, str);
}

BOOL CEditMsgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	OnEnChangeEdit1();

	return TRUE;
}
