-- SQL Server (T-SQL) 版本的 3NF 教务系统建表脚本
-- 适用于在 SQL Server Management Studio (SSMS) 中执行

SET NOCOUNT ON;

-- 使用示例数据库（请先创建并使用）
CREATE DATABASE SchoolDB;
GO
USE SchoolDB;
GO

IF OBJECT_ID('dbo.term', 'U') IS NOT NULL DROP TABLE dbo.term;
IF OBJECT_ID('dbo.assessment', 'U') IS NOT NULL DROP TABLE dbo.assessment;
IF OBJECT_ID('dbo.enrollment', 'U') IS NOT NULL DROP TABLE dbo.enrollment;
IF OBJECT_ID('dbo.class_offering', 'U') IS NOT NULL DROP TABLE dbo.class_offering;
IF OBJECT_ID('dbo.course', 'U') IS NOT NULL DROP TABLE dbo.course;
IF OBJECT_ID('dbo.teacher', 'U') IS NOT NULL DROP TABLE dbo.teacher;
IF OBJECT_ID('dbo.student', 'U') IS NOT NULL DROP TABLE dbo.student;

CREATE TABLE dbo.term (
  id INT IDENTITY(1,1) PRIMARY KEY,
  name NVARCHAR(50) NOT NULL,
  start_date DATE NULL,
  end_date DATE NULL,
  created_at DATETIME2 DEFAULT SYSUTCDATETIME()
);

CREATE TABLE dbo.student (
  id INT IDENTITY(1,1) PRIMARY KEY,
  student_no NVARCHAR(32) NOT NULL UNIQUE,
  full_name NVARCHAR(128) NOT NULL,
  gender NCHAR(1) DEFAULT 'O',
  birth_date DATE NULL,
  email NVARCHAR(255) NULL,
  phone NVARCHAR(32) NULL,
  major NVARCHAR(128) NULL,
  created_at DATETIME2 DEFAULT SYSUTCDATETIME()
);

CREATE TABLE dbo.teacher (
  id INT IDENTITY(1,1) PRIMARY KEY,
  teacher_no NVARCHAR(32) NOT NULL UNIQUE,
  full_name NVARCHAR(128) NOT NULL,
  email NVARCHAR(255) NULL,
  phone NVARCHAR(32) NULL,
  department NVARCHAR(128) NULL,
  created_at DATETIME2 DEFAULT SYSUTCDATETIME()
);

CREATE TABLE dbo.course (
  id INT IDENTITY(1,1) PRIMARY KEY,
  course_code NVARCHAR(64) NOT NULL UNIQUE,
  title NVARCHAR(256) NOT NULL,
  credits DECIMAL(4,2) DEFAULT 0,
  description NVARCHAR(MAX) NULL,
  created_at DATETIME2 DEFAULT SYSUTCDATETIME()
);

CREATE TABLE dbo.class_offering (
  id INT IDENTITY(1,1) PRIMARY KEY,
  offering_no NVARCHAR(64) NOT NULL,
  course_id INT NOT NULL,
  term_id INT NOT NULL,
  teacher_id INT NULL,
  capacity INT NULL,
  location NVARCHAR(128) NULL,
  schedule NVARCHAR(256) NULL,
  created_at DATETIME2 DEFAULT SYSUTCDATETIME(),
  CONSTRAINT ux_offering_course_term_no UNIQUE (offering_no)
);

CREATE TABLE dbo.enrollment (
  id INT IDENTITY(1,1) PRIMARY KEY,
  student_id INT NOT NULL,
  offering_id INT NOT NULL,
  enroll_date DATE NULL,
  status NVARCHAR(32) DEFAULT 'enrolled',
  grade_status NVARCHAR(64) NULL,
  created_at DATETIME2 DEFAULT SYSUTCDATETIME(),
  CONSTRAINT ux_student_offering UNIQUE (student_id, offering_id)
);

CREATE TABLE dbo.assessment (
  id INT IDENTITY(1,1) PRIMARY KEY,
  enrollment_id INT NOT NULL,
  assessment_type NVARCHAR(64) NOT NULL,
  score DECIMAL(5,2) NULL,
  graded_at DATE NULL,
  remarks NVARCHAR(255) NULL,
  created_at DATETIME2 DEFAULT SYSUTCDATETIME()
);

-- 添加外键约束
ALTER TABLE dbo.class_offering WITH CHECK ADD CONSTRAINT fk_offering_course FOREIGN KEY(course_id) REFERENCES dbo.course(id);
ALTER TABLE dbo.class_offering WITH CHECK ADD CONSTRAINT fk_offering_term FOREIGN KEY(term_id) REFERENCES dbo.term(id);
ALTER TABLE dbo.class_offering WITH CHECK ADD CONSTRAINT fk_offering_teacher FOREIGN KEY(teacher_id) REFERENCES dbo.teacher(id);

ALTER TABLE dbo.enrollment WITH CHECK ADD CONSTRAINT fk_enroll_student FOREIGN KEY(student_id) REFERENCES dbo.student(id) ON DELETE CASCADE;
ALTER TABLE dbo.enrollment WITH CHECK ADD CONSTRAINT fk_enroll_offering FOREIGN KEY(offering_id) REFERENCES dbo.class_offering(id) ON DELETE CASCADE;

ALTER TABLE dbo.assessment WITH CHECK ADD CONSTRAINT fk_assess_enroll FOREIGN KEY(enrollment_id) REFERENCES dbo.enrollment(id) ON DELETE CASCADE;

-- 建议索引
CREATE INDEX idx_course_code ON dbo.course(course_code);
CREATE INDEX idx_student_no ON dbo.student(student_no);
CREATE INDEX idx_teacher_no ON dbo.teacher(teacher_no);
CREATE INDEX idx_offering_course ON dbo.class_offering(course_id);
CREATE INDEX idx_offering_term ON dbo.class_offering(term_id);

PRINT 'T-SQL schema creation script finished.';
