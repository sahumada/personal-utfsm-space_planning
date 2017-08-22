/*********************************************************
 *  Copyright (c) 2005 Sergio Ahumada                    *
 *  Proyecto  : Space Planning con Branch and Bound      *
 *  Alumno    : Sergio Ahumada Navea                     *
 *                                                       *
 *  Profesor  : Dr. Carlos Castro                        *
 *                                                       *
 *  Fecha     : 21 de junio de 2005                      *
 *  Archivo   : fhandle.c                                *
 *********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "include/data_types.h"

void lee_archivo (int * n, int * w, int * h, int * W, int * H, char * camino)
{
   int m = 1, i[MAXITEMS];

   FILE * entrada;
   if ((entrada = fopen(camino, "r")) == NULL) {
      printf("No se encontro archivo de entrada\n");
      exit(1);
   }
   else {
      fscanf(entrada, "%d %d", W, H);
      fscanf(entrada, "%d", n);
      do {
	 fscanf(entrada, "%d %d %d", i+m, w+m, h+m);
	 m++;
      } while (!feof(entrada));
   }
   fclose(entrada);
}

void escribe_archivo (int n, int W, int H, item * fsol)
{
   item * i;

   FILE * salida;
   if ((salida = fopen("output/sal-bb.txt", "w")) == NULL) {
      printf("No se pudo escribir en archivo de salida\n");
      exit(1);
   }
   else {
      /* usado para comprobar graficamente con gl_desplegar.c */
      fprintf(salida, "%d %d\n", W, H);
      fprintf(salida, "%d\n", n);
      /* */
      for (i = fsol; i != fsol+n; i++ ) {
         fprintf(salida, "%d %d %d %d %d %d\n", i->no, i->w, i->h, i->x, i->y, i->r);
         /* fprintf(salida, "%d %d %d %d\n", i->no, i->x, i->y, i->r); */
      }
   }
   fclose(salida);
}
