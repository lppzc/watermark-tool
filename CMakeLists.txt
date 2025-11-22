cmake_minimum_required(VERSION 3.10)

# 项目信息
project(WatermarkTool VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找OpenCV包
find_package(OpenCV REQUIRED)

# 添加可执行文件
add_executable(watermark watermark.cpp)

# 链接OpenCV库
target_link_libraries(watermark ${OpenCV_LIBS})

# 设置输出目录为当前目录
set_target_properties(watermark PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# 安装规则
install(TARGETS watermark
    RUNTIME DESTINATION bin
)

install(FILES batch_watermark.sh
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    DESTINATION bin
)

install(FILES README.md
    DESTINATION share/doc/watermark
)