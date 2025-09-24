# -C-11-muduo-

本项目是在学习并参考 muduo 源码的基础上,使用C++ 11 对muduo网络库进行了重构，去除 muduo对boost库的依赖，并根据muduo的设计思想，实现的基于多 Reactor 多线程模型的网络库。

## 构建项目

#执行脚本构建项目

```shell
bash build.sh
```

## 执行生成文件

```shell
# 测试代码
cd ./example
./test 
```

```shell
# 测试连接
telnet 127.0.0.1 8000
```
