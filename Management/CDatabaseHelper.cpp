#include "pch.h"

#include "CDatabaseHelper.h"

CDatabaseHelper::CDatabaseHelper()
    : m_pConnection(NULL)
    , m_bConnected(FALSE)
{
}

CDatabaseHelper::~CDatabaseHelper()
{
    Disconnect();
}

BOOL CDatabaseHelper::Connect(const CString& strHost, const CString& strUser,
    const CString& strPassword, const CString& strDatabase, int nPort)
{
    // 如果已经连接，先断开
    if (m_bConnected)
        Disconnect();

    // 初始化MySQL
    m_pConnection = mysql_init(NULL);
    if (m_pConnection == NULL)
    {
        AfxMessageBox(_T("MySQL初始化失败！"));
        return FALSE;
    }

    // 转换字符串为ANSI
    CStringA strHostA(strHost);
    CStringA strUserA(strUser);
    CStringA strPasswordA(strPassword);
    CStringA strDatabaseA(strDatabase);

    // 连接数据库
    if (!mysql_real_connect(m_pConnection,
        strHostA,
        strUserA,
        strPasswordA,
        strDatabaseA,
        nPort, NULL, 0))
    {
        CString strError;
        strError.Format(_T("数据库连接失败！错误：%hs"), mysql_error(m_pConnection));
        AfxMessageBox(strError);
        mysql_close(m_pConnection);
        m_pConnection = NULL;
        return FALSE;
    }

    // 设置字符集为utf8
    if (mysql_set_character_set(m_pConnection, "utf8") != 0)
    {
        CString strError;
        strError.Format(_T("设置字符集失败！错误：%hs"), mysql_error(m_pConnection));
        AfxMessageBox(strError);
        mysql_close(m_pConnection);
        m_pConnection = NULL;
        return FALSE;
    }

    m_bConnected = TRUE;
    return TRUE;
}

void CDatabaseHelper::Disconnect()
{
    if (m_pConnection)
    {
        mysql_close(m_pConnection);
        m_pConnection = NULL;
    }
    m_bConnected = FALSE;
}

BOOL CDatabaseHelper::DisplayTableData(CListCtrl& listCtrl,
    const CString& strTableName,
    const CString& strColumns,
    const CString& strWhere)
{
    if (!m_bConnected)
    {
        AfxMessageBox(_T("数据库未连接!"));
        return FALSE;
    }

    CWaitCursor wait; // 显示等待光标

    // 清空List Control
    listCtrl.DeleteAllItems();

    // 删除所有列
    int nColumnCount = listCtrl.GetHeaderCtrl()->GetItemCount();
    for (int i = nColumnCount - 1; i >= 0; i--)
    {
        listCtrl.DeleteColumn(i);
    }

    // 构建SQL查询
    CString strSQL;
    if (strWhere.IsEmpty())
    {
        strSQL.Format(_T("SELECT %s FROM %s"), strColumns, strTableName);
    }
    else
    {
        strSQL.Format(_T("SELECT %s FROM %s WHERE %s"), strColumns, strTableName, strWhere);
    }

    CStringA strSQLA = CStringToUtf8(strSQL);

    // 执行查询
    if (mysql_query(m_pConnection, strSQLA))
    {
        CString strError;
        strError.Format(_T("查询失败！错误：%hs"), mysql_error(m_pConnection));
        AfxMessageBox(strError);
        return FALSE;
    }

    MYSQL_RES* res = mysql_store_result(m_pConnection);
    if (!res)
    {
        CString strError;
        strError.Format(_T("获取结果失败！错误：%hs"), mysql_error(m_pConnection));
        AfxMessageBox(strError);
        return FALSE;
    }

    int num_fields = mysql_num_fields(res);
    MYSQL_FIELD* fields = mysql_fetch_fields(res);

    // 设置List Control列
    for (int i = 0; i < num_fields; ++i)
    {
        CString colName = Utf8ToCString(fields[i].name);
        listCtrl.InsertColumn(i, colName, LVCFMT_LEFT, 120);
    }

    // 设置List Control扩展样式
    listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);

    // 填充数据
    int rowIndex = 0;
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)) != NULL)
    {
        // 第一列
        CString strValue = Utf8ToCString(row[0]);
        int nItem = listCtrl.InsertItem(rowIndex, strValue);

        // 其余列
        for (int i = 1; i < num_fields; ++i)
        {
            CString strCol;
            if (row[i])
            {
                strCol = Utf8ToCString(row[i]);
            }
            else
            {
                strCol = _T("NULL");
            }
            listCtrl.SetItemText(nItem, i, strCol);
        }
        ++rowIndex;
    }

    // 自动调整列宽
    for (int i = 0; i < num_fields; i++)
    {
        listCtrl.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
    }

    // 清理结果集
    mysql_free_result(res);

    return TRUE;
}

