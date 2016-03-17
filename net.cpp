#include "net.h"

#include <err.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

// WolfSSL
#include <wolfssl/options.h>
#include <wolfssl/ssl.h>
#include <wolfssl/test.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// TODO reduce connection time-out to 1-5 sec

int load_ical_from_url(string &ical, const string &host, const string &URL)
{
	printf("INFO: Load schedule from %s\n", host.c_str());

	// FIXME just load from test file for now
	ifstream f("out.txt");
    string ln;
    ical="";
    bool v_cal_output = false;
    while(std::getline(f,ln))
    {
        if (ln.compare(0,13,"END:VCALENDAR") == 0)
        {
            ical += "END:VCALENDAR";
            break;
        }
        if (ln.compare(0,15,"BEGIN:VCALENDAR") == 0)
            v_cal_output = true;
        
        if(v_cal_output)
            ical += ln + "\n";
    }
    f.close();

	return NET_SUCCESS;

    int sock;
    int error_num;
    struct sockaddr_in saddr;
    struct addrinfo ai_hints, *ai_res, *ai_iter;
    
    char *url = "https://calendar.google.com/calendar/ical/04n0submlvfodumeo7ola6f90s%40group.calendar.google.com/private-b40357d4bfaee14d76ffaa65e910d554/basic.ics";
    char *hostname = "calendar.google.com";
    
    
    memset(&ai_hints, 0, sizeof ai_hints);
    ai_hints.ai_family = PF_UNSPEC;
    ai_hints.ai_socktype = SOCK_STREAM;
    error_num = getaddrinfo(hostname, "https", &ai_hints, &ai_res);
    if(error_num) {
        errx(1, "%s", gai_strerror(error_num));
    }
    
    for (ai_iter = ai_res; ai_iter; ai_iter = ai_iter->ai_next) {
        struct in_addr *addr;
        if (ai_iter->ai_family == AF_INET) {
            
            struct sockaddr_in *ipv = (struct sockaddr_in *)ai_iter->ai_addr;
            addr = &(ipv->sin_addr);
            
        }
        else
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ai_iter->ai_addr;
            addr = (struct in_addr *) &(ipv6->sin6_addr);
        }
        printf(" %s\n", inet_ntoa( *addr ));
    }
    
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock<0) {
        perror("Can't create socket\n");
        return -1;
    }
    
    error_num = connect(sock, ai_res->ai_addr, sizeof(sockaddr_in));
    if (error_num) {
        errx(1, "%s", gai_strerror(error_num));
    }

    freeaddrinfo( ai_res );
    
    /*char req[1024];
    sprintf( req, "GET %s\n\n", url);
    write(sock, req, strlen(req) );
    
    while(read(sock, req, 1024)) {
        printf("%s",req);
    }*/
    
    WOLFSSL_CTX* ctx;
    WOLFSSL* ssl;
    WOLFSSL_METHOD* method;
    
    /* initialize wolfssl library */
    wolfSSL_Init();
    
    method = wolfTLSv1_2_client_method(); /* use TLS v1.2 */
    
    /* make new ssl context */
    if ( (ctx = wolfSSL_CTX_new(method)) == NULL) {
        err_sys("wolfSSL_CTX_new error");
    }
    
    /* make new wolfSSL struct */
    if ( (ssl = wolfSSL_new(ctx)) == NULL) {
        err_sys("wolfSSL_new error");
    }
    
    //wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
    
    /* Add cert to ctx */
    /*if (wolfSSL_CTX_load_verify_locations(ctx, "GeoTrust_Global_CA.pem", 0) != SSL_SUCCESS) {
        err_sys("Error loading GeoTrust_Global_CA.pem");
    }*/
    if (wolfSSL_CTX_load_verify_locations(ctx, "cacert.pem", 0) != SSL_SUCCESS) {
        err_sys("Error loading cacert.pem");
    }
    
    char message[1024];
    sprintf( message, "GET %s\n\n", url);

    
    /* Connect wolfssl to the socket, server, then send message */
    wolfSSL_set_fd(ssl, sock);
    if(wolfSSL_connect(ssl) != SSL_SUCCESS)
    {
        int err = wolfSSL_get_error(ssl, 0);
        printf("WolfSSL error connect: %d\n", err);
        char buffer[1024];
        wolfSSL_ERR_error_string(err, buffer);
        printf("%s\n", buffer);
    }
    
    if(wolfSSL_write(ssl, message, strlen(message)) != strlen(message))
    {
        printf("WolfSSL error write: %d\n", wolfSSL_get_error(ssl, 0));
    }
    
    int sel_val = tcp_select(sock, 5);
    if(sel_val == TEST_RECV_READY)
        while(wolfSSL_read(ssl, message, 1023) > 0)
            printf("%s", message);
    
    /* frees all data before client termination */
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();
    
    
    close(sock);
}