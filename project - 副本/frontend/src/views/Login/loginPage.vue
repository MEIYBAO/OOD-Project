<script setup lang="ts">
import { ref, reactive } from 'vue'
import { User, Lock } from '@element-plus/icons-vue'
import { ElMessage } from 'element-plus'
import { useRouter } from 'vue-router'
import request from '@/utils/request'          // 你的 axios 实例
import { roleHome } from '@/router/role-map'   // 角色 → 主页映射

const router = useRouter()

// 表单引用 & 数据
const formRef = ref()
const formModel = reactive({
  username: '',
  password: '',
  remember: false, // “记住我”可选（这里仅作为 UI，不做区分存储）
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
}

const loading = ref(false)

// API 封装
const apiLogin = (p: { username: string; password: string }) =>
  request.post('/auth/login', p) // 返回 { token }
const apiGetMe = () => request.get('/auth/me') // 返回 { id, username, role }

// 登录逻辑
const login = async () => {
  await formRef.value?.validate()
  loading.value = true
  try {
    // 1) 登录获取 token
    const { token } = await apiLogin({
      username: formModel.username.trim(),
      password: formModel.password,
    })

    // “记住我”：记到 localStorage；否则短期会话用 sessionStorage
    if (formModel.remember) {
      localStorage.setItem('token', token)
    } else {
      sessionStorage.setItem('token', token)
    }

    // 2) 拉取用户信息（含角色）
    const me = await apiGetMe() as { id: number; username: string; role: string }
    const storage = formModel.remember ? localStorage : sessionStorage
    storage.setItem('role', me.role)

    // 3) 计算目标主页并显式等待导航
    const home = roleHome[me.role as keyof typeof roleHome] || '/main'
    console.log('[login] role =', me.role)
    console.log('[login] goto =', home)

    try {
      await router.replace(home) // 一定 await，方便观察守卫是否重定向
      console.log('[login] navigated to', router.currentRoute.value.fullPath)
      ElMessage.success('登录成功')
    } catch (navErr) {
      console.error('[login] navigation error:', navErr)
    }
  } catch (e: any) {
    // 登录失败：清理所有登录态
    localStorage.removeItem('token')
    localStorage.removeItem('role')
    sessionStorage.removeItem('token')
    sessionStorage.removeItem('role')
    ElMessage.error(e?.response?.data?.message || '登录失败')
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
          />
        </el-form-item>

        <el-form-item prop="password">
          <el-input
            v-model="formModel.password"
            :prefix-icon="Lock"
            type="password"
            placeholder="请输入密码"
          />
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
