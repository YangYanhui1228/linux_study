
#include <cstdio>
#include <unistd.h>
#include <ctype.h>
#include <iostream>
using namespace std;


#include <fcntl.h>
#include <sys/file.h>
#include <errno.h>//strerror
#include <string.h>
#include <sys/stat.h>

void lesson01()
{
	const char* str = "hello world!\r\n1981-12-08\r\n冯攀\r\n";
	size_t len = sizeof(str);
	for (size_t i = 0; i < len; i++) {
		if (isalnum(str[i]))std::cout << "A" << std::endl;
		if (isalpha(str[i]))std::cout << "B" << std::endl;
	}
}


void lesson_io_7()
{
	//int fd = open("/home/lxx/test_io_7.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	int fd = open("/home/lxx/test_io_7.txt", O_RDWR | O_CREAT, 0755);//权限是八进制的
	if (fd >= 0) {
		printf("flock: %d", flock(fd, LOCK_EX));
		printf("write %d\n", write(fd, "hello", 5));
		sleep(6);
		flock(fd, LOCK_UN);
		close(fd);
	}
	else {
		std::cout << strerror(errno) << std::endl;
	}
}

#include <assert.h>
void lesson33_exit()
{
	printf("%s\n", __FUNCTION__);
}
void lesson33_on_exit(int status, void* p) {
	printf("%s p=%p status=%d\n", __FUNCTION__, p, status);
}
void lesson33()
{
	pid_t pid = fork();
	if (pid > 0) {//父进程
		atexit(lesson33_exit);
		//sleep(1);
		std::cout << "hello, here is parent!\n" << pid << std::endl;
		abort();
		exit(0);
	}
	else {//子进程
		on_exit(lesson33_on_exit, (void*)1);
		sleep(3);
		assert(0);
		_exit(-1);//不会触发atexit或者on_exit
		//execl("/bin/ls", "ls", NULL);//argv的第一个参数，一定是命令本身
		//assert(0);
		//execl("/bin/ls", "ls", "-l", NULL);//argv的第一个参数，一定是命令本身
	}
}

#include <setjmp.h>
#include <signal.h>
jmp_buf jmpbuf;
void test002()
{
	longjmp(jmpbuf, 1);
}
void test003()
{
	longjmp(jmpbuf, 2);
}
void test001()
{
	test002();
}
void signal_deal(int sig) {
	if (sig == SIGSEGV) {
		longjmp(jmpbuf, SIGSEGV);
	}
}
void lesson34()
{
	signal(SIGSEGV, signal_deal);//异常捕获
	//struct sigaction act,actold;
	//act.sa_restorer
	//sigaction(SIGSEGV, &act, actold);
	int ret = setjmp(jmpbuf);
	if (ret == 0) {
		test001();
		*(int*)(NULL) = 0;
	}
	else if (ret == 1) {
		printf("error 1\n");
	}
	else if (ret == 2) {
		printf("error 2\n");
	}
	else if (ret == SIGSEGV) {//错误2的处理和恢复
		printf("error SIGSEGV\n");
	}
}

#include <sys/socket.h>
#include <arpa/inet.h>

void lesson57()
{
	int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock != -1) {
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr("0.0.0.0");
		addr.sin_port = htons(9527);
		int ret = bind(sock, (sockaddr*)&addr, sizeof(addr));
		if (ret != 0) {
			//报错在此

		}
	}
}


void lesson60()
{
	int server, client;
	struct sockaddr_in seraddr, cliaddr;
	socklen_t cliaddrlen;
	const char* message = "hello world!\n";
	server = socket(PF_INET, SOCK_STREAM, 0);//TCP
	if (server < 0) {
		std::cout << "create socket failed!" << std::endl;
		return;
	}
	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	seraddr.sin_port = htons(9527);
	int ret = bind(server, (sockaddr*)&seraddr, sizeof(seraddr));
	if (ret == -1) {
		std::cout << strerror(errno) << std::endl;
		std::cout << "bind failed!" << std::endl;
		close(server);
		return;
	}
	ret = listen(server, 3);
	if (ret == -1) {
		std::cout << "listen failed!" << std::endl;
		close(server);
		return;
	}
	printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
	client = accept(server, (sockaddr*)&cliaddr, &cliaddrlen);
	if (client == -1) {
		std::cout << "accept failed!" << std::endl;
		close(server);
		return;
	}
	printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
	ssize_t len = write(client, message, strlen(message));
	if (len != (ssize_t)strlen(message)) {
		std::cout << "write failed!" << std::endl;
		close(server);
		return;
	}
	printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
	close(client);//可以不执行
	close(server);//因为服务端关闭时，客户端会自动关闭
	printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
}

#include <sys/wait.h>
void lesson62()
{
	pid_t pid = fork();
	if (pid == 0) {
		//开启客户端 子进程
		sleep(1);
		int client = socket(PF_INET, SOCK_STREAM, 0);
		struct sockaddr_in seraddr;
		memset(&seraddr, 0, sizeof(seraddr));
		seraddr.sin_family = AF_INET;
		seraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
		seraddr.sin_port = htons(9527);
		int ret = connect(client, (sockaddr*)&seraddr, sizeof(seraddr));
		if (ret == 0) {
			printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
			char buffer[256] = "";
			read(client, buffer, sizeof(buffer));
			std::cout << "client recv " << buffer;
		}
		else {
			printf("%s(%d):%s %d\n", __FILE__, __LINE__, __FUNCTION__, ret);
		}
		close(client);
		std::cout << "client done!" << std::endl;
	}
	else if (pid > 0) {
		lesson60();
		int status = 0;
		wait(&status);
	}
	else {
		std::cout << "fork failed! " << pid << std::endl;
	}
}

void run_client()
{
	int client = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in seraddr;
	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	seraddr.sin_port = htons(9527);
	int ret = connect(client, (sockaddr*)&seraddr, sizeof(seraddr));
	if (ret == 0) {
		printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
		char buffer[256] = "hello, here is client!\n";
		write(client, buffer, strlen(buffer));
		memset(buffer, 0, sizeof(buffer));
		read(client, buffer, sizeof(buffer));
		std::cout << buffer;
	}
	else {
		printf("%s(%d):%s %d\n", __FILE__, __LINE__, __FUNCTION__, ret);
	}
	close(client);
	std::cout << "client done!" << std::endl;
}

