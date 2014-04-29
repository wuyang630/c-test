#include "basic_server.h"
#include <sys/epoll.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

enum EServerType
{
	kUdpServer = 1,
	kTcpServer = 2,
};

class EchoClient : public BasicServer
{
	public:
		EchoClient( EServerType _type):server_type(_type)
		{
		}
		int server_init();
		int get_req();
		int handle_msg(int fd);
		int send_rsp();
		int on_loop();
	private:
		EServerType server_type;
		int listened_fd;
		int epoll_fd ;
		struct epoll_event events[65535];
};


int EchoClient::get_req()
{
	int nfd = epoll_wait(epoll_fd,events,sizeof(events)/sizeof(events[0]),1);
	if(nfd<0)
	{
		printf("epoll_wait failed! ret=%d errno=%d\n",nfd,errno);
	}
	for ( int i = 0 ; i < nfd ; i ++) 
	{
		if( (events[i].events&EPOLLIN) || (events[i].events&EPOLLPRI) )
		{
			handle_msg(events[i].data.fd);
		}
		else if(events[i].events&EPOLLOUT)
		{
			send_rsp();
		}
		else
		{
			printf("epoll event[%u] get %u\n",i,events[i].events);
		}
	}
	return 0;
}

int EchoClient::handle_msg(int fd)
{
	char cBuf[65535] = {0};
	if(fd == STDIN_FILENO)
	{
		scanf("%s\n",cBuf);	
		printf("%s\n",cBuf);
	}
	return 0;
}

int EchoClient::send_rsp()
{
	return 0;
}


int EchoClient::server_init()
{
	int ret = 0;
	int fd = 0;

	if(server_type == kUdpServer)
	{
		fd = socket(AF_INET,SOCK_DGRAM,0);
	}
	else if(server_type == kTcpServer)
	{
		fd = socket(AF_INET,SOCK_STREAM,0);
	}
	else
	{
		fd = socket(AF_INET,SOCK_RAW,0);
	}

	if( fd < 0 )
	{
		printf("make socket failed! ret=%d errno=%d\n",fd,errno);
		return -1;
	}

	// Set ReuseAddr
	
	// BindServer
	/*
	struct sockaddr_in srv_addr;
	memset(&srv_addr,0,sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_addr.sin_port = htons(local_port);
	
	ret = bind(fd,(struct sockaddr *)&srv_addr,sizeof(srv_addr));
	if(ret < 0)
	{
		printf("bind port %u failed! ret=%d errno=%d\n",local_port,ret,errno);
		return -1;
	}

	if (server_type == kTcpServer)
	{
		ret = listen(fd,atoi(getenv("LISTENQ")));
		if(ret < 0)
		{
			printf("listen failed! ret=%d errno=%d\n",ret,errno);
			return -1;
		}
	}
	*/

	// Set Nonblocking

	epoll_fd = epoll_create(255);
	if ( epoll_fd < 0 )
	{
		printf("epoll_create failded! ret=%d errno=%d\n",epoll_fd,errno);
		return -1;
	}
	
	struct epoll_event event;
	event.events = EPOLLIN|EPOLLPRI;	
	event.data.fd = fd; 
	ret = epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&event);
	if( ret < 0)
	{
		printf("epoll_ctl failed! ret=%d errno=%d\n",ret,errno);
		return -1;
	}

	event.events = EPOLLIN|EPOLLPRI;	
	event.data.fd = STDIN_FILENO; 
	ret = epoll_ctl(epoll_fd,EPOLL_CTL_ADD,STDIN_FILENO,&event);
	if( ret < 0)
	{
		printf("epoll_ctl failed! ret=%d errno=%d\n",ret,errno);
		return -1;
	}

	return 0;
}

int EchoClient::on_loop()
{
	return 0;
}

int main()
{
	EchoClient client(kUdpServer);
	client.server_start();
	return 0;
}
