/*
** server.c -- a stream socket server demo
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define PORT "3490" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold

#define STUDENTS "students.txt"
#define PROFESSORS "professors.txt"
#define COURSES "courses.txt"
#define MAXDATASIZE 10000

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);
        errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int repeat_send(int fd, void *buffer, int size)
{
    char *string = (char*) buffer;

    while (size > 0)
    {
        int counter = send(fd, string, size, 0);

        if (counter < 1)
            return -1;

        string += counter;
        size -= counter;
    }
    return 1;
}

int send_func_login(int fd) {

    char string[130] = "Boas vindas ao Sistema de Disciplinas da UNICAMP\nSe deseja logar como professor, digite 1. Se deseja logar como aluno, digite 2.\n";
    char buffer[MAXDATASIZE];
    int numbytes;
    
    if (repeat_send(fd, string, sizeof(string)) == -1) {
        perror("send");
        return -1;

    }

    if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        return -1;
    }

    buffer[numbytes] = '\0';
    printf("server: received '%s'\n",buffer);

    char erro[27] = "Por favor, digite 1 ou 2.\n";

    while (strcmp(buffer, "1") != 0 && strcmp(buffer, "2") != 0) {

        if (repeat_send(fd, erro, sizeof(erro)) == -1) {
            perror("send");
            return -1;

        }

        if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            return -1;
        }


    }

    if ( strcmp(buffer, "1") == 0) {
        return 1;
    }
    else if ( strcmp(buffer, "2") == 0) {
        return 2;
    }

    return -1;
}
// int ementa(int fd) {

//     char string = "Digite o código da disciplina\n";
//     char buffer[MAXDATASIZE];
//     int numbytes;

//     if (send(fd, string, sizeof(string), 0) == -1) {
//         perror("send");
//         return -1;

//     }

//     if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
//         perror("recv");
//         return -1;
//     }

//     FILE* fp ;

//     while (fp = fopen(COURSES, "r")) {



//     }

//     return 0;
// }

// int infos(int fd) {

//     char string = "Digite o código da disciplina\n";
//     char buffer[MAXDATASIZE];
//     int numbytes;

//     if (send(fd, string, sizeof(string), 0) == -1) {
//         perror("send");
//         return -1;

//     }

//     if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
//         perror("recv");
//         return -1;
//     }


// }

// int todas_infos(int fd) {

// }

// int cod_titulo(int fd) {

// }

// int escrever_com(int fd) {

//     char string = "Digite o código da disciplina\n";
//     char buffer[MAXDATASIZE];
//     int numbytes;

//     if (send(fd, string, sizeof(string), 0) == -1) {
//         perror("send");
//         return -1;

//     }

//     if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
//         perror("recv");
//         return -1;
//     }


// }

// int ler_com(int fd) {

//     char string = "Digite o código da disciplina\n";
//     char buffer[MAXDATASIZE];
//     int numbytes;

//     if (send(fd, string, sizeof(string), 0) == -1) {
//         perror("send");
//         return -1;

//     }

//     if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
//         perror("recv");
//         return -1;
//     }

// }

// int send_login_prof(int fd) {

//     char string = "Olá!\nDigite o número da funcionalidade que deseja:\n1)Receber ementa de uma disciplina a partir do seu código\n
//   2)Receber todas as informações de uma disciplina a partir do seu código\n3)Listar todas as informações de todas as disciplinas\n
//   4)Listar todos os códigos de disciplinas com seus respectivos títulos\n
//   5)Escrever comentário sobre próxima aula de uma de suas disciplinas\n
//   6)Receber o comentário da próxima aula de uma disciplina a partir de seu código.\n";
//     char buffer[MAXDATASIZE];
//     int numbytes;

//     if (send(fd, string, sizeof(string), 0) == -1) {
//         perror("send");
//         return -1;

//     }


//     if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
//         perror("recv");
//         return -1;
//     }

//     char erro = "Por favor, digite números de 1 a 6.\n"

//     while (buffer != "1" && buffer != "2" && buffer != "3" && buffer != "4" && buffer != "5" && buffer != "6") {

//         if (send(fd, erro, sizeof(erro), 0) == -1) {
//             perror("send");
//             return -1;

//         }

//         if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
//             perror("recv");
//             return -1;
//         }

//     }

//     switch (buffer) {

//         case "1":
//             return ementa(fd);
//             break;
//         case "2":
//             return infos(fd);
//             break;
//         case "3":
//             return todas_infos(fd);
//             break;
//         case "4":
//             return cod_titulo(fd);
//             break;
//         case "5":
//             return escrever_com(fd);
//             break;
//         case "6":
//             return ler_com(fd);
//             break;

//     }

//     return 1;
// }

// int send_login_student(int fd) {

//     char string = "Olá!\nDigite o número da funcionalidade que deseja:\n1)Receber ementa de uma disciplina a partir do seu código\n
//   2)Receber todas as informações de uma disciplina a partir do seu código\n3)Listar todas as informações de todas as disciplinas\n
//   4)Listar todos os códigos de disciplinas com seus respectivos títulos\n
//   5)Receber o comentário da próxima aula de uma disciplina a partir de seu código.\n";
//     char buffer[MAXDATASIZE];
//     int numbytes;

//     if (send(fd, string, sizeof(string), 0) == -1) {
//         perror("send");
//         return -1;

//     }


//     if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
//         perror("recv");
//         return -1;
//     }

//     char erro = "Por favor, digite números de 1 a 5.\n"

//     while (buffer != "1" && buffer != "2" && buffer != "3" && buffer != "4" && buffer != "5") {

//         if (send(fd, erro, sizeof(erro), 0) == -1) {
//             perror("send");
//             return -1;

//         }

//         if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
//             perror("recv");
//             return -1;
//         }

//     }

//     switch (buffer) {

//         case "1":
//             return ementa(fd);
//             break;
//         case "2":
//             return infos(fd);
//             break;
//         case "3":
//             return todas_infos(fd);
//             break;
//         case "4":
//             return cod_titulo(fd);
//             break;
//         case "5":
//             return ler_com(fd);
//             break;

//     }

//     return 1;

// }

int validate_login_prof(int fd) {

    char string[51] = "Olá!\nDigite seu nome e senha, em linhas separadas.\n";
    char nome[MAXDATASIZE];
    char senha[MAXDATASIZE];
    int numbytes;

    if (repeat_send(fd, string, 51) == -1) {
        perror("send");
        return -1;
    }

    if ((numbytes = recv(fd, nome, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        return -1;
    }


    nome[numbytes] = '\0';
    printf("server: received '%s'\n",nome);

    if ((numbytes = recv(fd, senha, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        return -1;
    }


    senha[numbytes] = '\0';
    printf("server: received '%s'\n",senha);

    FILE* fp ;

    char line[255];
    char *string_nome;
    char *string_senha;

    if (fp = fopen(PROFESSORS, "r")) {

        while (fgets(line, 255, fp) ) {
            // string_nome = strtok(line, " ");
            // string_senha = strtok(line, "\n");

            if (strcmp(string_nome, nome) == 0 && strcmp(string_senha, senha) == 0 ) {
                printf("fadfasdfads");
                fclose(fp);
                return 1;
            }
        }

    }

    fclose(fp);
    return -1;

}
int validate_login_student(int fd) {

}

void send_func(int fd) {

    int login = send_func_login(fd);

    if (login == 1) {
        login = validate_login_prof(fd);
    
    }
    else if (login == 2) {
        login = validate_login_student(fd);

    }

    close(fd);
    exit(0);
}

int main(void) {
    int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
        p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
        sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    freeaddrinfo(servinfo); // all done with this structure
    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) { // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        inet_ntop(their_addr.ss_family,
        get_in_addr((struct sockaddr *)&their_addr),
        s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            

            // MUDAR AQUI
            send_func(new_fd);
            // if (send(new_fd, "Hello, world!", 13, 0) == -1)
            //     perror("send");

            // close(new_fd);
            // exit(0);
        }
        close(new_fd); // parent doesn't need this
    }
    return 0;
}







// CUIDAR DE BUF OVERFLOW E ERROS DE RETORNO