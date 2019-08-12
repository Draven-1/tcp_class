#include<sstream>
#include"tcp_thread_server.hpp"

int main()
{
    TcpThreadServer server;
    server.Start("0.0.0.0", 9090, [](const std::string& req, std::string* resp)
            {
                //1.反序列化(空格为分隔符)
                std::stringstream ss(req);
                int num1 = 0;
                int num2 = 0;
                std::string op;
                ss >> num1;
                ss >> num2;
                ss >> op;
                printf("req: %d, %d, %s\n", num1, num2, op.c_str());
                //2.进行计算
                int ret = 0;
                if(op == "+")
                    ret = num1 + num2;
                else if(op == "-")
                    ret= num1 - num2;
                    
                //3.把结果再序列花
                //*resp = std::to_string(ret);
                std::stringstream ss_resp;
                ss_resp << ret;
                *resp = ss_resp.str();
            });
}
