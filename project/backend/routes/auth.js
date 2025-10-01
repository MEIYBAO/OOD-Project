// routes/auth.js
const express = require('express')
const jwt = require('jsonwebtoken')

module.exports = function authRoutes(pool) {
    const router = express.Router()
    const JWT_SECRET = process.env.JWT_SECRET || 'change-me'

    // POST /api/auth/login  {username, password}
    router.post('/login', async (req, res) => {
        const { username, password } = req.body || {}
        if (!username || !password) {
            return res.status(400).json({ message: '缺少用户名或密码' })
        }
        const conn = await pool.getConnection()
        try {
            // 你的表结构：user(user_id, username, password, role, status, ...)
            const [rows] = await conn.query(
                'SELECT user_id, username, `password`, `role`, `status` FROM `user` WHERE username=? LIMIT 1',
                [username]
            )
            const user = rows[0]
            if (!user) return res.status(401).json({ message: '用户名或密码错误' })
            if (user.status !== 1) return res.status(403).json({ message: '账号已禁用' })

            // 你当前表里 password 是 MD5(明文)，用 MySQL 计算 MD5 比较
            const [m] = await conn.query('SELECT MD5(?) AS m', [password])
            if (m[0].m !== user.password) return res.status(401).json({ message: '用户名或密码错误' })

            const token = jwt.sign({ uid: user.user_id }, JWT_SECRET, { expiresIn: '7d' })
            await conn.query('UPDATE `user` SET last_login=NOW() WHERE user_id=?', [user.user_id])

            res.json({ token })
        } catch (e) {
            console.error(e)
            res.status(500).json({ message: '服务器错误' })
        } finally {
            conn.release()
        }
    })

    // GET /api/auth/me   (Authorization: Bearer <token>)
    router.get('/me', async (req, res) => {
        try {
            const auth = req.headers.authorization || ''
            const token = auth.startsWith('Bearer ') ? auth.slice(7) : ''
            if (!token) return res.status(401).json({ message: '未登录' })

            const { uid } = jwt.verify(token, process.env.JWT_SECRET || 'change-me')
            const [rows] = await pool.query(
                'SELECT user_id, username, `role`, employee_id FROM `user` WHERE user_id=? LIMIT 1',
                [uid]
            )
            const me = rows[0]
            if (!me) return res.status(401).json({ message: '用户不存在' })
            res.json({ id: me.user_id, username: me.username, role: me.role, employeeId: me.employee_id })
        } catch (e) {
            res.status(401).json({ message: '登录已过期' })
        }
    })

    return router
}
