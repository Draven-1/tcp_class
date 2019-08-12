//通用的TCP服务器
//
#pragma once
#include<functional>
#include"tcp_socket.hpp"

#define CHECK_RET(exp) if(!(exp)){ return false; }

typedef std::function<void (const std::string&, std::string*)> Handler;
//typedef std::function<void (const Requset& req, const Response* resp)> Handler;

class TcpServer
{
public:
    TcpServer()
    {}
    bool Start(const std::string& ip, uint16_t port, Handler handler)
    {
        //1.创建一个socket
        CHECK_RET(_listen_sock.Socket());

        //2.绑定端口号
        CHECK_RET(_listen_sock.Bind(ip, port));

        //3.进行监听
        CHECK_RET(_listen_sock.Listen());
        printf("Server Start OK\n");
        
        //4进入主循环
        while(true)
        {
            //5.通过accept 获取到一个连接
            TcpSocket client_sock;
            std::string ip;
            uint16_t port;
            bool ret = _listen_sock.Accept(&client_sock, &ip, &port);
            if(!ret)
            {
                continue;
            }
            printf("[%s:%d] 有客户端连接!\n", ip.c_str(), port);
            //6.和客户端进行具体的沟通,一次连接和客户端进行多次沟通
            while(true)
            {
                // a）读取请求
                std::string req;
                int r = client_sock.Recv(&req);
                if(r < 0)
                    continue;
                if(r == 0)
                {
                    //对端关闭了 socket
                    client_sock.Close();
                    printf("[%s:%d] 对端关闭了连接\n", ip.c_str(), port);
                    break;
                }
                printf("[%s:%d] 客户端发送了:%s\n", ip.c_str(), port, req.c_str());
                // b）根据请求计算响应
                std::string resp;
                handler(req, &resp);
                // c）把响应写回客户端
                client_sock.Send(resp);
            }
        }
    }
private:
    TcpSocket _listen_sock;
};

