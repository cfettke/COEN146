//UDP client
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>

// Declare a Header structure that holds, sequence/acknowledgement number, checksum
typedef struct {
    int seq_ack;
    int len;
    int cksum;
} Header;

// Declare a packet structure that holds data and header
typedef struct {
    Header header;
    char data[10];
} Packet;

// Calculate the Checksum
int getChecksum(Packet packet) {
    packet.header.cksum = 0;
    int checksum = 0;
    char *ptr = (char *)&packet;
    char *end = ptr + sizeof(Header) + packet.header.len;
    while (ptr < end) {
        checksum ^= *ptr++;
    }
    return checksum;
}

// Client sending packet with checksum and sequence number, waits for acknowledgement
void clientSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, Packet packet, int retries) {
    while (1) {
        // If retries is greater than 3, we give up and move on
        if (retries >= 3) {
            break;
        }
        // Calculate checksum of packet
        packet.header.cksum = getChecksum(packet);
        // Simulate loss of a packet
        if (rand() % 5 == 0) { // Simulate a drop of packet (probability = 20%)
            printf("Dropping packet\n");
        } else {
            // Send the packet
            printf("Client sending packet\n");
            sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
        }

        // local variables needed
        struct timeval tv; // timer
        int rv; // select returned value
        // set up reads file descriptor at the beginning of the function to be checked for being ready to read
        fd_set readfds;
        fcntl (sockfd, F_SETFL, O_NONBLOCK);
        // start before calling select
        FD_ZERO (&readfds); //initializes readfds to have zero bits
        FD_SET (sockfd, &readfds); //sets readfds bit
        // set the timer
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        // call select () which returns the number of ready descriptors that are contained in the bit masks.
        // if the time limit expires, select returns zero and sets errno
        rv = select (sockfd + 1, &readfds, NULL, NULL, &tv);


         
        if (rv == 0) {
            printf("Timeout\n");
            // Increment retries if packet is dropped
            retries++;
        } else {
            // Receive an ACK from the server
            Packet recvpacket;
            recvfrom(sockfd, &recvpacket, sizeof(recvpacket), 0, (struct sockaddr *)address, &addrlen);
            // Print received packet (ACK) and checksum
            printf("Client received ACK %d, checksum %d\n", recvpacket.header.seq_ack, recvpacket.header.cksum);
            // Calculate checksum of received packet (ACK)
            int e_cksum = getChecksum(recvpacket);
            // Check the checksum
            if (recvpacket.header.cksum != e_cksum) {
                // Bad checksum, resend packet
                printf("Client: Bad checksum, expected checksum was: %d\n", e_cksum);
            } else if (recvpacket.header.seq_ack != packet.header.seq_ack) {
                // Incorrect sequence number, resend packet
                printf("Client: Bad seqnum, expected sequence number was: %d\n", packet.header.seq_ack);
            } else {
                // Good ACK, we're done
                printf("Client: Good ACK\n");
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // Get from the command line: server IP, Port and source file
    if (argc != 4) {
        printf("Usage: %s <ip> <port> <srcfile>\n", argv[0]);
        exit(0);
    }
    // Declare socket file descriptor.
    int sockfd;
    // Open a UDP socket, if successful, returns a descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Failed to setup endpoint socket. In");
        exit(1);
    } else {
        printf("Setup endpint\n");
    }

    // Declare server address to connect to
    struct sockaddr_in servAddr;
    struct hostent *host;
    host = (struct hostent *)gethostbyname(argv[1]);
    // Set the server address to send using socket addressing structure
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_port = htons(atoi(argv[2]));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr = *(struct in_addr *)host->h_addr;

    // Open file using argv[3]
    int fp = open(argv[3], O_RDWR);
    if (fp < 0) {
        perror("Failed to open file\n");
        exit(1);
    }

    // Send file contents to server packet by packet
    int seq = 0;
    socklen_t addr_len = sizeof(servAddr);
    printf("Length = %d\n", sizeof(servAddr));
    Packet packet;
    int bytes;

    while ((bytes = read(fp, packet.data, sizeof(packet.data))) > 0) {
        packet.header.seq_ack = seq;
        packet.header.len = bytes;
        packet.header.cksum = getChecksum(packet);
        clientSend(sockfd, (struct sockaddr *)&servAddr, addr_len, packet, 0);
        seq = (seq + 1) % 2;
        printf("                                seq number = %d\n", seq);
    }
    // Send zero-length packet to server to end connection
    Packet final;
    final.header.seq_ack = seq;
    final.header.len = 0;
    final.header.cksum = getChecksum(final);
    clientSend(sockfd, (struct sockaddr *)&servAddr, addr_len, final, 0);

    // Close file and socket
    close(fp);
    close(sockfd);
    return 0;
}