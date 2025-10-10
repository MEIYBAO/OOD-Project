-- 教务系统示例数据（MySQL）
-- 该脚本创建最小表并插入示例数据，用于测试外键与查询

CREATE DATABASE IF NOT EXISTS SchoolDB DEFAULT CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci;
USE SchoolDB;

DROP TABLE IF EXISTS user_account;
DROP TABLE IF EXISTS teacher;
DROP TABLE IF EXISTS counselor;
DROP TABLE IF EXISTS student;

CREATE TABLE student (
  student_uid CHAR(12) NOT NULL PRIMARY KEY,
  name VARCHAR(128),
  major VARCHAR(128),
  telephone VARCHAR(32),
  wechat_id VARCHAR(64),
  email VARCHAR(255)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE counselor (
  counselor_uid CHAR(10) NOT NULL PRIMARY KEY,
  name VARCHAR(128),
  telephone VARCHAR(32),
  wechat_id VARCHAR(64),
  email VARCHAR(255)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE teacher (
  teacher_uid CHAR(10) NOT NULL PRIMARY KEY,
  name VARCHAR(128),
  telephone VARCHAR(32),
  wechat_id VARCHAR(64),
  email VARCHAR(255)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;



-- 插入示例数据
INSERT INTO student (student_uid, name, major, telephone, wechat_id, email) VALUES
('202400000001', '张三', '计算机科学', '13811110001', 'weixin_zs', 'zhangsan@example.com'),
('202400000002', '李四', '软件工程', '13811110002', 'weixin_ls', 'lisi@example.com');

INSERT INTO teacher (teacher_uid, name, telephone, wechat_id, email) VALUES
('10000001','王老师','13900000001','wechat_w','wang@example.com'),
('10000002','赵老师','13900000002','wechat_z','zhao@example.com');

INSERT INTO counselor (counselor_uid, name, telephone, wechat_id, email) VALUES
('C000000001','辅导员甲','13800000001','counselor1','counselor1@example.com');

-- 结束

-- 额外：创建 user_account 表，并根据 student.teacher 的 UID 自动生成用户名
DROP TABLE IF EXISTS user_account;
CREATE TABLE user_account (
  username VARCHAR(64) PRIMARY KEY,
  password CHAR(32) NOT NULL,
  role ENUM('student','teacher','counselor','manager') NOT NULL,
  created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 从 student 表插入用户（用户名 = student_uid），密码默认为 MD5('123456')
INSERT INTO user_account (username, password, role)
SELECT student_uid, MD5('123456'), 'student' FROM student;

-- 从 teacher 表插入用户（用户名 = teacher_uid），密码默认为 MD5('123456')
INSERT INTO user_account (username, password, role)
SELECT teacher_uid, MD5('123456'), 'teacher' FROM teacher;

-- 从 counselor 表插入用户（用户名 = counselor_uid），密码默认为 MD5('123456')
INSERT INTO user_account (username, password, role)
SELECT counselor_uid, MD5('123456'), 'counselor' FROM counselor;

-- 插入 manager 示例用户
INSERT INTO user_account (username, password, role) VALUES
('myb', MD5('123456'), 'manager'),
('dcc', MD5('123456'), 'manager'),
('djb', MD5('123456'), 'manager');