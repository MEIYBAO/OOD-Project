#pragma once
#include "afxdialogex.h"
#include "CDatabaseHelper.h"
#include "tool.h"

// manager 对话框

class manager : public CDialogEx
{
	DECLARE_DYNAMIC(manager)

public:
	manager(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~manager();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANAGER_DLG};
#endif

protected:
	CListCtrl m_list;  //列表
	CDatabaseHelper m_dbHelper;  //数据库类
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedstudent_show();
	afx_msg void OnBnClickedteacher_show();
	afx_msg void OnBnClickedcourse_show();
	afx_msg void OnBnClickedcounselor_show();

	afx_msg void OnListRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMenuAdd();
	afx_msg void OnMenuDelete();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditKillFocus();
	afx_msg void OnEditKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void manager::OnEnChangeSemesterEdit();
	afx_msg void OnBnClickedchangeid();

	virtual BOOL PreTranslateMessage(MSG* pMsg); 

private:
	virtual BOOL OnInitDialog();

	CString m_currentTable = _T("无");
	CEdit editItem;
	CEdit m_semesterEdit;

	void SaveSemesterEdit();

	int hitRow;
	int hitCol;
};