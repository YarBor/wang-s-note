```cmake
cmake_minimum_required(VERSION 3.5)
project(test1)
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
find_package(gRPC CONFIG REQUIRED)
set(_GRPC_GRPCPP gRPC::grpc++)

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}/../gen_code")
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/../gen_code/test1.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/../gen_code/test1.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/../gen_code/test1.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/../gen_code/test1.grpc.pb.h")
# hw_grpc_proto
add_library(hw_grpc_proto
  ${hw_grpc_srcs}
  ${hw_grpc_hdrs}
  ${hw_proto_srcs}
  ${hw_proto_hdrs})
target_link_libraries(hw_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# Targets greeter_[async_](client|server)
foreach(_target
  test1_server test1_client
  )
  add_executable(${_target} "${_target}.cc")
  target_link_libraries(${_target}
    hw_grpc_proto
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()
```