void lesson63_()
{
	int server, client;
	struct sockaddr_in seraddr, cliaddr;
	socklen_t cliaddrlen;
	//const char* message = "hello world!\n";
	server = socket(PF_INET, SOCK_STREAM, 0);//TCP
	if (server < 0) {
		std::cout << "create socket failed!" << std::endl;
		return;
	}
	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	seraddr.sin_port = htons(9527);
	int ret = bind(server, (sockaddr*)&seraddr, sizeof(seraddr));
	if (ret == -1) {
		std::cout << strerror(errno) << std::endl;
		std::cout << "bind failed!" << std::endl;
		close(server);
		return;
	}
	ret = listen(server, 3);
	if (ret == -1) {
		std::cout << "listen failed!" << std::endl;
		close(server);
		return;
	}
	printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
	char buffer[1024];
	while (1) {
		client = accept(server, (sockaddr*)&cliaddr, &cliaddrlen);
		if (client == -1) {
			std::cout << "accept failed!" << std::endl;
			close(server);
			return;
		}
		printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
		read(client, buffer, sizeof(buffer));
		ssize_t len = write(client, buffer, strlen(buffer));
		if (len != (ssize_t)strlen(buffer)) {
			std::cout << "write failed!" << std::endl;
			close(server);
			return;
		}
		else {
			std::cout << "server recv: " << buffer << std::endl;
		}
		printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
		close(client);//可以不执行
	}
	close(server);//因为服务端关闭时，客户端会自动关闭
	printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
}
void lesson63()
{
	pid_t pid = fork();
	if (pid == 0) {
		//开启客户端 子进程
		sleep(1);
		run_client();
		run_client();

	}
	else if (pid > 0) {
		lesson63_();
		int status = 0;
		wait(&status);
	}
	else {
		std::cout << "fork failed! " << pid << std::endl;
	}

}

void run_client64()
{
	int client = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in seraddr;
	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	seraddr.sin_port = htons(9527);
	int ret = connect(client, (sockaddr*)&seraddr, sizeof(seraddr));
	while (ret == 0) {
		printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
		char buffer[256] = "";
		fputs("Input message(Q to quit):", stdout);
		fgets(buffer, sizeof(buffer), stdin);
		if ((strcmp(buffer, "q\n") == 0) || (strcmp(buffer, "Q\n") == 0)) {
			break;
		}
		size_t len = strlen(buffer);
		size_t send_len = 0;
		while (send_len < len) {
			ssize_t ret = write(client, buffer + send_len, len - send_len);
			if (ret <= 0) {
				fputs("write failed\n", stdout);
				close(client);
				std::cout << "client done!" << std::endl;
				return;
			}
			send_len += (ssize_t)ret;
		}
		memset(buffer, 0, sizeof(buffer));
		size_t read_len = 0;
		while (read_len < len) {
			ssize_t ret = read(client, buffer + read_len, len - read_len);
			if (ret < (ssize_t)len) {
				fputs("read failed\n", stdout);
				close(client);
				std::cout << "client done!" << std::endl;
				return;
			}
			read_len += (ssize_t)ret;
		}
		std::cout << "from server:" << buffer;
	}
	close(client);
	std::cout << "client done!" << std::endl;
}

void server64()
{
	int server, client;
	struct sockaddr_in seraddr, cliaddr;
	socklen_t cliaddrlen;
	//const char* message = "hello world!\n";
	server = socket(PF_INET, SOCK_STREAM, 0);//TCP
	if (server < 0) {
		std::cout << "create socket failed!" << std::endl;
		return;
	}
	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	seraddr.sin_port = htons(9527);
	int ret = bind(server, (sockaddr*)&seraddr, sizeof(seraddr));
	if (ret == -1) {
		std::cout << strerror(errno) << std::endl;
		std::cout << "bind failed!" << std::endl;
		close(server);
		return;
	}
	ret = listen(server, 3);
	if (ret == -1) {
		std::cout << "listen failed!" << std::endl;
		close(server);
		return;
	}
	printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
	char buffer[1024];
	for (int i = 0; i < 2; i++) {
		client = accept(server, (sockaddr*)&cliaddr, &cliaddrlen);
		if (client == -1) {
			std::cout << "accept failed!" << std::endl;
			close(server);
			return;
		}
		printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
		//ssize_t len = read(client, buffer, sizeof(buffer));
		ssize_t len = 0;
		while ((len = read(client, buffer, sizeof(buffer))) > 0) {
			len = write(client, buffer, len);
			if (len != (ssize_t)strlen(buffer)) {
				std::cout << "write failed!len:" << len << "buffer:" << buffer << std::endl;
				close(server);
				return;
			}
			memset(buffer, 0, len);
		}
		printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
		close(client);//可以不执行
	}
	close(server);//因为服务端关闭时，客户端会自动关闭
	printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
}


void lesson64()
{
	pid_t pid = fork();
	if (pid == 0) {
		//开启服务器 子进程
		server64();
	}
	else if (pid > 0) {
		for (int i = 0; i < 2; i++)
			run_client64();
		int status = 0;
		wait(&status);
	}
	else {
		std::cout << "fork failed! " << pid << std::endl;
	}
}





