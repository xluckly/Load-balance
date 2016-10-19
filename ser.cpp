#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <unistd.h>

#define MAX_EVENT_NUMBER 1024
#define MAX_SIZE 1024

int setnonblocking(int fd)
{
    int oldop = fcntl(fd,F_GETFL);
    int newop = oldop | O_NONBLOCK;
    fcntl(fd,F_SETFL,newop);
    return oldop;
}

void addfd(int epollfd,int fd,bool enable_et)
{
    epoll_event events;
    events.data.fd = fd;
    events.events = EPOLLIN;
    if(enable_et)
    {
        events.events |= EPOLLET;
    }
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&events);
    setnonblocking(fd);
}

void lt(epoll_event *events,int number,int epollfd,int sockfd)
{
    char buf[MAX_SIZE];
    for(int i = 0; i < number; ++i)
    {
        int fd = events[i].data.fd;
        if(fd == sockfd)
        {
            struct sockaddr_in cli_addr;
            socklen_t cli_size = sizeof(cli_addr);
            int connfd = accept(sockfd,(struct sockaddr*)&cli_addr,&cli_size);
            assert(connfd != -1);
            addfd(epollfd,connfd,false);
        }
        else if(events[i].events & EPOLLIN)
        {
            std::cout<<"event trigger once\n";
            memset(buf,'\0',sizeof(MAX_SIZE));
            int ret = recv(fd,buf,MAX_SIZE-1,0);
            if(ret <= 0)
            {
                close(sockfd);
                continue;
            }
            buf[ret+1] = '\0';
            std::cout<<buf<<"\n";
        }
        else 
        {
            printf("something happen\n");
        }
    }
}

void et(epoll_event *events,int number,int epollfd,int sockfd)
{
    char buf[MAX_SIZE];
    bzero(buf,MAX_SIZE);
    for(int i = 0; i < number; ++i)
    {
        int fd = events[i].data.fd;
        if(fd == sockfd)
        {
            struct sockaddr_in cli_addr;
            socklen_t cli_size = sizeof(cli_addr);
            int connfd = accept(sockfd,(struct sockaddr*)&cli_addr,&cli_size);
            assert(connfd != -1);
            addfd(epollfd,connfd,true);
        }
        else if(events[i].events & EPOLLIN)
        {
            while(1)
            {
                memset(buf,'\0',MAX_SIZE);
                int ret = recv(fd,buf,MAX_SIZE-1,0);
                if(ret < 0)
                {
                    if(errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        //std::cout<<"read later\n";
                        break;
                    }
                    close(fd);
                    break;
                }
                else if(ret == 0)
                {
                    close(fd);
                }
                else
                {
                    printf("%s\n",buf);
                    send(fd,"OK",2,0);
                }

            }
        }
    }
}


int main(int argc,char *argv[])
{
    if(argc <= 2)
    {
        std::cout<<"argc errno\n";
        return 1;
    }
    int port = atoi(argv[2]);
    char *ip = argv[1];

    struct sockaddr_in srv_addr;
    bzero(&srv_addr,sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(port);
    inet_pton(AF_INET,ip,&srv_addr.sin_addr);

    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    assert(sockfd >= 0);
    int ret = bind(sockfd,(struct sockaddr*)&srv_addr,sizeof(srv_addr));
    assert(ret != -1);

    ret = listen(sockfd,5);
    assert(ret != -1);

    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd,sockfd,true);
    
    while(1)
    {
        int ret = epoll_wait(epollfd,events,MAX_EVENT_NUMBER,-1);
        if(ret < 0)
        {
            std::cout<<"epoll faile\n";
            break;
        }
        et(events,ret,epollfd,sockfd);
    }
    close(sockfd);
    return 0;
}
