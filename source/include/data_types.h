/*********************************************************
 *  Copyright (c) 2005 Sergio Ahumada                    *
 *  Proyecto  : Space Planning con Branch and Bound      *
 *  Alumno    : Sergio Ahumada Navea                     *
 *                                                       *
 *  Profesor  : Dr. Carlos Castro                        *
 *                                                       *
 *  Fecha     : 21 de junio de 2005                      *
 *  Archivo   : include/data_types.h                     *
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

/* numero maximo de elementos permitidos */
#define MAXITEMS   101
/* numero maximo de iteraciones */
#define MAXITER   1000
/* variables booleanas */
#define TRUE  1
#define FALSE 0
/* rotacion de los items */
#define WDIM  0
#define HDIM  1

/* macros */
#define AREA(i)         ((i)->h * (long) (i)->w)
#define DIF(i,j)        ((int) ((j) - (i) + 1))
#define SWAP(a,b)       { register item t; t=*(a);*(a)=*(b);*(b)=t; }
#define SWAPP(a,b)      { register point t; t=*(a);*(a)=*(b);*(b)=t; }
#define DF(a,b)         ((r=(a).y-(b).y) != 0 ? r : (a).x-(b).x)

/* tipos de datos */
typedef short   boolean;
typedef short   ntype;
typedef short   itype;
typedef long    stype;
typedef double  ptype;

typedef int (*funcptr) (const void *, const void *);

/* objetos */
typedef struct irec {
   ntype    no;      /* objeto numero */
   itype    w;       /* dimension w */
   itype    h;       /* dimension h */
   itype    x;       /* posicion x optima */
   itype    y;       /* posicion y optima */
   boolean  r;       /* rotado o no rotado */
   boolean  k;       /* si ha sido elegido */
   stype    area;    /* area del item */
} item;

/* information completa del problema */
typedef struct {
  itype    W;        /* dimension W de la paleta */
  itype    H;        /* dimension H de la paleta */
  stype    BAREA;    /* area */
  ntype    n;        /* numbero total de objetos */
  item     *fitem;   /* primer item del problema */
  item     *litem;   /* ultimo item del problema */
  item     *fsol;    /* primer item en la solucion */
  item     *lsol;    /* ultimo item en la solucion */
  itype    mindim;   /* menor dimension de los items */
  itype    maxdim;   /* mayor dimension de los items */
  stype    maxfill;  /* maximo llenado de objetos */
  boolean  optimal;  /* solution optima ? */
  long     maxiter;  /* maximo numero de iterations */
  long     miss;     /* numero de objetos que faltan por poner */
  long     iter3d;   /* iterationes */
} allinfo;

/* estructura para fijar los puntos extremos */
typedef struct {
  itype    x;       /* posicion x */
  itype    y;       /* posicion y */
} point;
