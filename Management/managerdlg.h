#pragma once
#include "afxdialogex.h"
#include "CDatabaseHelper.h"

// manager 对话框

class manager : public CDialogEx
{
	DECLARE_DYNAMIC(manager)

public:
	manager(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~manager();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANAGER_DIALOG};
#endif

protected:
	CListCtrl m_list;
	CDatabaseHelper m_dbHelper;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedstudent_show();
	afx_msg void OnBnClickedteacher_show();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedcourse_show();
	afx_msg void OnBnClickedcounselor_show();

	afx_msg void OnListRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuAdd();
	afx_msg void OnMenuEdit();
	afx_msg void OnMenuDelete();

private:
	CString m_currentTable;
};
