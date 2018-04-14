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
#include "server_functionalities.h"

char login_nome[255];

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


int repeat_send(int fd, const void *buffer, int size)
{
    char *string = (char*) buffer;

    while (size > 0)
    {
        int counter = send(fd, string, size, 0);
        usleep(3000000);
        if (counter < 1)
            return -1;

        string += counter;
        size -= counter;
    }
    return 1;
}

int send_func_login(int fd) {

    char string[] = "Boas vindas ao Sistema de Disciplinas da UNICAMP\nSe deseja logar como professor, digite 1. Se deseja logar como aluno, digite 2. Se deseja sair, digite 3.\n";
    char buffer[MAXDATASIZE];
    int numbytes;

    if ((numbytes = send(fd, string, sizeof(string), 0)) == -1) { // trocando repeat_send por send para fins de testeaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
        perror("send");
        return -1;

    }
    printf("ja dei boas vindas ao sistema, %d bytes mandados\n", numbytes);

    if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        return -1;
    }

    buffer[numbytes] = '\0';
    printf("server: received '%s'\n",buffer);

    char erro[] = "Por favor, digite 1, 2 ou 3.\n";
    while (strcmp(buffer, "1") != 0 && strcmp(buffer, "2") != 0 && strcmp(buffer, "3") != 0) {
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

int ementa(int fd) {

    char string[] = "Digite o código da disciplina\n";
    char buffer[MAXDATASIZE];
    int numbytes;

    if (send(fd, string, sizeof(string), 0) == -1) {
        perror("send");
        return -1;

    }

    if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        return -1;
    }

    printf("received '%s'\n", buffer);

    FILE* fp ;
    char input[255];

    if (fp = fopen(COURSES, "r")) {
        // printf("dsadasda\n");

        while (fscanf(fp,"%s", input) ) {
            // printf("%s %s\n", string_nome, string_senha);

            if (strcmp(input, "[CÓDIGO]") == 0  ) {
                fscanf(fp,"%s", input);

                if (strcmp(input, buffer) == 0) {

                    while (fscanf(fp,"%s", input) ) {

                        if (strcmp(input, "[EMENTA]") == 0  ) {
                            fgets(input, 255, fp);
                            fgets(input, 255, fp);

                                printf("preparendo para mandar e ementa\n");
                                if (repeat_send(fd, input, sizeof(input)) == -1) {
                                    perror("send");
                                    return 1;
                                }
                                printf("ementa enviada\n");

                                fclose(fp);
                                return 1;

                        }
                    }

                }

            }
        }

    }

    char string_erro[] = "Disciplina não encontrada.\n";

    if (repeat_send(fd, string_erro, sizeof(string_erro)) == -1) {
        perror("send");
        return -1;
    }


    fclose(fp);
    return -1;
}

int infos(int fd) {

    char string[] = "Digite o código da disciplina\n";
    char buffer[MAXDATASIZE];
    int numbytes;

    if (send(fd, string, sizeof(string), 0) == -1) {
        perror("send");
        return -1;

    }

    if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        return -1;
    }

    FILE* fp ;
    char input[255];

    if (fp = fopen(COURSES, "r")) {

        while (fscanf(fp,"%s", input) ) {
            if (strcmp(input, "[CÓDIGO]") == 0  ) {
                fscanf(fp,"%s", input);

                if (strcmp(input, buffer) == 0) {

                    // Título
                    fscanf(fp,"%s", input);
                    fgets(input, 255, fp);
                    fgets(input, 255, fp);
                    printf("%s", input);
                    if (repeat_send(fd, "Título: ", 8) == -1) {
                        perror("send");
                        return 1;
                    }

                    if (repeat_send(fd, input, sizeof(input)) == -1) {
                        perror("send");
                        return 1;
                    }

                    // Instituto
                    fscanf(fp,"%s", input);

                    fgets(input, 255, fp);
                    fgets(input, 255, fp);
                    printf("%s", input);
                    if (repeat_send(fd, "Instituto: ", 11) == -1) {
                        perror("send");
                        return 1;
                    }

                    if (repeat_send(fd, input, sizeof(input)) == -1) {
                        perror("send");
                        return 1;
                    }

                    // Sala
                    fscanf(fp,"%s", input);
                    fgets(input, 255, fp);
                    fgets(input, 255, fp);
                    printf("%s", input);
                    if (repeat_send(fd, "Sala: ", 7) == -1) {
                        perror("send");
                        return 1;
                    }

                    if (repeat_send(fd, input, sizeof(input)) == -1) {
                        perror("send");
                        return 1;
                    }

                    // Horário
                    fscanf(fp,"%s", input);
                    fgets(input, 255, fp);
                    fgets(input, 255, fp);
                    printf("%s", input);
                    if (repeat_send(fd, "Horário: ", 10) == -1) {
                        perror("send");
                        return 1;
                    }

                    if (repeat_send(fd, input, sizeof(input)) == -1) {
                        perror("send");
                        return 1;
                    }

                    // Ementa
                    fscanf(fp,"%s", input);
                    fgets(input, 255, fp);
                    fgets(input, 255, fp);
                    printf("%s", input);
                    if (repeat_send(fd, "Horário: ", 11) == -1) {
                        perror("send");
                        return 1;
                    }

                    if (repeat_send(fd, input, sizeof(input)) == -1) {
                        perror("send");
                        return 1;
                    }

                    // Prof
                    fscanf(fp,"%s", input);
                    fgets(input, 255, fp);
                    fgets(input, 255, fp);
                    printf("%s", input);
                    if (repeat_send(fd, "Professor: ", 11) == -1) {
                        perror("send");
                        return 1;
                    }

                    if (repeat_send(fd, input, sizeof(input)) == -1) {
                        perror("send");
                        return 1;
                    }

                    // Comentario
                    fscanf(fp,"%s", input);
                    fgets(input, 255, fp);
                    fgets(input, 255, fp);
                    printf("%s", input);
                    if (repeat_send(fd, "Comentario:", 11) == -1) {
                        perror("send");
                        return 1;
                    }

                    if (repeat_send(fd, input, sizeof(input)) == -1) {
                        perror("send");
                        return 1;
                    }

                    fclose(fp);
                    return 1;
                }

            }

        }

    }

    char string_erro[] = "Disciplina não encontrada.\n";

    if (repeat_send(fd, string_erro, sizeof(string_erro)) == -1) {
        perror("send");
        return -1;
    }


    fclose(fp);
    return -1;

}

