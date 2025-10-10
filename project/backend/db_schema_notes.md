进度摘要

- 我已在仓库中创建示例数据库模式文件：`project/backend/db_schema.sql`（面向教务系统，遵循 3NF）。

picture 目录（已检查）：
````markdown
进度摘要

- 我已在仓库中创建示例数据库模式文件：`project/backend/db_schema.sql`（面向教务系统，遵循 3NF）。

picture 目录（已检查）：
- picture/选课信息.png
- picture/课程.png
- picture/老师.png
- picture/教学班级.png
- picture/教务系统.png
- picture/成绩表.png
- picture/学生.png
- picture/8958223c612e0c7af4aaf197e3b286b.jpg

已做工作

1. 在 `project/backend/db_schema.sql` 中增加了一个教务系统示例建表脚本（包含 `term`, `student`, `teacher`, `course`, `class_offering`, `enrollment`, `assessment` 等表），并为常见的约束、索引与外键关系提供了合理设置。
2. 检查了 `picture/` 目录，列出所有图片文件名（上面列出）。这些图片看起来是教务系统相关的类图或实体图。若将这些图片内容文本化（OCR），我可以把图片中的类属性和关系直接并入最终模式中。

建议的下一步（请选择其中一项或多个）

A) 图片 OCR 与字段抽取（推荐）
- 我可以对 `picture/` 中的教务相关图片运行 OCR，将类图/字段提取为文本并生成更精确的数据库表结构。
- 需要你确认是否允许我读取这些图片并执行 OCR。此操作在本地工作区进行，不会上传任何文件到外部网络。

B) 合并到现有数据库（可选）
- 如果你希望把教务系统 schema 合并到现有的 `project/backend/restaurant_db.sql` 中，我可以分析现有 SQL（字段名、类型、外键命名习惯）并做兼容性调整。

C) 确认额外实体或业务规则
- 是否需要院系（department）、班级/年级（student_class）、课程先修关系（prerequisite）、课程标签（tag）、教室（room）、更细粒度的排课（class_schedule）等？
- 是否有业务规则：如成绩录入策略、补考规则、选课冲突检测策略、选课容量限制等？这些会影响索引与完整性约束的设计。

---

## 在 SQL Server Management Studio (SSMS) 中运行说明

1. 在 SSMS 中连接你的 SQL Server 实例。
2. 新建查询并执行（可先创建数据库）：

```sql
CREATE DATABASE SchoolDB;
GO
USE SchoolDB;
GO
-- 然后把 `project/backend/db_schema_mssql.sql` 中的内容复制到查询窗口并执行
```

3. 如果你喜欢直接从文件导入，可以在 SSMS 中使用“文件 -> 打开 -> 文件”，打开 `db_schema_mssql.sql`，再执行查询。

注意事项：
- 若你的 SQL Server 使用不同的默认架构（非 dbo），请相应调整表名前缀或修改脚本中的架构名。
- 若你需要把示例脚本导入到已存在的数据库，请先备份再操作，以免覆盖现有表名冲突的数据。

我已把最后一步（生成修改建议与下一步）标记为完成；如果一切 ok，我会把 `project/backend/db_schema_mssql.sql` 和 `project/backend/db_schema.sql` 以及说明文件合并进最终文档或提交 pull request（如你需要）。

---

## 各表字段中文说明（基于 `db_schema_mssql.sql`）

下面为脚本中每张表的字段、类型与中文含义汇总，方便在 SSMS 中查看和校验。

### 表：term（学期）
- id — INT IDENTITY(1,1) 主键：学期ID（自增）
- name — NVARCHAR(50) NOT NULL：学期名称（如 2024-秋）
- start_date — DATE：学期开始日期
- end_date — DATE：学期结束日期
- created_at — DATETIME2 DEFAULT SYSUTCDATETIME()：记录创建时间

### 表：student（学生）
- id — INT IDENTITY(1,1) 主键：学生ID（自增）
- student_no — NVARCHAR(32) NOT NULL UNIQUE：学号（业务唯一）
- full_name — NVARCHAR(128) NOT NULL：学生姓名
- gender — NCHAR(1) DEFAULT 'O'：性别（M/F/O）
- birth_date — DATE：出生日期
- email — NVARCHAR(255)：电子邮件
- phone — NVARCHAR(32)：联系电话
- major — NVARCHAR(128)：专业
- created_at — DATETIME2 DEFAULT SYSUTCDATETIME()：记录创建时间

