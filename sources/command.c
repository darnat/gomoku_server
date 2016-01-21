#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "struct_team.h"
#include "struct_pawn.h"
#include "struct_board.h"
#include "struct_play.h"
#include "struct_server.h"
#include "board.h"

void		referee_setpawn(t_play *play,
				int id,
				int x,
				int y)
{
  t_pawn	*tmp;
  
  if (play->board == NULL)
    if ((play->board = newBoard()) == NULL)
      return ;
  if (setPawn(play->board, x, y, id) == 0)
    {
      dprintf(play->fd_player_1, "PAWNAT %d %d %d\n", x, y, id);
      dprintf(play->fd_player_2, "PAWNAT %d %d %d\n", x, y, id);
    }
  else
    {
      return ;
    }
  /* else */
  /*   { */
  /*     if (id == 1) */
  /* 	dprintf(play->fd_player_1, "ERROR\n"); */
  /*     else */
  /* 	dprintf(play->fd_player_2, "ERROR\n"); */
  /*   } */
  tmp = checkEaten(play->board);
  while (tmp)
    {
      dprintf(play->fd_player_1, "REMOVEPAWNAT %d %d\n", tmp->x, tmp->y);
      dprintf(play->fd_player_2, "REMOVEPAWNAT %d %d\n", tmp->x, tmp->y);
      tmp = tmp->next;
    }
  if (checkWin(play->board))
    {
      dprintf(play->fd_player_1, "WIN %d\n", id);
      dprintf(play->fd_player_2, "WIN %d\n", id);
    }
}

void	interprete_command(t_play *play, char *buff, int len, int id)
{
  int	i;
  int	params1;
  int	params2;  
  char	*token;
  
  if (len > 7 && strncmp("SETPAWN", buff, 7) == 0)
    {
      i = 0;
      token = strtok(buff, " ");
      while (token)
	{
	  if (i == 1)
	    params1 = atoi(token);
	  if (i == 2)
	    params2 = atoi(token);
	  ++i;
	  token = strtok(NULL, " ");
	}
      if (i >= 2)
	{
	  referee_setpawn(play, id, params1, params2);
	}
    }
}
