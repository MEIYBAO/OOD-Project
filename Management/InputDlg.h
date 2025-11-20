#pragma once
#include "afxdialogex.h"


// InputDlg 对话框

class InputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(InputDlg)

public:
	InputDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~InputDlg();

	CString m_strInput; // 用户输入内容
	CString m_strHint; // 字段提示

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	void SetHint(const CString& hint);
	virtual BOOL OnInitDialog();
};
