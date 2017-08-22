/*********************************************************
 *  Copyright (c) 2005 Sergio Ahumada                    *
 *  Proyecto  : Space Planning con Branch and Bound      *
 *  Alumnos   : Sergio Ahumada Navea                     *
 *                                                       *
 *  Profesor  : Dr. Carlos Castro                        *
 *                                                       *
 *  Fecha     : 21 de junio de 2005                      *
 *  Archivo   : branch_bound.c                           *
 *********************************************************/

/* This code solves the three-dimensional bin-packing problem, which
 * asks for an orthogonal packing of a given set of rectangular-shaped
 * boxes into the minimum number of three-dimensional rectangular bins.
 * Each box j=1,..,n is characterized by a width w_j, height h_j, and
 * depth d_j. An unlimited number of indentical three-dimensional bins,
 * having width W, height H and depth D is available. The boxes have fixed
 * orientation, i.e. they may not be rotated, but no further restriction
 * is imposed.
 *
 * A description of this code is found in the following papers:
 *
 *   S.Martello, D.Pisinger, D.Vigo (1998)
 *   "An exact algorithm for the three-dimensional bin packing problem"
 *   submitted.
 *
 *   S.Martello, D.Pisinger, D.Vigo (1998)
 *   "The three-dimensional bin packing problem"
 *   to appear in Operations Research.
 *
 * The present code is written in ANSI-C, and has been compiled with
 * the GNU-C compiler using option "-ansi -pedantic" as well as the
 * HP-UX C compiler using option "-Aa" (ansi standard).
 *
 * This file contains the callable routine binpack3d with prototype
 *
 *   void binpack3d(int n, int W, int H, int D,
 *          int *w, int *h, int *d,
 *          int *x, int *y, int *z, int *bno,
 *          int *lb, int *ub, int timelimit);
 *
 * the meaning of the parameters is the following:
 *   n         Size of problem, i.e. number of boxes to be packed.
 *             This value must be smaller than MAXITEMS defined below.
 *   W,H,D     Width, height and depth of every bin.
 *   w,h,d     Integer arrays of length n, where w[j], h[j], d[j]
 *             are the dimensions of box j for j=0,..,n-1.
 *   x,y,z,bno Integer arrays of length n where the solution found
 *             is returned. For each box j=0,..,n-1, the bin number
 *             it is packed into is given by bno[j], and x[j], y[j], z[j]
 *             are the coordinates of it lower-left-backward corner.
 *   lb        Lower bound on the solution value (returned by the procedure).
 *   ub        Objective value of the solution found, i.e. number of bins
 *             used to pack the n boxes. (returned by the procedure).
 *   timelimit Time limit for solving the problem expressed in seconds.
 *             If set to zero, the algorithm will run until an optimal
 *             solution is found; otherwise it terminates after timelimit
 *             seconds with a heuristic solution.
 *
 * (c) Copyright 1998,
 *
 *   David Pisinger                        Silvano Martello, Daniele Vigo
 *   DIKU, University of Copenhagen        DEIS, University of Bologna
 *   Universitetsparken 1                  Viale Risorgimento 2
 *   Copenhagen, Denmark                   Bologna, Italy
 *
 * This code can be used free of charge for research and academic purposes
 * only.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <values.h>
#include "include/data_types.h"
#include "include/prototipos.h"

/* variables globales */
boolean stopped;
/* tiempo maximo a usar en computacion */
int maxtime;

void error(char *str, ...)
{
  va_list args;

  va_start(args, str);
  vprintf(str, args); printf("\n");
  va_end(args);
  printf("\nTERMINO DE PROGRAMA IRREGULAR!\n");
  exit(-1);
}

void timer(double *time) {
   static double tstart, tend, tprev;

   if (time == NULL) {
      clock();
      tstart = tprev = clock();
   } else {
      tend = clock();
      if (tend < tprev) tstart -= ULONG_MAX;
      tprev = tend;
      *time = (tend-tstart) / CLOCKS_PER_SEC;
   }
}

