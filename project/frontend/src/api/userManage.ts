// src/api/user.ts
import request from '@/utils/request'

// 统一在此处定义角色类型（不再从 ./auth 引入）
export type Role = 'manager' | 'teacher' | 'counselor' | 'student'

export interface UserAccount {
    id?: string        // 后端用 username 作为 id 时为 string
    username: string
    role: Role
}

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
    password?: string   // 不传则后端自行默认
}

export interface UpdateUserDto {
    username?: string   // 如果后端不支持改用户名，可不传
    role?: Role
    password?: string   // 传了则重置
}

const prefix = '/users'  // 配合后端 REST: /api/users

export function getUsers(params: UserQuery) {
    return request.get<UserListResp>(`${prefix}`, { params })
}

export function createUser(data: CreateUserDto) {
    return request.post(`${prefix}`, data)
}

export function updateUser(id: string, data: UpdateUserDto) {
    return request.put(`${prefix}/${id}`, data)
}

export function deleteUser(id: string) {
    return request.delete(`${prefix}/${id}`)
}