int todas_infos(int fd) {

    FILE* fp ;
    char input[255];

    if (fp = fopen(COURSES, "r")) {

        while (fscanf(fp,"%s", input) != EOF ) {
            if (strcmp(input, "[CÓDIGO]") == 0  ) {

                // fscanf(fp,"%s", input);
                fgets(input, 255, fp);
                fgets(input, 255, fp);
                printf("%s", input);
                if (repeat_send(fd, "Código: ", 8) == -1) {
                    perror("send");
                    return 1;
                }

                if (repeat_send(fd, input, sizeof(input)) == -1) {
                    perror("send");
                    return 1;
                }

                // Título
                fscanf(fp,"%s", input);
                fgets(input, 255, fp);
                fgets(input, 255, fp);
                printf("%s", input);
                if (repeat_send(fd, "Título: ", 8) == -1) {
                    perror("send");
                    return 1;
                }

                if (repeat_send(fd, input, sizeof(input)) == -1) {
                    perror("send");
                    return 1;
                }

                // Instituto
                fscanf(fp,"%s", input);

                fgets(input, 255, fp);
                fgets(input, 255, fp);
                printf("%s", input);
                if (repeat_send(fd, "Instituto: ", 11) == -1) {
                    perror("send");
                    return 1;
                }

                if (repeat_send(fd, input, sizeof(input)) == -1) {
                    perror("send");
                    return 1;
                }

                // Sala
                fscanf(fp,"%s", input);
                fgets(input, 255, fp);
                fgets(input, 255, fp);
                printf("%s", input);
                if (repeat_send(fd, "Sala: ", 7) == -1) {
                    perror("send");
                    return 1;
                }

                if (repeat_send(fd, input, sizeof(input)) == -1) {
                    perror("send");
                    return 1;
                }

                // Horário
                fscanf(fp,"%s", input);
                fgets(input, 255, fp);
                fgets(input, 255, fp);
                printf("%s", input);
                if (repeat_send(fd, "Horário: ", 10) == -1) {
                    perror("send");
                    return 1;
                }

                if (repeat_send(fd, input, sizeof(input)) == -1) {
                    perror("send");
                    return 1;
                }

                // Ementa
                fscanf(fp,"%s", input);
                fgets(input, 255, fp);
                fgets(input, 255, fp);
                printf("%s", input);
                if (repeat_send(fd, "Horário: ", 11) == -1) {
                    perror("send");
                    return 1;
                }

                if (repeat_send(fd, input, sizeof(input)) == -1) {
                    perror("send");
                    return 1;
                }


                // Prof
                fscanf(fp,"%s", input);
                fgets(input, 255, fp);
                fgets(input, 255, fp);
                printf("%s", input);
                if (repeat_send(fd, "Professor: ", 11) == -1) {
                    perror("send");
                    return 1;
                }

                if (repeat_send(fd, input, sizeof(input)) == -1) {
                    perror("send");
                    return 1;
                }

                // Comentario
                fscanf(fp,"%s", input);
                fgets(input, 255, fp);
                fgets(input, 255, fp);
                printf("%s", input);
                if (repeat_send(fd, "Comentario:", 11) == -1) {
                    perror("send");
                    return 1;
                }

                if (repeat_send(fd, input, sizeof(input)) == -1) {
                    perror("send");
                    return 1;
                }
            }
        }

        if (repeat_send(fd, "done\n", 11) == -1) {
            perror("send");
            return 1;
        }
    }

    fclose(fp);
    return 1;
}

