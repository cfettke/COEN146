//UDP server
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

// Declare client address to fill in address of sender
struct sockaddr_in servAddr, clienAddr;

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

void printPacket(Packet packet) {
    printf("Packet{ header: { seq_ack: %d, len: %d, cksum: %d }, data: \"",
           packet.header.seq_ack,
           packet.header.len,
           packet.header.cksum);
    fwrite(packet.data, (size_t)packet.header.len, 1, stdout);
    printf("\")\n");
}

// Server sending ACK to the client
void serverSend(int sockfd, const struct sockaddr *address, socklen_t addrlen, int seqnum) {
    // Simulating a chance that ACK gets lost
    if (rand() % 5 == 0) { // this give a probability of 20% for a loss of ACK
        printf("Dropping ACK\n");
    } else {
        Packet packet;
        packet.header.seq_ack = seqnum;
        packet.header.len = sizeof(packet.data);
        packet.header.cksum = getChecksum(packet);
        // Send packet
        sendto(sockfd, &packet, sizeof(packet), 0, address, addrlen);
        printf("\t Server sending ACK %d, checksum %d\n", packet.header.seq_ack, packet.header.cksum);
    }
}

// Server receiving Packet from the client
Packet serverReceive(int sockfd, struct sockaddr *address, socklen_t *addrlen, int seqnum) {
    Packet packet;
    printf("bruh");
    while (1) {
        // Receive packets from the client
        recvfrom(sockfd, &packet, sizeof(packet), 0, address, addrlen);
        // Print received packet
        printPacket(packet);
        // Calculate the checksum
        int checksum = getChecksum(packet);
        // Check the checksum
        if (packet.header.cksum != checksum) {
            printf("It Server: Bad checksum, expected checksum was: %d\n", checksum);
            serverSend(sockfd, address, *addrlen, seqnum);
            // Check the sequence number
        } else if (packet.header.seq_ack != seqnum) {
            printf("It Server: Bad seqnum, expected sequence number was: %d\n", seqnum);
            // Resend packet
            serverSend(sockfd, address, *addrlen, seqnum);
            // If the checksum and sequence numbers are correct send ack with the right seq
        } else {
            printf("It Server: Good packet\n");
            // Send ack
            serverSend(sockfd, address, *addrlen, seqnum);
            break;
        }
    }
    return packet;
}

int main(int argc, char *argv[]) {
    // Get from the command line: server port and dst file
    if (argc != 3) {
        printf("Usage: %s <port> <dstfile>\n", argv[0]);
        exit(0);
    }

    // Declare socket file descriptor
    int sockfd;
    // Open a UDP socket, if successful, returns a descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("failure to setup an endpoint socket\n");
        exit(1);
    }

    // Setup the server address to bind using socket addressing structure
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[1]));
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Bind IP address and port for server endpoint socket
    if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) < 0) {
        perror("failure to bind server socket to endpoint socket\n");
        exit(1);
    }

    // Open file using argv[2]
    int fp = open(argv[2], O_CREAT | O_RDWR, 0666);
    if (fp < 0) {
        perror("File failed to open\n");
        exit(1);
    }

    // Initialize sequence number to 0
    int seqnum = 0;
    // Get file contents (as packets) from client
    Packet packet;
    socklen_t addr_len = sizeof(clienAddr);
    printf("Waiting for packets to come..... \n");
    do {
        packet = serverReceive(sockfd, (struct sockaddr *)&clienAddr, &addr_len, seqnum);
        write(fp, packet.data, packet.header.len);
        // Alternate the sequence number between 0 and 1
        seqnum = (seqnum + 1) % 2;
    } while (packet.header.len != 0);

    // Close file and socket descriptors
    close(fp);
    close(sockfd);
    return 0;
}