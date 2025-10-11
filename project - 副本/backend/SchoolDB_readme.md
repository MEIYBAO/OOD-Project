# 教务系统数据库说明书

本文档说明当前 `db.sql` 中的表结构、字段含义、主键/外键关系、常用示例查询及改进建议。脚本示例为 MySQL (InnoDB, utf8mb4) 格式。

## 概览

当前最小化示例数据库包含以下表：

- `student`：学生基本信息表
- `teacher`：教师基本信息表
- `counselor`：辅导员基本信息表
- `user_account`：系统登录账号表（由 student/teacher/counselor 生成示例数据）

所有表使用 InnoDB 引擎以支持事务与外键约束（示例脚本已设置 ENGINE=InnoDB DEFAULT CHARSET=utf8mb4）。

---

## 表说明

### `student`

- 说明：存储学生基础信息。
- 主键：`student_uid`（CHAR(12)） — 设计为固定长度 12 的学号/UID，由上层系统生成并保证唯一性。
- 字段：
  - `student_uid` CHAR(12) NOT NULL PRIMARY KEY — 学生唯一标识（示例：'202400000001'）
  - `name` VARCHAR(128) — 姓名
  - `major` VARCHAR(128) — 专业
  - `telephone` VARCHAR(32] — 电话
  - `wechat_id` VARCHAR(64) — 微信号
  - `email` VARCHAR(255) — 邮箱

注意事项：保持 `student_uid` 的固定长度和唯一性，避免使用自增数作为学号，以便与学校学籍系统或工号保持一致。

### `teacher`

- 说明：存储教师基础信息。
- 主键：`teacher_uid`（CHAR(8)） — 设计为固定长度 8 的教师 UID（示例：'10000001'）。
- 字段：
  - `teacher_uid` CHAR(8 or 10?) NOT NULL PRIMARY KEY — 教师唯一标识（仓库文件使用 CHAR(10)；如果你想严格 8 位，请统一为 CHAR(8) 并同步脚本）
  - `name` VARCHAR(128) — 姓名
  - `telephone` VARCHAR(32) — 电话
  - `wechat_id` VARCHAR(64) — 微信号
  - `email` VARCHAR(255) — 邮箱

注意事项：确认 `teacher_uid` 定长策略（当前脚本示例使用 10 字符列，但需求中曾指定 8 位）。若需要 8 位，请把表与相关 FK 一并调整。

### `counselor`

- 说明：存储辅导员信息（用于示例）。
- 主键：`counselor_uid` CHAR(10)
- 字段：
  - `counselor_uid` CHAR(10) NOT NULL PRIMARY KEY
  - `name` VARCHAR(128)
  - `telephone` VARCHAR(32)
  - `wechat_id` VARCHAR(64)
  - `email` VARCHAR(255)

示例已插入一条辅导员记录：`C000000001`。

### `user_account`

- 说明：保存系统登录账号信息。示例脚本将 student/teacher/counselor 的 UID 导出为用户名并插入默认密码（MD5('123456')）。仅用于演示，生产环境请使用更安全的哈希（bcrypt/argon2）与盐值。
- 主键：`username` VARCHAR(64)
- 字段：
  - `username` VARCHAR(64) PRIMARY KEY — 帐号名，示例使用 student_uid/teacher_uid/counselor_uid
  - `password` CHAR(32) NOT NULL — 示例使用 MD5（32 字符），生产须用更安全方案
  - `role` ENUM('student','teacher','counselor','manager') NOT NULL — 角色
  - `created_at` DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP — 创建时间

示例插入：
- 从 `student` 插入：username = student_uid, role = 'student'
- 从 `teacher` 插入：username = teacher_uid, role = 'teacher'
- 从 `counselor` 插入：username = counselor_uid, role = 'counselor'
- 另外插入了三位 manager 示例用户：`myb`, `dcc`, `djb`（密码 MD5('123456')）

注意：脚本中的一处错误已修复（原来从 teacher 表 SELECT counselor 的 uid），现已改为从 `counselor` 表 SELECT；并补上 manager INSERT 的分号。

---

## 表关系（关系描述）

- `user_account.username` 对应 `student.student_uid` / `teacher.teacher_uid` / `counselor.counselor_uid`（逻辑关联，脚本中为示例导入，未强制外键约束）。
- 当前最小脚本仅包含用户/人员基础信息表；后续可添加下列表并建立外键：
  - `course`：课程表（course_id PK）
  - `class_offering`：教学班/开课信息（包含 course_id, teacher_uid, capacity 等）
  - `enrollment`：选课表（student_uid FK -> student, class_offering_id FK -> class_offering）
  - `assessment`：成绩表（enrollment_id FK 或 student_uid+class_offering_id 复合 FK）

若添加这些表，应确保：
- `class_offering.teacher_uid` 使用与 `teacher.teacher_uid` 相同的列类型与长度；
- `enrollment.student_uid` 使用与 `student.student_uid` 相同类型与长度；
- 在外键上建立索引以提高查询性能（例如 enrollment(student_uid)、class_offering(course_id)、enrollment(class_offering_id)）。

---

## 常用示例查询

- 查询某学生的用户信息：

```sql
SELECT u.username,u.role,u.created_at,s.name,s.major
FROM user_account u
JOIN student s ON u.username = s.student_uid
WHERE s.student_uid = '202400000001';
```

- 列出所有教师账号：

```sql
SELECT u.username,u.created_at,t.name,t.email
FROM user_account u
JOIN teacher t ON u.username = t.teacher_uid
WHERE u.role = 'teacher';
```

---

## 改进建议与注意事项

1. 密码安全：不要在生产中使用 MD5；采用 bcrypt/argon2 并存储盐值。
2. 主键长度一致性：统一 `teacher_uid` 长度（8 或 10），并在整个 schema 中保持一致。
3. 外键约束：如果要强制 referential integrity，请在 `user_account` 或其它表上添加外键，但注意 `user_account` 为通用账号表时，跨表外键（针对多种实体）不易建模，可使用单独的 profile 表或在应用层保持一致。
4. 索引：为查询频繁的列建立索引（如 student_uid、teacher_uid、email）。
5. 事务与并发：为选课类操作设计事务和锁或乐观并发策略，避免超额选课（详见待办项“并发/容量/冲突处理方案”）。

---

## 结尾

如果你希望，我可以：

- 把 `teacher_uid` 改为严格 CHAR(8) 并更新所有脚本；
- 增加课程/教学班/选课/成绩表并写完整的 seed 数据；
- 将 `user_account` 改为更安全的 password 存储方式示例（bcrypt），并演示如何迁移现有 MD5 密码；
- 编写选课事务存储过程并加入并发测试脚本。

完成：`project/backend/db_schema_readme.md`
