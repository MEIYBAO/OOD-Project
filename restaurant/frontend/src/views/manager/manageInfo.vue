<template>
  <div class="employee-list">
    <div class="page-header">
      <h2>用户管理</h2>
      <el-button type="primary" @click="handleAdd">添加用户</el-button>
    </div>

    <el-card shadow="hover" class="table-container">
      <el-table
        v-loading="loading"
        :data="userList"
        border
        style="width: 100%"
      >
        <el-table-column type="index" width="50" align="center" />
        <el-table-column prop="username" label="用户名" min-width="140" />
        <el-table-column prop="role" label="角色" min-width="120">
          <template #default="{ row }">
            {{ roleLabel(row.role) }}
          </template>
        </el-table-column>
        <el-table-column label="操作" min-width="160" align="center">
          <template #default="{ row }">
            <el-button size="mini" type="primary" @click="handleEdit(row)">编辑</el-button>
            <el-button size="mini" type="danger" @click="handleDelete(row)">删除</el-button>
          </template>
        </el-table-column>
      </el-table>

      <div class="pagination-container">
        <el-pagination
          @size-change="handleSizeChange"
          @current-change="handleCurrentChange"
          :current-page="query.page"
          :page-sizes="[10, 20, 50, 100]"
          :page-size="query.limit"
          layout="total, sizes, prev, pager, next, jumper"
          :total="total"
        />
      </div>
    </el-card>

    <!-- 添加/编辑 用户（无状态、无创建时间） -->
    <el-dialog :title="dialogTitle" v-model="dialogVisible" width="500px">
      <el-form ref="formRef" :model="form" :rules="rules" label-width="80px">
        <el-form-item label="用户名" prop="username">
          <el-input v-model="form.username" placeholder="请输入用户名" :disabled="!!form.id" />
        </el-form-item>

        <el-form-item label="角色" prop="role">
          <el-select v-model="form.role" placeholder="请选择角色" style="width:100%">
            <el-option label="管理员" value="admin" />
            <el-option label="教师" value="teacher" />
            <el-option label="辅导员" value="counselor" />
            <el-option label="学生" value="student" />
          </el-select>
        </el-form-item>

        <!-- 仅新建时显示密码 -->
        <el-form-item v-if="!form.id" label="初始密码" prop="password">
          <el-input v-model="form.password" placeholder="默认 123456，不填则用默认" show-password />
        </el-form-item>
      </el-form>
      <template #footer>
        <el-button @click="dialogVisible = false">取 消</el-button>
        <el-button type="primary" @click="submitForm">确 定</el-button>
      </template>
    </el-dialog>
  </div>
</template>

<script setup lang="ts">
import { ref, reactive, onMounted } from 'vue'
import { ElMessageBox, ElMessage } from 'element-plus'
import { getUsers, createUser, updateUser, deleteUser } from '@/api/userManage'

const loading = ref(false)
const userList = ref<any[]>([])
const total = ref(0)
const query = reactive({
  page: 1,
  limit: 10,
  username: '',
  role: ''
})

const dialogVisible = ref(false)
const dialogTitle = ref('添加用户')
const formRef = ref()
const form = reactive<any>({
  id: null,
  username: '',
  role: 'student',
  password: '123456'
})

const rules = {
  username: [{ required: true, message: '请输入用户名', trigger: 'blur' }],
  role: [{ required: true, message: '请选择角色', trigger: 'change' }]
}

onMounted(() => {
  fetchList()
})

async function fetchList() {
  loading.value = true
  try {
    const res = await getUsers(query)
    userList.value = res.data || res?.data?.data || res?.data
    total.value = res.total ?? res?.data?.total ?? 0
  } catch (e) {
    console.error(e)
    ElMessage.error('获取用户失败')
  } finally {
    loading.value = false
  }
}

function handleSizeChange(v: number) { query.limit = v; fetchList() }
function handleCurrentChange(v: number) { query.page = v; fetchList() }

function roleLabel(role: string) {
  const map: Record<string, string> = {
    admin: '管理员', teacher: '教师', counselor: '辅导员', student: '学生'
  }
  return map[role] || role
}

function handleAdd() {
  dialogTitle.value = '添加用户'
  Object.assign(form, { id: null, username: '', role: 'student', password: '123456' })
  dialogVisible.value = true
}

function handleEdit(row: any) {
  dialogTitle.value = '编辑用户'
  Object.assign(form, { ...row, password: '' }) // 编辑不强制改密码
  dialogVisible.value = true
}

async function handleDelete(row: any) {
  try {
    await ElMessageBox.confirm(`确认删除用户「${row.username}」吗？`, '提示', { type: 'warning' })
    await deleteUser(row.id)
    ElMessage.success('删除成功')
    fetchList()
  } catch {}
}

function submitForm() {
  formRef.value.validate(async (valid: boolean) => {
    if (!valid) return
    try {
      if (form.id) {
        const payload: any = { username: form.username, role: form.role }
        if (form.password) payload.password = form.password  // 传了就重置
        await updateUser(form.id, payload)
        ElMessage.success('更新成功')
      } else {
        await createUser({
          username: form.username,
          role: form.role,
          password: form.password || '123456'
        })
        ElMessage.success('创建成功')
      }
      dialogVisible.value = false
      fetchList()
    } catch (e: any) {
      ElMessage.error(e?.response?.data?.message || '保存失败')
    }
  })
}
</script>

<style scoped>
.employee-list { padding: 20px; }
.page-header {
  display: flex; justify-content: space-between; align-items: center; margin-bottom: 20px;
}
.table-container { margin-bottom: 20px; }
.pagination-container { margin-top: 20px; text-align: right; }
</style>
