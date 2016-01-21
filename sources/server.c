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
#include "struct_team.h"
#include "struct_pawn.h"
#include "struct_board.h"
#include "struct_play.h"
#include "struct_server.h"
#include "server.h"
#include "command.h"

void		disconnect_client(t_play *play, t_server *server)
{
  t_play	*tmp;
  t_play	*prev;
  
  write(play->fd_player_1, "QUIT\n", 5);
  write(play->fd_player_2, "QUIT\n", 5);
  close(play->fd_player_1);
  close(play->fd_player_2);
  tmp = server->plays;
  prev = NULL;
  while (tmp)
    {
      if (tmp == play)
	{
	  if (prev == NULL)
	    server->plays = play->next;
	  else
	    prev->next = play->next;
	  free(play);
	  return ;
	}
      prev = tmp;
      tmp = tmp->next;
    }
}

int		receive_data(t_play **play, t_server *server, int id)
{
  char		buff[512];
  int		len;
  int		fd;
  
  memset(&(*buff), 0, 512);
  fd = (*play)->fd_player_1;
  if (id == 2)
    fd = (*play)->fd_player_2;
  if ((len = read(fd, buff, 511)) == -1)
    {
      fprintf(stderr, "Error : read : %s\n", strerror(errno));
      return (1);
    }
  if (len == 0)
    {
      disconnect_client(*play, server);
      *play = server->plays;
      return (0);
    }
  if ((*play)->player_number > 1)
	{
  		interprete_command(*play, buff, len, id);
	}
  return (0);
}

void		close_all_sockets(t_server *server)
{
  t_play	*plays;

  plays = server->plays;
  if (close(server->fd_server) == -1)
    fprintf(stderr, "Error : close : %s\n", strerror(errno));    
  while (plays)
    {
      if (close(plays->fd_player_1) == -1)
	fprintf(stderr, "Error : close : %s\n", strerror(errno));
      if (close(plays->fd_player_2) == -1)
	fprintf(stderr, "Error : close : %s\n", strerror(errno));
      plays = plays->next;
    }
}

void		set_clients(t_server *server, fd_set *readfs)
{
  t_play	*plays;

  plays = server->plays;
  FD_ZERO(readfs);
  FD_SET(server->fd_server, readfs);
  while (plays)
    {
      FD_SET(plays->fd_player_1, readfs);
      FD_SET(plays->fd_player_2, readfs);
      plays = plays->next;
    }
}

int		max_client_fd(t_server *server)
{
  int		max;
  t_play	*plays;

  max = -1;
  plays = server->plays;
  while (plays)
    {
      if (plays->fd_player_1 > max)
	max = plays->fd_player_1;
      if (plays->fd_player_2 > max)
	max = plays->fd_player_2;
      plays = plays->next;
    }
  if (server->fd_server > max)
    max = server->fd_server;
  return (max);
}

int			play_host(t_server *server)
{
  struct sockaddr_in	s_in;
  socklen_t		s_size;
  t_play		*tmp;

  s_size = sizeof(s_in);
  tmp = server->plays;
  while (tmp)
    {
      if (tmp->player_number == 1)
	{
	  if ((tmp->fd_player_2 = accept(server->fd_server, (struct sockaddr *)(&s_in), &s_size)) == -1)
	    {
	      fprintf(stderr, "Error : accept : %s\n", strerror(errno));
	      if (close(server->fd_server) == -1)
		fprintf(stderr, "Error : close : %s\n", strerror(errno));
	      return (2);
	    }
	  tmp->player_number = 2;
	  write(tmp->fd_player_1, "READY\n", 6);
	  write(tmp->fd_player_2, "READY\n", 6);
	  return (1);
	}
      tmp = tmp->next;
    }
  return (0);
}

int			accept_client(t_server *server)
{
  struct sockaddr_in	s_in;
  socklen_t		s_size;
  int			v;
  t_play		*new;
  t_play		*tmp;

  if ((v = play_host(server)) == 1)
    return (0);
  else if (v == 2)
    return (1);
  s_size = sizeof(s_in);
  if ((new = malloc(sizeof(t_play))) == NULL)
    {
      fprintf(stderr, "Error : malloc failed\n");
      if (close(server->fd_server) == -1)
	fprintf(stderr, "Error : close : %s\n", strerror(errno));
      return (1);
    }
  if ((new->fd_player_1 = accept(server->fd_server, (struct sockaddr *)(&s_in), &s_size)) == -1)
    {
      fprintf(stderr, "Error : accept : %s\n", strerror(errno));
      if (close(server->fd_server) == -1)
	fprintf(stderr, "Error : close : %s\n", strerror(errno));
      return (1);
    }
  new->fd_player_2 = -1;
  new->player_number = 1;
  new->board = NULL;
  printf("New connection %s\n", inet_ntoa(s_in.sin_addr));
  tmp = server->plays;
  new->next = tmp;
  server->plays = new;
  return (0);
}

int		handle_clients(t_server *server, fd_set *readfs)
{
  t_play	*tmp;
  
  set_clients(server, readfs);
  if (select(max_client_fd(server) + 1, readfs, NULL, NULL, NULL) == -1)
    {
      fprintf(stderr, "Error : select : %s\n", strerror(errno));
      close_all_sockets(server);
      return (1);
    }
  if (FD_ISSET(server->fd_server, readfs))
    if (accept_client(server))
      return (1);
  tmp = server->plays;
  while (tmp)
    {
      if (tmp && FD_ISSET(tmp->fd_player_1, readfs))
	if (receive_data(&tmp, server, 1))
	  return (1);
      if (tmp && FD_ISSET(tmp->fd_player_2, readfs))
	if (receive_data(&tmp, server, 2))
	  return (1);
      if (tmp)
	tmp = tmp->next;
    }
  return (0);
}

int		launch_server(t_server *server)
{
  fd_set	readfs;

  while (server->is_running)
    {
      if (handle_clients(server, &readfs))
	return (1);
    }
  close_all_sockets(server);
  return (0);
}
