//COEN 146L : Lab2, step 4
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

    if (argc != 2) { // check correct usage
        fprintf(stderr, "usage: %s <n connections>\n", argv[0]);
        exit(1);
    }

    //Declare and initialize values for n (n connections), np (np parallel connections), RTTO, RTT1, RTT2, RTTHTTP, RTTDNS
    int n = atoi(argv[1]); // num connections
    int np = 0;

    if(n > 6) {
        //if num objects is greater than 6, then number parallel con. = 1
        np = 1;
    } else {
        //otherwise if there's 6 or fewer, divide by 6 to get num parallel con.
        np = 6 / n;
    }

    //Check if the num objects is less than 6 and doesn't evenly divide by 6; then you need to add 1 more parallel
    if(n < 6 && (6 % n != 0)) {
        np += 1;
    }

    int RTTO = 3;      // msecs
    int RTT1 = 20;
    int RTT2 = 26;
    int RTTHTTP = 47;
    int RTTDNS = RTTO + RTT1 + RTT2;

    // For one object, you do the round trip time for HTTP + two of the RTTHTTP since it involves 2 trips
    // (basically sum of DNS lookup time + 2x round trip HTTP)
    printf("One object: %d msec\n", RTTDNS + (2 * RTTHTTP));

    // Same thing as above, but there are 6 separate round trips for each object
    // So it's the DNS lookup time, 2x round trip of HTTP, then round trip time for 6 objects
    printf("Non-Persistent 6 objects: %d msec\n", RTTDNS + (2 * RTTHTTP + 2 * 6 * RTTHTTP));

    int timel = RTTDNS + (2 * RTTHTTP) + (np * RTTHTTP);
    int time2 = RTTDNS + (2 * RTTHTTP) + (np * 2 * RTTHTTP);

    printf("%d parallel connection - Persistent: %d msec\n", n, timel);
    printf("%d parallel connection - Non-Persistent: %d msec\n", n, time2);

    return 0;
}
