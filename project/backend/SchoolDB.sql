-- 教务系统示例数据（MySQL）
-- 该脚本创建最小表并插入示例数据，用于测试外键与查询

drop database if exists schooldb;

CREATE DATABASE IF NOT EXISTS SchoolDB DEFAULT CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci;
USE SchoolDB;

CREATE TABLE student (
  student_uid CHAR(12) NOT NULL PRIMARY KEY,
  name VARCHAR(128),
  major VARCHAR(128),
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

CREATE TABLE course (
  course_uid CHAR(10) NOT NULL PRIMARY KEY,
  course_name VARCHAR(128),
  credits INT,   
  category VARCHAR(64)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE counselor (
  counselor_uid CHAR(10) NOT NULL PRIMARY KEY,
  name VARCHAR(128),
  telephone VARCHAR(32),
  wechat_id VARCHAR(64),
  email VARCHAR(255)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE student_counselor (
  student_uid CHAR(12) NOT NULL,
  counselor_uid CHAR(10) NOT NULL,
  PRIMARY KEY (student_uid, counselor_uid),
  FOREIGN KEY (student_uid) REFERENCES student(student_uid),
  FOREIGN KEY (counselor_uid) REFERENCES counselor(counselor_uid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE user_account (
  username VARCHAR(64) PRIMARY KEY,
  password CHAR(32) NOT NULL,
  role ENUM('student','teacher','counselor','manager') NOT NULL,
  created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- ==========================
-- 插入 10 条真实风格的示例数据（每张表）
-- 使用 INSERT IGNORE 保持幂等，便于重复执行脚本
-- ==========================

-- 学生（10 条）
INSERT IGNORE INTO student (student_uid, name, major, telephone, wechat_id, email) VALUES
('202400000001','陈晓明','计算机科学与技术','13810000001','chenxm','chenxm@example.com'),
('202400000002','李晓雨','软件工程','13810000002','lixy','lixy@example.com'),
('202400000003','王子涵','数据科学与大数据技术','13810000003','wangzh','wangzh@example.com'),
('202400000004','赵俊豪','人工智能','13810000004','zhaojh','zhaojh@example.com'),
('202400000005','孙丽娜','信息安全','13810000005','sunln','sunln@example.com'),
('202400000006','周子瑜','物联网工程','13810000006','zhouzy','zhouzy@example.com'),
('202400000007','吴昊天','计算机工程','13810000007','wuht','wuht@example.com'),
('202400000008','郑玉梅','软件工程','13810000008','zhengym','zhengym@example.com'),
('202400000009','冯思远','多媒体技术','13810000009','fengsy','fengsy@example.com'),
('202400000010','黄瑞霖','信息管理与信息系统','13810000010','huangrl','huangrl@example.com');

-- 教师（10 条）
INSERT IGNORE INTO teacher (teacher_uid, name, telephone, wechat_id, email) VALUES
('10000001','王建国','13910000001','wangjg','wangjg@example.com'),
('10000002','赵丽华','13910000002','zhaolh','zhaolh@example.com'),
('10000003','陈思远','13910000003','chensy','chensy@example.com'),
('10000004','朱晓宇','13910000004','zhuxy','zhuxy@example.com'),
('10000005','何婷婷','13910000005','hett','hett@example.com'),
('10000006','高明哲','13910000006','gaomz','gaomz@example.com'),
('10000007','林海峰','13910000007','linhf','linhf@example.com'),
('10000008','郑丽君','13910000008','zhenglj','zhenglj@example.com'),
('10000009','吕彬','13910000009','lvbin','lvbin@example.com'),
('10000010','韩冰','13910000010','hanbing','hanbing@example.com');

-- 辅导员（10 条）
INSERT IGNORE INTO counselor (counselor_uid, name, telephone, wechat_id, email) VALUES
('C000000001','李勤','13710000001','liq','liq@example.com'),
('C000000002','王敏','13710000002','wangmin','wangmin@example.com'),
('C000000003','张强','13710000003','zhangq','zhangq@example.com'),
('C000000004','刘燕','13710000004','liuyan','liuyan@example.com'),
('C000000005','赵勇','13710000005','zhaoy','zhaoy@example.com'),
('C000000006','周倩','13710000006','zhouq','zhouq@example.com'),
('C000000007','吴敏','13710000007','wumin','wumin@example.com'),
('C000000008','郑刚','13710000008','zhengg','zhengg@example.com'),
('C000000009','冯菲','13710000009','fengf','fengf@example.com'),
('C000000010','黄慧','13710000010','huangh','huangh@example.com');

-- 课程（10 条）
INSERT IGNORE INTO course (course_uid, course_name, credits, category) VALUES
('COURSE0001','程序设计基础',3,'必修'),
('COURSE0002','离散数学',4,'必修'),
('COURSE0003','数据结构与算法',4,'必修'),
('COURSE0004','操作系统',3,'必修'),
('COURSE0005','数据库系统概论',3,'必修'),
('COURSE0006','计算机网络',3,'必修'),
('COURSE0007','软件工程',3,'选修'),
('COURSE0008','人工智能导论',2,'选修'),
('COURSE0009','信息安全概论',2,'选修'),
('COURSE0010','多媒体技术',2,'选修');

-- student_counselor：每位学生分配一位辅导员（10 条）
INSERT IGNORE INTO student_counselor (student_uid, counselor_uid) VALUES
('202400000001','C000000001'),
('202400000002','C000000002'),
('202400000003','C000000003'),
('202400000004','C000000004'),
('202400000005','C000000005'),
('202400000006','C000000006'),
('202400000007','C000000007'),
('202400000008','C000000008'),
('202400000009','C000000009'),
('202400000010','C000000010');

-- user_account：为所有学生/教师/辅导员创建账户，若已存在则忽略
-- 从 student 表插入用户（用户名 = student_uid），密码默认为 MD5('123456')
INSERT IGNORE INTO user_account (username, password, role)
SELECT student_uid, MD5('123456'), 'student' FROM student;

-- 从 teacher 表插入用户（用户名 = teacher_uid），密码默认为 MD5('123456')
INSERT IGNORE INTO user_account (username, password, role)
SELECT teacher_uid, MD5('123456'), 'teacher' FROM teacher;

-- 从 counselor 表插入用户（用户名 = counselor_uid），密码默认为 MD5('123456')
INSERT IGNORE INTO user_account (username, password, role)
SELECT counselor_uid, MD5('123456'), 'counselor' FROM counselor;

INSERT INTO user_account (username, password, role) VALUES
('myb', MD5('123456'), 'manager'),
('dcc', MD5('123456'), 'manager'),
('djb', MD5('123456'), 'manager');


