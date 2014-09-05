#pragma once

#include "resource.h"

// CEditMsgDlg ダイアログ
class CEditMsgDlg : public CDialog
{
	DECLARE_DYNAMIC(CEditMsgDlg)

public:
	CEditMsgDlg(CWnd* pParent = NULL);
	virtual ~CEditMsgDlg();

// ダイアログ データ
	enum { IDD = IDD_EDITMSGDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	class CggncfgDlg*	m_ggncfgdlg;

	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnEnChangeEdit1();
public:
	virtual BOOL OnInitDialog();
};
