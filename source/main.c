/*********************************************************
 *  Copyright (c) 2005 Sergio Ahumada                    *
 *  Proyecto  : Space Planning con Branch and Bound      *
 *  Alumno    : Sergio Ahumada Navea                     *
 *                                                       *
 *  Profesor  : Dr. Carlos Castro                        *
 *                                                       *
 *  Fecha     : 21 de junio de 2005                      *
 *  Archivo   : main.c                                   *
 *********************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "include/data_types.h"
#include "include/prototipos.h"

int main(int argc, char ** argv)
{

   item *f, *i, *m, tab[MAXITEMS];
   int W, H, n, j;
   int w[MAXITEMS], h[MAXITEMS];
   int x[MAXITEMS], y[MAXITEMS], r[MAXITEMS];

   if (argc == 1) {
      lee_archivo(&n, w, h, &W, &H, "entrada.txt");
   } else if (argc == 2) {
      lee_archivo(&n, w, h, &W, &H, argv[1]);
   }
   else {
      printf("%s: numero de argumentos incorrecto\n", argv[0]);
      printf("Modo de empleo: %s [archivo]\n", argv[0]);
      exit(1);
   }

   f = &tab[0];

   /* crea los items con sus respectivas dimensiones */
   /* e inicializa soluciones en cero */
   for (i = f, m = f+n, j = 1; i != m; i++, j++) {
      i->no = j;
      i->w = w[j];
      i->h = h[j];
      i->x = i->y = i->r = 0;
      i->area = AREA(i);
   }

   branch_bound(n, W, H, w, h, x, y, r, 10000);
   return(0);
}
