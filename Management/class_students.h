#pragma once
#include "afxdialogex.h"


// class_students 对话框

class class_students : public CDialogEx
{
	DECLARE_DYNAMIC(class_students)

public:
	class_students(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~class_students();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_class_student };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
