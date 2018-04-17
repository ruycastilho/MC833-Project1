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
#include <sys/time.h>
#include "server_functionalities.h"

#define PORT "4000" 

struct timeval tv1, tv2;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int send_ack(int sockfd) {

    char ack[] = "ack";
    if (send(sockfd, ack, sizeof(ack), 0) == -1) {
        perror("send");
        exit(1);
    }
    return 1;
}


void choose_opt(int sockfd) {
    char buf[MAXDATASIZE];
    int numbytes;
    char choice[2];

    do {

        send_ack(sockfd);

        // receive login menu
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("%s",buf);

        printf("Opcao: ");
        scanf("%s", choice);
        choice[1] = '\0';

        // send selected option
        if (send(sockfd, choice, sizeof(choice), 0) == -1) {
            perror("send");
            return;
        }

        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("%s",buf);

    } while (strcmp(buf, "\nPor favor, digite 1 ou 2.\n") == 0);

}

user* input_user_and_pass (int sockfd) {
    char buf[MAXDATASIZE];
    int numbytes;

    send_ack(sockfd);

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);

    }
    buf[numbytes] = '\0';
    printf("%s",buf);

    printf("Nome: ");
    char username[NAME_LENGTH];
    scanf("%s", username);

    printf("Senha: ");
    char password[PWD_LENGTH];
    scanf("%s", password);

    user* new_user = (user*)malloc(sizeof(user));
    strcpy(new_user->name, username);
    strcpy(new_user->pwd, password);

    // send struct with user info
    if (send(sockfd, new_user, sizeof(user), 0) == -1) {
        perror("send");
        free(new_user);
        return NULL;
    }

    // receive the status
    int status;
    if ((numbytes = recv(sockfd, &status, sizeof(int), 0)) == -1) {
        perror("recv");
        exit(1);
    }

    // send message acknowledgement
    send_ack(sockfd);
    free(new_user);

    user* client = (user*)malloc((sizeof(user)));
    if (status) {
        if ((numbytes = recv(sockfd, client, sizeof(user), 0)) == -1) {
            perror("recv");
            exit(1);

        }

        // send message acknowledgement
        send_ack(sockfd);


    }

    else {
        client = NULL;
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);

        }
    
        buf[numbytes] = '\0';
        printf("%s",buf);

    }

    return client;
}

int interface_codigo(int sockfd) {

    char buf[MAXDATASIZE];
    int numbytes, status;

    // receive 'digite o codigo da disciplina:'
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    printf("%s", buf);

    printf("Codigo: ");

    char subj_name[6];
    scanf("%s", subj_name);
    subj_name[5] = '\0';

    
    // start the timer
    gettimeofday(&tv1, NULL);

    // sends the code value
    if (send(sockfd, subj_name, sizeof(subj_name), 0) == -1) {
        perror("send");
        return -1;
    }

    // receive the status
    if ((numbytes = recv(sockfd, &status, sizeof(int), 0)) == -1) {
        perror("recv");
        exit(1);
    } 

    send_ack(sockfd);

    return status;

}

void interface_ementa(int sockfd) {
    interface_codigo(sockfd);

    int numbytes;
    course* received_course = (course*)malloc(sizeof(course));

    //receive the subject's description
    if ((numbytes = recv(sockfd, received_course, sizeof(course), 0)) == -1) {
        perror("recv");
        exit(1);
    }

    gettimeofday(&tv2, NULL);
    printf("\nEmenta: %s\n", received_course->description);

    printf("Tempo total da operação: %.2f usecs\n", (float)(tv2.tv_usec - tv1.tv_usec));

    send_ack(sockfd);
    free(received_course);

}

void interface_infos(int sockfd) {
    course* received_course = (course*)malloc(sizeof(course));
    int numbytes;
    char buf[MAXDATASIZE];

    if (interface_codigo(sockfd)) {

        if ((numbytes = recv(sockfd, received_course, sizeof(course), 0)) == -1) {
            perror("recv");
            exit(1);
        } 

        gettimeofday(&tv2, NULL);

        printf("\nCodigo: %s\nTitulo: %s\nInstituto: %s\nSala: %s\nHorario: %s\nEmenta: %s\nProfessor: %s\nComentario: %s\n", 
                received_course->code, received_course->name, received_course->institute,
                received_course->room, received_course->schedule, received_course->description,
                received_course->professor, received_course->comment);


        printf("Tempo total da operação: %.2f usecs\n", (float)(tv2.tv_usec - tv1.tv_usec));    }
    
    else {
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0';
        printf("%s",buf);

    }

    free(received_course);
} 

