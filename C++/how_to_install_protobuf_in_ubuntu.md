关于 ubuntu 中install grpc过程中的问题


Q1: 
在ubuntu系统中`sudo apt-get install protobuf-compiler libprotobuf-dev`安装的protobuf版本老旧 

在c++编译链接时 通常会报error
![image.png](https://s2.loli.net/2023/06/08/NqYUmy2M9ZTV7xv.png)

原因是 /usr/include/google/protobuf中的header版本老街
解决方法是 从源码构建

A: 
可以看官方文档 https://github.com/protocolbuffers/protobuf/blob/3.6.x/src/README.md

下载过的 先卸载
```bash
sudo apt-get update
mkdir -p /tmp/test_google_protobuf
pushd /tmp/test_google-protobuf
git clone https://github.com/abseil/abseil-cpp.git
cd abseil-cpp
mkdir build
cd build
cmake ..
make
sudo make install

```