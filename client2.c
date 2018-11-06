 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 #include <netinet/in.h>
 
 #define BUFFSIZE 128
 #define HOST_IP "192.168.159.3"
 #define PORT 8000
 
 int sockfd;
 
 void snd();
 
 int main()
 {
         pthread_t thread;       // pthread_t 线程，gcc编译时需加上-lpthread
         struct sockaddr_in serv_addr;   // 处理网络通信地址 端口、ip地址，sin_zero用来填充字节
         char buf[BUFFSIZE];
         // 初始化服务端地址结构
         bzero(&serv_addr, sizeof(struct sockaddr_in));  // 将serv_addr的前sizeof(struct sockaddr_in)位清零 memset
         serv_addr.sin_family = AF_INET;         // sin_family   AF_INET
         serv_addr.sin_port = htons(PORT);       // sin_port     htons(PORT) 将主机字节顺序转换成网络通信字节顺序
         inet_pton(HOST_IP, &serv_addr.sin_addr);        // inet_pton 将点分十进制的ip地址转化为用于网络传输的数值格式
         // 创建客户端套接字
         sockfd = socket(AF_INET, SOCK_STREAM, 0);       // socket 创建套接字
         if(sockfd < 0)
         {
                 perror("fail to socket");
                 exit(-1);
         }
         // 与服务器建立连接 else
         printf("connecting... \n");
         if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // connect
         {
                 perror("fail to connect");
                 exit(-2);
         }
         /* === 从此处开始 程序分做两个线程 === */
         // 创建发送消息的线程，调用发送消息的函数snd
         pthread_create(&thread, NULL, (void *)(&snd), NULL);    // pthread_create 创建一个线程并将其加入主进程-lpthread
         // 接收消息的线程
         while(1)
         {
                 int len;
                 if((len=read(sockfd, buf, BUFFSIZE)) > 0)       // read 读取通信套接字
                 {
                         buf[len] = '\0';        // 添加结束符，避免显示缓冲区中残留的内容
                         printf("\n%s", buf);
                         fflush(stdout);         // fflush 刷新缓冲区并把缓冲区的内容输出
                 }
         }
         return 0;
 }
 
 // 发送消息的函数
 void snd()
 {
         char name[32], buf[BUFFSIZE];
         
         fgets(name, 32, stdin); // fgets 会读取输入字符串后的换行符
         write(sockfd, name, strlen(name));      // strlen检查name实际长度 将name写入通信套接字
         while(1)
         {
                 fgets(buf, BUFFSIZE, stdin);
                 printf("\n%s", buf);
                 write(sockfd, buf, strlen(buf));
                 if(strcmp(buf, "bye\n") == 0)   // 注意此处的\n
                         exit(0);
         }
 }


