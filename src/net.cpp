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

using namespace std;

// TODO reduce connection time-out to 1-5 sec

// Do not send Since-Modified if mod_since == 0
int load_ical_from_url(string &ical, const string &host, const string &URL, time_t mod_since)
{
	printf("INFO: Load schedule from %s\n", host.c_str());

    int sock;
    int error_num;
    struct sockaddr_in saddr;
    struct addrinfo ai_hints, *ai_res, *ai_iter;
    
    //const char *url = "https://calendar.google.com/calendar/ical/04n0submlvfodumeo7ola6f90s%40group.calendar.google.com/private-b40357d4bfaee14d76ffaa65e910d554/basic.ics";
    //const char *hostname = "calendar.google.com";

    const char *url = URL.c_str();
    const char *hostname = host.c_str();

    // Build HTTP request
    char message[1024];

    // Build HTTP request
    // FIXME Issue#6 Turned off "mod_since" feature as google calendar doens't seem to support it
    if (false) // (mod_since) 
    {
        // Add If-Modified-Since: header
        // Sat, 29 Oct 1994 19:43:31 GMT
        char t[256];
        strftime(t, 255, "If-Modified-Since: %a, %d %b %Y %H:%M:%S GMT", gmtime(&mod_since));
        snprintf( message, sizeof message, "GET %s\nHost: %s\n%s\n\n", url, hostname, t);
    }
    else
        snprintf( message, sizeof message, "GET %s\n\n", url);

    cout << message << endl;
    //exit(-1);
    
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
    if (wolfSSL_CTX_load_verify_locations(ctx, "./cacert.pem", 0) != SSL_SUCCESS) {
        err_sys("Error loading cacert.pem");
    }

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

    string http_text="";
    int r_len;
    if(sel_val == TEST_RECV_READY)
        while( (r_len=wolfSSL_read(ssl, message, 1023)) > 0)
        {
            message[r_len] = 0;
        	http_text += string(message);
            //printf("%s", message);
        }
    
    /* frees all data before client termination */
    wolfSSL_free(ssl);
    wolfSSL_CTX_free(ctx);
    wolfSSL_Cleanup();
    
    close(sock);

    // Check if 304 return - NOT_CHANGED
    //cout << http_text << endl;
    //exit(-1);

    // Simple parsing to extract iCal only data
    stringstream ss(http_text);
    ical="";
    string ln;
    bool v_cal_output = false;
    while(std::getline(ss,ln))
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


    return NET_SUCCESS;
}

