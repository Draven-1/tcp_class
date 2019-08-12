#include"tcp_client.hpp"


int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        printf("Usage: ./dict_client [ip]\n");
        return 1;
    }
    TcpClient client;
    bool ret = client.Connect(argv[1], 9090);
    if(!ret)
    {
        printf("connect faild!\n");
        return 1;
    }
    while(true)
    {
        //从标准输入读数据
        printf("请输入一个要查询的单词：");
        fflush(stdout);
        char req[1024] = {0};
        scanf("%s", req);
        //把读到的数据发送给服务器
        client.Send(req);
        //读取服务器的响应结果
        std::string resp;
        client.Recv(&resp);
        //把响应结果显示到标准输出上
        printf("resp:%s\n", resp.c_str());
    }
}
