/*
** board.h for Gomoku in /home/hirt_r/rendu/gomoku_ai
**
** Made by hirt_r
** Login   <hirt_r@epitech.net>
**
** Started on  Tue Jan 19 14:35:08 2016 hirt_r
** Last update Tue Jan 19 17:48:31 2016 hirt_r
*/

#ifndef board_h
#define board_h

t_board	*newBoard();
t_pawn	*checkEaten(t_board *);
t_pawn	*addPawn(t_pawn **, int, int);
int	checkWin(t_board *);

#endif /* board_h */
