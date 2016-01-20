#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include "struct_play.h"
#include "struct_server.h"
#include "server.h"

int			bind_socket(t_server *server)
{
  struct sockaddr_in	s_in;
  int			yes;

  yes = 1;
  if (setsockopt(server->fd_server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
      fprintf(stderr, "Error : setsockopt : %s\n", strerror(errno));
      return (1);
    }
  s_in.sin_family = AF_INET;
  s_in.sin_port = htons(server->port);
  s_in.sin_addr.s_addr = INADDR_ANY;
  if (bind(server->fd_server, (const struct sockaddr *)&s_in, sizeof(s_in)) == -1)
    {
      fprintf(stderr, "Error : bind : %s\n", strerror(errno));
      if (close(server->fd_server) == -1)
	fprintf(stderr, "Error : close : %s\n", strerror(errno));
      return (1);
    }
  return (0);
}

int			init_listen(t_server *server)
{
  if (listen(server->fd_server, SOMAXCONN) == -1)
    {
      fprintf(stderr, "Error : listen : %s\n", strerror(errno));
      if (close(server->fd_server) == -1)
	fprintf(stderr, "Error : close : %s\n", strerror(errno));
      return (1);
    }
  return (0);
}

int			create_socket(t_server *server)
{
  struct protoent	*p;

  if ((p = getprotobyname("tcp")) == NULL)
    {
      fprintf(stderr, "Error : getprotobyname fail\n");
      return (1);
    }
  if ((server->fd_server = socket(AF_INET, SOCK_STREAM, p->p_proto)) == -1)
    {
      fprintf(stderr, "Error : %s\n", strerror(errno));
      return (1);
    }
  return (0);
}

int		init_server(t_server *server)
{
  server->port = 8888;
  server->is_running = 1;
  server->plays = NULL;
  if (create_socket(server))
    return (1);
  if (bind_socket(server))
    return (1);
  if (init_listen(server))
    return (1);
  if (launch_server(server))
    return (1);
  if (close(server->fd_server) == -1)
    {
      fprintf(stderr, "Error : close : %s\n", strerror(errno));
      return (1);
    }
  return (0);
}
