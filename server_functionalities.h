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

#define STUDENTS "students.txt"
#define PROFESSORS "professors.txt"
#define COURSES "courses.txt"
#define MAXDATASIZE 10000

void sigchld_handler(int s);
void *get_in_addr(struct sockaddr *sa);
int repeat_send(int fd, const void *buffer, int size);
int send_func_login(int fd);
int ementa(int fd);
int infos(int fd);
int todas_infos(int fd);
int cod_titulo(int fd);
int escrever_com(int fd);
int ler_com(int fd);
int send_login_prof(int fd);
// int send_login_student(int fd);
int validate_login_prof(int fd);
int validate_login_student(int fd);
void send_func(int fd);
