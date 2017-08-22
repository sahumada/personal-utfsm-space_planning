/*********************************************************
 *  Copyright (c) 2005 Sergio Ahumada                    *
 *  Proyecto  : Space Planning con Branch and Bound      *
 *  Alumno    : Sergio Ahumada Navea                     *
 *                                                       *
 *  Profesor  : Dr. Carlos Grandon                       *
 *                                                       *
 *  Fecha     : 21 de junio de 2005                      *
 *  Archivo   : include/prototipos.h                     *
 *********************************************************/
/* prototipos */
void branch_bound(int, int, int, int *, int *, int *, int *, int *, int);
void lee_archivo (int *, int *, int *, int *, int *, char *);
void escribe_archivo (int, int, int, item *);
