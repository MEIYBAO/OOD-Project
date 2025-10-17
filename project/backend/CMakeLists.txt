cmake_minimum_required(VERSION 3.26)
project(backend VERSION 0.1.0)

# 设置目标名称（可修改）
set(backend backend)

# 可通过 CMake 缓存覆盖 MySQL 安装根目录（例如 -DMYSQL_ROOT_DIR="C:/Program Files/MySQL/MySQL Server 8.0"）
set(MYSQL_ROOT_DIR "C:/Program Files/MySQL/MySQL Server 8.0" CACHE PATH "Path to MySQL root (install) directory")

# 首先在典型子目录中查找 include/lib/bin，允许用户只设置根目录
find_path(MYSQL_INCLUDE_DIR
                NAMES mysql.h
                PATHS
                "${MYSQL_ROOT_DIR}/include"
                "${MYSQL_ROOT_DIR}/include/mysql"
)

find_library(MYSQL_LIBRARY
                NAMES libmysql libmysql.lib mysqlclient libmysqlclient mysqlclient
                PATHS
                "${MYSQL_ROOT_DIR}/lib"
                "${MYSQL_ROOT_DIR}/lib64"
                "${MYSQL_ROOT_DIR}/lib32"
)

find_file(MYSQL_DLL
                NAMES libmysql.dll libmysqlclient.dll mysqlclient.dll
                PATHS
                "${MYSQL_ROOT_DIR}/lib"
                "${MYSQL_ROOT_DIR}/lib64"
                "${MYSQL_ROOT_DIR}/bin"
)

# 如果没找到，用更宽松的默认搜索（让 CMake 尝试系统路径）
if(NOT MYSQL_INCLUDE_DIR)
        find_path(MYSQL_INCLUDE_DIR NAMES mysql.h)
endif()
if(NOT MYSQL_LIBRARY)
        find_library(MYSQL_LIBRARY NAMES libmysql libmysql.lib mysqlclient libmysqlclient mysqlclient)
endif()
if(NOT MYSQL_DLL)
        find_file(MYSQL_DLL NAMES libmysql.dll libmysqlclient.dll mysqlclient.dll)
endif()

if(MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
        message(STATUS "Found MySQL include dir: ${MYSQL_INCLUDE_DIR}")
        message(STATUS "Found MySQL library: ${MYSQL_LIBRARY}")
        if(MYSQL_DLL)
                message(STATUS "Found MySQL DLL: ${MYSQL_DLL}")
        endif()
        set(MySQL_FOUND TRUE)
else()
        message(FATAL_ERROR "MySQL not found. Please check MYSQL_ROOT_DIR or install MySQL development components.")
endif()

# 创建可执行文件 - 指定目标名和源文件
add_executable(${backend} cpp/main.cpp)

# 设置包含目录和链接库
target_include_directories(${backend} PRIVATE ${MYSQL_INCLUDE_DIR})
target_link_libraries(${backend} PRIVATE ${MYSQL_LIBRARY})

# 如果找到了 DLL，构建后复制到可执行文件目录，便于运行时加载（Windows）
if(MYSQL_DLL)
        add_custom_command(TARGET ${backend} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${MYSQL_DLL}"
                        "$<TARGET_FILE_DIR:${backend}>"
                        COMMENT "Copying MySQL DLL to output directory"
        )
else()
        message(WARNING "MySQL DLL not found; you may need to ensure the DLL is on PATH at runtime.")
endif()

# Windows / MSVC 特定设置
if(MSVC)
        target_compile_definitions(${backend} PRIVATE
                        _CRT_SECURE_NO_WARNINGS
                        _WINSOCK_DEPRECATED_NO_WARNINGS
        )
        # MSVC 编译时默认运行时库设置可以通过 CMake 变量控制，这里不强制覆盖全部标志
endif()