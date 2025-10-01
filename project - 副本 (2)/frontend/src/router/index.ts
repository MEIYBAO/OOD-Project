import { createRouter, createWebHistory } from 'vue-router'
import LoginPage from '@/views/Login/loginPage.vue'
import RoleLayout from '@/views/Layouts/RoleLayout.vue' // ← 通用布局（自动识别根前缀）

// 业务页（按需复用到不同角色的 children）
import Dish from '@/views/Dish/List.vue'
import Category from '@/views/dish/Category.vue'
import Supplier from '@/views/Supplier/index.vue'
import Ingredient from '@/views/Ingredient/index.vue'
import Order from '@/views/Order/List.vue'
import Inventory from '@/views/Inventory/Record.vue'
import Member from '@/views/Member/index.vue'
import Employee from '@/views/Employee/index.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    { path: '/', redirect: '/login' },

    // 登录页（不带侧栏）
    { path: '/login', name: 'Login', component: LoginPage, meta: { public: true } },

    // ===== 通用入口（可作为默认角色或公共区）=====
    {
      path: '/main',
      component: RoleLayout,
      children: [
        { path: '', redirect: 'dish' }, // /main -> /main/dish
        { path: 'dish', name: 'Dish', component: Dish },
        { path: 'category', name: 'Category', component: Category },
        { path: 'supplier', name: 'Supplier', component: Supplier },
        { path: 'ingredient', name: 'Ingredient', component: Ingredient },
        { path: 'order', name: 'Order', component: Order },
        { path: 'inventory', name: 'Inventory', component: Inventory },
        { path: 'member', name: 'Member', component: Member },
        { path: 'employee', name: 'Employee', component: Employee },
      ],
    },

    // ===== 管理员主页（带侧栏）=====
    {
      path: '/admin-home',
      component: RoleLayout,
      children: [
        { path: '', redirect: 'dish' },
        // 先复用一套页面，后续可按需裁剪/扩展为管理员专属
        { path: 'dish', component: Dish },
        { path: 'category', component: Category },
        { path: 'supplier', component: Supplier },
        { path: 'ingredient', component: Ingredient },
        { path: 'order', component: Order },
        { path: 'inventory', component: Inventory },
        { path: 'member', component: Member },
        { path: 'employee', component: Employee },
      ],
    },

    // ===== 教师主页（带侧栏）=====
    {
      path: '/teacher-home',
      component: RoleLayout,
      children: [
        { path: '', redirect: 'dish' },
        { path: 'dish', component: Dish },
        { path: 'order', component: Order },
        { path: 'ingredient', component: Ingredient },
        // 需要再加就继续挂子路由
      ],
    },

    // ===== 辅导员主页（带侧栏）=====
    {
      path: '/counselor-home',
      component: RoleLayout,
      children: [
        { path: '', redirect: 'member' },
        { path: 'member', component: Member },
        { path: 'order', component: Order },
      ],
    },

    // ===== 学生主页（带侧栏）=====
    {
      path: '/student-home',
      component: RoleLayout,
      children: [
        { path: '', redirect: 'dish' },
        { path: 'dish', component: Dish },
        { path: 'order', component: Order },
      ],
    },

    // 兜底
    { path: '/:pathMatch(.*)*', redirect: '/login' },
  ],
})

import { roleHome } from './role-map'

router.beforeEach((to, _from, next) => {
  // A. 调试/紧急入口：/login?force=1 直接放行，并顺手清理登录态
  if (to.path === '/login' && to.query.force === '1') {
    ;['token', 'role'].forEach(k => {
      localStorage.removeItem(k); sessionStorage.removeItem(k)
    })
    return next()
  }

  // B. 读取并“去污”token（把 'undefined'/'null' 视为无）
  const raw =
    localStorage.getItem('token') || sessionStorage.getItem('token') || ''
  const token =
    raw && raw !== 'undefined' && raw !== 'null' ? raw : null

  const role =
    (localStorage.getItem('role') || sessionStorage.getItem('role') || '') ||
    ''

  console.log('[guard] to=', to.fullPath, 'token?', !!token, 'role=', role, 'raw=', raw)

  // ① 已登录又访问 /login → 回角色首页（必须放最前）
  if (token && to.path === '/login') {
    return next(roleHome[role as keyof typeof roleHome] || '/main')
  }

  // ② 公开页（/login）直接放行（确保路由写了 meta.public）
  if (to.meta?.public) return next()

  // ③ 需要鉴权但没 token → 去登录
  if (!token) return next('/login')

  // ④ 其余放行
  next()
})




export default router