int cod_titulo(int fd) {

    FILE* fp ;
    char input[255];

    if (fp = fopen(COURSES, "r")) {

        while (fscanf(fp,"%s", input) != EOF) {
            if (strcmp(input, "[CÓDIGO]") == 0  ) {

                // fscanf(fp,"%s", input);
                fgets(input, 255, fp);
                fgets(input, 255, fp);
                printf("%s", input);
                if (repeat_send(fd, "Código: ", 8) == -1) {
                    perror("send");
                    return 1;
                }

                if (repeat_send(fd, input, sizeof(input)) == -1) {
                    perror("send");
                    return 1;
                }

                // Título
                fscanf(fp,"%s", input);
                fgets(input, 255, fp);
                fgets(input, 255, fp);
                printf("%s", input);
                if (repeat_send(fd, "Título: ", 8) == -1) {
                    perror("send");
                    return 1;
                }

                if (repeat_send(fd, input, sizeof(input)) == -1) {
                    perror("send");
                    return 1;
                }
            }
        }

        if (repeat_send(fd, "done\n", 11) == -1) {
                perror("send");
                return 1;
            }
    }
}

int escrever_com(int fd) {


    char string[] = "Digite o código da disciplina\n";
    char buffer[MAXDATASIZE];
    int numbytes;

    if (send(fd, string, sizeof(string), 0) == -1) {
        perror("send");
        return -1;

    }

    if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        return -1;
    }

    FILE* fp ;
    char input[255];

    if (fp = fopen(COURSES, "r+")) {
        // printf("dsadasda\n");

        while (fscanf(fp,"%s", input) != EOF) {

            if (strcmp(input, "[CÓDIGO]") == 0  ) {
                fscanf(fp,"%s", input);

                if (strcmp(input, buffer) == 0) {

                    while (fscanf(fp,"%s", input) != EOF) {

                        if (strcmp(input, "[NOME_PROFESSOR]") == 0  ) {
                            fgets(input, 255, fp);
                            // fgets(input, 255, fp);
                            fscanf(fp,"%s", input);

                            if (strcmp(login_nome, input) == 0) {
                            printf("segundo if\n");

                                while (fscanf(fp,"%s", input) ) {

                                    if (strcmp(input, "[COMENTARIO]") == 0  ) {
                                        fgets(input, 255, fp);


                                        char string[23] = "Digite o comentário:\n";
                                        char buffer[MAXDATASIZE];
                                        int numbytes;

                                        if (send(fd, string, sizeof(string), 0) == -1) {
                                            perror("send");
                                            return -1;

                                        }

                                        if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
                                            perror("recv");
                                            return -1;
                                        }
                                        printf("'received: '%s'\n", buffer);

                                        fprintf(fp, "%s", buffer);

                                        fclose(fp);

                                        if (repeat_send(fd, "Comentario adicionado.", 23) == -1) {
                                            perror("send");
                                            return 1;
                                        }

                                        return 1;
                                    }
                                }

                            }

                        }

                    }

                }

            }
        }

    }

    char string_erro[] = "Disciplina não encontrada.\n";

    if (repeat_send(fd, string_erro, sizeof(string_erro)) == -1) {
        perror("send");
        return -1;
    }


    fclose(fp);
    return -1;
}

int ler_com(int fd) {


    char string[] = "Digite o código da disciplina\n";
    char buffer[MAXDATASIZE];
    int numbytes;

    if (send(fd, string, sizeof(string), 0) == -1) {
        perror("send");
        return -1;

    }

    if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        return -1;
    }

    FILE* fp ;
    char input[255];

    if (fp = fopen(COURSES, "r")) {
        // printf("dsadasda\n");

        while (fscanf(fp,"%s", input) ) {
            // printf("%s %s\n", string_nome, string_senha);

            if (strcmp(input, "[CÓDIGO]") == 0  ) {
                fscanf(fp,"%s", input);

                if (strcmp(input, buffer) == 0) {

                    while (fscanf(fp,"%s", input) ) {

                        if (strcmp(input, "[COMENTARIO]") == 0  ) {
                            fgets(input, 255, fp);
                            fgets(input, 255, fp);

                                if (repeat_send(fd, input, sizeof(input)) == -1) {
                                    perror("send");
                                    return 1;
                                }

                                fclose(fp);
                                return 1;

                        }
                    }

                }

            }
        }

    }

    char string_erro[] = "Disciplina não encontrada.\n";

    if (repeat_send(fd, string_erro, sizeof(string_erro)) == -1) {
        perror("send");
        return -1;
    }


    fclose(fp);
    return -1;

}

