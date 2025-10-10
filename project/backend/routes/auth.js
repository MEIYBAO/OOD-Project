// routes/auth.js
const express = require('express')
const jwt = require('jsonwebtoken')

module.exports = function authRoutes(pool) {
    const router = express.Router()
    const JWT_SECRET = process.env.JWT_SECRET || 'change-me'

    // 工具：用 MySQL 计算 MD5(明文) 并与库内32位HEX比较
    async function md5Equals(conn, plain, hex32) {
        const [m] = await conn.query('SELECT MD5(?) AS m', [plain])
        return m[0]?.m === hex32
    }

    // POST /api/auth/login  {username, password, role?}
    // - 用户名允许重复；若携带 role，按 username+role 精确定位
    // - 若未携带 role，则取同名候选逐条比对 MD5
    router.post('/login', async (req, res) => {
        const { username, password, role } = req.body || {}
        if (!username || !password) {
            return res.status(400).json({ message: '缺少用户名或密码' })
        }

        const conn = await pool.getConnection()
        try {
            let user = null

            if (role) {
                // 精确：username + role
                const [rows] = await conn.query(
                    'SELECT user_id, username, `password`, `role`, `status` FROM `Users` WHERE username=? AND role=? LIMIT 1',
                    [username, role]
                )
                if (rows.length === 0) return res.status(401).json({ message: '用户名或密码错误' })
                if (rows[0].status !== 1) return res.status(403).json({ message: '账号已禁用' })
                const ok = await md5Equals(conn, password, rows[0].password)
                if (!ok) return res.status(401).json({ message: '用户名或密码错误' })
                user = rows[0]
            } else {
                // 模糊：仅 username，允许重名 → 逐条比对 MD5
                const [cands] = await conn.query(
                    'SELECT user_id, username, `password`, `role`, `status` FROM `Users` WHERE username=?',
                    [username]
                )
                if (cands.length === 0) return res.status(401).json({ message: '用户名或密码错误' })

                // 逐条尝试匹配 MD5
                const matches = []
                for (const u of cands) {
                    if (u.status !== 1) continue
                    const ok = await md5Equals(conn, password, u.password)
                    if (ok) matches.push(u)
                }

                if (matches.length === 0) {
                    return res.status(401).json({ message: '用户名或密码错误或账号被禁用' })
                }
                if (matches.length > 1) {
                    // 同名账户在多个身份/账号上使用了同一口令，需要用户补充身份或序号登录
                    return res.status(409).json({ message: '存在多个同名匹配，请选择身份或使用序号登录' })
                }
                user = matches[0]
            }

            const token = jwt.sign({ uid: user.user_id }, JWT_SECRET, { expiresIn: '7d' })
            await conn.query('UPDATE `Users` SET last_login=NOW() WHERE user_id=?', [user.user_id])

            // 可按需返回基本资料，便于前端展示
            res.json({
                token,
                user: {
                    id: user.user_id, // 6位序号主键
                    username: user.username,
                    role: user.role
                }
            })
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

            const { uid } = jwt.verify(token, JWT_SECRET)

            const [rows] = await pool.query(
                'SELECT user_id, username, `role`, `status`, last_login FROM `Users` WHERE user_id=? LIMIT 1',
                [uid]
            )
            const me = rows[0]
            if (!me) return res.status(401).json({ message: '用户不存在' })
            if (me.status !== 1) return res.status(403).json({ message: '账号已禁用' })

            res.json({
                id: me.user_id,
                username: me.username,
                role: me.role,
                lastLogin: me.last_login
            })
        } catch (e) {
            res.status(401).json({ message: '登录已过期' })
        }
    })

    return router
}