void interface_todas_infos(int sockfd) {
    course* received_course = (course*)malloc(sizeof(course));
    int numbytes, status;
    char buf[MAXDATASIZE];

    // receive the status
    if ((numbytes = recv(sockfd, &status, sizeof(int), 0)) == -1) {
        perror("recv");
        exit(1);
    } 

    send_ack(sockfd);

    if (status) {

        // receive the status
        if ((numbytes = recv(sockfd, &status, sizeof(int), 0)) == -1) {
            perror("recv");
            exit(1);
        } 

        send_ack(sockfd);

        while (status) {

            if ((numbytes = recv(sockfd, received_course, sizeof(course), 0)) == -1) {
                perror("recv");
                exit(1);
            } 

            printf("\nCodigo: %s\nTitulo: %s\nInstituto: %s\nSala: %s\nHorario: %s\nEmenta: %s\nProfessor: %s\nComentario: %s\n", 
                    received_course->code, received_course->name, received_course->institute,
                    received_course->room, received_course->schedule, received_course->description,
                    received_course->professor, received_course->comment);

            send_ack(sockfd);

            // receive the status
            if ((numbytes = recv(sockfd, &status, sizeof(int), 0)) == -1) {
                perror("recv");
                exit(1);
            } 
            
            send_ack(sockfd);

        }
        gettimeofday(&tv2, NULL);
        printf("Tempo total da operação: %.2f usecs\n", (float)(tv2.tv_usec - tv1.tv_usec));



    }
    else {
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0';
        printf("%s",buf);

        send_ack(sockfd);

    }

    free(received_course);
}

void interface_cod_titulos(int sockfd) {
    course* received_course = (course*)malloc(sizeof(course));
    int numbytes, status;
    char buf[MAXDATASIZE];

    // receive the status
    if ((numbytes = recv(sockfd, &status, sizeof(int), 0)) == -1) {
        perror("recv");
        exit(1);
    } 

    send_ack(sockfd);


    if (status) {

        // receive the status
        if ((numbytes = recv(sockfd, &status, sizeof(int), 0)) == -1) {
            perror("recv");
            exit(1);
        } 

        send_ack(sockfd);


        while (status) {

            if ((numbytes = recv(sockfd, received_course, sizeof(course), 0)) == -1) {
                perror("recv");
                exit(1);
            } 

            printf("\nCodigo: %s\nTitulo: %s\n", 
                    received_course->code, received_course->name);


            send_ack(sockfd);

            // receive the status
            if ((numbytes = recv(sockfd, &status, sizeof(int), 0)) == -1) {
                perror("recv");
                exit(1);
            } 

            send_ack(sockfd);

        }

        gettimeofday(&tv2, NULL);
        printf("Tempo total da operação: %.2f usecs\n", (float)(tv2.tv_usec - tv1.tv_usec));

    }
    else {
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0';
        printf("%s",buf);

        send_ack(sockfd);


    }

    free(received_course);
}

void interface_ler_com(int sockfd) {
    interface_codigo(sockfd);

    int numbytes;
    course* received_course = (course*)malloc(sizeof(course));

    //receive the subject's comment
    if ((numbytes = recv(sockfd, received_course, sizeof(course), 0)) == -1) {
        perror("recv");
        exit(1);
    }

    gettimeofday(&tv2, NULL);

    printf("\nComentario: %s\n", received_course->comment);
    free(received_course);

    printf("Tempo total da operação: %.2f usecs\n", (float)(tv2.tv_usec - tv1.tv_usec));
}

void interface_esc_com(int sockfd) {

    char buf[MAXDATASIZE];
    int numbytes;

    if (interface_codigo(sockfd)) {

        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0';
        printf("%s",buf);

        char comment[COMMENT_LENGTH];
        scanf("\n");
        fgets(comment, sizeof(comment), stdin);

        for(int i = COMMENT_LENGTH-1; i >= 0; i--) {
            if (comment[i] == '\n')
                comment[i] = '\0';
        }

        gettimeofday(&tv1, NULL);

        if (send(sockfd, comment, sizeof(comment), 0) == -1) {
            perror("send");
            return;
        }


        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        gettimeofday(&tv2, NULL);

        buf[numbytes] = '\0';
        printf("%s",buf);

        printf("Tempo total da operação: %.2f usecs\n", (float)(tv2.tv_usec - tv1.tv_usec));

        send_ack(sockfd);

    }
    else {
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0';
        printf("%s",buf);
        
    }
 

}    

void interface(int sockfd) {

    char buf[MAXDATASIZE];
    user *client;
    int numbytes;

    do {

        // send selected option and receive 'digite seu nome e senha em linhas separadas'
        choose_opt(sockfd);

        // send username and password 
        client = input_user_and_pass(sockfd);

    } while ( client == NULL );


    while (1) {

        send_ack(sockfd);

        // receive options menu
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            free(client);   
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("%s",buf);
        
        printf("Opcao: ");
        char selected_option[2];
        scanf("%s", selected_option);
        selected_option[1] = '\0';

        gettimeofday(&tv1, NULL);

        // send selected menu option
        if (send(sockfd, selected_option, sizeof(selected_option), 0) == -1) {
            perror("send");
            free(client);
            return;
        }

        switch (selected_option[0] - '0') {

            case 1:
                interface_ementa(sockfd);
                break;
            case 2:
                interface_infos(sockfd);
                break;
            case 3:
                interface_todas_infos(sockfd);
                break;
            case 4:
                interface_cod_titulos(sockfd);
                break;
            case 5:
                interface_ler_com(sockfd);
                break;
            case 6:
                if (client->is_prof) {
                    interface_esc_com(sockfd);

                }
                else {
                    free(client);
                    return;
                }
                break;
            case 7:
                if (client->is_prof) {
                    free(client);
                    return;

                }

        }

    }

}

int main(int argc, char *argv[]) {
    int sockfd, numbytes;
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

    interface(sockfd);
    close(sockfd);

    return 0;
}
