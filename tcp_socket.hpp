#pragma once
#include<cstdio>
#include<cstring>
#include<string>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

class TcpSocket
{
public:
    TcpSocket()
        :_fd(-1)
    {}

    TcpSocket(int fd)
      :_fd(fd)
    {}

    bool Socket()
    {
        //和UDP 不同的是第二个参数
        _fd = socket(AF_INET, SOCK_STREAM, 0);
        if(_fd < 0)
        {
            perror("socket");
            return false;
        }
        return true;
    }
    //给服务器使用
    bool Bind(const std::string& ip, uint16_t port)
    {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);
        int ret = bind(_fd, (sockaddr*)&addr, sizeof(addr));
        if(ret < 0)
        {
            perror("bind");
            return false;
        }

        return true;
    }
    //给服务器使用
    bool Listen()
    {
        //listen进入监听状态
        int ret = listen(_fd, 10);
        if(ret < 0)
        {
            perror("listen");
            return false;
        }
        return true;
    }
    //给服务器使用
    bool Accept(TcpSocket* peer, std::string* ip = NULL, uint16_t* port = NULL)
    {
        //accept 从连接队列中取一个连接到用户代码中
        //如果队列中没有连接，就会阻塞（默认行为）
        sockaddr_in peer_addr;
        socklen_t len = sizeof(peer);
        //返回值也是一个 socket
        int client_sock = accept(_fd, (sockaddr*)&peer_addr, &len);
        if(client_sock < 0)
        {
            perror("accept");
            return false;
        }
        peer->_fd = client_sock;
        if(ip != NULL)
            *ip = inet_ntoa(peer_addr.sin_addr);
        if(port != NULL)
            *port = ntohs(peer_addr.sin_port);

        return true;
    }
    //给服务器和客户端使用
    int Recv(std::string* msg)
    {
        msg->clear();
        char buf[1024 * 10] = {0};
        ssize_t n = recv(_fd, buf, sizeof(buf) - 1, 0);
        //recv的返回值：如果读取成功，返回结果为读到的字节数
        //如果读取失败，返回结果为-1
        //如果对段关闭了 socket 返回结果为0
        //buf[n] = '\0';
        if(n < 0)
        {
            perror("recv");
            return -1;
        }
        else if(n == 0)
        {
            //需要考虑到返回0 的情况
            return 0;                       
        }

        msg->assign(buf);
        
        return 1;
    }
    //给服务器和客户端使用
    bool Send(const std::string& msg)
    {
        ssize_t n = send(_fd, msg.c_str(), msg.size(), 0);
        if(n < 0)
        {
            perror("send");
            return false;
        }

        return true;
    }
    //给客户端使用
    bool Connect(const std::string& ip, uint16_t port)
    {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);
        int ret = connect(_fd, (sockaddr*)&addr, sizeof(addr));
        if(ret < 0)
        {
            perror("connet");
            return false;
        }

        return true;
    }
    bool Close()
    {
        if(_fd != -1)
            close(_fd);

        return true;
    }

    int GetFd()const 
    {
      return _fd;
    }

private:
    int _fd;
};

