<script setup lang="ts">
import { ref, reactive } from 'vue'
import { User, Lock } from '@element-plus/icons-vue'
import { ElMessage } from 'element-plus'
import { useRouter } from 'vue-router'
import request from '@/utils/request'
import { roleHome } from '@/router/role-map'

type Role = 'admin' | 'teacher' | 'counselor' | 'student'

const router = useRouter()

// 角色选项：中文显示 -> 英文值
const roleOptions = [
  { label: '管理员', value: 'admin' as Role },
  { label: '教师',   value: 'teacher' as Role },
  { label: '辅导员', value: 'counselor' as Role },
  { label: '学生',   value: 'student' as Role },
]

const formRef = ref()
const formModel = reactive({
  username: '',
  password: '',
  role: '' as '' | Role,
  remember: false,
})

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
    { required: true, message: '请选择身份', trigger: 'change' },
  ],
}

const loading = ref(false)

// API
const apiLogin = (p: { username: string; password: string; role: Role }) =>
  request.post('/auth/login', p)
const apiGetMe = () => request.get('/auth/me')

// 登录
const login = async () => {
  await formRef.value?.validate()
  loading.value = true
  try {
    const resp = await apiLogin({
      username: formModel.username.trim(),
      password: formModel.password,
      role: formModel.role as Role,
    }) as any

    const token: string = resp?.token
    if (!token) throw new Error('服务器未返回 token')

    const storage = formModel.remember ? localStorage : sessionStorage
    storage.setItem('token', token)

    // 优先使用返回里的 user.role，否则再调 /me
    let role: Role | null = resp?.user?.role ?? null
    if (!role) {
      const me = await apiGetMe() as { id: string; username: string; role: Role }
      role = me.role
    }
    if (!role) throw new Error('无法获取用户角色')

    storage.setItem('role', role)

    const home = roleHome[role] || '/main'
    await router.replace(home)
    ElMessage.success('登录成功')
  } catch (e: any) {
    localStorage.removeItem('token'); localStorage.removeItem('role')
    sessionStorage.removeItem('token'); sessionStorage.removeItem('role')
    ElMessage.error(e?.response?.data?.message || e?.message || '登录失败')
  } finally {
    loading.value = false
  }
}
</script>

<template>
  <el-row class="login-page">
    <el-col :span="6" :offset="9" class="form">
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

        <!-- 身份选择：中文显示，英文值 -->
        <el-form-item prop="role">
          <el-select
            v-model="formModel.role"
            placeholder="请选择身份"
            style="width: 100%"
            clearable
          >
            <el-option
              v-for="opt in roleOptions"
              :key="opt.value"
              :label="opt.label"
              :value="opt.value"
            />
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
