# 13-1
```
    1) 复制 200K 文件
    1 bytes: 0.560 secs
    10 bytes: 0.386 secs
    512 bytes: 0.053 secs
    512000 bytes: 0.010 secs

    2) 调用 O_SYNC 相当于每次都直接往磁盘输送, 速度会下降很多

    3) 没测试
```

# 13-2
```
    ./write_bytes ../../abc.text 100000000 10  1.10s user 10.20s system 99% cpu 11.305 total
    ./write_bytes ../../abc.text 100000000 100  0.14s user 0.73s system 99% cpu 0.875 total
    ./write_bytes ../../abc.text 100000000 1000  0.04s user 0.73s system 99% cpu 0.875 total
    ./write_bytes ../../abc.text 100000000 10000  0.00s user 0.07s system 99% cpu 0.072 total
```

# 13-3
```
    fflush(fp)
    通过 write 将 用户缓冲区 刷新 到 与 fp 流 相关联 内核缓冲区 
    fsync(fileno(fp));
    fileno 获取 fp 相关联的文件, fsync 刷新 内核缓冲区的数据到 相关联的文件处(硬盘)
```

# 13-4
```
    若 stdin 和 stdout 指向一终端, 那么无论何时从 stdin 中读取输入时, 都将隐含调用一次 fflush(stdout) 的函数.
    write 的话, 会立即触发往 buffer I/O.
```