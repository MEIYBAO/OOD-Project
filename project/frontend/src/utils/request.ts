// src/utils/request.ts
import axios from 'axios'
import router from '@/router'

function getToken(): string | null {
  return localStorage.getItem('token') || sessionStorage.getItem('token')
}

const request = axios.create({
  baseURL: import.meta.env.VITE_API_BASE || '/api',
  timeout: 10000,
})

// 请求拦截：自动在 Authorization 前面补上 Bearer
request.interceptors.request.use((config) => {
  const t = getToken()
  if (t) {
    config.headers = config.headers ?? {}
    // 避免重复拼接 Bearer
    const hasBearer = /^Bearer\s+/i.test(t)
    config.headers.Authorization = hasBearer ? t : `Bearer ${t}`
  }
  return config
})

// 响应拦截：统一解包 data；401 清理并跳转；其它错误透传
request.interceptors.response.use(
  (resp) => {
    // 若后端统一在 data 里返回业务数据，这里直接解包
    return resp.data
  },
  (error) => {
    const status = error?.response?.status
    const url = error?.config?.url
    // 可选：在控制台打印，便于调试
    console.warn('[axios:error]', status, url, error?.response?.data)

    if (status === 401) {
      // token 失效：清理并回登录页
      localStorage.removeItem('token')
      localStorage.removeItem('role')
      sessionStorage.removeItem('token')
      sessionStorage.removeItem('role')

      if (router.currentRoute.value.path !== '/login') {
        router.replace('/login')
      }
    }
    // 其它状态（如 400/403/409/500）交由调用方处理
    return Promise.reject(error)
  }
)

export default request
