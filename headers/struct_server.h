#ifndef STRUCT_SERVER_H_
# define STRUCT_SERVER_H_

typedef struct		s_server
{
  int			port;
  int			fd_server;
  int			is_running;
  t_play		*plays;
}			t_server;

#endif
