#include<stdio.h>
#include<string.h>	//strlen
#include<stdlib.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include <sstream>
#include<pthread.h> //for threading , link with lpthread
#include "ws.h"
#include <fcntl.h>

#define SHA_DIGEST_LENGTH 20
#define SW_BUF 65552
#define WS_TEXT_FRAME 0x01
#define WS_PING_FRAME 0x09
#define WS_PONG_FRAME 0x0A
#define WS_CLOSING_FRAME 0x08

#include "ws_func.h"
#include "gcomm.h"

void *connection_handler(void *);

int main(int argc , char *argv[])
{
    if((sets_fd = open("../piCNCsetings", O_RDWR)) < 0){
        puts("Coud not open PiCNCSetings");
    }else{
        read(sets_fd, setsBuf, sizeof(MSets_t));
        close(sets_fd);
    }

	int socket_desc , new_socket , c , *new_sock;
	struct sockaddr_in server , client;

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 8888 );

	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("bind failed");
		return 1;
	}
	puts("bind done");

	//Listen
	listen(socket_desc , 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("Connection accepted");

		//Reply to the client
//		message = "Hello Client , I have received your connection. And now I will assign a handler for you\n";
//		write(new_socket , message , strlen(message));

		pthread_t sniffer_thread;
		new_sock = (int*)malloc(1);
		*new_sock = new_socket;

		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}

		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
		puts("Handler assigned");
	}

	if (new_socket<0)
	{
		perror("accept failed");
		return 1;
	}

	return 0;
}

void mSendFilse(int mfd, char* resp_tipe, size_t tipe_size, int* fsock)
{
    char* response_buf;
	size_t send_size, left_pos = 0, response_buf_size;
	std::string send_size_str("");

    if(mfd < 0)
    {
      if(close(mfd) == -1) puts("open file close client_fd.");
      puts("Not File.");
    } else {
        send_size = lseek(mfd, 0, SEEK_END);
        lseek (mfd, 0, SEEK_SET);

        send_size_str = std::to_string(send_size);
        response_buf_size = tipe_size + send_size_str.length() + 4 + send_size;
        response_buf = (char*)malloc(response_buf_size);

        memcpy(response_buf, resp_tipe, tipe_size-1);
        left_pos += tipe_size-1;

        memcpy(response_buf+left_pos, send_size_str.c_str(), send_size_str.length());
        left_pos += send_size_str.length();

        response_buf[left_pos] = '\r'; left_pos++;
        response_buf[left_pos] = '\n'; left_pos++;
        response_buf[left_pos] = '\r'; left_pos++;
        response_buf[left_pos] = '\n'; left_pos++;

        read(mfd, response_buf+left_pos, send_size);
        if(close(mfd) == -1) puts("Err close file.");

        write(*fsock , response_buf , response_buf_size);
        free(response_buf);
    }

}
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
	//Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size;
	char client_message[2000];
	char *p = NULL;
    std::stringstream response; // сюда будет записываться ответ клиенту
    std::stringstream response_body; // тело ответа

	//Send some messages to the client
//	message = "Greetings! I am your connection handler\n";
//	write(sock , message , strlen(message));