/* chequar tiempo limite */
void timelimit(long max)
{
   double t;
   if (max == 0) return;
   timer(&t); if (t >= max) stopped = TRUE;
}

/* comparaciones usadas en qsort() */
int hcomp(item *a, item *b)
{ int r; r = b->h - a->h; if (r != 0) return r; else return b->no - a->no; }
int vcomp(item *a, item *b)
{ int r; r = b->area-a->area; if (r != 0) return r; else return b->no - a->no; }

/* verifica la correctitud de las soluciones */
void checksol(allinfo *a, item *f, item *l)
{
   item *i, *j, *m;

   for (i = f, m = l+1; i != m; i++) {
      if (!i->k) continue;
      for (j = f; j != m; j++) {
	 if (i == j) continue;
	 if (i->no == j->no) error("\nItem %d duplicado !\n", i->no);
	 if (!j->k) continue;
	 if ((i->x + i->w > j->x) && (j->x + j->w > i->x)
	     && (i->y + i->h > j->y) && (j->y + j->h > i->y)) {
	    error("\nItems superpuestos %d, %d: [%d,%d] [%d,%d]",
		  i->no, j->no, i->w, i->h, j->w, j->h);
	 }
      }
   }
}

/* muestra los items por pantalla */
void printitems(item *f, item *l, itype W, itype H, int n)
{
   item *i;
   stype area;
   int m;

   area = 0; m = 0;
   printf("\n     [  w,  h] [  x,  y] area   r\n\n");
   for (i = f; i != l+1; i++) {
      i->area = AREA(i); area += i->area;
      printf("%3d: [%3hd,%3hd] [%3hd,%3hd] %4ld %3d\n",
	     i->no, i->w, i->h, i->x, i->y, i->area, i->r);
      m++;
   }
   printf("\nItems en total      : n = %3d\n", n);
   printf("Items empaquetados  : n = %3d\n", m);
   printf("Dimensiones tablero : W = %3d H = %3d\n", W, H);
   printf("Area items          : A = %3ld\n", area);
   printf("Area tablero        : A = %3ld\n", W*(long)H);
}

/* ordena los puntos en orden decreciente por la coordenada y */
/* mas rapido q qsort() segun Pisinger */
void psortdecr(point *f, point *l)
{
   register point mi;
   register point *i, *j, *m;
   register int d, r;

   d = l - f + 1;
   if (d <= 1) return;
   m = f + d / 2; if (DF(*f,*m) < 0) SWAPP(f,m);
   if (d == 2) return;
   if (DF(*m,*l)<0) { SWAPP(m,l); if (DF(*f,*m) < 0) SWAPP(f,m); }
   if (d <= 3) return;
   mi = *m; i = f; j = l;
   for (;;) {
      do i++; while (DF(*i,mi) > 0);
      do j--; while (DF(*j,mi) < 0);
      if (i > j) break; else SWAPP(i, j);
   }
   psortdecr(f, i-1); psortdecr(i, l);
}

/* encuentra los puntos de factivilidad entre f y l */
void envelope(point *f, point *l, point *s1, point **sm, itype W,
	      itype H, itype RW, itype RH, point **ll, stype *area)
{
   register point *i, *s, *t;
   register itype x, xx, y, ix, iy;
   register stype sum;

   x = xx = 0; y = H; sum = 0;
   for (i = t = f, s = s1; i != l; i++) {
      ix = i->x;
      if (ix <= x) {
	 continue;
      }
      iy = i->y;
      if ((x <= RW) && (iy <= RH)) {
	 s->x = x;
	 s->y = iy;
	 s++;
	 sum += (x - xx) * (ptype) y;
	 y = iy;
	 xx = x;
      }
      x = ix;
      *t = *i;
      t++;
   }
   if (y != 0) sum += (W - xx) * (ptype) y;
   *sm = s-1;
   *area = sum;
   *ll = t-1;
}

