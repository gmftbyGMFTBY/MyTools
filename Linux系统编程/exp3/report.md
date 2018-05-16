## Mshell

---

>Mshell 全部使用  C 语言编写完成，利用系统调用实现对 Linux shell 命令的模拟，并且在此基础上进行一系列的功能扩展，完善用户的 shell 体验

1. 特点
   * 全面的 shell 环境，不用切换到父进程的 shell ，基本功能在 Mshell 中得到全部实现
   * 采用动态链接库将所有的基础命令和主控程序连接在一起形成一个完整的 shell 执行环境，编译后的程序的大小比较小，并且对代码的维护和调试工作很容易进行
   * 针对基础的命令，还实现了一批对应的参数，使得 shell 的功能的多样化
2. 基础命令实现
   * ls
   * cd
   * cp
   * mv
   * time
   * mkdir
   * du
   * pwdl
   * touch
   * echo
   * nano
   * cat
   * find
   * grep
   * rm
   * history
   * exit
3. 扩展命令
   * 回收站命令 - m2t / r4t
   * 管道和重定向
   * tree 展开目录树
   * autojump 目录跳跃