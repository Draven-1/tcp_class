#pragma once
#include<pthread.h>
#include<thread>
#include<functional>
#include"tcp_socket.hpp"

#define CHECK_RET(exp) if(!(exp)) { return false; }

typedef std::function<void(const std::string&, std::string*)> Handler;

class TcpThreadServer
{
public:
    TcpThreadServer()
    {}

    ~TcpThreadServer()
    {
        _listen_sock.Close();       
    }

    bool Start(const std::string& ip, uint16_t port, Handler handler)
    {
        //1.创建socket
        CHECK_RET(_listen_sock.Socket());
        //2.绑定端口号
        CHECK_RET(_listen_sock.Bind(ip, port));
        //3.监听
        CHECK_RET(_listen_sock.Listen());
        //4.进入主循环
        while(true)
        {
            //5.调用accept
            TcpSocket client_sock;
            std::string peer_ip;
            uint16_t peer_port;
            bool ret = _listen_sock.Accept(&client_sock, &peer_ip, &peer_port);
            if(!ret)
                continue;

            printf("[%s:%d] 客户端连接!\n", peer_ip.c_str(), peer_port);
            //6.创建线程处理客户端的逻辑
            ProcessConnect(client_sock, peer_ip, peer_port, handler);
        }

    }

private:
    TcpSocket _listen_sock;

    struct ThreadEntryArg
    {
        TcpSocket client_sock;
        std::string ip;
        uint16_t port;
        Handler handler;
    };

    void ProcessConnect(TcpSocket& client_sock, const std::string& ip, 
            uint16_t port, Handler handler)
    {
        //1.创建线程
        pthread_t tid;

        //得在堆上申请,不然下面的函数将无法使用 要记得delete
        //如果在栈上,下面函数访问会产生未知的不确定行为
        ThreadEntryArg* arg = new ThreadEntryArg;
        arg->client_sock = client_sock;
        arg->ip = ip;
        arg->port = port;
        arg->handler = handler;
        pthread_create(&tid, NULL, ThreadEntry,(void*)arg);
        //2.对于主线程来说,让函数立即返回
        pthread_detach(tid);
        //3.对于新线程来说,循环处理客户端的操作
    }

    static void* ThreadEntry(void* arg)
    {
        ThreadEntryArg* argument = (ThreadEntryArg*)arg;
        TcpSocket& client_sock = argument->client_sock;
        std::string ip = argument->ip;
        uint16_t port = argument->port;
        Handler handler = argument->handler;

        while(true)
        {
            //  a)读取客户端请求
            std::string req;
            int ret = client_sock.Recv(&req);
            if(ret < 0)
                continue;

            if(ret == 0)
            {
                //客户端断开
                client_sock.Close();
                printf("[%s:%d] 客户端断开连接!\n", ip.c_str(), port);
                break;
            }
            //  b)根据请求计算响应        
            printf("[%s:%d] 客户端请求 %s\n", ip.c_str(), port, req.c_str());
            std::string resp;
            handler(req, &resp);
            //  c)把响应写回客户端    
            client_sock.Send(resp);
        }   
        //delete
        delete argument;
    }

};
