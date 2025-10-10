// routes/auth.js
const express = require('express')
const jwt = require('jsonwebtoken')
const crypto = require('crypto')

module.exports = function authRoutes(pool) {
    const router = express.Router()
    const JWT_SECRET = process.env.JWT_SECRET || 'change-me'

    // 计算明文的 MD5（与库中 CHAR(32) HEX 比较）
    function md5hex(plain) {
        return crypto.createHash('md5').update(plain).digest('hex')
    }

    /**
     * POST /api/auth/login
     * body: { username, password }
     * 基于 user_account(username PK, password MD5, role ENUM) 认证
     */
    router.post('/login', async (req, res) => {
        const { username, password } = req.body || {}
        if (!username || !password) {
            return res.status(400).json({ message: '缺少用户名或密码' })
        }

        const conn = await pool.getConnection()
        try {
            // user_account 的主键就是 username，天然唯一
            const [rows] = await conn.query(
                'SELECT username, `password`, `role`, `created_at` FROM `user_account` WHERE username = ? LIMIT 1',
                [username]
            )
            if (rows.length === 0) {
                return res.status(401).json({ message: '用户名或密码错误' })
            }

            const user = rows[0]
            // 比较 MD5
            if (user.password !== md5hex(password)) {
                return res.status(401).json({ message: '用户名或密码错误' })
            }

            // 令牌只需携带 username/role（没有 user_id）
            const token = jwt.sign(
                { username: user.username, role: user.role },
                JWT_SECRET,
                { expiresIn: '7d' }
            )

            // 返回给前端的用户对象。为兼容旧前端，这里将 id 等同于 username。
            return res.json({
                token,
                user: {
                    id: user.username,        // 兼容旧字段：用 username 作为 id
                    username: user.username,
                    role: user.role,
                    createdAt: user.created_at
                }
            })
        } catch (e) {
            console.error(e)
            return res.status(500).json({ message: '服务器错误' })
        } finally {
            conn.release()
        }
    })

    /**
     * GET /api/auth/me
     * Header: Authorization: Bearer <token>
     * 验证并返回当前用户基本信息
     */
    router.get('/me', async (req, res) => {
        try {
            const auth = req.headers.authorization || ''
            const token = auth.startsWith('Bearer ') ? auth.slice(7) : ''
            if (!token) return res.status(401).json({ message: '未登录' })

            const { username } = jwt.verify(token, JWT_SECRET)

            // 再查一次数据库，保证账号仍然存在且拿到最新角色
            const [rows] = await pool.query(
                'SELECT username, `role`, `created_at` FROM `user_account` WHERE username = ? LIMIT 1',
                [username]
            )
            if (rows.length === 0) {
                return res.status(401).json({ message: '用户不存在或已被删除' })
            }

            const u = rows[0]
            return res.json({
                id: u.username,           // 与 /login 返回保持一致
                username: u.username,
                role: u.role,
                createdAt: u.created_at
            })
        } catch (e) {
            return res.status(401).json({ message: '登录已过期' })
        }
    })

    return router
}
