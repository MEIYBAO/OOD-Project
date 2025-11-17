#pragma once
#include "afxdialogex.h"
#include "CDatabaseHelper.h"

// teacherdlg 对话框

class teacherdlg : public CDialogEx
{
	DECLARE_DYNAMIC(teacherdlg)

public:
	teacherdlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~teacherdlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_teacher_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CDatabaseHelper m_dbHelper;
	CListCtrl m_list;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedshowclass();

	virtual BOOL OnInitDialog();
	afx_msg void OnLvnItemchangedworkpanel(NMHDR* pNMHDR, LRESULT* pResult);
};
