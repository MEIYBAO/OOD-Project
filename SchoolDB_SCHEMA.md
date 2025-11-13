# SchoolDB 数据库说明文档

## 概述
本文件描述仓库 `backend/SchoolDB.sql` 中定义的测试数据库 `SchoolDB` 的模式（schema）、字段说明、表之间的关系，以及导入与验证步骤和常见问题提示。该数据库用于教学/演示场景，包含学生、教师、辅导员、课程、选课与授课安排等基本实体。

## 重要说明
- 脚本使用 `INSERT IGNORE`，因此可重复运行以安全填充示例数据（不会因为主键冲突而失败）。
- 所有表使用 InnoDB 引擎并以 `utf8mb4` 字符集存储。
- UID 字段长度必须与应用端一致（脚本使用 student: CHAR(12)、teacher/counselor/class/course: CHAR(10) 等）。

## 表结构（摘要）

下面为每张表的字段逐项注释，包含字段含义、示例与约束建议。

1) student
- student_uid CHAR(12) PRIMARY KEY
	- 含义：学生唯一标识（建议格式：入学年 + 序号，如 '202400000001'）。
	- 约束：非空、固定长度 12。
	- 示例：'202400000001'
- name VARCHAR(128)
	- 含义：学生姓名。
	- 示例：'陈晓明'
- major VARCHAR(128)
	- 含义：所学专业。
	- 示例：'计算机科学与技术'
- telephone VARCHAR(32)
	- 含义：手机号或座机。
	- 示例：'13810000001'
- wechat_id VARCHAR(64)
	- 含义：微信号或第三方标识。
	- 示例：'chenxm'
- email VARCHAR(255)
	- 含义：邮箱地址（用于通知/找回密码）。
	- 示例：'chenxm@example.com'

2) teacher
- teacher_uid CHAR(10) PRIMARY KEY
	- 含义：教师唯一标识（例如：'10000001'）。
	- 约束：非空、长度 10。
- name VARCHAR(128)
	- 教师姓名。
- telephone VARCHAR(32)
	- 联系电话。
- wechat_id VARCHAR(64)
	- 微信号。
- email VARCHAR(255)
	- 邮箱地址。

3) course
- course_uid CHAR(10) PRIMARY KEY
	- 含义：课程编号（例如：'COURSE0001'）。
	- 约束：非空、长度 10。
- course_name VARCHAR(128)
	- 课程名称。
- credits INT
	- 学分数。
- category VARCHAR(64)
	- 课程类别（如 '必修' / '选修'）。

4) counselor
- counselor_uid CHAR(10) PRIMARY KEY
	- 含义：辅导员编号（例如：'C000000001'）。
- name VARCHAR(128)
	- 姓名。
- telephone VARCHAR(32)
	- 联系电话。
- wechat_id VARCHAR(64)
	- 微信号。
- email VARCHAR(255)
	- 邮箱。

5) student_counselor
- student_uid CHAR(12)
	- 学生 UID，外键，引用 `student(student_uid)`。
- counselor_uid CHAR(10)
	- 辅导员 UID，外键，引用 `counselor(counselor_uid)`。
- PRIMARY KEY (student_uid, counselor_uid)
	- 用途：避免重复记录一对学生-辅导员关系。

6) courseSelection
- student_uid CHAR(12)
	- 外键，引用 `student(student_uid)`。
- course_uid CHAR(10)
	- 外键，引用 `course(course_uid)`。
- selection_date DATETIME DEFAULT CURRENT_TIMESTAMP
	- 含义：选课时间。
	- 示例：'2025-09-01 09:00:00'
- grade DECIMAL(5,2)
	- 含义：成绩，保留两位小数。
	- 示例：88.50
- FirstRepair BOOLEAN DEFAULT 0
	- 含义：是否初修（1 表示初修，0 表示重修）。
- PRIMARY KEY (student_uid, course_uid, FirstRepair)
	- 说明：允许在区分初修/重修状态下记录多条同课记录；若不需要此语义，可改为仅 student_uid+course_uid 为主键。

7) teacher_course
- teacher_uid CHAR(10)
	- 外键，引用 `teacher(teacher_uid)`。
