import { defineStore } from 'pinia'
import { apiLogin, apiGetMe } from '@/api/user'

type Role = 'admin' | 'teacher' | 'counselor' | 'student'
interface UserInfo { id: number; username: string; role: Role }

export const useUserStore = defineStore('user', {
    state: () => ({
        token: localStorage.getItem('token') || '',
        info: null as UserInfo | null
    }),
    actions: {
        async login(payload: { username: string; password: string }) {
            const { token } = await apiLogin(payload)
            this.token = token
            localStorage.setItem('token', token)
        },
        async fetchMe() {
            const me = await apiGetMe()
            this.info = me
            localStorage.setItem('role', me.role)
        },
        logout() {
            this.token = ''
            this.info = null
            localStorage.removeItem('token')
            localStorage.removeItem('role')
        }
    }
})
