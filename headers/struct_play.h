#ifndef STRUCT_PLAY_H_
# define STRUCT_PLAY_H_

typedef struct		s_play
{
  int			player_number;
  int			fd_player_1;
  int			fd_player_2;
  struct s_play		*next;
}			t_play;

#endif
