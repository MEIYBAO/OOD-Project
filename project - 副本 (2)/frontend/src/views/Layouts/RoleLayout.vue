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
import { useRoute } from 'vue-router'
import {
  Notification, PieChart, Compass, Edit,
  Finished, MessageBox, Crop, ZoomIn
} from '@element-plus/icons-vue'
import { roleChildren, type Role } from '@/router/role-map'

const route = useRoute()

// 当前根前缀：/admin-home /teacher-home /counselor-home /student-home 或 /main
const base = computed(() => route.matched[0]?.path || '/main')
// 生成绝对路径：/根前缀/子路由
const link = (p: string) => `${base.value.replace(/\/$/, '')}/${p}`

// 读取当前登录角色（登录成功已存到 localStorage）
const role = (localStorage.getItem('role') || 'student') as Role

// 全部可选菜单（key 必须与子路由 path 保持一致）
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

// 按角色白名单过滤菜单
const visibleMenus = computed(() => {
  const allow = new Set(roleChildren[role] || [])
  // 如果拿不到角色或未配置白名单，就显示空或全部（这里选择显示白名单内，否则为空）
  return allMenus.filter(m => allow.has(m.key))
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
