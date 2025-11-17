// class_students.cpp: 实现文件
//

#include "pch.h"
#include "Management.h"
#include "afxdialogex.h"
#include "class_students.h"


// class_students 对话框

IMPLEMENT_DYNAMIC(class_students, CDialogEx)

class_students::class_students(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_class_student, pParent)
{

}

class_students::~class_students()
{
}

void class_students::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(class_students, CDialogEx)
END_MESSAGE_MAP()


// class_students 消息处理程序