void run_client66()
{
	int client = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in seraddr;
	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	seraddr.sin_port = htons(9527);
	int ret = connect(client, (sockaddr*)&seraddr, sizeof(seraddr));
	char buffer[1024] = "";

	while (ret == 0) {
		//printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);

		fputs("Operand count:", stdout);
		int opnd_cnt = 0;
		scanf("%d", &opnd_cnt);
		if (opnd_cnt <= 1 && opnd_cnt >= 256) {
			fputs("opnd_cnt error, too small or too big!\n", stdout);
			close(client);
			std::cout << "client done!" << opnd_cnt << std::endl;
			return;
		}
		buffer[0] = (char)opnd_cnt;//服务器此处要解释为无符号
		for (int i = 0; i < opnd_cnt; i++) {
			scanf("%d", buffer + 1 + i * 4);
		}
		fgetc(stdin);
		fputs("Operator:", stdout);
		buffer[1 + opnd_cnt * 4] = (char)fgetc(stdin);

		size_t len = opnd_cnt * 4 + 2;
		size_t send_len = 0;
		while (send_len < len) {
			ssize_t ret = write(client, buffer + send_len, len - send_len);
			if (ret <= 0) {
				fputs("write failed\n", stdout);
				close(client);
				std::cout << "client done!" << std::endl;
				return;
			}
			send_len += (ssize_t)ret;
		}
		memset(buffer, 0, sizeof(buffer));
		size_t read_len = 0;
		while (read_len < 4) {
			ssize_t ret = read(client, buffer + read_len, 4 - read_len);
			if (ret <= 0) {
				fputs("read failed\n", stdout);
				close(client);
				std::cout << "client done!" << std::endl;
				return;
			}
			read_len += (ssize_t)ret;
			printf("from server count:%d\n", ret);
		}
		printf("from server:%d\n", *(int*)buffer);
		break;
	}
	close(client);
	std::cout << "client done!" << std::endl;
}


int calculate(int count, int oprand[], char op)
{
	int result = 0;
	switch (op) {
	case '+':
		for (int i = 0; i < count; i++) {
			result += oprand[i];
		}
		break;
	case '-':
		result = 2 * oprand[0];
		for (int i = 0; i < count; i++) {
			result -= oprand[i];
		}
		break;
	case '*':
		result = 1;
		for (int i = 0; i < count; i++) {
			result *= oprand[i];
		}
		break;
	default:
		break;
	}
	return result;
}

void server66()
{
	int server, client;
	struct sockaddr_in seraddr, cliaddr;
	socklen_t cliaddrlen;
	//const char* message = "hello world!\n";
	server = socket(PF_INET, SOCK_STREAM, 0);//TCP
	if (server < 0) {
		std::cout << "create socket failed!" << std::endl;
		return;
	}
	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	seraddr.sin_port = htons(9527);
	int ret = ::bind(server, (sockaddr*)&seraddr, sizeof(seraddr));
	if (ret == -1) {
		std::cout << strerror(errno) << std::endl;
		std::cout << "bind failed!" << std::endl;
		close(server);
		return;
	}
	ret = listen(server, 3);
	if (ret == -1) {
		std::cout << "listen failed!" << std::endl;
		close(server);
		return;
	}
	printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
	char buffer[1024];
	client = accept(server, (sockaddr*)&cliaddr, &cliaddrlen);
	//std::cout << strerror(errno) << std::endl;
	if (client == -1) {
		std::cout << strerror(errno) << std::endl;
		std::cout << "accept failed!" << std::endl;
		close(server);
		return;
	}

	//printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
	//ssize_t len = read(client, buffer, sizeof(buffer));
	ssize_t len = 0;
	int result = 0;
	len = read(client, buffer, 1);
	if (len <= 0) {
		std::cout << "server read failed\n";
	}
	if (len > 0) {
		for (unsigned i = 0; i < ((unsigned)buffer[0] & 0xFF); i++)
			read(client, buffer + 1 + i * 4, 4);
		read(client, buffer + 1 + ((unsigned)buffer[0] & 0xFF) * 4, 1);
		result = calculate(((int)buffer[0] & 0xFF), (int*)(buffer + 1), buffer[1 + ((unsigned)buffer[0] & 0xFF) * 4]);
		write(client, &result, 4);
		std::cout << " result:" << result << std::endl;
	}
	//printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
	//read(client, buffer, 1);

	//close(client);//可以不执行
	close(server);//因为服务端关闭时，客户端会自动关闭
	printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
}


void lesson66()
{
	pid_t pid = fork();
	if (pid == 0) {
		//开启服务器 子进程
		server66();
	}
	else if (pid > 0) {
		run_client66();
		int status = 0;
		wait(&status);
	}
	else {
		std::cout << "fork failed! " << pid << std::endl;
	}
}

void handle_error(const char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}

int lesson73(int argc, char* argv[])
{
	int ser_sock = -1;
	char message[512] = "";
	sockaddr_in servaddr, clientaddr;
	socklen_t clientlen = 0;
	if (argc != 2) {
		printf("usage:%s <port>\n", argv[0]);
		handle_error("argement is error!");
	}

	ser_sock = socket(PF_INET, SOCK_DGRAM, 0);//UDP
	if (ser_sock == -1)
	{
		handle_error("create socket failed!");
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//0.0.0.0
	servaddr.sin_port = htons((short)atoi(argv[1]));

	if (bind(ser_sock, (sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		handle_error("bind failed!");
	}
	for (int i = 0; i < 10; i++) {
		clientlen = sizeof(clientaddr);
		ssize_t len = recvfrom(ser_sock, message, sizeof(message), 0, (sockaddr*)&clientaddr, &clientlen);
		sendto(ser_sock, message, len, 0, (sockaddr*)&clientaddr, clientlen);
	}


	close(ser_sock);
	return 0;
}


int lesson74(int argc, char* argv[])
{
	int client_sock;
	sockaddr_in servaddr;
	socklen_t serv_len = sizeof(servaddr);
	char message[512] = "";

	if (argc != 3) {
		printf("usage:%s ip port\n", argv[0]);
		handle_error("argement error!");
	}

	client_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (client_sock == -1) {
		handle_error("socket create failed!");
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons((short)atoi(argv[2]));

	while (1)
	{
		printf("input message(q to Quit):");
		scanf("%s", message);
		if ((strcmp(message, "q") == 0) || (strcmp(message, "Q") == 0))
		{
			break;
		}
		ssize_t len = sendto(client_sock, message, strlen(message), 0, (sockaddr*)&servaddr, serv_len);
		memset(&message, 0, len);
		recvfrom(client_sock, message, sizeof(message), 0, (sockaddr*)&servaddr, &serv_len);
		printf("recv:%s\n", message);
	}

	close(client_sock);
	return 0;
}

void lesson75(char* argv0)
{
	if (fork() > 0) {//主进程
		int argc = 3;
		char* argv[] = {
			argv0,
			(char*)"127.0.0.1",
			(char*)"9999"
		};

		lesson74(argc, argv);//客户端
		int status = 0;
		wait(&status);
	}
	else {//子进程
		int argc = 2;
		char* argv[] = {
			argv0,
			(char*)"9999"
		};
		lesson73(argc, argv);//服务器
	}
}


void lesson77()
{
	int tcp_sock, udp_sock;
	int optval = 0;
	socklen_t len = sizeof(optval);
	tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
	udp_sock = socket(PF_INET, SOCK_DGRAM, 0);

	printf("SOCK_STREAM:%d\n", SOCK_STREAM);
	printf("SOCK_DGRAM:%d\n", SOCK_DGRAM);

	getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (void*)&optval, &len);
	printf("tcp_sock type is : %d\n", optval);
	//optval = 0;
	getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (void*)&optval, &len);
	printf("udp_sock type is : %d\n", optval);

	getsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&optval, &len);
	printf("tcp_sock send buffer size is : %d\n", optval);
	// 	getsockopt(udp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&optval, &len);
	// 	printf("udp_sock send buffer size is : %d\n", optval);

	optval = 1024 * 256;
	setsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&optval, len);
	printf("*tcp_sock send buffer size is : %d\n", optval);

	getsockopt(tcp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&optval, &len);
	printf("*tcp_sock send buffer size is : %d\n", optval);

	// 	setsockopt(udp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&optval, len);
	// 	getsockopt(udp_sock, SOL_SOCKET, SO_SNDBUF, (void*)&optval, &len);
	// 	printf("*udp_sock send buffer size is : %d\n", optval);

	close(tcp_sock);
	close(udp_sock);
}


