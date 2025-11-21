// InputDlg.cpp: 实现文件
//

#include "pch.h"
#include "Management.h"
#include "afxdialogex.h"
#include "InputDlg.h"


// InputDlg 对话框

IMPLEMENT_DYNAMIC(InputDlg, CDialogEx)

InputDlg::InputDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INPUT_DIALOG, pParent)
{

}

InputDlg::~InputDlg()
{
}

void InputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INPUT, m_strInput);
}


BEGIN_MESSAGE_MAP(InputDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &InputDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// InputDlg 消息处理程序
void InputDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE); // 获取输入内容
	CDialogEx::OnOK();
}

void InputDlg::SetHint(const CString& hint)
{
	m_strHint = hint;
}

BOOL InputDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetDlgItemText(IDC_STATIC_HINT, m_strHint);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
