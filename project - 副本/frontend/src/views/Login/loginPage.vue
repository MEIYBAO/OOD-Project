<script setup lang="ts">
import { ref, reactive } from 'vue'
import { User, Lock } from '@element-plus/icons-vue'
import { ElMessage } from 'element-plus'
import { useRouter } from 'vue-router'
import request from '@/utils/request'          // 你的 axios 实例
import { roleHome } from '@/router/role-map'   // 角色 → 主页映射

type Role = 'admin' | 'teacher' | 'counselor' | 'student'

const router = useRouter()

// 表单引用 & 数据
const formRef = ref()
const formModel = reactive({
  username: '',
  password: '',
  role: '' as '' | Role,     // 新增：角色
  remember: false,
})

// 登录表单校验规则
const rules = {
  username: [
    { required: true, message: '请输入用户名', trigger: 'blur' },
    { min: 3, max: 20, message: '用户名长度 3-20 位', trigger: 'blur' },
  ],
  password: [
    { required: true, message: '请输入密码', trigger: 'blur' },
    { pattern: /^\S{6,32}$/, message: '密码需 6-32 位非空字符', trigger: 'blur' },
  ],
  role: [
    { required: true, message: '请选择身份', trigger: 'change' }, // 建议必选，避免同名冲突
  ],
}

const roleOptions: Role[] = ['admin', 'teacher', 'counselor', 'student']
const loading = ref(false)

// API 封装（加上 role）
const apiLogin = (p: { username: string; password: string; role: Role }) =>
  request.post('/auth/login', p) // 可能返回 { token, user }
const apiGetMe = () => request.get('/auth/me') // 返回 { id, username, role, ... }

// 登录逻辑
const login = async () => {
  await formRef.value?.validate()
  loading.value = true
  try {
    // 1) 登录获取 token（优先提交带 role）
    const payload = {
      username: formModel.username.trim(),
      password: formModel.password,
      role: formModel.role as Role,
    }
    const resp = await apiLogin(payload) as any
    const token: string = resp?.token

    if (!token) throw new Error('服务器未返回 token')

    // “记住我”：记到 localStorage；否则 sessionStorage
    const storage = formModel.remember ? localStorage : sessionStorage
    storage.setItem('token', token)

    // 2) 解析角色：优先从登录返回的 user 里拿；否则调用 /me
    let role: Role | null = resp?.user?.role ?? null
    if (!role) {
      const me = await apiGetMe() as { id: string; username: string; role: Role }
      role = me.role
    }
    if (!role) throw new Error('无法获取用户角色')

    storage.setItem('role', role)

    // 3) 跳转到角色主页
    const home = roleHome[role] || '/main'
    await router.replace(home)
    ElMessage.success('登录成功')
  } catch (e: any) {
    // 登录失败：清理所有登录态
    localStorage.removeItem('token'); localStorage.removeItem('role')
    sessionStorage.removeItem('token'); sessionStorage.removeItem('role')

    // 后端若返回 409（未选择身份或同名同密多匹配），这里也会走到
    const msg = e?.response?.data?.message || e?.message || '登录失败'
    ElMessage.error(msg)
  } finally {
    loading.value = false
  }
}
</script>

<template>
  <el-row class="login-page">
    <el-col :span="6" :offset="9" class="form">
      <!-- 登录表单 -->
      <el-form
        :model="formModel"
        :rules="rules"
        ref="formRef"
        size="large"
        autocomplete="off"
        @keyup.enter="login"
      >
        <el-form-item>
          <h1>登录</h1>
        </el-form-item>

        <el-form-item prop="username">
          <el-input
            v-model="formModel.username"
            :prefix-icon="User"
            placeholder="请输入用户名"
            clearable
          />
        </el-form-item>

        <el-form-item prop="password">
          <el-input
            v-model="formModel.password"
            :prefix-icon="Lock"
            type="password"
            placeholder="请输入密码"
            show-password
            clearable
          />
        </el-form-item>

        <!-- 新增：角色选择 -->
        <el-form-item prop="role">
          <el-select
            v-model="formModel.role"
            placeholder="请选择身份"
            style="width: 100%"
            clearable
          >
            <el-option v-for="r in roleOptions" :key="r" :label="r" :value="r" />
          </el-select>
        </el-form-item>

        <el-form-item class="flex">
          <div class="flex">
            <el-checkbox v-model="formModel.remember">记住我</el-checkbox>
            <el-link type="primary" :underline="false">忘记密码？</el-link>
          </div>
        </el-form-item>

        <el-form-item>
          <el-button
            class="button"
            type="primary"
            auto-insert-space
            :loading="loading"
            @click="login"
          >
            登录
          </el-button>
        </el-form-item>
      </el-form>
    </el-col>
  </el-row>
</template>

<style lang="scss" scoped>
.login-page {
  height: 100vh;
  background-color: #fff;
  .form {
    display: flex;
    flex-direction: column;
    justify-content: center;
    user-select: none;
    .button { width: 100%; }
    .flex {
      width: 100%;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
  }
}
</style>