BOOL CDatabaseHelper::InsertRecord(const CString& strTableName, const CStringArray& arrFields, const CStringArray& arrValues)
{
    if (!m_bConnected || arrFields.GetSize() != arrValues.GetSize())
        return FALSE;

    CString strSQL;
    strSQL.Format(_T("INSERT INTO %s ("), strTableName);

    // 添加字段名
    for (int i = 0; i < arrFields.GetSize(); i++)
    {
        strSQL += arrFields[i];
        if (i < arrFields.GetSize() - 1)
            strSQL += _T(", ");
    }

    strSQL += _T(") VALUES (");

    // 添加字段值
    for (int i = 0; i < arrValues.GetSize(); i++)
    {
        strSQL += _T("'") + EscapeString(arrValues[i]) + _T("'");
        if (i < arrValues.GetSize() - 1)
            strSQL += _T(", ");
    }

    strSQL += _T(")");

    return ExecuteQuery(strSQL);
}

BOOL CDatabaseHelper::UpdateRecord(const CString& strTableName, const CString& strWhere, const CStringArray& arrFields, const CStringArray& arrValues)
{
    if (!m_bConnected || arrFields.GetSize() != arrValues.GetSize())
        return FALSE;

    CString strSQL;
    strSQL.Format(_T("UPDATE %s SET "), strTableName);

    // 添加更新字段
    for (int i = 0; i < arrFields.GetSize(); i++)
    {
        strSQL += arrFields[i] + _T(" = '") + EscapeString(arrValues[i]) + _T("'");
        if (i < arrFields.GetSize() - 1)
            strSQL += _T(", ");
    }

    if (!strWhere.IsEmpty())
    {
        strSQL += _T(" WHERE ") + strWhere;
    }

    return ExecuteQuery(strSQL);
}

BOOL CDatabaseHelper::DeleteRecord(const CString& strTableName, const CString& strWhere)
{
    if (!m_bConnected)
        return FALSE;

    CString strSQL;
    if (strWhere.IsEmpty())
    {
        strSQL.Format(_T("DELETE FROM %s"), strTableName);
    }
    else
    {
        strSQL.Format(_T("DELETE FROM %s WHERE %s"), strTableName, strWhere);
    }

    return ExecuteQuery(strSQL);
}

BOOL CDatabaseHelper::ExecuteQuery(const CString& strSQL)
{
    if (!m_bConnected)
        return FALSE;

    CStringA strSQLA = CStringToUtf8(strSQL);

    if (mysql_query(m_pConnection, strSQLA))
    {
        CString strError;
        strError.Format(_T("执行SQL失败！错误：%hs"), mysql_error(m_pConnection));
        AfxMessageBox(strError);
        return FALSE;
    }

    return TRUE;
}

CString CDatabaseHelper::EscapeString(const CString& strValue)
{
    if (!m_bConnected)
        return strValue;

    CStringA strValueA = CStringToUtf8(strValue);

    // 分配足够的空间用于转义后的字符串
    char* szEscaped = new char[strValueA.GetLength() * 2 + 1];
    mysql_real_escape_string(m_pConnection, szEscaped, strValueA, strValueA.GetLength());

    CString strResult = Utf8ToCString(szEscaped);
    delete[] szEscaped;

    return strResult;
}

// 静态工具函数
CString CDatabaseHelper::Utf8ToCString(const char* utf8Str)
{
    if (!utf8Str) return _T("");

    int len = MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, NULL, 0);
    if (len <= 0) return _T("");

    CString ret;
    MultiByteToWideChar(CP_UTF8, 0, utf8Str, -1, ret.GetBuffer(len), len);
    ret.ReleaseBuffer();
    return ret;
}

CStringA CDatabaseHelper::CStringToUtf8(const CString& str)
{
    int len = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
    if (len <= 0) return CStringA();

    CStringA ret;
    WideCharToMultiByte(CP_UTF8, 0, str, -1, ret.GetBuffer(len), len, NULL, NULL);
    ret.ReleaseBuffer();
    return ret;
}