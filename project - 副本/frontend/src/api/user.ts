import request from '@/utils/request'

export function apiLogin(params: { username: string; password: string }) {
    // 后端已提供 /api/auth/login，返回 { token }
    return request.post('/auth/login', params)
}

export function apiGetMe() {
    // 返回 { id, username, role }
    return request.get('/auth/me')
}
