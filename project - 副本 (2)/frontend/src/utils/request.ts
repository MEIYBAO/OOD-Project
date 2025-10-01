import axios from 'axios'
import router from '@/router'

const request = axios.create({
  baseURL: import.meta.env.VITE_API_BASE || 'http://localhost:3000/api',
  timeout: 10000,
})

/** 请求拦截：带上 Authorization */
request.interceptors.request.use((config) => {
  const t = localStorage.getItem('token') || sessionStorage.getItem('token')
  if (t) {
    config.headers = config.headers || {}
    // 根据你后端要求二选一——大多数后端需要 Bearer：
    // 1) 若后端需要 Bearer 方案（常见）
    config.headers.Authorization = t.startsWith('Bearer ') ? t : `Bearer ${t}`
    // 2) 若你的后端就是“纯 token”，请改成：
    // config.headers.Authorization = t
  }
  return config
})

/** 响应拦截：只有 401 才跳登录；其它错误不跳 */
request.interceptors.response.use(
  (resp) => resp.data,
  (error) => {
    const url = error?.config?.url
    const status = error?.response?.status
    console.warn('[axios] error', status, url, error?.response?.data)

    if (status === 401) {
      localStorage.removeItem('token'); localStorage.removeItem('role')
      sessionStorage.removeItem('token'); sessionStorage.removeItem('role')
      if (router.currentRoute.value.path !== '/login') router.replace('/login')
    }
    return Promise.reject(error)
  }
)

export default request
