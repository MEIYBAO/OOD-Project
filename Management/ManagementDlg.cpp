
// ManagementDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Management.h"
#include "ManagementDlg.h"
#include "afxdialogex.h"
#include "Testdlg.h"
#include <mysql.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CManagementDlg 对话框



CManagementDlg::CManagementDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MANAGEMENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CManagementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CManagementDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(login_button, &CManagementDlg::OnBnClickedlogin_button)
	ON_BN_CLICKED(exit_button, &CManagementDlg::OnBnClickedexit_button)
END_MESSAGE_MAP()


// CManagementDlg 消息处理程序

BOOL CManagementDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CManagementDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CManagementDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CManagementDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CManagementDlg::OnBnClickedlogin_button()
{

	// TODO: 在此添加控件通知处理程序代码

	MYSQL* conn = mysql_init(NULL);
	if (conn == NULL) {
		AfxMessageBox(_T("MySQL初始化失败！"));
		return;
	}

	// 连接数据库
	if (!mysql_real_connect(conn, "localhost", "root", "060613", "Schooldb", 3306, NULL, 0)) {
		AfxMessageBox(_T("数据库连接失败！"));
		mysql_close(conn);
		return;
	}
	// 获取用户输入的用户名和密码
	CString strusername;
	GetDlgItemText(username_input, strusername);

	CString strpassword;
	GetDlgItemText(password_input, strpassword);

	// 构造SQL语句
	CStringA usernameA(strusername);
	CStringA passwordA(strpassword);
	CStringA sqlA;
	sqlA.Format("SELECT role FROM user_account WHERE username='%s' AND password=MD5('%s')",
		usernameA.GetString(), passwordA.GetString());

	if (mysql_query(conn, sqlA.GetString()) == 0) {
		MYSQL_RES* res = mysql_store_result(conn);
		if (res && mysql_num_rows(res) > 0) {
			MYSQL_ROW row = mysql_fetch_row(res);
			CStringA roleA(row[0] ? row[0] : "");
			CString role(roleA);

			AfxMessageBox(_T("登录成功！"));
			mysql_free_result(res);
			mysql_close(conn);

			EndDialog(IDOK);

			if (role == _T("student")) {
				// 学生界面
				// StudentDlg dlg;
				// dlg.DoModal();
				Testdlg dlg; // 示例用Testdlg
				dlg.DoModal();
			}
			else if (role == _T("teacher")) {
				// 教师界面
				// TeacherDlg dlg;
				// dlg.DoModal();
				Testdlg dlg; // 示例用Testdlg
				dlg.DoModal();
			}
			else if (role == _T("counselor")) {
				// 辅导员界面
				// CounselorDlg dlg;
				// dlg.DoModal();
				Testdlg dlg; // 示例用Testdlg
				dlg.DoModal();
			}
			else if (role == _T("manager")) {
				// 管理员界面
				// ManagerDlg dlg;
				// dlg.DoModal();
				Testdlg dlg; // 示例用Testdlg
				dlg.DoModal();
			}
			else {
				AfxMessageBox(_T("未知身份，无法进入系统！"));
			}
			return;
		}
		else {
			AfxMessageBox(_T("用户名或密码错误！"));
			if (res) mysql_free_result(res);
		}
	}
	else {
		AfxMessageBox(_T("查询失败！"));
	}

	mysql_close(conn);
}

void CManagementDlg::OnBnClickedexit_button()
{

	// TODO: 在此添加控件通知处理程序代码

	EndDialog(IDOK);

}
