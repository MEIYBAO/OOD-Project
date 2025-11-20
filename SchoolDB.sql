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

CREATE TABLE user_account (
  username VARCHAR(64) PRIMARY KEY,
  password CHAR(32) NOT NULL,
  role ENUM('student','teacher','counselor','manager') NOT NULL,
  created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE teacher_course (
  teacher_uid CHAR(10) NOT NULL,
  course_uid CHAR(10) NOT NULL,
  semester VARCHAR(16) NOT NULL, 
  PRIMARY KEY (teacher_uid, course_uid, semester),
  FOREIGN KEY (teacher_uid) REFERENCES teacher(teacher_uid),
  FOREIGN KEY (course_uid) REFERENCES course(course_uid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE courseSelection (
  student_uid CHAR(12) NOT NULL,
  course_uid CHAR(10) NOT NULL,
  selection_date DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  grade DECIMAL(5,2), 
  FirstRepair BOOLEAN DEFAULT 0, 
  PRIMARY KEY (student_uid, course_uid, FirstRepair),
  FOREIGN KEY (student_uid) REFERENCES student(student_uid),
  FOREIGN KEY (course_uid) REFERENCES course(course_uid)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE teach_class (
  class_id CHAR(10) NOT NULL PRIMARY KEY,
  course_uid CHAR(10) NOT NULL,
  teacher_uid CHAR(10) NOT NULL,
  semester VARCHAR(16) NOT NULL,
  schedule VARCHAR(64), 
  location VARCHAR(64), 
  FOREIGN KEY (course_uid) REFERENCES course(course_uid),
  FOREIGN KEY (teacher_uid) REFERENCES teacher(teacher_uid)
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


-- ==========================
-- 创建触发器来处理删除操作，避免外键约束错误
-- ==========================

-- 删除已存在的触发器（如果存在）
DROP TRIGGER IF EXISTS before_delete_student;
DROP TRIGGER IF EXISTS before_delete_teacher;
DROP TRIGGER IF EXISTS before_delete_counselor;

-- 创建删除student前的触发器
DELIMITER $$
CREATE TRIGGER before_delete_student
BEFORE DELETE ON student
FOR EACH ROW
BEGIN
    -- 删除courseSelection表中相关的选课记录
    DELETE FROM courseSelection WHERE student_uid = OLD.student_uid;
    -- 删除user_account表中对应的用户账户
    DELETE FROM user_account WHERE username = OLD.student_uid;
END$$
DELIMITER ;

-- ==========================
-- 插入示例数据：teacher_course, teach_class, courseSelection
-- ==========================

-- teacher_course 示例（老师授课）
INSERT IGNORE INTO teacher_course (teacher_uid, course_uid, semester) VALUES
('10000001','COURSE0001','2024-1'),
('10000002','COURSE0002','2024-1'),
('10000003','COURSE0003','2024-1'),
('10000004','COURSE0004','2024-1'),
('10000005','COURSE0005','2024-1'),
('10000006','COURSE0006','2024-1'),
('10000007','COURSE0007','2024-1'),
('10000008','COURSE0008','2024-1'),
('10000009','COURSE0009','2024-1'),
('10000010','COURSE0010','2024-1');

-- teach_class 示例（教学班）
INSERT IGNORE INTO teach_class (class_id, course_uid, teacher_uid, semester, schedule, location) VALUES
('CLASS0001','COURSE0001','10000001','2024-1','周一 08:00-10:00','教学楼 A101'),
('CLASS0002','COURSE0002','10000002','2024-1','周二 10:00-12:00','教学楼 A102'),
('CLASS0003','COURSE0003','10000003','2024-1','周三 14:00-16:00','教学楼 B201'),
('CLASS0004','COURSE0004','10000004','2024-1','周四 08:00-10:00','教学楼 B202'),
('CLASS0005','COURSE0005','10000005','2024-1','周五 10:00-12:00','实验楼 C101'),
('CLASS0006','COURSE0006','10000006','2024-1','周一 14:00-16:00','实验楼 C102'),
('CLASS0007','COURSE0007','10000007','2024-1','周二 08:00-10:00','教学楼 A201'),
('CLASS0008','COURSE0008','10000008','2024-1','周三 10:00-12:00','教学楼 A202'),
('CLASS0009','COURSE0009','10000009','2024-1','周四 14:00-16:00','教学楼 B301'),
('CLASS0010','COURSE0010','10000010','2024-1','周五 14:00-16:00','多媒体室 D101');

-- courseSelection 示例（学生选课），包含部分成绩和重修标记
INSERT IGNORE INTO courseSelection (student_uid, course_uid, selection_date, grade, FirstRepair) VALUES
('202400000001','COURSE0001', '2024-03-01 09:00:00', 85.50, 0),
('202400000001','COURSE0003', '2024-03-02 09:30:00', 78.00, 0),
('202400000002','COURSE0002', '2024-03-01 10:00:00', 88.00, 0),
('202400000002','COURSE0005', '2024-03-03 11:00:00', 92.00, 0),
('202400000003','COURSE0003', '2024-03-02 14:00:00', 69.50, 1),
('202400000003','COURSE0004', '2024-03-04 09:00:00', NULL, 0),
('202400000004','COURSE0008', '2024-03-05 13:00:00', 95.00, 0),
('202400000005','COURSE0009', '2024-03-06 15:00:00', 73.00, 0),
('202400000006','COURSE0006', '2024-03-01 08:30:00', 81.75, 0),
('202400000007','COURSE0007', '2024-03-02 16:00:00', 87.00, 0),
('202400000008','COURSE0002', '2024-03-03 09:30:00', 60.00, 1),
('202400000009','COURSE0010', '2024-03-04 10:30:00', 78.25, 0),
('202400000010','COURSE0005', '2024-03-05 11:30:00', NULL, 0),
('202400000001','COURSE0001', '2024-09-01 09:00:00', 91.00, 1),
('202400000002','COURSE0001', '2024-09-02 09:00:00', NULL, 0);

-- 说明：上面样例使用 INSERT IGNORE，可重复执行脚本而不产生冲突。


-- 创建删除teacher前的触发器
DELIMITER $$
CREATE TRIGGER before_delete_teacher
BEFORE DELETE ON teacher
FOR EACH ROW
BEGIN
    -- 删除teacher_course表中相关的授课记录
    DELETE FROM teacher_course WHERE teacher_uid = OLD.teacher_uid;
    -- 删除teach_class表中相关的教学班记录
    DELETE FROM teach_class WHERE teacher_uid = OLD.teacher_uid;
    -- 删除user_account表中对应的用户账户
    DELETE FROM user_account WHERE username = OLD.teacher_uid;
END$$
DELIMITER ;

-- 创建删除counselor前的触发器
DELIMITER $$
CREATE TRIGGER before_delete_counselor
BEFORE DELETE ON counselor
FOR EACH ROW
BEGIN
    -- 删除user_account表中对应的用户账户
    DELETE FROM user_account WHERE username = OLD.counselor_uid;
END$$
DELIMITER ;