int send_login_prof(int fd) {

    char string[] = "Olá!\nDigite o número da funcionalidade que deseja:\n1)Receber ementa de uma disciplina a partir do seu código\n2)Receber todas as informações de uma disciplina a partir do seu código\n3)Listar todas as informações de todas as disciplinas\n4)Listar todos os códigos de disciplinas com seus respectivos títulos\n5)Escrever comentário sobre próxima aula de uma de suas disciplinas\n6)Receber o comentário da próxima aula de uma disciplina a partir de seu código\n7)Fechar conexão.";
    char buffer[MAXDATASIZE];
    int numbytes;

    printf("preparando para enviar opcoes\n");
    if ((numbytes = send(fd, string, sizeof(string), 0)) == -1) { ////////////// repeat_send trocado por send
        perror("send");
        return -1;

    }
    printf("opcoes enviadas, %d bytes transmitidos\n", numbytes);

    if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        return -1;
    }

    char erro[] = "Por favor, digite números de 1 a 7.\n";

    while (strcmp(buffer, "1") != 0 && strcmp(buffer, "2") != 0 && strcmp(buffer, "3") != 0 && strcmp(buffer, "4") != 0 && strcmp(buffer, "5") != 0 && strcmp(buffer, "6") != 0 && strcmp(buffer, "7") != 0) {
        if (send(fd, erro, sizeof(erro), 0) == -1) {
            perror("send");
            return -1;

        }

        if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            return -1;
        }

    }

    printf("received: %d\n", buffer[0] - '0');
    switch (buffer[0] - '0') {

        case 1:
            ementa(fd);
            break;
        case 2:
            infos(fd);
            break;
        case 3:
            todas_infos(fd);
            break;
        case 4:
            cod_titulo(fd);
            break;
        case 5:
            escrever_com(fd);
            break;
        case 6:
            ler_com(fd);
            break;
        case 7:
            return -1;

    }

    return 1;
}

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

    char string[] = "Ola!\nDigite seu nome e senha, em linhas separadas.\n";
    char nome[MAXDATASIZE];
    char senha[MAXDATASIZE];
    int numbytes;

    if (send(fd, string, 53, 0) == -1) {
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
    char string_nome[255];
    char string_senha[255];

    if (fp = fopen(PROFESSORS, "r")) {

        while (fscanf(fp,"%s %s", string_nome, string_senha) != EOF ) {

            if (strcmp(string_nome, nome) == 0 && strcmp(string_senha, senha) == 0 ) {
                strcpy(login_nome, string_nome);

                fclose(fp);
                return 1;
            }
        }

    }

    char string_erro[] = "Erro na validacao.\n";

    if (send(fd, string_erro, 20, 0) == -1) {
        perror("send");
        return -1;
    }

    fclose(fp);
    return -1;

}
int validate_login_student(int fd) {

    char string[] = "Ola!\nDigite seu nome e senha, em linhas separadas.\n";
    char nome[MAXDATASIZE];
    char senha[MAXDATASIZE];
    int numbytes;

    if (repeat_send(fd, string, 52) == -1) {
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
    char string_nome[255];
    char string_senha[255];

    if (fp = fopen(STUDENTS, "r")) {
        // printf("dsadasda\n");

        while (fscanf(fp,"%s %s", string_nome, string_senha) ) {
            // printf("%s %s\n", string_nome, string_senha);

            if (strcmp(string_nome, nome) == 0 && strcmp(string_senha, senha) == 0 ) {
                // printf("fadfasdfads");
                fclose(fp);
                return 1;
            }
        }

    }

    fclose(fp);
    return 1;

}

void send_func(int fd) {

    int login;

    do {
        printf("comeco do do\n");
        login = send_func_login(fd);

        if (login == 1) {

            login = validate_login_prof(fd);

            if (login != -1) {
                do {
                    login = send_login_prof(fd);
                } while (login == 1);
            }

            return; /*?????*/

        }
        else if (login == 2) {
            login = validate_login_student(fd);


            // if (login != -1) {
            //     do {
            //         login = send_login_students(fd);
            //     } while (login == 1);
            // }

            // return;


        }

    } while (login == -1);

}


/*
STRUCTS para disciplina, mandar numero de bytes certo e dar cast numa struct no cliente
controlar flow de send no servidor, pesquisar como
header com tamanho da mensagem


*/
