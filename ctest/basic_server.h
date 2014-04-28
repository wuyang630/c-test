#ifdef __BASIC_SERVER_H__
#undef __BASIC_SERVER_H__

class BasicServer
{
	public:
		BasicServer(){};
		BasicServer(uint16_t _port):local_port(_port){};
		virtual int server_init();
		virtual int get_req() = 0;
		virtual int handle_msg() = 0;
		virtual int send_rsp() = 0;
	private:
		uint16_t local_port;
};


#endif
