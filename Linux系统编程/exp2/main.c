#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

/*
   socket
   socket 是一种特殊的文件描述符，一种编程接口
   1. 套接字的类型
        1. SOCK_STREAM
            面向连接的，可靠的数据传输服务，数据无差错的发送，并且按照顺序接收， FTP 协议使用的就是流式套接字
        2. SOCK_DGRAM
            提供一个无连接的服务，数据包使用功能独立包的形式发送，不提供容错机制
   2. 头文件
     #include <sys/types.h>
     $include <sys/socket.h>
     #include <sys/un.h>: 相关数据结构的定义

   3. 基本流程
        1. server socket create
        2. bind
        3. listen
        4. accept
        5. client socket create
        6. connect to sercer socket
        7. read, write, close, ...

    4. 系统调用
        1. 创建无名套接字
            int socket(int domain, int type, int protocol)
                调用成功返回新的和套接字关联的文件描述符,调用失败返回 -1
            * domain: 域名
                PF_LOCAL: 客户端和服务器在本机中
                PF_INET: 客户端和服务端在 IPV4 网络中
                PF_INET6: 客户端和服务端在 IPV6 网络中
            * type: 类型
            * protocol: 协议,默认是 0

        2. 套接字命名,创建套接字文件
           int bind(int fd, const struct sockaddr* address, size_t addressLen) 
               调用成功返回 0 ,失败返还 -1
            * fd: 将文件描述符 fd 关联到制定存储在 address 中的套接字地址
            * addresslen: 表示地质结构的长度
            * address: 值和类型取决于套接字的域
                PF_LOCAL: sockaddr_un (sockaddr_unix)结构的指针强制转换成 sockaddr*
                PF_INET: 指向 sockaddr_in (socket_inet)结构的指针必须强制转换成 sockaddr* 类型
            * sockaddr_un  #include <sys/un.h>
                struct  sockaddr_un{
                    unsigned short  sun_family;     //PF_LOCAL / PF_UNIX
                    char sun_path[108];             //套接字的路径,名称
                }
            * sockaddr_in #include <netinet/in.h>
               struct sockaddr_in{
                short int sin_family;  // PF_INET
                unsigned short int sin_port;     //端口号，必须是网络字节顺序
                struct in_addr sin_addr;    //保存Internet地址的地址,in_addr类型的结构，必须是网络字节顺序
                unsigned char sin_zero;   //填0,和struct sockaddr一样大小
               }
        3. listen
            int listen(int fd,int  queueLength)
            服务器监听长度
                * fd: 套接字文件
                * queueLength: 监听队列的长度 

        4. accept
            int accept(int fd, struct sockaddr* address, int* addressLen)
            * fd: 监听 fd 指向的文件描述符的命名套接字，知道客户端连接到达
                    收到请求之后，accept()创建一个未命名套接字，其属性和原来的服
                    务器命名套接字相同，把该未命名套接字连接到客户端套接字，并返
                    回一个新的文件描述符，用于和客户端的通信
                    原来的服务器命名套接字继续接受其他客户端的连接请求。
                    调用失败返回 -1
            * address: 填写客户端的地址
            * addresslen: address 结构体的大小
        5. connect
            int connect(int fd,struct sockaddr* address, int addressLen) 
            调用成功返回0，失败返回 -1,客户端使用 fd 和服务器的 socket 通信
    5. Internet 上的 socket 连接通信
        1. PF_INET: 
            * 32位的 IP 地址
            * 16位的 port

 */

int main(int argc, char* argv)
{
    return 0;
}