/* define los bordes de un objeto posicionado y calcula las dimensiones */
/* minimas de aquellos q no estan posionados */
void initboxes(item *f, item *l, point *fc, point **lc, int *minw, int *minh)
{
   register item *j, *m;
   register point *k;
   register int minx, miny;

   minx = *minw;
   miny = *minh;
   for (j = f, k = fc, m = l+1; j != m; j++) {
      if (j->k) {
	 k->x = j->x+j->w; k->y = j->y+j->h;
	 k++;
      } else {
	 if (j->w < minx) minx = j->w;
	 if (j->h < miny) miny = j->h;
      }
   }
   *minw = minx;
   *minh = miny;
   *lc = k-1;
}

/* cambia ancho por alto y alto por ancho, setea r = 1 */
void rotar_item(item *i)
{
   register itype w;
   w = i->w; i->w = i->h; i->h = w;
   i->r = 1;
}

stype findplaces(allinfo *a, item *f, item *l,
                 point *s1, point **sm, stype fill)
{
   register point *k;
   int minw, minh, W, H, RW, RH;
   point *lc, *sl, *st;
   stype area;
   point fc[MAXITEMS+1];

   minw = W = a->W; minh = H = a->H;
   initboxes(f, l, fc, &lc, &minw, &minh);

   /* ordena los objetos de acuerdo al mayor "y" y mayor "x" despues */
   if (lc >= fc) psortdecr(fc, lc); /* orden decreciente */

   /* S contendra la lista de puntos factibles de posicionar un item */
   sl = s1-1;
   /* se debe verificar que quede espacio suficiente en longuitud para items restantes */
   RW = W - minw; RH = H - minh;
   lc++; k = lc; k->x = W+1; k->y = 0;

   envelope(fc, lc+1, sl+1, &st, W, H, RW, RH, &lc, &area);
   sl = st;
   *sm = sl;
   return fill + (a->BAREA - area); /* la cota es lo que llevamos + lo que queda */
}

/* Algoritmo recursivo que en cada iteracion trata de posicionar
   un objeto y se llama recursivamente.
   code: http://www.diku.dk/~pisinger/3dbpp.c */

/* Recursive algorithm for solving a knapsack filling of a single bin. */
/* In each iteration, the set of feasible positions for placing a new */
/* box is found, and an upper bound on the filling is derived. If the */
/* bound indicates that an improved solution still may be obtained, every */
/* box is tried to be placed at every feasible position, before calling */
/* the algorithm recursively. */
void branch(allinfo *a, item *f, item *l, int miss, stype fill)
{
   register item *i;
   register point *s;

   int d;
   stype bound;
   point *sl, s1[4*MAXITEMS];

   if (stopped) return;
   a->iter3d++;
   if (a->iter3d % 100000 == 0) timelimit(maxtime);
   if (a->iter3d == a->maxiter) a->optimal = TRUE;
   if (a->optimal) return;

   if (miss == 0) {
      memcpy(a->fsol, f, sizeof(item) * DIF(f,l));
      a->maxfill = a->BAREA;
      a->optimal = TRUE;
      a->miss = miss;
   } else {
      if (fill > a->maxfill) {
	 memcpy(a->fsol, f, sizeof(item) * DIF(f,l));
	 a->maxfill = fill;
	 a->miss = miss;
      }

      bound = findplaces(a, f, l, s1, &sl, fill);

      if (bound > a->maxfill) {
	 /* por cada posicion en S, tratar de poner un item */
	 for (s = s1; s != sl+1; s++) {
	    for (i = f, d = 0; i != l+1; i++) {
	       if (i->k) continue;
	       /* ver si el item cabe en la posicion s */
	       if ((int) (s->x) + i->w > a->W) {
		  if ((int) (s->x) + i->h > a->W) {
		     continue;
		  } else {
		     if ((int) (s->y) + i->w > a->H) {
			continue;
		     }
		     else rotar_item(i);
		  }
	       } else {
		  if ((int) (s->y) + i->h > a->H) {
		     if ((int) (s->y) + i->w > a->H) {
			continue;
		     } else {
			if ((int) (s->x) + i->h > a->W) {
			   continue;
			} else rotar_item(i);
		     }
		  }
	       }

	       /* pone un item y se llama recursivamente */
	       i->k = TRUE; i->x = s->x; i->y = s->y;
	       printf("poniendo item %d en posicion (%d,%d) lleva %d area y faltan %d\n", i->no, i->x, i->y, (int)fill, miss-1);
	       branch(a, f, l, miss - 1, fill + i->area);
	       printf("puesto item %d en posicion (%d,%d) lleva %d area y faltan %d\n", i->no, i->x, i->y, (int) fill, miss);
	       i->k = FALSE; i->x = i->y = 0; d++;
	       if (a->optimal) break;
	    }
	 }
      }
   }
}

