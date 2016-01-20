#include <stdio.h>
#include <stdlib.h>
#include "struct_play.h"
#include "struct_server.h"
#include "init.h"



int			main()
{
 t_server		*server;

 if ((server = malloc(sizeof(t_server))) == NULL) {
   return (EXIT_FAILURE);
 }
 if (init_server(server)) {
   return (EXIT_FAILURE);
 }
 return (EXIT_SUCCESS);
}
