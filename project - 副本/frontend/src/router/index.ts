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

    // ===== 管理员主页（带侧栏）=====
    {
      path: '/admin-home',
      component: RoleLayout,
      children: [
        { path: '', redirect: '/admin-home/dish' },
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
        { path: '', redirect: '/teacher-home/dish' },
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
        { path: '', redirect: '/counselor-home/member' },
        { path: 'member', component: Member },
        { path: 'order', component: Order },
      ],
    },

    // ===== 学生主页（带侧栏）=====
    {
      path: '/student-home',
      component: RoleLayout,
      children: [
        { path: '', redirect: '/student-home/dish' },
        { path: 'dish', component: Dish },
        { path: 'order', component: Order },
      ],
    },
  ],
})


export default router
