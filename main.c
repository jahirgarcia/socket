#include <errno.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

enum errors {
  ESOCKET = 1,
  EBIND,
  ELISTEN,
  EACCEPT
};

void logerr(const char *src) {
  fprintf(stderr, "%s call has failed with error code %d", src, errno);
}

int parseopt(int argc, char **argv) {
  int opt, port;
  struct option longopts = { .name = "port", .has_arg = required_argument, .flag = NULL, .val = 24 };

  opterr = 0;
  while((opt = getopt_long(argc, argv, "p:", &longopts, NULL)) != -1) {
    switch(opt) {
      case 'p':
        printf("-p %s\n", optarg);
        break;
      case 24:
        printf("--port %s\n", optarg);
        break;
      case ':':
        fprintf(stderr, "Option -p or --port is required\n");
        break;
      case '?':
        fprintf(stderr, "Invalid option: -%c\n", optopt);
        break;
    }
  }

  return port;
}

int main(int argc, char **argv) {
  int sfd, err, accsfd;
  socklen_t accaddrl;
  struct sockaddr_in addr, accaddr;

  parseopt(argc, argv);

  return 0;

  sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (sfd == -1) {
    logerr("socket");
    return ESOCKET;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(3000);
  addr.sin_addr.s_addr = INADDR_ANY;
  err = bind(sfd, (struct sockaddr *)&addr, sizeof(addr));

  if (err == -1) {
    logerr("bind");
    return EBIND;
  }

  err = listen(sfd, SOMAXCONN);

  if (err == -1) {
    logerr("listen");
    return ELISTEN;
  }

  accaddrl = sizeof(accaddr);
  accsfd = accept(sfd, (struct sockaddr *)&accaddr, &accaddrl);

  if (accsfd == -1) {
    logerr("accept");
    return EACCEPT;
  }

  char buffer[1024];
  read(accsfd, buffer, sizeof(buffer));
  printf("%s\n", buffer);

  close(accsfd);
  shutdown(sfd, SHUT_RDWR);

  return 0;
}
