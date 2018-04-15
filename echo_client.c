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
#include "server_functionalities.h"

#define PORT "3494" 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
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
        char choice[2];
        scanf("%s", choice);
        choice[1] = '\0';

        if (send(sockfd, choice, sizeof(choice), 0) == -1) {
            perror("send");
            return -1;
        }

        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("server: received '%s'\n",buf);

    } while (strcmp(buf, "Por favor, digite 1 ou 2.\n") == 0);

    return 0;
}

user* input_user_and_pass (int sockfd) {
    char buf[MAXDATASIZE];
    int numbytes;

    char username[NAME_LENGTH];
    scanf("%s", username);
    
    // fgets(username, sizeof(username), stdin);
    for (int i = sizeof(username); i > 0; i--) {
        if (username[i] == '\n') {
            username[i] = '\0';
        }
    }

    char password[PWD_LENGTH];
    scanf("%s", password);

    for (int i = sizeof(password); i > 0; i--) {
        if (password[i] == '\n') {
            password[i] = '\0';
        }
    }

    user* new_user = malloc(sizeof(user));
    strcpy(new_user->name, username);
    strcpy(new_user->pwd, password);

    printf("user: '%s' '%s'\n", new_user->name, new_user->pwd);

    if (send(sockfd, new_user, sizeof(user), 0) == -1) {
        perror("send");
        free(new_user);
        return NULL;

    }


    // free(new_user);
    return new_user;
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

    user *client;

    do {

        printf("esperando mensagem\n");

        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0';
        printf("client: received '%s'\n",buf);

        int choose_opt_ret = choose_opt(sockfd);

        client = input_user_and_pass(sockfd);

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

        char selected_option[2];
        scanf("%s", selected_option);
        selected_option[1] = '\0';

        if (send(sockfd, selected_option, sizeof(selected_option), 0) == -1) {
            perror("send");
            return -1;
        }

        if ( (strcmp(selected_option, "7") == 0  && client->is_prof ) || (strcmp(selected_option, "6") == 0  && !client->is_prof) ) {
            break;
        }

        else if (strcmp(selected_option, "1") == 0) {
            if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
                perror("recv");
                exit(1);
            }
            buf[numbytes] = '\0';
            printf("client: received '%s'\n",buf);


            char subj_name[6];
            scanf("%s", subj_name);
            subj_name[5] = '\0';

            if (send(sockfd, subj_name, sizeof(subj_name), 0) == -1) {
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

    free(client);
    close(sockfd);

    return 0;
}