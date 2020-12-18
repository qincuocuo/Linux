#include "tcpsocket.hpp"

int main()
{
    TcpSocket lst_sock;//监听套接字
    //1. 创建套接字
    CHECK_RET(lst_sock.Socket());
    //2. 绑定地址信息
    CHECK_RET(lst_sock.Bind("0.0.0.0", 9000));
    //3. 开始监听
    CHECK_RET(lst_sock.Listen());
    while(1) {
    //4. 获取新建连接
        TcpSocket con_sock;//与指定客户端通信的套接字
        std::string cli_ip;
        int cli_port;
        bool ret=lst_sock.Accept(&con_sock,&cli_ip,&cli_port);
        if (ret == false) {
            continue;
        }
    //5. 收发数据
        std::string buf;
        ret = con_sock.Recv(&buf);
        if (ret == false) {
            con_sock.Close();
            continue;
        }
        printf("%s:%d say:%s\n", cli_ip.c_str(), 
                cli_port, buf.c_str());

        std::cout << "server say:";
        std::cin >> buf;
        ret = con_sock.Send(buf);
        if (ret == false) {
            con_sock.Close();
        }
    }
    //6. 关闭套接字
    lst_sock.Close();
    return 0;
}
