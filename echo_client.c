/*
** client.c -- a stream socket client demo
*/

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "3494" // the port client will be connecting to
#define MAXDATASIZE 1000 // max number of bytes we can get at once

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void receive_func() {
    

}

int choose_opt (int sockfd) {
    char buf[MAXDATASIZE];
    int numbytes;

    do {
        char choice[10];
        fgets(choice, 10, stdin);
        for (int i = 10; i > 0; i--) {
            if (choice[i] == '\n') {
                choice[i] = '\0';
                break;
            }
        }
        if (send(sockfd, choice, 10, 0) == -1) {
            perror("send");
            return -1;
        }

        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("server: received '%s'\n",buf);

    } while (strcmp(buf, "Por favor, digite 1, 2 ou 3.\n") == 0);

    return 0;
}

int input_user_and_pass (int sockfd) {
    char buf[MAXDATASIZE];
    int numbytes;

    char username[32];
    fgets(username, 32, stdin);
    for (int i = 32; i > 0; i--) {
        if (username[i] == '\n') {
            username[i] = '\0';
        }
    }
    printf("username: '%s'\n", username);
    if (send(sockfd, username, 32, 0) == -1) {
        perror("send");
        return -1;

    }

    char password[32];
    fgets(password, 32, stdin);
    for (int i = 32; i > 0; i--) {
        if (password[i] == '\n') {
            password[i] = '\0';
        }
    }
    printf("password: '%s'\n", password);
    if (send(sockfd, password, 32, 0) == -1) {
        perror("send");
        return -1;

    }

    return 0;
}

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    
    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
    s, sizeof s);

    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); // all done with this structure

    // -----------------------------------------------------------------------------------------------------------------------------------------

    do {

        printf("esperando mensagem\n");

        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0';
        printf("client: received '%s'\n",buf);

        int choose_opt_ret = choose_opt(sockfd);

        int input_user_and_pass_ret = input_user_and_pass(sockfd);

        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0';
        printf("client: received '%s'\n",buf);

    } while (strcmp(buf, "Erro na validacao.\n") == 0);


    int first_time = 0;
    for (;;) {

        if (first_time == 1) {
            printf("waiting for msg\n");
            if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
                perror("recv");
                exit(1);
            }

            buf[numbytes] = '\0';
            printf("client: received '%s'\n",buf);
        }

        char selected_option[32];
        fgets(selected_option, 32, stdin);
        for (int i = 32; i > 0; i--) {
            if (selected_option[i] == '\n') {
                selected_option[i] = '\0';
            }
        }
        if (send(sockfd, selected_option, 32, 0) == -1) {
            perror("send");
            return -1;
        }

        if (strcmp(selected_option, "7") == 0) {
            ;
        }
        else if (strcmp(selected_option, "1") == 0) {
            if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            buf[numbytes] = '\0';
            printf("client: received '%s'\n",buf);


            char subj_name[32];
            fgets(subj_name, 32, stdin);
            for (int i = 32; i > 0; i--) {
                if (subj_name[i] == '\n') {
                    subj_name[i] = '\0';
                }
            }
            if (send(sockfd, subj_name, 32, 0) == -1) {
                perror("send");
                return -1;
            }

            if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            buf[numbytes] = '\0';
            printf("client: received '%s'\n",buf);
        }

        first_time = 1;
    }

    close(sockfd);

    return 0;
}