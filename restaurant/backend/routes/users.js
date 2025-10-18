// routes/users.js
const express = require('express')
const jwt = require('jsonwebtoken')
const crypto = require('crypto')

module.exports = function usersRoutes(pool) {
    const router = express.Router()
    const JWT_SECRET = process.env.JWT_SECRET || 'change-me'

    // —— 工具：MD5(plain) → hex，与表中 CHAR(32) 存储对比 ——
    function md5hex(plain) {
        return crypto.createHash('md5').update(plain).digest('hex')
    }

    // —— 简单鉴权中间件（校验 Bearer Token） ——
    function requireAuth(req, res, next) {
        const auth = req.headers.authorization || ''
        const token = auth.startsWith('Bearer ') ? auth.slice(7) : ''
        if (!token) return res.status(401).json({ message: '未登录' })
        try {
            req.user = jwt.verify(token, JWT_SECRET) // { username, role }
            next()
        } catch {
            return res.status(401).json({ message: '登录已过期' })
        }
    }

    // ========== 列表 ==========
    /**
     * GET /api/users?page=&limit=&username=&role=
     * 返回：{ data: Array<{id, username, role}>, total }
     * 说明：id 等同 username（与前端保持一致）
     */
    router.get('/', requireAuth, async (req, res) => {
        const page = Math.max(parseInt(req.query.page) || 1, 1)
        const limit = Math.min(Math.max(parseInt(req.query.limit) || 10, 1), 200)
        const { username = '', role = '' } = req.query || {}
        const offset = (page - 1) * limit

        const where = []
        const params = []
        if (username) { where.push('username LIKE ?'); params.push(`%${username}%`) }
        if (role) { where.push('role = ?'); params.push(role) }
        const whereSql = where.length ? `WHERE ${where.join(' AND ')}` : ''

        const conn = await pool.getConnection()
        try {
            const [rows] = await conn.query(
                `SELECT username AS id, username, role
         FROM user_account
         ${whereSql}
         ORDER BY created_at DESC
         LIMIT ? OFFSET ?`,
                [...params, Number(limit), Number(offset)]
            )

            const [[{ total }]] = await conn.query(
                `SELECT COUNT(*) AS total FROM user_account ${whereSql}`, params
            )

            return res.json({ data: rows, total })
        } catch (e) {
            console.error(e)
            return res.status(500).json({ message: '服务器错误' })
        } finally {
            conn.release()
        }
    })

    // ========== 新建 ==========
    /**
     * POST /api/users
     * body: { username, password, role }
     * 注意：password 明文传入，这里做 MD5 存储
     */
    router.post('/', requireAuth, async (req, res) => {
        const { username, password, role } = req.body || {}
        if (!username || !password || !role) {
            return res.status(400).json({ message: '用户名/密码/角色 不能为空' })
        }
        if (!['admin', 'teacher', 'counselor', 'student'].includes(role)) {
            return res.status(400).json({ message: '角色不合法' })
        }

        const conn = await pool.getConnection()
        try {
            // 唯一性
            const [exists] = await conn.query(
                'SELECT 1 FROM user_account WHERE username=? LIMIT 1', [username]
            )
            if (exists.length) {
                return res.status(409).json({ message: '用户名已存在' })
            }

            // 插入
            await conn.query(
                'INSERT INTO user_account (username, `password`, role) VALUES (?,?,?)',
                [username, md5hex(password), role]
            )

            // 返回统一结构
            return res.status(201).json({ id: username, username, role })
        } catch (e) {
            console.error(e)
            return res.status(500).json({ message: '服务器错误' })
        } finally {
            conn.release()
        }
    })

    // ========== 更新 ==========
    /**
     * PUT /api/users/:id
     * params.id：即 username（前端编辑禁用用户名，不支持改用户名）
     * body: { role?, password? }
     */
    router.put('/:id', requireAuth, async (req, res) => {
        const id = req.params.id // 即 username
        const { role, password } = req.body || {}
        if (!role && !password) {
            return res.status(400).json({ message: '无可更新字段' })
        }
        if (role && !['admin', 'teacher', 'counselor', 'student'].includes(role)) {
            return res.status(400).json({ message: '角色不合法' })
        }

        const sets = []
        const params = []

        if (role) { sets.push('role=?'); params.push(role) }
        if (password) { sets.push('`password`=?'); params.push(md5hex(password)) }

        const conn = await pool.getConnection()
        try {
            const [ret] = await conn.query(
                `UPDATE user_account SET ${sets.join(', ')} WHERE username=?`,
                [...params, id]
            )
            if (!ret.affectedRows) {
                return res.status(404).json({ message: '用户不存在' })
            }

            // 查询并回显
            const [rows] = await conn.query(
                'SELECT username AS id, username, role FROM user_account WHERE username=? LIMIT 1', [id]
            )
            return res.json(rows[0])
        } catch (e) {
            console.error(e)
            return res.status(500).json({ message: '服务器错误' })
        } finally {
            conn.release()
        }
    })

    // ========== 删除 ==========
    /**
     * DELETE /api/users/:id
     * params.id：即 username
     */
    router.delete('/:id', requireAuth, async (req, res) => {
        const id = req.params.id

        const conn = await pool.getConnection()
        try {
            const [ret] = await conn.query(
                'DELETE FROM user_account WHERE username=?', [id]
            )
            if (!ret.affectedRows) {
                return res.status(404).json({ message: '用户不存在或已删除' })
            }
            return res.json({ success: true })
        } catch (e) {
            console.error(e)
            return res.status(500).json({ message: '服务器错误' })
        } finally {
            conn.release()
        }
    })

    return router
}