/* Knapsack filling of a single bin. The following procedure initializes */
/* some variables before calling the recursive branch-and-bound algorithm. */

boolean onebin_fill(allinfo *a, item *f, item *l, boolean fast)
{
   item *i, *j, *m;
   stype vol;
   int n;

   /* inicializa items */
   for (i = f, m = l+1, vol = 0; i != m; i++) {
      i->x = 0; i->y = 0; i->r = i->k = 0; vol += i->area;
   }

   n = DIF(f,l);
   a->iter3d  = 0;
   a->maxfill = vol-1;
   a->miss = n;
   a->maxiter = (fast ? MAXITER : 0);
   a->optimal = FALSE;

   branch(a, f, l, n, 0);

   /* se guarda la solucion */
   if (a->maxfill == a->BAREA) {
      for (i = a->fsol, j = f, m = l+1; j != m; i++, j++) *j = *i;
      return TRUE;
   }
   return FALSE;
}

void clearitems(allinfo *a)
{
   item *i, *m;

   for (i = a->fitem, m = a->litem+1; i != m; i++) {
      i->x = i->y = 0; i->r = i->k = FALSE; i->area = AREA(i);
   }

   /* ordenar en orden no decreciente de area */
   qsort(a->fitem, (m-a->fitem), sizeof(item), (funcptr) vcomp);
}

void inititems(allinfo *a, int *w, int *h)
{
   item *i, *m;
   int k;

   for (i = a->fitem, m = a->litem+1, k = 1; i != m; i++, k++) {
      i->no = k; i->w = w[k]; i->h = h[k]; i->area = AREA(i);
   }
   clearitems(a);
}

void branch_bound(int n, int W, int H, int * w, int * h,
		  int * x, int * y, int * r, int timelimit)
{
   allinfo a;
   item t0[MAXITEMS], t1[MAXITEMS];

   stopped = FALSE; maxtime = timelimit;

   /* verificar q no hayan mas de MAXITEMS objetos */
   if (n+1 > MAXITEMS) error("Demasiados objetos\n");
   a.n = n;
   a.W = W;
   a.H = H;
   a.fitem    = t0;
   a.litem    = a.fitem + a.n - 1;
   a.fsol     = t1;
   a.lsol     = a.fsol + a.n - 1;
   a.BAREA    = W * (ptype) H;
   a.optimal  = FALSE;
   a.maxfill  = 0;

   /* copiar items a la estructura principal */
   inititems(&a, w, h);

   /* branch and bound */
   onebin_fill(&a, a.fitem, a.litem, FALSE);

   /* verificar e imprimir la solucion */
   checksol(&a, a.fsol, a.lsol);
   printitems(a.fsol, a.lsol, a.W, a.H, a.n);
   escribe_archivo(a.n, a.W, a.H, a.fsol);
}
