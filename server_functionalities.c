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

void sigchld_handler(int s) {
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

int repeat_send(int fd, const void *buffer, int size) {
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

course* code_search(int fd) {
    char string[] = "Digite o código da disciplina:";
    char buffer[MAXDATASIZE];
    int numbytes;

    if (send(fd, string, sizeof(string), 0) == -1) {
        perror("send");
        return NULL;

    }

    if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        return NULL;
    }

    FILE* courses_f ;
    int status = 0;

    if (courses_f = fopen(COURSES, "rb")) {
        course* existing_course = malloc(sizeof(course));

        while ( fread(existing_course, sizeof(course), 1, courses_f) ) {

            if (strcmp(existing_course->code, buffer) == 0 ) {
                status = 1;
                if (repeat_send(fd, &status, sizeof(int)) == -1) {
                    perror("send");
                    return NULL;
                }

                free(existing_course);
                fclose(courses_f);
                return existing_course;
   
            }
        }

        if (repeat_send(fd, &status, sizeof(int)) == -1) {
            perror("send");
            return NULL;
        }

        free(existing_course);
        fclose(courses_f);

        char notfound_error[] = "Disciplina não encontrada.";

        if (repeat_send(fd, notfound_error, sizeof(notfound_error)) == -1) {
            perror("send");
            fclose(courses_f);
            return NULL;
        }
    }

    char file_error[] = "Erro ao abrir arquivo.";

    if (repeat_send(fd, file_error, sizeof(file_error)) == -1) {
        perror("send");
        return NULL;
    }

    return NULL;

}

int ementa(int fd) {
    course* course_info = code_search(fd);

    if (course_info != NULL) {
        if (repeat_send(fd, course_info->description, sizeof(course_info->description)) == -1) {
            perror("send");
            free(course_info);
            return -1;
        }
        return 1;
    }

    free(course_info);

    return -1;

}

int infos(int fd) {

    course* course_info = code_search(fd);

    if (course_info != NULL) {
        if (repeat_send(fd, course_info, sizeof(course)) == -1) {
            perror("send");
            free(course_info);
            return -1;
        }
        return 1;
    }

    free(course_info);

    return -1;

}

int todas_infos(int fd) {
    FILE* courses_f ;
    course* existing_course = malloc(sizeof(course));

    if (courses_f = fopen(COURSES, "rb")) {
        // printf("dsadasda\n");

        while ( fread(existing_course, sizeof(course), 1, courses_f) ) {
                
            if (repeat_send(fd, existing_course, sizeof(course)) == -1) {
                perror("send");
                free(existing_course);
                fclose(courses_f);
                return -1;
            }
        
        }

        free(existing_course);
        fclose(courses_f);
        return 1;

    }

    char file_error[] = "Erro ao abrir arquivo.\n";
    free(existing_course);

    if (repeat_send(fd, file_error, sizeof(file_error)) == -1) {
        perror("send");
        return -1;
    }

    return -1;

}

int escrever_com(user* prof, int fd) {
    char string[] = "Digite o código da disciplina:";
    char buffer[MAXDATASIZE];
    int numbytes, found_course = 0, counter=0;

    if (send(fd, string, sizeof(string), 0) == -1) {
        perror("send");
        return -1;

    }

    if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        return -1;
    }

    FILE* courses_f ;
    course* existing_course = malloc(sizeof(course));
    int status = 0;

    if (courses_f = fopen(COURSES, "rb")) {

        while ( fread(existing_course, sizeof(course), 1, courses_f) ) {

            if (strcmp(existing_course->code, buffer) == 0 ) {
                status = 1;
                if (repeat_send(fd, &status, sizeof(int)) == -1) {
                    perror("send");
                    return -1;
                }

                found_course = 1;
                break;

            }
            counter++;

        }

        if (!found_course) {
            if (repeat_send(fd, &status, sizeof(int)) == -1) {
                perror("send");
                return -1;
            }

            free(existing_course);
            fclose(courses_f);

            char notfound_error[] = "Disciplina não encontrada.";

            if (repeat_send(fd, notfound_error, sizeof(notfound_error)) == -1) {
                perror("send");
                return -1;
            }   

            return 1;
        }

    }
    else {
        if (repeat_send(fd, &status, sizeof(int)) == -1) {
            perror("send");
            return -1;
        }
        char file_error[] = "Erro ao abrir arquivo.";

        if (repeat_send(fd, file_error, sizeof(file_error)) == -1) {
            perror("send");
            return -1;
        }

    }

    if (strcmp(existing_course->professor, prof->name) == 0) {

        char new_comment[COMMENT_LENGTH];
        // printf("counter: %d\n", counter);
        fseek(courses_f, counter*sizeof(course), SEEK_SET);

        char comment_question[] = "Digite o comentario:";

        if (repeat_send(fd, comment_question, sizeof(comment_question)) == -1) {
            perror("send");
            return -1;
        }

        if ((numbytes = recv(fd, new_comment, MAXDATASIZE-1, 0)) == -1) {
            perror("recv");
            return -1;
        }

        strcpy(existing_course->comment, new_comment);
        fwrite(existing_course, sizeof(course), 1, courses_f);

        if ( fwrite != 0 ) {
            char feedback[] = "Comentario adicionado.";
            free(existing_course);
            fclose(courses_f);

            if (repeat_send(fd, feedback, sizeof(feedback)) == -1) {
                perror("send");
                return -1;
            }
            return 1;
        }
        else {
            char feedback[] = "Erro ao escrever no arquivo.";
            free(existing_course);
            fclose(courses_f);
            
            if (repeat_send(fd, feedback, sizeof(feedback)) == -1) {
                perror("send");
                return -1;
            }
        }



    }
    else {

        char login_error[] = "Voce nao tem permissao para alterar comentarios dessa disciplina.";
        free(existing_course);
        fclose(courses_f);

        if (repeat_send(fd, login_error, sizeof(login_error)) == -1) {
            perror("send");
            return -1;
        }

    }
    
    return 1;
}

