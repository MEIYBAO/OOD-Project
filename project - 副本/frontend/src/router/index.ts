import { createRouter, createWebHistory } from 'vue-router'
import LoginPage from '@/views/Login/loginPage.vue'
import MainLayout from '@/views/Layouts/MainLayout.vue'

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

    // 主布局（带侧栏）
    {
      path: '/main',
      component: MainLayout,
      children: [
        { path: '', redirect: 'dish' },        // /main -> /main/dish
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

    // 兜底
    { path: '/:pathMatch(.*)*', redirect: '/login' },
  ],
})

export default router
