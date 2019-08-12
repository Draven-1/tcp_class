#pragma once

#include<signal.h>
#include<functional>
#include"tcp_socket.hpp"

#define CHECK_RET(exp) if(!(exp)){return false;}

typedef std::function<void (const std::string&, std::string*)> Handler;

class TcpProcessServer
{
public:
    TcpProcessServer()
    {}

    ~TcpProcessServer()
    {
        _listen_sock.Close();
    }

    bool Start(const std::string& ip, uint16_t port, Handler handler)
    {
        signal(SIGCLD, SIG_IGN); //防止客户端关闭时,出现僵尸进程

        //1.创建socket
        CHECK_RET(_listen_sock.Socket());
        //2.绑定端口号
        CHECK_RET(_listen_sock.Bind(ip, port));
        //3.监听
        CHECK_RET(_listen_sock.Listen());
        //4.进入主循环
        while(true)
        {
            //5.调用Accept
            TcpSocket client_sock;
            std::string peer_ip;
            uint16_t peer_port;
            bool ret = _listen_sock.Accept(&client_sock, &peer_ip, &peer_port);
            if(!ret)
                continue;

            printf("[%s:%d] 客户端建立连接!\n", peer_ip.c_str(), peer_port);
            //6.创建子进程，让子进程去处理客户端的请求，
            //父进程继续调用Accept

            ProcessConnect(client_sock, peer_ip, peer_port, handler);
        }
    }
    
private:
    TcpSocket _listen_sock;
    void ProcessConnect(TcpSocket& client_sock, const std::string&ip, 
            uint16_t port, Handler handler)
    {
        //1.创建子进程
        pid_t ret = fork();
        //2.父进程直接结束这个函数
        if(ret > 0)
        {
            //父进程
            //父进程也需要关闭这个socket
            //否则就会出现文件描述符泄露
            client_sock.Close();
            return;
        }
        //3.子进程循环的做以下事情
        //  a) 读取客户端请求
        while(true)
        {
            //再让子进程创建一个孙子进程,孙子进程去轮回读取,子进程立即返回,
            //父进程等待回收.
            //这样孙子进程就会成为孤儿进程,孙子进程结束的时候,1号进程回收.
            //或者Start函数一开始就利用信号也可以解决僵尸进程问题.

            std::string req;
            int r = client_sock.Recv(&req);
            if(r < 0)
                continue;
            if(r == 0)
            {
                printf("[%s:%d] 客户端断开连接!\n", ip.c_str(), port);
                break;
            }
            printf("[%s:%d] 客户端发送了:%s\n", ip.c_str(), port, req.c_str());
            //  b) 根据请求计算响应
            std::string resp;
            handler(req, &resp);
            //  c) 把响应写回给客户端
            client_sock.Send(resp);
        }
        //子进程收尾工作
        //1.关闭socket
        client_sock.Close();
        //2.结束进程
        exit(0);
    }
};
