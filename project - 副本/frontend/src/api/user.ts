// src/api/auth.ts
import request from '@/utils/request'

export type Role = 'admin' | 'teacher' | 'counselor' | 'student'

/**
 * 登录：
 * - 若系统存在重名用户名，建议携带 role 一起提交；
 * - 若不携带 role，后端可能返回 409，届时再补充 role 重试。
 */
export function apiLogin(params: { username: string; password: string; role?: Role }) {
    // 后端 /api/auth/login 支持 {username, password, role?}
    return request.post('/auth/login', params)
}

/** 获取当前用户 */
export function apiGetMe() {
    // 后端返回 { id, username, role, ... }，多出的字段前端可忽略
    return request.get('/auth/me')
}