### 表：teacher（教师）
- id — INT IDENTITY(1,1) 主键：教师ID（自增）
- teacher_no — NVARCHAR(32) NOT NULL UNIQUE：教工号（业务唯一）
- full_name — NVARCHAR(128) NOT NULL：教师姓名
- email — NVARCHAR(255)：电子邮件
- phone — NVARCHAR(32)：联系电话
- department — NVARCHAR(128)：所属院系/部门
- created_at — DATETIME2 DEFAULT SYSUTCDATETIME()：记录创建时间

### 表：course（课程）
- id — INT IDENTITY(1,1) 主键：课程ID（自增）
- course_code — NVARCHAR(64) NOT NULL UNIQUE：课程代码（如 CS101）
- title — NVARCHAR(256) NOT NULL：课程名称
- credits — DECIMAL(4,2) DEFAULT 0：学分
- description — NVARCHAR(MAX)：课程描述
- created_at — DATETIME2 DEFAULT SYSUTCDATETIME()：记录创建时间

### 表：class_offering（教学班/开课班次）
- id — INT IDENTITY(1,1) 主键：教学班ID（自增）
- offering_no — NVARCHAR(64) NOT NULL：教学班编号
- course_id — INT NOT NULL（外键 -> course.id）：所属课程
- term_id — INT NOT NULL（外键 -> term.id）：所属学期
- teacher_id — INT NULL（外键 -> teacher.id）：授课教师
- capacity — INT：班级容量
- location — NVARCHAR(128)：上课地点
- schedule — NVARCHAR(256)：上课时间安排（文本）
- created_at — DATETIME2 DEFAULT SYSUTCDATETIME()：记录创建时间

### 表：enrollment（选课记录）
- id — INT IDENTITY(1,1) 主键：选课记录ID（自增）
- student_id — INT NOT NULL（外键 -> student.id）：学生ID
- offering_id — INT NOT NULL（外键 -> class_offering.id）：教学班ID
- enroll_date — DATE：选课日期
- status — NVARCHAR(32) DEFAULT 'enrolled'：选课状态（enrolled/dropped/completed）
- grade_status — NVARCHAR(64)：成绩状态或备注
- created_at — DATETIME2 DEFAULT SYSUTCDATETIME()：记录创建时间

### 表：assessment（评估/成绩记录）
- id — INT IDENTITY(1,1) 主键：评估ID（自增）
- enrollment_id — INT NOT NULL（外键 -> enrollment.id）：所属选课记录
- assessment_type — NVARCHAR(64) NOT NULL：评估类型（期中/期末/平时等）
- score — DECIMAL(5,2)：得分
- graded_at — DATE：评分日期
- remarks — NVARCHAR(255)：备注
- created_at — DATETIME2 DEFAULT SYSUTCDATETIME()：记录创建时间

### 索引与约束（摘要）
- 主键：各表 `id` 字段为主键（IDENTITY）
- 唯一约束：`student.student_no`, `teacher.teacher_no`, `course.course_code`, `class_offering.offering_no` 等
- 选课去重：`enrollment` 的 UNIQUE(student_id, offering_id)
- 外键：class_offering -> course/term/teacher；enrollment -> student/class_offering；assessment -> enrollment
- 建议索引（脚本已创建）：`idx_course_code`, `idx_student_no`, `idx_teacher_no`, `idx_offering_course`, `idx_offering_term`

### 可选改进建议
- 若需更精细的排课和教室管理，请将 `schedule`/`location` 拆为 `class_schedule` 与 `room` 表。
- 若需院系/班级/先修课等管理，建议增加 `department`, `student_class`, `prerequisite` 等表并使用外键引用。
- 若需记录操作人或审计，请为关键表添加 `created_by`, `updated_by`, `updated_at` 等字段或触发器写入审计表。

---

（以上为已写入 `project/backend/db_schema_notes.md` 的表字段中文汇总）

````
