#include <iostream>
#include <mysql.h>

int main()
{
    MYSQL *mysql = mysql_init(0);
    if (!mysql_real_connect(mysql, "localhost", "root", "djb1234567890", "SchoolDB", 0, 0, 0)) // 注意改成自己的
    {
        printf("连接数据库出错：%s", mysql_error(mysql));
        return -1;
    }
    std::cout << "DB connect success" << std::endl;

    if (!mysql_query(mysql, "SELECT * FROM user_account"))
    {
        MYSQL_RES *res = mysql_store_result(mysql);
        if (res && mysql_num_rows(res) > 0)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            // 第 0 列就是 email
            printf("email = %s\n", row[0] ? row[0] : "NULL");
        }
    }
    return 0;
}