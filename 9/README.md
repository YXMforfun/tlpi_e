# 9-1
```
    real = 1000, effective = 0, saved = 0, fs = 0
```
#### 实际用户 不是 root(0), 有效用户 为 root (0), 类似 sudo 之类
### a) setuid(2000);
```
    real = 2000, effective = 2000, saved = 2000, fs = 2000
    进程为 特权进程(有效用户为 0), setuid 传非 0 参数, 实际用户 有效用户 设置用户 文件用户 全更新为 该值.
```
### b) setreuid(-1, 2000);
```
    real = 1000, effective = 2000, saved = 2000, fs = 2000
    -1 不修改 实际用户 其余同上
```
### c) seteuid(2000);
```
    real = 1000, effective = 2000, saved = 0, fs = 2000
    修改 有效用户
```
### d) setfsuid(2000);
```
    real = 1000, effective = 0, saved = 0, fs = 2000
    修改 文件用户
```
### e) setresuid(-1, 2000, 3000);
```
    real = 1000, effective = 2000, saved = 3000, fs = 2000
```

# 9-2
```
    real = 0, effective = 1000, saved = 1000, fs = 1000

    不完全 享有特权, 因为 有效用户 不为 root(0) 也不为 实际用户
```

# 9-4
```
    real = X, effective = X, saved = X => real = X, effective = Y, saved = Y

    setuid(getuid()); // 挂起
    setuid(geteuid()); // 恢复
    setuid 无法永久放弃 set-user-ID

    seteuid(getuid()); // 挂起
    seteuid(geteuid()); // 恢复
    seteuid 无法永久放弃 set-user-ID

    setreuid(-1, getuid()); // 挂起
    setreuid(-1, geteuid()); // 恢复
    setreuid(getuid(), getuid()); // 永久放弃 set-user-ID

    setresuid(-1, getuid(), -1); // 挂起
    setresuid(-1, geteuid(), -1); // 恢复
    setresuid(getuid(), getuid(), getuid()); // 永久放弃 set-user-ID
```

# 9-5
```
    real = X, effective = 0, saved = 0

    setuid() 三者均不可能实现

    seteuid(getuid()); // 挂起
    seteuid(0); // 恢复
    seteuid 无法永久放弃 set-user-ID

    setreuid(-1, geteuid()); // 挂起
    setreuid(-1, 0); // 恢复
    setreuid(getuid(), getuid()); // 永久放弃 set-user-ID

    setresuid(-1, getuid(), -1); // 挂起
    setresuid(-1, 0, -1); // 恢复
    setresuid(getuid(), getuid(), getuid()); // 永久放弃 set-user-ID
```
