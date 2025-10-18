// src/api/auth.ts
import request from '@/utils/request'

/** 与后端 user_account.role 保持一致 */
export type Role = 'student' | 'teacher' | 'counselor' | 'manager'

export interface LoginResponse {
    token: string
    user: {
        id: string            // 与后端约定：等同于 username
        username: string
        role: Role
        createdAt?: string
    }
}

export interface MeResponse {
    id: string
    username: string
    role: Role
    createdAt?: string
}

/**
 * 登录（仅账号 + 密码）
 * 后端基于 user_account 表校验，不再接收 role。
 */
export function apiLogin(params: { username: string; password: string }) {
    // 后端路由：/auth/login
    return request.post<LoginResponse>('/auth/login', params)
}

/** 获取当前用户信息（需携带 Bearer Token） */
export function apiGetMe() {
    // 后端路由：auth/me
    return request.get<MeResponse>('/auth/me')
}
