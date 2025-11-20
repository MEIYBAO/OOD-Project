#pragma once
#include "afxdialogex.h"
#include "CDatabaseHelper.h"


// class_students 对话框

class class_students : public CDialogEx
{
	DECLARE_DYNAMIC(class_students)

public:
	class_students(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~class_students();

	// 设置数据库帮助器与 course_uid
	void SetDatabaseHelper(CDatabaseHelper* pDb) { m_pDb = pDb; }
	void SetCourseUid(const CString& uid) { m_courseUid = uid; }

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_class_student };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	CDatabaseHelper* m_pDb = nullptr;
	CString m_courseUid;
	CListCtrl m_studentsList;

	// 双击编辑支持
	CEdit    editItem;
	int      hitRow = -1;
	int      hitCol = -1;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditKillFocus();
	void OnEditKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