int ler_com(int fd) {

    course* course_info = code_search(fd);

    if (course_info != NULL) {
        if (repeat_send(fd, course_info->comment, sizeof(course_info->comment)) == -1) {
            perror("send");
            free(course_info);
            return -1;
        }
        return 1;
    }

    free(course_info);

    return -1;

}

int send_func_login(int fd) {

    char string[] = "Boas vindas ao Sistema de Disciplinas da UNICAMP\nSe deseja logar digite 1. Se deseja sair, digite 2.";
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

    char erro[] = "Por favor, digite 1 ou 2.";
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

    return -1;

}

int send_login(user* user_info, int fd) {

    char string_prof[] = "Ola!\nDigite o número da funcionalidade que deseja:\n1)Receber ementa de uma disciplina a partir do seu código\n2)Receber todas as informações de uma disciplina a partir do seu código\n3)Listar todas as informações de todas as disciplinas\n4)Listar todos os códigos de disciplinas com seus respectivos títulos\n5)Receber o comentário da próxima aula de uma disciplina a partir de seu código\n6)Escrever comentário sobre próxima aula de uma de suas disciplinas\n7)Fechar conexão";
    char string_stud[] = "Ola!\nDigite o número da funcionalidade que deseja:\n1)Receber ementa de uma disciplina a partir do seu código\n2)Receber todas as informações de uma disciplina a partir do seu código\n3)Listar todas as informações de todas as disciplinas\n4)Listar todos os códigos de disciplinas com seus respectivos títulos\n5)Receber o comentário da próxima aula de uma disciplina a partir de seu código\n6)Fechar conexão";  
    
    char buffer[MAXDATASIZE];
    int numbytes;

    
    if (user_info->is_prof) {
        if (send(fd, string_prof, sizeof(string_prof), 0) == -1) {
            perror("send");
            return -1;

        }

    }
    else {
        if (send(fd, string_stud, sizeof(string_stud), 0) == -1) {
            perror("send");
            return -1;

        }

    }

    if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        return -1;
    }

    char error_prof[] = "Por favor, digite números de 1 a 7.";
    char error_stud[] = "Por favor, digite números de 1 a 6.";

    if (user_info->is_prof) {

        while (strcmp(buffer, "1") != 0 && strcmp(buffer, "2") != 0 && strcmp(buffer, "3") != 0 && strcmp(buffer, "4") != 0 && strcmp(buffer, "5") != 0 && strcmp(buffer, "6") != 0 && strcmp(buffer, "7") != 0) {
            if (send(fd, error_prof, sizeof(error_prof), 0) == -1) {
                perror("send");
                return -1;

            }

            if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
                perror("recv");
                return -1;
            }

        }
    }
    else {

        while (strcmp(buffer, "1") != 0 && strcmp(buffer, "2") != 0 && strcmp(buffer, "3") != 0 && strcmp(buffer, "4") != 0 && strcmp(buffer, "5") != 0 && strcmp(buffer, "6") != 0) {
            if (send(fd, error_stud, sizeof(error_stud), 0) == -1) {
                perror("send");
                return -1;

            }

            if ((numbytes = recv(fd, buffer, MAXDATASIZE-1, 0)) == -1) {
                perror("recv");
                return -1;
            }

        }

    }

    printf("comando: %d\n", buffer[0] - '0');
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
            todas_infos(fd);
            break;
        case 5:
            ler_com(fd);
            break;
        case 6:
            if (user_info->is_prof) {
                escrever_com(user_info, fd);

            }
            else {
                return -1;
            }
            break;
        case 7:
            return -1;

    }

    return 1;
}