//	message = "Now type something and i shall repeat what you type \n";
//	write(sock , message , strlen(message));

	//Receive a message from client
	while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
	{
        if(strstr(client_message, "GET") != NULL || strstr(client_message, "POST") != NULL){
            if((strstr(client_message, "favicon.ico")) != NULL)
            {
                mSendFilse(open("favicon.ico", O_RDONLY), response_xicon, sizeof(response_xicon), &sock);
            }else if(strstr(client_message, "GET /ws ") != NULL)
            {
               if((p = strstr(client_message, "Sec-WebSocket-Key:")) != NULL)
                {
                  char resultstr[64] = {0,};
                  int i = 0, it = 0;
                  for(i = 19; it < 24; i++, it++)
                   {
                     resultstr[it] = p[i];
                   }

                  strcat(resultstr, GUIDKey);

                  ////////////////////////////sha1///////////////////////////////////////
                  unsigned char temp[SHA_DIGEST_LENGTH] = {0,};
                  SHA1(temp, resultstr, strlen(resultstr));

                  ////////////////////////////Base64////////////////////////////////////
                  unsigned char key_out[64] = {0,};
                  base64_encode(temp, key_out, sizeof(temp));

                  char resp[131] = {0,};
                  snprintf(resp, 130, "%s%s%s", response_ws, key_out, "\r\n\r\n");
                  write(sock , resp , strlen(resp));
    //              if(send(client_fd, resp, sizeof(char) * strlen(resp), MSG_NOSIGNAL) == -1) warning_access_log("send response_ws.");


                }
            }else
            {
                mSendFilse(open("index.html", O_RDONLY), response_html, sizeof(response_html), &sock);
            }
        }else{
             // puts(client_message);

                 char masking_key[4] = {0,}; // сюда положим маску
                 char opcode; // сюда тип фрейма
                 unsigned char payload_len; // сюда длину сообщения (тела), то есть без служебных байтов либо цифры 126 или 127

                 opcode = client_message[0] & 0x0F;

                 payload_len = client_message[1] & 0x7F;

                 unsigned char payload[SW_BUF] = {0,};


                 if(opcode == WS_CLOSING_FRAME) // от клиента получен код закрытия соединения
                  {
                    puts("ws_closing_frame");
                  }

                 else if(opcode == WS_PONG_FRAME) // от клиента получен PONG (маскированный)
                  {
                    masking_key[0] = client_message[2];
                    masking_key[1] = client_message[3];
                    masking_key[2] = client_message[4];
                    masking_key[3] = client_message[5];

                    unsigned int i = 6, pl = 0;
                    for(; pl < payload_len; i++, pl++)
                     {
                       payload[pl] = client_message[i]^masking_key[pl % 4];
                     }

                    printf("Payload_len: %d\n", client_message[1] & 0x7F);
                    printf("\nRecive PONG and text \"%s\"\n", payload);
                  }

                 else if(opcode == WS_TEXT_FRAME && payload_len < 126) // от клиента получен текст
                  {
                    masking_key[0] = client_message[2];
                    masking_key[1] = client_message[3];
                    masking_key[2] = client_message[4];
                    masking_key[3] = client_message[5];

                    unsigned int i = 6, pl = 0;
                    for(; pl < payload_len; i++, pl++)
                     {
                       payload[pl] = client_message[i]^masking_key[pl % 4];
                     }
                    if(payload[0] == 'C'){
                        ManualControl(payload[1]);
                    }else if(payload[0] == 'A'){
                        unsigned int e=1;
                        std::string xPos, yPos, zPos;
                        while(payload[e] != '|'){
                            xPos += payload[e];
                            e++;
                        }
                        XYZMsg[0] = std::stod(xPos);
                        e++;
                        while(payload[e] != '|'){
                            yPos += payload[e];
                            e++;
                        }
                        XYZMsg[1] = std::stod(yPos);
                        e++;
                        while(payload[e] != '|'){
                            zPos += payload[e];
                            e++;
                        }
                        XYZMsg[2] = std::stod(zPos);
                        GoToPos();
                    }

                  } // END if < 126

                 else if(opcode == WS_TEXT_FRAME && payload_len == 126) // от клиента получен текст
                  {
                    unsigned char len16[2] = {0,};
                    unsigned int payload_len16 = 0;
                    len16[0] = client_message[2];
                    len16[1] = client_message[3];
                    payload_len16 = (len16[0] << 8) | len16[1]; // собираем длину сообщения

                    masking_key[0] = client_message[4];
                    masking_key[1] = client_message[5];
                    masking_key[2] = client_message[6];
                    masking_key[3] = client_message[7];

                    unsigned int i = 8, pl = 0;
                    for(; pl < payload_len16; i++, pl++)
                     {
                       payload[pl] = client_message[i]^masking_key[pl % 4];
                     }

                    printf("Payload_code: %d\n", client_message[1] & 0x7F);
                    printf("Payload_len16: %u\n", payload_len16);
                    printf("\nReciv TEXT_FRAME payload: %s\n", payload);
                  }

                 /*else if(opcode == WS_TEXT_FRAME && payload_len == 127) // от клиента получен текст
                  {
                    // text > 65535
                  }*/

                 else
                  {
                    //
                  }

        }
	}
/*
HTTP/1.0 200 OK\r\n
Content-Type: image/png\r\n
Content-Length: 7316\r\n
Connection: close\r\n\r\n
*/
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}

	//Free the socket pointer
//    closesocket(sock);
	free(socket_desc);
	return 0;
}
