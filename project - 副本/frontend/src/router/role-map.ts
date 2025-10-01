// 角色映射与工具
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
    return (role && (roleHome as Record<string, string>)[role]) || '/main'
}

/**
 * 角色是否允许访问指定路径
 * @param role     例如 'admin'
 * @param fullPath 例如 '/admin-home/order' | '/teacher-home/dish' | '/admin-home/dish/123?x=1#t'
 */
export function allowByRole(role: string, fullPath: string): boolean {
    const base = homeByRole(role)                  // 如 '/admin-home'
    const list = (roleChildren as Record<string, string[]>)[role] || []

    // 去掉查询与哈希
    const clean = fullPath.split(/[?#]/)[0]

    // 不以该角色 base 开头：不放行（避免把 '/dish' 当成 'dish'）
    if (clean !== base && !clean.startsWith(base + '/')) return false

    // 提取 base 之后的第一段子路径；根本身记为空串
    const rest = clean.slice(base.length)          // '', '/dish', '/dish/123'
    const child = rest.replace(/^\/+/, '').split('/')[0] // '' | 'dish'

    // 根路由本身放行；其余看白名单
    return child === '' || list.includes(child)
}
