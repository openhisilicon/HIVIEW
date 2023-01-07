
# 编译
## make
1. 配置环境`. ./build/3516d`
2. 根目录下`make`全部编译
3. 单独编译使用`make -C ./mod/bsp/`

## 更新应用
1. 使用`./tools/update-dev.sh`, 不带参数会有帮助
2. 公网ip服务器编译, `python3 -m http.server 8001`简单的http传输

# 调试
## 查看日志
```
tail -F /tmp/log
```

# 疑难问题记录

## 64位系统编译32位程序
```
sudo apt-get install gcc-multilib g++-multilib
sudo apt-get install build-essential module-assistant
```
