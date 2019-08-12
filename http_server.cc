#include <string>
#include <cstdlib>
#include "tcp_epool_server.hpp"
//#include"tcp_thread_server.hpp"

void HttpProcess(const std::string& req, std::string* resp)
{
    //不管用户输入的是啥请求,都返回hello world
    (void) req;
    
    std::string first_line = "HTTP/1.1 200 OK\n";
    std::string body = "<html><div>hello world</div></html>";
    std::string header = "Content-Type: text/html\nContent-Length: " +
    std::to_string(body.size()) + "\n";
    

    /*
    std::string first_line = "HTTP/1.1 302 Found\n";
    std::string header = "Location: http://www.sogou.com\n"
        "Content-Type: text/html\n"
        "Content-Length: 10\n";
    std::string body = "aaaaaaaaaa";
    */ 
        
    *resp = first_line + header + "\n" + body;
}

//void HttpProcess(const Request& req, Response* resp)
//{
//  
//}

int main(int argc, char* argv[])
{
    //TcpThreadServer server;
    if(argc != 3)
    {
      printf("Usage:./http_server ip port");
      return 1;
    }
    TcpEpollServer server;
    std::string ip = argv[1];
    int port = atoi(argv[2]);
    server.Start(ip, port, HttpProcess);
    server.~TcpEpollServer();
    return 0;
}
