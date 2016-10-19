#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;
const int MAXSIZE = 1024;

int main(int argc,char *argv[])
{
    if(argc <= 2)
    {
        printf("usage : %s ip_address port_number\n",basename(argv[0]));
        return 1;
    }
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    char buf[MAXSIZE];
    memset(buf,'\0',MAXSIZE);
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    assert(sockfd >= 0);
    if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
    {
        std::cout<<"the connect failed\n";
        close(sockfd);
        return 1;
    }
    int ret = -1;
    while(1)
    {
        fgets(buf,MAXSIZE-1,stdin);
        send(sockfd,buf,strlen(buf),0);
        memset(buf,'\0',sizeof(buf));
        ret = recv(sockfd,buf,MAXSIZE-1,0);
        if(ret < 0)
        {
            std::cout<<"the serve close\n" ;
            break;
        }
        if(ret == 0)
        
            continue;
        
        std::cout<<buf<<"\n";

    }
close(sockfd);
return 0;
    
}
