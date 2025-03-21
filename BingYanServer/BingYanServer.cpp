#include <stdio.h>
#include <winsock2.h>
#include<string.h>
#pragma comment(lib, "ws2_32.lib")

#define MAX_STR_LEN 100
#define DECIMAL_DIGITS 3

typedef struct {
	char str[MAX_STR_LEN];
	double value;
} StringValue;

int my_inet_pton(int af, const char* src, void* dst);
char* server();
int text(char* buffer);
int parseValue(const char* str, double* result);
int compare(const void* a, const void* b);
void sortArray(char arr[][MAX_STR_LEN], int n);

char record[10000][100];

char* server()
{
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);
    static char buffer[1024] = { 0 };

    // initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        //if fail
        printf("WSAStartup失败: %d\n", WSAGetLastError());
        return;
    }

    // create Socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        //if fail
        printf("创建Socket失败: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // server's address and port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(5210);

    // add Socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        //if fail
        printf("绑定失败: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return;
    }

    // listening...
    listen(server_socket, 3);
    printf("服务器已启动，等待客户端连接...\n");

    // link and listen
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
    if (client_socket == INVALID_SOCKET)
    {
        //if fail
        printf("接受连接失败: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return;
    }

    // receive messages
    recv(client_socket, buffer, sizeof(buffer), 0);
    printf("客户端发来消息: %s\n", buffer);
    send(client_socket, buffer, strlen(buffer) + 1, 0);

    // close Socket and clean up
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
    return buffer;
}

int text(char* buffer)
{
	WSADATA wsa;
	SOCKET client_socket;
	struct sockaddr_in server_addr;
	// initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		//if fail
		printf("WSAStartup失败: %d\n", WSAGetLastError());
		return 1;
	}

	// create Socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET)
	{
		//if fail
		printf("创建Socket失败: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// server's address + port
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(5210);
	if (my_inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) != 1)
		//virtual ip in linux
	{
		//if fail
		printf("IP地址转换失败\n");
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}

	// connect the server
	if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		//if fail
		printf("连接服务器失败: %d\n", WSAGetLastError());
		closesocket(client_socket);
		WSACleanup();
		return 1;
	}

	// send off messages
	send(client_socket, buffer, strlen(buffer) + 1, 0);
	recv(client_socket, buffer, sizeof(buffer), 0);
	printf("服务器返回消息: %s\n", buffer);

	// close Socket and clean
	closesocket(client_socket);
	WSACleanup();
	return 0;
}

int my_inet_pton(int af, const char* src, void* dst)
{
	if (af == AF_INET)
	{
		unsigned char* p = (unsigned char*)dst;
		int a, b, c, d;
		if (sscanf_s(src, "%d.%d.%d.%d", &a, &b, &c, &d) == 4 &&
			a >= 0 && a <= 255 &&
			b >= 0 && b <= 255 &&
			c >= 0 && c <= 255 &&
			d >= 0 && d <= 255)
		{
			p[0] = (unsigned char)a;
			p[1] = (unsigned char)b;
			p[2] = (unsigned char)c;
			p[3] = (unsigned char)d;
			return 1;
		}
	}
	return 0;
}

int main()
{
	for (int i = 0;i < 10000;i++)
	{
		strcpy(record[i], server());
		if (strstr("华科是985",record[i]) != 0)
		{
			memset(record[i], 0, 10000);
			i--;
			//返回去
            for (int j = 0;j < i;j++)
            {
                text(record[j]);
            }
		}
        sortArray(record, i);
	}
}

int parseValue(const char* str, double* result) 
{
    const char* p = str;
    while (*p && !isdigit(*p))
    {
        p++;
    }
    if (*p == '\0') 
    {
        return -1; 
    } 

    char* endptr;
    *result = strtod(p, &endptr);

    if (endptr > p) 
    {
        const char* dot = strchr(p, '.');
        if (dot != NULL) 
        {
            int decimal_count = 0;
            const char* dec = dot + 1;
            while (*dec && isdigit(*dec) && decimal_count < DECIMAL_DIGITS) 
            {
                decimal_count++;
                dec++;
            }
            if (decimal_count < DECIMAL_DIGITS) 
            {
                fprintf(stderr, "Warning: Decimal part has less than 3 digits in '%s'\n", str);
            }
        }
        return 0; //success
    }
    return -1; //failure
}

int compare(const void* a, const void* b) 
{
    const StringValue* sa = (const StringValue*)a;
    const StringValue* sb = (const StringValue*)b;
    if (sa->value < sb->value) return -1;
    else if (sa->value > sb->value) return 1;
    return 0;
}

void sortArray(char arr[][MAX_STR_LEN], int n) 
{
    StringValue* svArray = (StringValue*)malloc(n * sizeof(StringValue));
    if (!svArray) 
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) 
    {
        strncpy(svArray[i].str, arr[i], MAX_STR_LEN - 1);
        svArray[i].str[MAX_STR_LEN - 1] = '\0'; 
        if (parseValue(arr[i], &svArray[i].value) != 0) 
        {
            fprintf(stderr, "Error parsing value in '%s'\n", arr[i]);
            free(svArray);
            exit(EXIT_FAILURE);
        }
    }

    qsort(svArray, n, sizeof(StringValue), compare);

    for (int i = 0; i < n; i++) 
    {
        strncpy(arr[i], svArray[i].str, MAX_STR_LEN - 1);
        arr[i][MAX_STR_LEN - 1] = '\0';
    }

    free(svArray);
}
