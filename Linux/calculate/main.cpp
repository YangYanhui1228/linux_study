#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>


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

int main()
{
	lesson66();

	return 0;
}

