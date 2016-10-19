#include<iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

using namespace std;
const int MAXSIZE = 1024;

class Balance
{
    private:
    int socked;
    int sock_load;
    struct sockaddr_in cli_addr;
    struct sockaddr_in ser_addr;
    
    public:

    Balance(const struct sockaddr_in &addr)
    {
        ser_addr.sin_family = addr.sin_family;
        ser_addr.sin_port = addr.sin_port;
        ser_addr.sin_addr.s_addr = addr.sin_addr.s_addr;
        socked = socket(ser_addr.sin_family,SOCK_STREAM, 0);
    }
    ~Balance()
    {
        close(socked);
    }

    bool Connect()
    {
        sock_load = socket(AF_INET,SOCK_STREAM,0);
        struct sockaddr_in tem_addr;
        tem_addr.sin_family = AF_INET;
        tem_addr.sin_port = htons(6553);
        tem_addr.sin_addr.s_addr = inet_addr("192.168.72.134");
        int c = connect(sock_load,(struct sockaddr*)&tem_addr,sizeof(tem_addr));
        if(c == -1)
        {
            perror("connect serve error");
            exit(-1);
        }
        return true;
    }
    bool Bind()
    {
        int a = bind(socked, (struct sockaddr*)&ser_addr,sizeof(ser_addr));
        if(a == -1)
        {
            perror("bind error");
            return false;
        }
        return true;
    }

    bool Listen(int backlg)
    {
        int a = listen(socked,backlg);
        if(a == -1)
        {
            perror("listen failed");
            return false;
        }
        return true;
    }

    int  Accept()
    {
        int len = sizeof(cli_addr);
        int a = accept(socked,(struct sockaddr*)&cli_addr,(socklen_t*)&len);
        if(a == -1)
        {
            perror("accept failed");
            exit(-1);
        }
        return a;
    }
    int  recive_client(int a)
    {
        int result = -1;
        memset(cli_buf,'\0',1024);
        result = recv(a,cli_buf,1023,0);
        return result;
    } 

    int send_client(){}
    int recive_serve(){}
    int send_serve()
    {
        send(sock_load,cli_buf,strlen(cli_buf),0);
    }
    public:
        char cli_buf[MAXSIZE];
        char serve_buf[MAXSIZE];
};

int main()
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(65533);
    addr.sin_addr.s_addr = inet_addr("192.168.72.134");
    Balance ba(addr);
    
    ba.Connect();
    
    ba.Bind();
    ba.Listen(5);
    while(1)
    {
        int a = ba.Accept();
        while(ba.recive_client(a) > 0)
        {
            cout<<ba.cli_buf<<"\n";
            ba.send_serve();
        }
    }
    return 0;
}

