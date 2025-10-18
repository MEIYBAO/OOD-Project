// server.js
const express = require('express')
const mysql = require('mysql2/promise')
const cors = require('cors')
const dotenv = require('dotenv')

dotenv.config()

const app = express()
const port = process.env.PORT || 3000

// ===== 中间件 =====
app.use(cors({
  origin: process.env.CORS_ORIGIN?.split(',') || ['http://localhost:5173'],
  credentials: false, // 如需携带 cookie 再改成 true；Bearer 不需要
}))
app.use(express.json({ limit: '2mb' }))
app.use(express.urlencoded({ extended: true }))

// ===== 数据库连接池 =====
const pool = mysql.createPool({
  host: process.env.DB_HOST || 'localhost',
  user: process.env.DB_USER || 'root',
  password: process.env.DB_PASSWORD || '',
  database: process.env.DB_NAME || 'schooldb',
  waitForConnections: true,
  connectionLimit: 10,
  queueLimit: 0,
})

// ===== 健康检查 & DB 测试 =====
app.get('/api/health', (_req, res) => res.json({ ok: true, ts: Date.now() }))
app.get('/api/test-db', async (_req, res) => {
  try {
    const [rows] = await pool.query('SELECT 1 + 1 AS result')
    res.json({ code: 200, message: '数据库连接成功', data: rows[0] })
  } catch (error) {
    console.error('数据库连接失败:', error)
    res.status(500).json({ code: 500, message: '数据库连接失败', error: error.message })
  }
})

// ===== 业务路由 =====
// 这些路由文件需导出函数：module.exports = (pool) => router
app.use('/api/auth', require('./routes/auth')(pool))   // 登录/个人信息
app.use('/api/users', require('./routes/users')(pool))  // ✅ 用户管理（新增这行）

// ===== 静态资源 =====
app.use(express.static('public'))

// ===== 404 兜底 =====
app.use((req, res) => {
  res.status(404).json({ code: 404, message: `Not Found: ${req.method} ${req.originalUrl}` })
})

// ===== 启动服务 =====
app.listen(port, () => {
  console.log(`服务器运行在 http://localhost:${port}`)
})

process.on('uncaughtException', (err) => console.error('未捕获的异常:', err))
process.on('unhandledRejection', (reason) => console.error('未处理的 Promise 拒绝:', reason))

module.exports = app
