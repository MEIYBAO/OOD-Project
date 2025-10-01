// src/router/role-map.ts
export type Role = 'admin' | 'teacher' | 'counselor' | 'student'

/** 各角色登录后的首页（根路径） */
export const roleHome: Record<Role, string> = {
    admin: '/admin-home',
    teacher: '/teacher-home',
    counselor: '/counselor-home',
    student: '/student-home',
}

/** 各角色允许访问的“相对子路由”清单（基于 RoleLayout 的 children） */
export const roleChildren: Record<Role, string[]> = {
    admin: ['dish', 'category', 'supplier', 'ingredient', 'order', 'inventory', 'member', 'employee'],
    teacher: ['dish', 'order', 'ingredient'],
    counselor: ['member', 'order'],
    student: ['dish', 'order'],
}

/** 获取角色首页（拿不到角色时回退 /main） */
export function homeByRole(role?: string): string {
    return role && (roleHome as Record<string, string>)[role] || '/main'
}

/**
 * 角色是否允许访问指定路径
 * @param role      例如 'admin'
 * @param fullPath  例如 '/admin-home/order' 或 '/teacher-home/dish'
 */
export function allowByRole(role: string, fullPath: string): boolean {
    const base = homeByRole(role)            // 如 '/admin-home'
    // 提取子路径段：'/admin-home/order' -> 'order'
    const child = fullPath.replace(new RegExp(`^${base.replace('/', '\\/')}/?`), '')
        .split('?')[0].split('#')[0]
    const list = (roleChildren as Record<string, string[]>)[role] || []
    return child === '' || list.includes(child) // 根路由本身放行；其余看是否在白名单
}
