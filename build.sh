#!/bin/sh

# shell 在执行命令前把她打印出来
set -x

SOURCE_DIR=`pwd`
# 环境变量的默认值定义，如果你没提前 export 它们，就用默认值：
BUILD_DIR=${BUILD_DIR:-../build}
# 构建类型，可改成 debug 或 RelWithDebInfo
BUILD_TYPE=${BUILD_TYPE:-release}
# 安装目录
INSTALL_DIR=${INSTALL_DIR:-../${BUILD_TYPE}-install-cpp11}
CXX=${CXX:-g++}

# 这是为 clangd、VSCode 等 LSP 工具创建的软链接，把 compile_commands.json 链接到源码目录，方便代码跳转、补全等功能。
ln -sf $BUILD_DIR/$BUILD_TYPE-cpp11/compile_commands.json

#  创建并进入构建目录
mkdir -p $BUILD_DIR/$BUILD_TYPE-cpp11 \
  && cd $BUILD_DIR/$BUILD_TYPE-cpp11 \
  && cmake \
           -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
           -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
           -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
           $SOURCE_DIR \
  && make $*
# 运行 cmake，再执行 make，$* 是把你传给 build.sh 的所有参数转发进去

# Use the following command to run all the unit tests
# at the dir $BUILD_DIR/$BUILD_TYPE :
# CTEST_OUTPUT_ON_FAILURE=TRUE make test

# cd $SOURCE_DIR && doxygen