void server78()
{
	printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
	int sock, client, optval = 0;
	sockaddr_in addr, cli;
	socklen_t addrlen = sizeof(addr);
	char message[256] = "";
	sock = socket(PF_INET, SOCK_STREAM, 0);
	getsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, &addrlen);
	printf("SO_REUSEADDR = %d\n", optval);
	optval = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, addrlen);
	getsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, &addrlen);
	printf("SO_REUSEADDR = %d\n", optval);


	memset(&addr, 0, addrlen);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9527);

	addrlen = sizeof(addr);
	if (bind(sock, (sockaddr*)&addr, addrlen) == -1) {
		handle_error("bind failed!");
	}

	listen(sock, 3);
	client = accept(sock, (sockaddr*)&cli, &addrlen);
	read(client, message, sizeof(message));
	write(client, message, sizeof(message));
	memset(&message, 0, sizeof(message));

	close(client);
	close(sock);
	return;
}

void client78()
{
	int client = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in seraddr;
	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	seraddr.sin_port = htons(9527);

	int ret = connect(client, (sockaddr*)&seraddr, sizeof(seraddr));
	while (ret == 0) {
		printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
		char buffer[256] = "";
		fputs("Input message(Q to quit):", stdout);
		fgets(buffer, sizeof(buffer), stdin);
		if ((strcmp(buffer, "q\n") == 0) || (strcmp(buffer, "Q\n") == 0)) {
			break;
		}
		size_t len = strlen(buffer);
		size_t send_len = 0;
		while (send_len < len) {
			ssize_t ret = write(client, buffer + send_len, len - send_len);
			if (ret <= 0) {
				fputs("write failed\n", stdout);
				close(client);
				std::cout << "client done!" << std::endl;
				return;
			}
			send_len += (ssize_t)ret;
		}
		memset(buffer, 0, sizeof(buffer));
		size_t read_len = 0;
		while (read_len < len) {
			ssize_t ret = read(client, buffer + read_len, len - read_len);
			if (ret < (ssize_t)len) {
				fputs("read failed\n", stdout);
				close(client);
				std::cout << "client done!" << std::endl;
				return;
			}
			read_len += (ssize_t)ret;
		}
		std::cout << "from server:" << buffer;
		break;
	}
	close(client);
	std::cout << "client done!" << std::endl;
}


void lesson78(char* option)
{
	if (strcmp(option, "1") == 0) {
		//服务器
		server78();
		server78();
		server78();
		server78();
	}
	else {
		//客户端
		client78();
		client78();
	}
}

void lesson82()
{
	pid_t pid = fork();
	if (pid > 0) {
		sleep(30);
		int status;
		waitpid(pid, &status, 0);
	}
	else {
		printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
		exit(-1);
	}

}

#include <signal.h>

void signal_func(int sig)
{
	switch (sig)
	{
	case SIGALRM:
		printf("tid %d pid %d\n", pthread_self(), getpid());
		alarm(2);
		break;
	case SIGINT:
		printf("Ctrl + C press...\n");
		exit(0);
		break;
	}
}
void lesson83()
{
	printf("========tid %d pid %d\n", pthread_self(), getpid());
	signal(SIGALRM, signal_func);
	signal(SIGINT, signal_func);
	alarm(1);
	while (true)
	{
		printf("========tid %d pid %d\n", pthread_self(), getpid());
		sleep(4);

	}
}




#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <signal.h>

void hand_childProc(int sig)
{
	pid_t pid;
	int status;
	pid = waitpid(-1, &status, WNOHANG);
	printf("%s(%d):%s removed sub proc:%d\r\n", __FILE__, __LINE__, __FUNCTION__, pid);


}


