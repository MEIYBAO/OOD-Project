// stu_dlg.cpp: 实现文件
//

#include "pch.h"
#include "Management.h"
#include "afxdialogex.h"
#include "stu_dlg.h"


// stu_dlg 对话框

IMPLEMENT_DYNAMIC(stu_dlg, CDialogEx)

stu_dlg::stu_dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
{

}

stu_dlg::~stu_dlg()
{
}

void stu_dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(stu_dlg, CDialogEx)
END_MESSAGE_MAP()


// stu_dlg 消息处理程序
