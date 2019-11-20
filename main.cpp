#include <iostream>
#include <netinet/in.h>
#include <strings.h>
#include <libnet.h>
#include <pthread.h>

using namespace std;

#define BUFFER_SIZE 65536
#define SEND_SIZE   96000*4*8
#define SERVER_PORT 33333
char server_ip_address[] = "192.168.1.250";

unsigned char recv_buff[BUFFER_SIZE];
void * socket_receive(void *arg){
    int sock_id = *((intptr_t*)arg);
    cout << "receive thread stand up." << endl;
    while(-1 != sock_id) {
        memset(recv_buff, 0, BUFFER_SIZE);
        int status = ::recv(sock_id, recv_buff, BUFFER_SIZE, 0);
        if(-1 == status)
            cout <<"error: status == -1, errno = " << errno << "in recv thread" << endl;
        else{
            recv_buff[status] = '\0';
            cout << "RECEIVED " << status << " BYTES: " << recv_buff << endl;
        }
    }
    cout << "Server disconnected, quit receive thread" << endl;
    return 0;
}
int main(int argc, char* argv[]) {
    int sock_id = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock_id = ::socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cout << "Socket creating error!" << std::endl;
        return -1;
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, server_ip_address, &serv_addr.sin_addr)<=0)
    {
        std::cout << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    if (::connect(sock_id, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cout << "Connection Failed" << std::endl;
        return -1;
    }
    cout << "Server " << server_ip_address << ":" << SERVER_PORT << " connected." << endl;
    cout << "Creating receive thread ......" << endl;
    pthread_t pthread_receive;
    if (0 != pthread_create(&pthread_receive, 0, socket_receive, &sock_id)){
        cout << "create receive thread error!" << endl;
    }
    char hello[] = "Hello from client.";
    ::send(sock_id , hello , strlen(hello) , 0 );
    cout << "Sending " << SEND_SIZE << " random bytes to server ......" << endl;
    unsigned char* send_buff = new unsigned char[SEND_SIZE];
    for(int i=0; i<SEND_SIZE; i++) {
        send_buff[i] = rand();
    }
    int send_size = 0;
    int delay_time = 0;
    int size_once = 8000;
    if(argc >= 3){
        size_once = atoi(argv[2]);
    }
    if(argc >= 2){
        delay_time = atoi(argv[1]);
    }
    if (0 == delay_time) {
        send_size = ::send(sock_id, send_buff, SEND_SIZE, 0);
    }else{
        for(int i=0; i<SEND_SIZE/size_once; i++){
            send_size += ::send(sock_id, send_buff+i*size_once, size_once, 0);
            usleep(1000*delay_time);
        }
    }
    if(SEND_SIZE == send_size)
        cout << "All bytes send" << endl;
    else
        cout << "Only send " << send_size << " bytes" << endl;
    cout << "waiting 5s ......" << endl;
    sleep(5);
    delete[] send_buff;
    cout << "Close socket" << endl;
    ::close(sock_id);
    return 0;
}