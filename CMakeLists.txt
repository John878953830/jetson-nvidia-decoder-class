cmake_minimum_required(VERSION 3.10)

project(decode_tx2)

set(CMAKE_CXX_STANDARD 11)

find_package(PkgConfig REQUIRED)

pkg_check_modules(opencv REQUIRED IMPORTED_TARGET opencv)

include_directories(/media/d0a2fe3d-1c3e-4b60-877e-72e014856c80/jetson_multimedia_api/include)

add_executable(decode_tx2 main.cpp decoder_h264.cpp decoder_h264.h NvApplicationProfiler.cpp NvBuffer.cpp NvEglRenderer.cpp NvElement.cpp NvElementProfiler.cpp NvLogging.cpp NvUtils.cpp NvV4l2Element.cpp NvV4l2ElementPlane.cpp NvVideoConverter.cpp NvVideoDecoder.cpp NvVideoEncoder.cpp)

target_link_libraries(${PROJECT_NAME} PkgConfig::opencv)

target_link_libraries(${PROJECT_NAME} "/usr/local/cuda/lib64/libcudart.so")

target_link_libraries(${PROJECT_NAME} "/usr/local/cuda/lib64/libnvrtc.so")

target_link_libraries(${PROJECT_NAME} "/usr/lib/aarch64-linux-gnu/tegra/libcuda.so")

target_link_libraries(${PROJECT_NAME} "/usr/lib/aarch64-linux-gnu/tegra/libnvbuf_utils.so")

target_link_libraries(${PROJECT_NAME} "/usr/lib/aarch64-linux-gnu/tegra/libnvjpeg.so")

target_link_libraries(${PROJECT_NAME} "/usr/lib/aarch64-linux-gnu/tegra/libnvosd.so")

target_link_libraries(${PROJECT_NAME} "/usr/lib/aarch64-linux-gnu/tegra/libdrm.so.2")

target_link_libraries(${PROJECT_NAME} "/usr/lib/aarch64-linux-gnu/libnvparsers.so")

target_link_libraries(${PROJECT_NAME} "/usr/lib/aarch64-linux-gnu/libnvinfer.so")

target_link_libraries(${PROJECT_NAME} "/usr/lib/aarch64-linux-gnu/libX11.so")

target_link_libraries(${PROJECT_NAME} "/usr/lib/aarch64-linux-gnu/libEGL.so")

target_link_libraries(${PROJECT_NAME} "/usr/lib/aarch64-linux-gnu/libGLESv2.so")

target_link_libraries(${PROJECT_NAME} "/usr/lib/aarch64-linux-gnu/libv4l2.so")

target_link_libraries(${PROJECT_NAME} "/usr/lib/aarch64-linux-gnu/libpthread.so")


