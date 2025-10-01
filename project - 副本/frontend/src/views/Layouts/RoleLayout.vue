<template>
  <div id="app">
    <el-container>
      <el-aside width="200px">
        <el-menu
          :router="true"
          :default-active="$route.path"
          background-color="#304156"
          text-color="#bfcbd9"
          active-text-color="#409EFF"
        >
          <!-- 只渲染当前角色允许的菜单项 -->
          <el-menu-item
            v-for="m in visibleMenus"
            :key="m.key"
            :index="link(m.key)"
          >
            <el-icon><component :is="m.icon" /></el-icon>
            <span>{{ m.label }}</span>
          </el-menu-item>
        </el-menu>
      </el-aside>

      <el-container>
        <el-main>
          <router-view />
        </el-main>
      </el-container>
    </el-container>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import {
  Notification, PieChart, Compass, Edit,
  Finished, MessageBox, Crop, ZoomIn
} from '@element-plus/icons-vue'
import { roleChildren, type Role, homeByRole } from '@/router/role-map'

const router = useRouter()
const route = useRoute()

// 1) 角色：同时从 localStorage / sessionStorage 读取，避免“未勾记住我”时失效
const role = (
  localStorage.getItem('role') ||
  sessionStorage.getItem('role') ||
  'student'
) as Role

// 2) 根前缀：直接根据角色映射（更稳），如 /admin-home /teacher-home ...
const base = computed(() => homeByRole(role))

// 3) 生成完整路径
const link = (p: string) => `${base.value.replace(/\/$/, '')}/${p}`

// 4) 全部可选菜单（key 必须与子路由 path 一致）
const allMenus = [
  { key: 'dish',       label: '菜品列表',   icon: Notification },
  { key: 'category',   label: '菜品类别',   icon: PieChart },
  { key: 'supplier',   label: '供应商管理', icon: Compass },
  { key: 'ingredient', label: '原材料管理', icon: Edit },
  { key: 'order',      label: '订单管理',   icon: Finished },
  { key: 'inventory',  label: '库存管理',   icon: MessageBox },
  { key: 'member',     label: '会员管理',   icon: Crop },
  { key: 'employee',   label: '员工管理',   icon: ZoomIn },
]

// 5) 先按角色白名单过滤，再校验“对应路由已注册”（避免死链）
const visibleMenus = computed(() => {
  const allow = new Set(roleChildren[role] || [])
  return allMenus
    .filter(m => allow.has(m.key))
    .map(m => ({ ...m, path: link(m.key) }))
    .filter(m => router.resolve(m.path).matched.length > 0)
})
</script>


<style>
body {
  margin: 0;
  padding: 0;
  font-family: "Helvetica Neue", Helvetica, "PingFang SC", "Hiragino Sans GB", "Microsoft YaHei", Arial, sans-serif;
}
#app { height: 100vh; }
.el-container { height: 100%; }
.el-aside { background-color: #304156; color: #fff; }
.logo {
  height: 60px; line-height: 60px; text-align: center;
  font-size: 18px; font-weight: bold; color: #fff; background-color: #2b3649;
}
.el-header {
  background-color: #fff; color: #333; line-height: 60px; border-bottom: 1px solid #e6e6e6;
}
.header-right { float: right; display: flex; align-items: center; }
.header-right span { margin-right: 10px; }
.el-main { background-color: #f0f2f5; padding: 20px; }
</style>
