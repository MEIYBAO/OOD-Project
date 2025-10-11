// src/router/index.ts
import { createRouter, createWebHistory } from 'vue-router'
import LoginPage from '@/views/Login/loginPage.vue'
import RoleLayout from '@/views/Layouts/RoleLayout.vue'

// 业务页（可在不同角色下复用）
import Dish from '@/views/Dish/List.vue'
import Category from '@/views/dish/Category.vue'
import Supplier from '@/views/Supplier/index.vue'
import Ingredient from '@/views/Ingredient/index.vue'
import Order from '@/views/Order/List.vue'
import Inventory from '@/views/Inventory/Record.vue'
import Member from '@/views/Member/index.vue'
import Employee from '@/views/Employee/index.vue'

// 角色 → 主页映射（与后端 user_account.role 一致）
export const roleHome: Record<string, string> = {
  manager: '/manager-home/dish',
  teacher: '/teacher-home/dish',
  counselor: '/counselor-home/member',
  student: '/student-home/dish',
}

// 路由表
const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    { path: '/', redirect: '/login' },

    // 登录页（公开）
    { path: '/login', name: 'Login', component: LoginPage, meta: { public: true } },

    // ===== 管理员（manager）主页 =====
    {
      path: '/manager-home',
      component: RoleLayout,
      meta: { role: 'manager' },
      children: [
        { path: '', redirect: '/manager-home/dish' },
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

    // ===== 教师主页 =====
    {
      path: '/teacher-home',
      component: RoleLayout,
      meta: { role: 'teacher' },
      children: [
        { path: '', redirect: '/teacher-home/dish' },
        { path: 'dish', component: Dish },
        { path: 'order', component: Order },
        { path: 'ingredient', component: Ingredient },
      ],
    },

    // ===== 辅导员主页 =====
    {
      path: '/counselor-home',
      component: RoleLayout,
      meta: { role: 'counselor' },
      children: [
        { path: '', redirect: '/counselor-home/member' },
        { path: 'member', component: Member },
        { path: 'order', component: Order },
      ],
    },

    // ===== 学生主页 =====
    {
      path: '/student-home',
      component: RoleLayout,
      meta: { role: 'student' },
      children: [
        { path: '', redirect: '/student-home/dish' },
        { path: 'dish', component: Dish },
        { path: 'order', component: Order },
      ],
    },

    // 兜底
    { path: '/:pathMatch(.*)*', redirect: '/login' },
  ],
})



export default router