- course_uid CHAR(10)
	- 外键，引用 `course(course_uid)`。
- semester VARCHAR(16)
	- 含义：学期标识，例如 '2025-1'。
- PRIMARY KEY (teacher_uid, course_uid, semester)
	- 说明：每学期每门课程对应一名教师记录。

8) teach_class
- class_id CHAR(10) PRIMARY KEY
	- 含义：教学班/课程班编号（例如 'CLS000001'）。
- course_uid CHAR(10)
	- 外键，引用 `course(course_uid)`。
- teacher_uid CHAR(10)
	- 外键，引用 `teacher(teacher_uid)`。
- semester VARCHAR(16)
	- 学期。
- schedule VARCHAR(64)
	- 上课安排（文本），例如 '周一 09:00-11:00'。
- location VARCHAR(64)
	- 上课地点，例如 '教学楼A-101'。

9) user_account
- username VARCHAR(64) PRIMARY KEY
	- 含义：账户名，通常等于 student_uid/teacher_uid/counselor_uid 或管理员自定义用户名。
	- 示例：'202400000001'、'admin01'
- password CHAR(32)
	- 含义：密码哈希（示例脚本使用 MD5）；生产环境请改用更安全的哈希算法并加盐。
- role ENUM('student','teacher','counselor','manager')
	- 含义：账户角色，决定权限范围。
- created_at DATETIME DEFAULT CURRENT_TIMESTAMP
	- 含义：账户创建时间。


## ER 关系（简要）
- student 1—* student_counselor *—1 counselor
- student 1—* courseSelection *—1 course
- teacher 1—* teacher_course *—1 course
- teacher 1—* teach_class *—1 course
- user_account 与 student/teacher/counselor 通过 username 映射（非强制外键）

## 导入指引（本地 MySQL）
1. 使用 MySQL 客户端导入（会在脚本里创建数据库并填充）：

```bash
mysql -u root -p < backend/SchoolDB.sql
```

2. 若要逐步执行便于排错，可打开 mysql shell：

```sql
SOURCE /path/to/backend/SchoolDB.sql;
```

3. 如需在重复运行脚本时避免外键问题，可临时禁用外键检查（谨慎使用）：

```sql
SET FOREIGN_KEY_CHECKS=0;
-- RUN SCRIPT
SET FOREIGN_KEY_CHECKS=1;
```

## 基本验证查询（运行导入后）
- 确认每表记录数：
```sql
SELECT COUNT(*) FROM student;
SELECT COUNT(*) FROM teacher;
SELECT COUNT(*) FROM counselor;
SELECT COUNT(*) FROM course;
SELECT COUNT(*) FROM student_counselor;
SELECT COUNT(*) FROM courseSelection;
SELECT COUNT(*) FROM teacher_course;
SELECT COUNT(*) FROM teach_class;
SELECT COUNT(*) FROM user_account;
```

- 示例联表查询：
```sql
-- 查询学生及其辅导员
SELECT s.student_uid, s.name AS student_name, c.name AS counselor_name
FROM student s
LEFT JOIN student_counselor sc ON s.student_uid = sc.student_uid
LEFT JOIN counselor c ON sc.counselor_uid = c.counselor_uid
LIMIT 20;

-- 学生成绩示例
SELECT s.name, cs.course_uid, cs.grade
FROM courseSelection cs
JOIN student s ON cs.student_uid = s.student_uid
ORDER BY cs.grade DESC
LIMIT 20;
```

## 注意事项与改进建议
- 密码存储：示例使用 MD5，仅用于测试；生产环境请使用 bcrypt/argon2 并加盐。
- UID 长度：确保前端/后端生成 UID 时与脚本一致（student 12 位，teacher/counselor 10 位）。
- 约束策略：若希望脚本每次重建数据库更干净，可在脚本顶部加上 `DROP DATABASE IF EXISTS SchoolDB;`。
- 若要生成大量随机数据，可使用脚本（我可以提供 Python/Node 实现）。

## 联系与支持
如需我进一步生成更大量的数据、绘制 ER 图（可输出为 PNG/SVG），或把此 schema 转为 ORM 模型（例如 Sequelize/TypeORM），告诉我目标工具与需求，我会继续实现。
