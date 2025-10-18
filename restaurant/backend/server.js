// server.js
const express = require('express')
const mysql = require('mysql2/promise')
const cors = require('cors')
const dotenv = require('dotenv')

// 加载环境变量
dotenv.config()

const app = express()
const port = process.env.PORT || 3000

// ===== 中间件 =====
app.use(cors({
  origin: process.env.CORS_ORIGIN?.split(',') || ['http://localhost:5173'],
  credentials: false,
}))
app.use(express.json({ limit: '2mb' }))
app.use(express.urlencoded({ extended: true }))

// ===== 数据库连接池 =====
const pool = mysql.createPool({
  host: process.env.DB_HOST || 'localhost',
  user: process.env.DB_USER || 'root',
  password: process.env.DB_PASSWORD || '',
  database: process.env.DB_NAME || 'restaurant_db',
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
// ⚠️ 注意：这些路由文件需按 CommonJS 导出函数：module.exports = (pool) => router
app.use('/api/auth', require('./routes/auth')(pool))           // << 新增：登录/获取个人信息
app.use('/api/category', require('./routes/category')(pool))
app.use('/api/dish', require('./routes/dish')(pool))
app.use('/api/supplier', require('./routes/supplier')(pool))
app.use('/api/ingredient', require('./routes/ingredient')(pool))
app.use('/api/employee', require('./routes/employee')(pool))
app.use('/api/member', require('./routes/member')(pool))
app.use('/api/order', require('./routes/order')(pool))
app.use('/api/inventory', require('./routes/inventory')(pool))
app.use('/api/statistics', require('./routes/statistics')(pool))

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

// ===== 进程级错误处理 =====
process.on('uncaughtException', (err) => {
  console.error('未捕获的异常:', err)
})
process.on('unhandledRejection', (reason) => {
  console.error('未处理的 Promise 拒绝:', reason)
})

module.exports = app
