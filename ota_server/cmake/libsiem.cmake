# 添加第三方依赖包
include(FetchContent)
# FetchContent_MakeAvailable was not added until CMake 3.14
if(${CMAKE_VERSION} VERSION_LESS 3.14)
    include(add_FetchContent_MakeAvailable.cmake)
endif()

set(libsiem_GIT_BRANCH master)  # 指定分支
set(libsiem_GIT_URL  https://github.com/nickyyy28/Siempre.git)  # 指定git仓库地址

FetchContent_Declare(
  libsiem
  GIT_REPOSITORY    ${libsiem_GIT_URL}
  GIT_BRANCH           ${libsiem_GIT_BRANCH}
)

FetchContent_MakeAvailable(libsiem)