#include <arpa/inet.h>
void server85()
{
	struct sigaction act;
	act.sa_flags = 0;
	act.sa_handler = hand_childProc;
	sigaction(SIGCHLD, &act, 0);
	int serv_sock;
	sockaddr_in serv_addr, client_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(9527);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (bind(serv_sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	{
		handle_error("bind failed!");
	}
	if (listen(serv_sock, 5) == -1) {
		handle_error("listen failed!");
	}
	int count = 0;
	while (true) {
		socklen_t seraddrlen = sizeof(serv_addr);
		int client = accept(serv_sock, (sockaddr*)&client_addr, &seraddrlen);
		printf("%s(%d):%s client is connected!\n", __FILE__, __LINE__, __FUNCTION__);
		if (client >= 0) {
			count++;
			pid_t pid = fork();
			if (pid == 0) {
				close(serv_sock);
				char buffer[2048] = "";
				ssize_t length = 0;
				while ((length = read(client, buffer, sizeof(buffer))) > 0) {
					write(client, buffer, sizeof(buffer));
				}
				close(client);
				printf("%s(%d):%s client is closed!\n", __FILE__, __LINE__, __FUNCTION__);
				return;
			}
			if (pid < 0) {
				close(client);
				printf("%s(%d):%s fork is failed!\n", __FILE__, __LINE__, __FUNCTION__);
				break;
			}
			close(client);
		}
		if (count >= 5) break;
	}
	printf("%s(%d):%s server is over!\n", __FILE__, __LINE__, __FUNCTION__);
	close(serv_sock);
	return;
}


void client85()
{
	int client = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in seraddr;
	memset(&seraddr, 0, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	seraddr.sin_port = htons(9527);

	int ret = connect(client, (sockaddr*)&seraddr, sizeof(seraddr));
	while (ret == 0) {
		printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
		char buffer[256] = "hello world!";
		//fputs("Input message(Q to quit):", stdout);
		//fgets(buffer, sizeof(buffer), stdin);
		//if ((strcmp(buffer, "q\n") == 0) || (strcmp(buffer, "Q\n") == 0)) {
		//	break;
		//}
		size_t len = strlen(buffer);
		size_t send_len = 0;
		while (send_len < len) {
			ssize_t ret = write(client, buffer + send_len, len - send_len);
			if (ret <= 0) {
				fputs("write failed\n", stdout);
				close(client);
				std::cout << "client done!" << std::endl;
				return;
			}
			send_len += (ssize_t)ret;
		}
		memset(buffer, 0, sizeof(buffer));
		size_t read_len = 0;
		while (read_len < len) {
			ssize_t ret = read(client, buffer + read_len, len - read_len);
			if (ret < (ssize_t)len) {
				fputs("read failed\n", stdout);
				close(client);
				std::cout << "client done!" << std::endl;
				return;
			}
			read_len += (ssize_t)ret;
		}
		std::cout << "from server:" << buffer;
		sleep(2);
		break;
	}

	close(client);
	std::cout << "client done!" << std::endl;
}


void lesson85()
{
	pid_t pid = fork();
	if (pid == 0) {//子进程
		//启动服务器
		server85();
	}
	else if (pid > 0) {//主进程
		//启动客户端
		printf("%s(%d):%s wait server invoking!\n", __FILE__, __LINE__, __FUNCTION__);
		for (int i = 0; i < 5; i++) {
			pid = fork();
			if (pid > 0) {
				continue;
			}
			else {//子进程
				//启动客户端
				sleep(1);
				client85();
				break;
			}
		}
	}
}

void singlePipe()
{
	int fds[2] = { -1,-1 };
	char str[64] = "send by sub process!!\n";
	char buf[128] = "";
	pipe(fds);//匿名管道
	pid_t pid = fork();
	if (pid == 0) {
		write(fds[1], str, sizeof(str));
	}
	else {
		read(fds[0], buf, sizeof(buf));
		printf("%s(%d):%s server:%s\n", __FILE__, __LINE__, __FUNCTION__, buf);
	}
}

void lesson87()
{
	int s2c[2], c2s[2];
	pipe(s2c);
	pipe(c2s);
	pid_t pid = fork();
	if (pid == 0) {
		char buffer[256] = "";
		write(c2s[1], "hello, i am subprocess!\n", 24);
		read(s2c[0], buffer, sizeof(buffer));
		printf("%s(%d):%s client:%s\n", __FILE__, __LINE__, __FUNCTION__, buffer);
	}
	else {
		char buffer[256] = "";
		read(c2s[0], buffer, sizeof(buffer));
		printf("%s(%d):%s server:%s\n", __FILE__, __LINE__, __FUNCTION__, buffer);
		write(s2c[1], "hello, i am mainprocess\n", 24);
		printf("%s(%d):%s %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
	}
	printf("%s(%d):%s %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());


}



void lesson88()
{
	mkfifo("./a.fifo", 0666);//命名管道
	pid_t pid = fork();
	if (pid == 0) {
		int fd = open("./a.fifo", O_RDONLY);
		char buffer[128] = "";
		ssize_t len = read(fd, buffer, sizeof(buffer));
		printf("%s(%d):%s server:%s\n", __FILE__, __LINE__, __FUNCTION__, buffer);
		printf("%s(%d):%s %d\n", __FILE__, __LINE__, __FUNCTION__, len);
		close(fd);
		printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
	}
	else {
		int fd = open("./a.fifo", O_WRONLY);
		write(fd, "hello world", 12);
		close(fd);
		printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
	}
}

#include <sys/ipc.h>
#include <sys/shm.h>
typedef struct {
	int signal;
	int id;
	char name[128];
	int age;
	bool sex;
}STUDENT, * PSTUDENT;

void lesson90()
{
	pid_t pid = fork();
	if (pid > 0) {
		int shm_id = shmget(ftok(".", 1), sizeof(STUDENT), IPC_CREAT | 0666);
		if (shm_id == -1) {
			printf("%s(%d):%s create share memory failed!\n", __FILE__, __LINE__, __FUNCTION__);
			return;
		}
		PSTUDENT pStu = (PSTUDENT)shmat(shm_id, NULL, 0);
		pStu->id = 666666;
		strcpy(pStu->name, "abcdefeddjdj");
		pStu->age = 18;
		pStu->sex = true;
		pStu->signal = 99;
		while (pStu->signal == 99) {
			usleep(1000);
		}
		shmdt(pStu);
		shmctl(shm_id, IPC_RMID, NULL);
	}
	else {
		//usleep(500000);
		int shm_id = shmget(ftok(".", 1), sizeof(STUDENT), IPC_CREAT | 0666);
		if (shm_id == -1) {
			printf("%s(%d):%s create share memory failed!\n", __FILE__, __LINE__, __FUNCTION__);
			return;
		}
		PSTUDENT pStu = (PSTUDENT)shmat(shm_id, NULL, 0);
		while (pStu->signal != 99) {//同步
			usleep(1000);
		}
		printf("%d,%s,%d,%s\n", pStu->id, pStu->name, pStu->age, pStu->sex ? "male" : "female");
		pStu->signal = 0;
		shmdt(pStu);
		shmctl(shm_id, IPC_RMID, NULL);
	}
}

#include <sys/sem.h>

void lesson91()
{
	pid_t pid = fork();
	if (pid > 0) {
		key_t key = ftok(".", 2);
		int sem_id = semget(key, 2, IPC_CREAT | 0666);
		semctl(sem_id, 0, SETVAL, 0);
		semctl(sem_id, 1, SETVAL, 0);
		int shm_id = shmget(ftok(".", 1), sizeof(STUDENT), IPC_CREAT | 0666);
		if (shm_id == -1) {
			printf("%s(%d):%s create share memory failed!\n", __FILE__, __LINE__, __FUNCTION__);
			return;
		}
		PSTUDENT pStu = (PSTUDENT)shmat(shm_id, NULL, 0);
		pStu->id = 666666;
		strcpy(pStu->name, "abcdefeddjdj");
		pStu->age = 18;
		pStu->sex = true;
		//信号量
		sembuf sop = {
		.sem_num = 0,
		.sem_op = 1
		};
		printf("%s(%d):%s %d\n", __FILE__, __LINE__, __FUNCTION__, semctl(shm_id, 0, GETVAL));//1
		semop(sem_id, &sop, 1);//V操作
		printf("%s(%d):%s %d\n", __FILE__, __LINE__, __FUNCTION__, semctl(shm_id, 0, GETVAL));//2 //3
		sop.sem_num = 1;
		sop.sem_op = -1;
		printf("%s(%d):%s %d\n", __FILE__, __LINE__, __FUNCTION__, semctl(shm_id, 1, GETVAL));//4 //5
		semop(sem_id, &sop, 1);//P操作
		printf("%s(%d):%s %d\n", __FILE__, __LINE__, __FUNCTION__, semctl(shm_id, 1, GETVAL));//8
		//共享内存
		shmdt(pStu);
		shmctl(shm_id, IPC_RMID, NULL);
		//删除信号量

		semctl(sem_id, 0, IPC_RMID);
		semctl(sem_id, 1, IPC_RMID);
		//sleep(10);

	}
	else {
		key_t key = ftok(".", 2);
		int sem_id = semget(key, 2, IPC_CREAT);

		int shm_id = shmget(ftok(".", 1), sizeof(STUDENT), IPC_CREAT | 0666);
		if (shm_id == -1) {
			printf("%s(%d):%s create share memory failed!\n", __FILE__, __LINE__, __FUNCTION__);
			return;
		}
		sembuf sop = {
		.sem_num = 0,
		.sem_op = -1//P操作
		};
		printf("%s(%d):%s %d\n", __FILE__, __LINE__, __FUNCTION__, semctl(shm_id, 0, GETVAL));//3 //2
		semop(sem_id, &sop, 1);//P操作
		printf("%s(%d):%s %d\n", __FILE__, __LINE__, __FUNCTION__, semctl(shm_id, 0, GETVAL));//5 //4
		PSTUDENT pStu = (PSTUDENT)shmat(shm_id, NULL, 0);
		//信号量
		printf("%d,%s,%d,%s\n", pStu->id, pStu->name, pStu->age, pStu->sex ? "male" : "female");
		sop.sem_num = 1;
		sop.sem_op = 1;//V操作
		printf("%s(%d):%s %d\n", __FILE__, __LINE__, __FUNCTION__, semctl(shm_id, 1, GETVAL));//6
		semop(sem_id, &sop, 1);//V操作
		printf("%s(%d):%s %d\n", __FILE__, __LINE__, __FUNCTION__, semctl(shm_id, 1, GETVAL));//7
		//sleep(10);
		shmdt(pStu);
		shmctl(shm_id, IPC_RMID, NULL);
	}
}

#include <sys/msg.h>
typedef struct {
	int type;
	//char data[1024];
	struct {
		int id;
		char name[64];
		int age;
		char message[256];
	}data;
}MSG, * PMSG;
void lesson92()
{
	pid_t pid = fork();
	if (pid > 0) {
		int msg_id = msgget(ftok(".", 3), IPC_CREAT | 0666);
		printf("%s(%d):%s %d %d\n", __FILE__, __LINE__, __FUNCTION__, msg_id, errno);
		printf("%s\n", strerror(errno));
		if (msg_id == -1)return;
		MSG msg;
		msg.type = 1;
		memset(&msg, 0, sizeof(msg));

		while (true) {
			ssize_t ret = msgrcv(msg_id, &msg, sizeof(msg.data), 1, 0);
			if (ret == -1)
			{
				sleep(1000);
				printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
			}
			else break;
		}
		printf("%d name: %s age:%d msg:%s\n", msg.data.id, msg.data.name, msg.data.age, msg.data.message);
		getchar();
		msgctl(msg_id, IPC_RMID, 0);

	}
	else {
		int msg_id = msgget(ftok(".", 3), IPC_CREAT | 0666);
		MSG msg;
		memset(&msg, 0, sizeof(msg));
		msg.type = 1;
		msg.data.id = 666;
		strcpy(msg.data.name, "Bingo");
		msg.data.age = 18;
		strcpy(msg.data.message, "hello world!");
		printf("***%d name: %s age:%d msg:%s\n", msg.data.id, msg.data.name, msg.data.age, msg.data.message);
		msgsnd(msg_id, &msg, sizeof(msg.data), 0);
		sleep(1);
		msgctl(msg_id, IPC_RMID, 0);
	}

}



#include <pthread.h>

void* threadEntry(void* arg)
{
	const char* msg = "i am from thread!";
	for (int i = 0; i < 5; i++) {
		std::string* pstr = (std::string*)arg;
		printf("%s(%d):%s thraed begin:%s\n", __FILE__, __LINE__, __FUNCTION__, pstr->c_str());
		usleep(1000);
	}
	return (void*)msg;
}

int create(pthread_t& tid)
{
	std::string info = "hello world!***";
	int ret = pthread_create(&tid, NULL, threadEntry, (void*)&info);// -lpthread
	return ret;
}

void lesson94()
{
	pthread_t tid;
	//const char* pInfo = "hello world!";
	int ret = create(tid);

	if (ret != -1) {
		void* result = NULL;
		pthread_join(tid, &result);
		printf("%s(%d):%s from thread:%s\n", __FILE__, __LINE__, __FUNCTION__, result);
	}

}



int num = 0;
pthread_mutex_t mutex;
void* thread_inc(void* arg)
{
	for (int i = 0; i < 1000; i++) {
		pthread_mutex_lock(&mutex);
		num++;
		pthread_mutex_unlock(&mutex);
		if (i % 100)printf("%s(%d):%s num is %d *************\n", __FILE__, __LINE__, __FUNCTION__, num);
	}
	return NULL;
}

void* thread_dec(void* arg)
{

	for (int i = 0; i < 1000; i++) {
		pthread_mutex_lock(&mutex);
		num--;
		pthread_mutex_unlock(&mutex);
		if (i % 100)printf("%s(%d):%s num is %d\n", __FILE__, __LINE__, __FUNCTION__, num);
	}

	return NULL;
}

void lesson95()
{
	pthread_mutex_init(&mutex, NULL);
	pthread_t thread_id[50];
	for (int i = 0; i < 50; i++) {
		if (i % 2)pthread_create(thread_id + i, NULL, thread_inc, NULL);
		else pthread_create(thread_id + i, NULL, thread_dec, NULL);
	}
	for (int i = 0; i < 50; i++) {
		pthread_join(thread_id[i], NULL);
	}
	printf("%s(%d):%s num is %d\n", __FILE__, __LINE__, __FUNCTION__, num);
	pthread_mutex_destroy(&mutex);
	return;
}




#include <semaphore.h>
sem_t sem_one;
sem_t sem_two;
//int num = 0;
void* input_num(void* arg)
{
	pthread_detach(pthread_self());
	int count = 0;
	//memcpy(&count, 4 + (char*)&arg, sizeof(int));//大顶端
	memcpy(&count, &arg, sizeof(int));//小顶端
	for (int i = 0; i < count; i++) {
		printf("Input num:");
		//sem_wait(&sem_two);
		scanf("%d", &num);
		sem_post(&sem_one);
	}
	pthread_exit(NULL);
}

void lesson96()
{
	int sum = 0;
	int count = 5;
	sem_init(&sem_one, 0, 0);//是否完成输入
	sem_init(&sem_two, 0, 1);//是否完成计算
	pthread_t thread;
	pthread_create(&thread, NULL, input_num, reinterpret_cast<void*>(count));
	for (int i = 0; i < count; i++)
	{
		sem_wait(&sem_one);
		sum += num;
		//sem_post(&sem_two);
	}
	printf("sum is %d\n", sum);
	sem_destroy(&sem_one);
	sem_destroy(&sem_two);
}




int clnt_socks[100] = { 0 };
int clnt_cnt = 0;
pthread_mutex_t mutex1;

void send_msg(const char* msg, ssize_t str_len)
{
	pthread_mutex_lock(&mutex1);
	for (int i = 0; i < clnt_cnt; i++) {
		if (clnt_socks[i] >= 0)
			write(clnt_socks[i], msg, str_len);
	}
	pthread_mutex_unlock(&mutex1);
}

void* handle_clnt(void* arg)
{
	pthread_detach(pthread_self());
	int clnt_sock = *(int*)arg;
	char msg[1024] = "";
	ssize_t str_len = 0;
	while ((str_len = read(clnt_sock, msg, sizeof(msg))) > 0)
	{
		//TODO:过滤敏感词
		send_msg(msg, str_len);
	}
	pthread_mutex_lock(&mutex1);
	// 	for (int i = 0; i < clnt_cnt; i++) {//TODO:优化逻辑
	// 		if (clnt_sock == clnt_socks[i]) {
	// 			clnt_socks[i] = -1;
	// 			break;
	// 		}
	// 	}
	*(int*)arg = -1;
	pthread_mutex_unlock(&mutex1);
	close(clnt_sock);
	pthread_exit(NULL);
}

void server98()
{
	int serv_sock, clnt_sock;
	sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_addr_sz = sizeof(clnt_addr);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(9527);
	pthread_mutex_init(&mutex1, NULL);
	if (bind(serv_sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		printf("bind error %d %s", errno, strerror(errno));
		return;
	}
	if (listen(serv_sock, 5) == -1)
	{
		printf("listen error %d %s", errno, strerror(errno));
		return;
	}
	for (;;) {
		clnt_sock = accept(serv_sock, (sockaddr*)&clnt_addr, &clnt_addr_sz);
		if (clnt_sock == -1) {
			printf("accept error %d %s", errno, strerror(errno));
			break;
		}
		pthread_mutex_lock(&mutex1);
		clnt_socks[clnt_cnt++] = clnt_sock;
		pthread_mutex_unlock(&mutex1);
		pthread_t thread;
		pthread_create(&thread, NULL, handle_clnt, &clnt_socks[clnt_cnt - 1]);

	}
	close(serv_sock);
	pthread_mutex_destroy(&mutex1);
}



sem_t semid;
char name[64] = "[DEFAULT]";
void* client_send_msg(void* arg) {
	pthread_detach(pthread_self());
	int sock = *(int*)arg;
	char msg[256] = "";
	char buffer[1024];
	while (true) {
		memset(buffer, 0, sizeof(buffer));
		fgets(msg, sizeof(msg), stdin);
		if ((strcmp(msg, "q\n") == 0) || (strcmp(msg, "Q\n") == 0)) {
			break;
		}
		snprintf(buffer, sizeof(msg), "%s %s", name, msg);
		write(sock, buffer, strlen(buffer));
	}
	sem_post(&semid);
	pthread_exit(NULL);
}
void* client_recv_msg(void* arg) {
	pthread_detach(pthread_self());
	int sock = *(int*)arg;
	char msg[256] = "";
	while (true) {
		ssize_t str_len = read(sock, msg, sizeof(msg));
		if (str_len <= 0) break;
		fputs(msg, stdout);
		memset(&msg, 0, str_len);
	}
	sem_post(&semid);
	pthread_exit(NULL);
}
void client98()
{
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons(9527);
	fputs("input your name:", stdout);
	scanf("%s", name);
	if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		printf("connect error %d %s", errno, strerror(errno));
		return;
	}

	pthread_t thsend, threcv;

	sem_init(&semid, 0, -1);
	pthread_create(&thsend, NULL, client_send_msg, (void*)&sock);
	pthread_create(&threcv, NULL, client_recv_msg, (void*)&sock);
	sem_wait(&semid);
	close(sock);
}


void lesson98(const char* arg)
{
	if (strcmp(arg, "s") == 0) {
		server98();
	}
	else {
		client98();
	}
}


#include <sys/select.h>
#include <sys/times.h>

void server101()
{
	int serv_sock, clnt_sock = -1;
	sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_sz = sizeof(clnt_addr);
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(9527);
	if (bind(serv_sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		printf("bind error %d %s", errno, strerror(errno));
		close(serv_sock);
		return;
	}
	if (listen(serv_sock, 5) == -1) {
		printf("listen error %d %s", errno, strerror(errno));
		close(serv_sock);
		return;
	}
	fd_set reads, copy_reads;
	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	timeval timeout = { 0, 500000 };
	int max_sock = serv_sock;
	while (true) {
		copy_reads = reads;
		int fd_num = select(max_sock + 1, &copy_reads, NULL, NULL, &timeout);
		if (fd_num <= -1) {
			printf("select error %d %s", errno, strerror(errno));
			close(serv_sock);
			return;
		}
		if (fd_num == 0)continue;
		printf("%s(%d):%s fd_num is %d\n", __FILE__, __LINE__, __FUNCTION__, fd_num);
		for (int i = 0; i < max_sock + 1; i++) {
			if (FD_ISSET(i, &copy_reads)) {
				if (i == serv_sock) {
					clnt_sock = accept(serv_sock, (sockaddr*)&clnt_addr, &clnt_sz);
					FD_SET(clnt_sock, &reads);
					printf("%s(%d):%s sock is %d\n", __FILE__, __LINE__, __FUNCTION__, i);
					if (max_sock < clnt_sock) {
						max_sock = clnt_sock;
					}
					printf("clinet is connected:%d\n", clnt_sock);
				}
				else {
					char buf[256] = "";
					ssize_t str_len = read(i, buf, sizeof(buf));
					if (str_len == 0) {
						FD_CLR(i, &reads);
						close(i);
						printf("client is disconnect:%d\n!", i);
					}
					else {
						write(i, buf, str_len);
					}
				}
			}
		}
	}
	close(serv_sock);
}
void client101()
{
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons(9527);
	if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		printf("connect error %d %s", errno, strerror(errno));
		close(sock);
		return;
	}
	char message[256] = "";
	while (true) {
		//memset(message, 0, sizeof(message));
		printf("Input message(q to quit):");
		fgets(message, sizeof(message), stdin);
		if ((!strcmp(message, "q\n")) || (!strcmp(message, "Q\n"))) break;
		write(sock, message, sizeof(message));
		read(sock, message, sizeof(message));
		printf("server:%s\n", message);
	}
	close(sock);
}

void lesson101(const char* arg)
{
	if (strcmp(arg, "s") == 0) {
		server101();
	}
	else {
		client101();
	}
}





#include <sys/epoll.h>

void server102()
{
	int serv_sock, clnt_sock;
	sockaddr_in serv_addr, clnt_addr;
	socklen_t clnt_sz;
	char buf[5] = "";
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(9527);
	if (bind(serv_sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		printf("bind error %d %s", errno, strerror(errno));
		close(serv_sock);
		return;
	}
	if (listen(serv_sock, 5) == -1) {
		printf("listen error %d %s", errno, strerror(errno));
		close(serv_sock);
		return;
	}
	epoll_event event;
	int epfd, event_cnt;
	epfd = epoll_create(1);
	if (epfd == -1) {
		printf("epoll_create error %d %s", errno, strerror(errno));
		close(serv_sock);
		return;
	}
	epoll_event* all_events = new epoll_event[100];

	event.events = EPOLLIN;
	event.data.fd = serv_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);
	printf("epoll_ctl error %d %s", errno, strerror(errno));
	while (true) {
		event_cnt = epoll_wait(epfd, all_events, 100, 1000);
		if (event_cnt == -1) {
			printf("epoll_wait error %d %s", errno, strerror(errno));
			break;
		}
		if (event_cnt == 0)continue;
		for (int i = 0; i < event_cnt; i++) {
			if (all_events[i].data.fd == serv_sock) {
				clnt_sz = sizeof(clnt_addr);
				clnt_sock = accept(serv_sock, (sockaddr*)&clnt_addr, &clnt_sz);
				event.events = EPOLLIN | EPOLLET;
				event.data.fd = clnt_sock;
				int flags = fcntl(clnt_sock, F_GETFL, 0);
				fcntl(clnt_sock, F_GETFL, O_NONBLOCK);
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				printf("client is connected!%d\n", clnt_sock);
			}
			else {
				while (true) {
					ssize_t len = read(all_events[i].data.fd, buf, sizeof(buf));
					if (len < 0) {
						if (errno == EAGAIN) 
							break;
						printf("read error %d %s", errno, strerror(errno));
						close(all_events[i].data.fd);
						break;
					}
					else if (len == 0) {
						epoll_ctl(epfd, EPOLL_CTL_DEL, all_events[i].data.fd, NULL);
						close(all_events[i].data.fd);
						printf("client is closed!%d\n", all_events[i].data.fd);
						break;
					}
					else {
						printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
						write(all_events[i].data.fd, buf, len);
					}
				}
			}
		}
		printf("%s(%d):%s\n", __FILE__, __LINE__, __FUNCTION__);
	}

	delete[] all_events;
	close(serv_sock);
	close(epfd);
}

void lesson102(const char* arg)
{
	if (strcmp(arg, "s") == 0) {
		server102();
	}
	else {
		client101();
	}
}



int main(int argc, char* argv[])
{
	//lesson01();
	//lesson_io_7();
	//lesson33();
	//lesson34();
	//lesson57();
	//lesson60();
	//lesson62();
	//lesson63();
	//lesson64();
	//lesson66();
	//lesson73(argc, argv);
	//lesson74(argc, argv);
	//lesson75(argv[0]);
	//lesson77();
	//lesson78(argv[1]);
	//lesson82();
	//lesson83();
	//lesson85();
	//lesson87();
	//lesson88();
	//lesson90();
	//lesson91();
	//lesson92();
	//lesson94();
	//lesson95();
	//lesson96();
	//lesson98(argv[1]);
	//lesson101(argv[1]);
	lesson102(argv[1]);
	return 0;
}




