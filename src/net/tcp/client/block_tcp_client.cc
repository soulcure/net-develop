#include "block_tcp_client.h"

BlockTcpClient::BlockTcpClient()
{

}


#include "log_util.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <errno.h>
#define BUFF_MAX 1024*10
#define BUFF_LENGTH 1024*5


BlockTcpClient::~BlockTcpClient(){
    if (sockfd != -1){
        close(sockfd);
    }
}

int BlockTcpClient::Read(PDUBase &base){
    int total_length = 0 ;
    char total_buffer[BUFF_MAX] ;
    char buf[BUFF_LENGTH];
   // PDUBase base ;

    while(true){
        int len = 0;
        memset( buf, 0, BUFF_LENGTH );
        len = read( sockfd, buf, BUFF_LENGTH);

        LOGD("recv %d", len );
        if(len <= 0){
            if (len < 0){
                char * mesg = strerror(errno);
                printf("NetClient Mesg:%s\n",mesg);
            }
            else{
                LOGD("NetClient disconnect.\n ");
            }
            /*
             * closed
             * finish reading block..
             */
            close(sockfd);
            sockfd = -1;
            return -1 ;
        }

        if (total_length+len > BUFF_MAX){
            total_length = 0;
            continue;
        }
        memcpy(total_buffer+total_length, buf, len);
        total_length += len ;
        int readed_size = 0 ;
        while( (readed_size = OnPduParse(total_buffer,total_length, base) ) > 0 ){
            //remove readed data.
            memmove(total_buffer, total_buffer+readed_size, total_length-readed_size);
            total_length -= readed_size ;
//            OnRecv(base);
//            return base;
            return 0;
        }

        printf("Received message!!: %s\n", buf);
        usleep(1);
    }
    return -1;
}


int BlockTcpClient::Connect(const char *_ip, int _port){
    this->ip = _ip ;
    this->port = _port ;
    struct sockaddr_in sad;

    LOGT("TCPClient connect\n");
    memset((char*)&sad, 0, sizeof(sad));
    sad.sin_family = AF_INET;
    sad.sin_port = htons( (u_short)_port );
    inet_aton( _ip, &sad.sin_addr );

    sockfd = socket( PF_INET, SOCK_STREAM, 0 );

    #ifdef DEBUG
    printf("Connecting to [%s]:[%d]...\n\n", _ip, _port);
    #endif
    struct timeval oldtime;
    socklen_t len = 0;
    getsockopt(sockfd,SOL_SOCKET, SO_SNDTIMEO, &oldtime, &len);


    //setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &newtime, (socklen_t)sizeof(newtime));
    int result = connect(sockfd, (struct sockaddr*)&sad, sizeof(sad));

    LOGT("TCPClient Running %d\n", result);

    if (result != 0){
        close(sockfd);
        sockfd = -1;
        return result;
    }

    return 0;

}
int BlockTcpClient::SendProto(::google::protobuf::Message &_msg,  int _commandid,int _seq_id){
    PDUBase pdu_base ;
    std::shared_ptr<char> body(new char[_msg.ByteSize()]);
    _msg.SerializeToArray(body.get(), _msg.ByteSize());
    pdu_base.body = body ;
    pdu_base.length = _msg.ByteSize();
    pdu_base.command_id = _commandid ;
    pdu_base.seq_id = _seq_id ;
print_pdu(pdu_base);
    Send(pdu_base);
}



int BlockTcpClient::Send(PDUBase &_base){
    if(sockfd<0)
        return -1;
    //build package.
    char * buf= NULL;
    int len = OnPduPack(_base, buf);
    if(len<=0)
        return -3;

    int totallen = 0;
    int resendnum = 0;              //发送失败当前重发次数

    //若缓冲区满引起发送不完全时，需要循环发送直至数据完整
    while (totallen < len)
    {
            int write_len =write(sockfd, buf+totallen, len-totallen);
            if (write_len <= 0)
            {
                //...重发
                resendnum++;
                if (resendnum >= resendnumlimit)
                {
                    //超过重发次数限制后，返回错误
                    free(buf);
                    return -2;
                }

                usleep(2000);
                continue;
            }

        totallen += write_len;
    }

    LOGT("TCP Send Data Out.");

    free(buf);
    return totallen;

}








