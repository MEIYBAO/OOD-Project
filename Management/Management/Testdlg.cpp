// Testdlg.cpp: 实现文件
//

#include "pch.h"
#include "Management.h"
#include "afxdialogex.h"
#include "Testdlg.h"


// Testdlg 对话框

IMPLEMENT_DYNAMIC(Testdlg, CDialogEx)

Testdlg::Testdlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

Testdlg::~Testdlg()
{
}

void Testdlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Testdlg, CDialogEx)
END_MESSAGE_MAP()


// Testdlg 消息处理程序
