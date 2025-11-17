#pragma once
#include "mysql.h"
#include "Global.h"

class CDatabaseHelper
{
public:
    CDatabaseHelper();
    virtual ~CDatabaseHelper();

    // 数据库连接管理
    BOOL Connect(const CString& strHost = _T("localhost"),
        const CString& strUser = _T("root"),
        const CString& strPassword = CString(mysql_password.c_str()),
        const CString& strDatabase = _T("Schooldb"),
        int nPort = 3306);
    void Disconnect();
    BOOL IsConnected() const { return m_bConnected; }

    // 表数据显示
    BOOL DisplayTableData(CListCtrl& listCtrl,
        const CString& strTableName,
        const CString& strColumns = _T("*"),
        const CString& strWhere = _T(""));

    // 数据操作
    BOOL InsertRecord(const CString& strTableName, const CStringArray& arrFields, const CStringArray& arrValues);
    BOOL UpdateRecord(const CString& strTableName, const CString& strWhere, const CStringArray& arrFields, const CStringArray& arrValues);
    BOOL DeleteRecord(const CString& strTableName, const CString& strWhere);

    // 工具函数
    static CString Utf8ToCString(const char* utf8Str);
    static CStringA CStringToUtf8(const CString& str);

private:
    MYSQL* m_pConnection;
    BOOL m_bConnected;

    BOOL ExecuteQuery(const CString& strSQL);
    CString EscapeString(const CString& strValue);
};

