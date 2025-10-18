// src/api/user.ts
import request from '@/utils/request'
import type { Role } from './auth'

export interface UserAccount {
    id?: number | string        // 若后端没有 id，可忽略；也可用 username 当 id
    username: string
    role: Role
    created_at?: string
}

// 列表查询参数（可选）
export interface UserQuery {
    page?: number
    limit?: number
    username?: string
    role?: '' | Role
}

export interface UserListResp {
    data: UserAccount[]
    total: number
}

export interface CreateUserDto {
    username: string
    role: Role
    /** 不传则后端默认 123456 或你自定义 */
    password?: string
}

export interface UpdateUserDto {
    username?: string
    role?: Role
    /** 传了则重置密码 */
    password?: string
}

const prefix = '/user' // 如果你的服务是 /api/user，这里改成 '/api/user'

/** 获取用户列表 */
export function getUsers(params: UserQuery) {
    return request.get<UserListResp>(`${prefix}/list`, { params })
}

/** 新增用户 */
export function createUser(data: CreateUserDto) {
    return request.post(`${prefix}/create`, data)
}

/** 更新用户 */
export function updateUser(id: number | string, data: UpdateUserDto) {
    return request.put(`${prefix}/update/${id}`, data)
}

/** 删除用户 */
export function deleteUser(id: number | string) {
    return request.delete(`${prefix}/delete/${id}`)
}
