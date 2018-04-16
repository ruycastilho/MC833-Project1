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

void choose_opt(int sockfd) {
    char buf[MAXDATASIZE];
    int numbytes;
    char choice[2];

    do {
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
        printf("%s\n",buf);

    } while (strcmp(buf, "Por favor, digite 1 ou 2.\n") == 0);

}

user* input_user_and_pass (int sockfd) {
    char buf[MAXDATASIZE];
    int numbytes;

    printf("Nome: ");
    char username[NAME_LENGTH];
    scanf("%s", username);

    printf("Senha: ");
    char password[PWD_LENGTH];
    scanf("%s", password);

    user* new_user = (user*)malloc(sizeof(user));
    strcpy(new_user->name, username);
    strcpy(new_user->pwd, password);

    // printf("user: '%s' '%s'\n", new_user->name, new_user->pwd);

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
    // printf("status: %d\n", status);

    // send message acknowledgement
    if (send(sockfd, "ack", 4, 0) == -1) {
        perror("send");
        free(new_user);
        return NULL;
    }

    if (status) {
        if ((numbytes = recv(sockfd, new_user, sizeof(user), 0)) == -1) {
            perror("recv");
            exit(1);

        }

        // send message acknowledgement
        if (send(sockfd, "ack", 4, 0) == -1) {
            perror("send");
            free(new_user);
            return NULL;
        }

    }

    else {
        new_user = NULL;

    }

    return new_user;
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
    // printf("%d\n", status);

    // send an acknowledgement
    if (send(sockfd, "ack", 4, 0) == -1) {
        perror("send");
        exit(1);
    }

    return status;

}

void interface_ementa(int sockfd) {
    interface_codigo(sockfd);

    char buf[MAXDATASIZE];
    int numbytes;

    //receive the subject's description
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';
    printf("\nEmenta: %s\n",buf);


    // send an acknowledgement
    if (send(sockfd, "ack", 4, 0) == -1) {
        perror("send");
        exit(1);
    }


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

        printf("\nCodigo: %s\nTitulo: %s\nInstituto: %s\nSala: %s\nHorario: %s\nEmenta: %s\nProfessor: %s\nComentario: %s\n", 
                received_course->code, received_course->name, received_course->institute,
                received_course->room, received_course->schedule, received_course->description,
                received_course->professor, received_course->comment);

    }
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

    // precisa do header pra esse
}

void interface_cod_titulos(int sockfd) {
    // precisa do header pra esse

}

void interface_ler_com(int sockfd) {
    interface_codigo(sockfd);

    char buf[MAXDATASIZE];
    int numbytes;

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';
    printf("\nComentario: %s\n",buf);

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

        if (send(sockfd, comment, sizeof(comment), 0) == -1) {
            perror("send");
            return;
        }


        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0';
        printf("%s",buf);


        // send an acknowledgement
        if (send(sockfd, "ack", 4, 0) == -1) {
            perror("send");
            exit(1);
        }


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

        // printf("esperando menu de login\n");

        // ajeitar aqui
        // receive login menu
        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        printf("%s",buf);

        // send selected option and receive 'digite seu nome e senha em linhas separadas'
        choose_opt(sockfd);
        // send username and password 
        client = input_user_and_pass(sockfd);

    } while ( client == NULL );


    while (1) {

        // printf("esperando menu de opcoes\n");
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
                free(client);
                return;

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

// erros: quando imprime todas infos de uma disciplina, codigo e titulo tao zuados
// acho que eh porque uma parte da mensagem ta sumindo.
// eu arrumei muita coisa mas falta o header, basicamente