user* validate_login(int fd) {

    char string[] = "Ola!\nDigite seu nome e senha, em linhas separadas.";
    int numbytes;
    user* user_logging = malloc(sizeof(user)), *existing_user = malloc(sizeof(user));

    printf("comeco da funcao validate login\n");
    if (send(fd, string, 53, 0) == -1) {
        perror("send");
        return NULL;
    }
    printf("depois do send\n");
    if ((numbytes = read(fd, user_logging, sizeof(user))) == -1) {
        perror("read");
        return NULL;
    }
    printf("depois do recv\n");

    printf("server: received '%s' '%s'\n", user_logging->name, user_logging->pwd);

    FILE* users_f ;
    int status = 0;

    if (users_f = fopen(USERS, "rb")) {
   
        fseek(users_f, 0, SEEK_SET);
        while ( fread(existing_user, sizeof(user), 1, users_f) ) {

            if (strcmp(existing_user->name, user_logging->name) == 0 && strcmp(existing_user->pwd, user_logging->pwd) == 0 ) {
                fclose(users_f);
                free(user_logging);
                status = 1;

    // ajeitar aqui
                if (send(fd, &status, sizeof(int), 0) == -1) {
                    perror("send");
                    return NULL;
                }

                if (send(fd, existing_user, sizeof(existing_user), 0) == -1) {
                    perror("send");
                    return NULL;
                }

                return existing_user;
            }
        }


    // ajeitar aqui
        if (send(fd, &status, sizeof(int), 0) == -1) {
            perror("send");
            return NULL;
        }


    }

    free(existing_user);
    free(user_logging);
    fclose(users_f);

    char string_erro[] = "Erro na validacao.";

    if (send(fd, string_erro, sizeof(string_erro), 0) == -1) {
        perror("send");
        return NULL;
    }


    return NULL;

}

void send_func(int fd) {

    int login;
    user* user_info;

    do {
        printf("comeco do do\n");
        login = send_func_login(fd);

        if (login == 1) {
    
            // Returns user info or null pointer
            user_info = validate_login(fd);

            printf("%s\n", user_info->name);

            if (user_info != NULL) {
                do {
                    login = send_login(user_info, fd);
                } while (login == 1);
            }
        }

        // If login fails, return to initial menu

    } while (login == -1);

    free(user_info);
}