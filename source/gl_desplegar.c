/*********************************************************
 *  Copyright (c) 2005 Sergio Ahumada                    *
 *  Proyecto  : Space Planning con Branch and Bound      *
 *  Alumno    : Sergio Ahumada Navea                     *
 *                                                       *
 *  Profesor  : Dr. Carlos Castro                        *
 *                                                       *
 *  Fecha     : 21 de junio de 2005                      *
 *  Archivo   : gl_desplegar.c                           *
 *********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glut.h>
#include "include/data_types.h"

GLfloat vertices [4][3] = {{-1.0,-1.0,1.0},{1.0,-1.0,1.0},{1.0,1.0,1.0},{-1.0,1.0,1.0}};
/* Definicion de los colores a usar */
GLfloat colores[][3] = {
     {0.0,0.0,0.0},{0.0,0.0,0.5},{0.0,0.0,1.0},
     {0.0,0.5,0.0},{0.0,0.5,0.5},{0.0,0.5,1.0},
     {0.0,1.0,0.0},{0.0,1.0,0.5},{0.0,1.0,1.0},
     {0.5,0.0,0.0},{0.5,0.0,0.5},{0.5,0.0,1.0},
     {0.5,0.5,0.0},{0.5,0.5,0.5},{0.5,0.5,0.1},
     {0.5,1.0,0.0},{0.5,1.0,0.5},{0.5,1.0,1.0},
     {1.0,0.0,0.0},{1.0,0.0,0.5},{1.0,0.0,0.0},
     {1.0,0.5,0.0},{1.0,0.5,0.5},{1.0,0.5,1.0},
     {1.0,1.0,0.0},{1.0,1.0,0.5},{1.0,1.0,1.0}};

int W, H, n;
int w[MAXITEMS], h[MAXITEMS];
int x[MAXITEMS], y[MAXITEMS];
int r[MAXITEMS];

void lee_archivo (int * n, int * w, int * h, int * W, int * H, char * camino)
{
   int m = 1, i[MAXITEMS];

   FILE * entrada;
   if ((entrada = fopen(camino, "r")) == NULL) {
      printf("No se encontro archivo de entrada\n");
      exit(1);
   } else {
      fscanf(entrada, "%d %d", W, H);
      fscanf(entrada, "%d", n);
      do {
	 fscanf(entrada, "%d %d %d %d %d %d", i+m, w+m, h+m, x+m, y+m, r+m);
	 m++;
      }
      while (!feof(entrada));
   }
   fclose(entrada);
}

void dibujar_contorno(float posx, float posy, float posw, float posh)
{
   glBegin(GL_LINES);
   glColor3fv(colores[26]);
   glVertex3f(posx,posy,0);
   glVertex3f(posx+posw,posy,0);
   glVertex3f(posx+posw,posy,0);
   glVertex3f(posx+posw,posy+posh,0);
   glVertex3f(posx+posw,posy+posh,0);
   glVertex3f(posx,posy+posh,0);
   glVertex3f(posx,posy+posh,0);
   glVertex3f(posx,posy,0);
   glEnd();
}

void dibujar_rectangulo (float posx, float posy, float posw, float posh, int color)
{
   /* Aca se dibuja cada objeto (parece q las coordenadas estan al reves) */
   glBegin(GL_POLYGON);
   glColor3fv(colores[color]);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, colores[color]);
   glVertex3f(posx, posy+posh, 0);
   glNormal3f(posx, posy+posh, 0);
   glVertex3f(posx+posw, posy+posh, 0);
   glNormal3f(posx+posw, posy+posh, 0);
   glVertex3f(posx+posw, posy, 0);
   glNormal3f(posx+posw, posy, 0);
   glVertex3f(posx, posy, 0);
   glNormal3f(posx, posy, 0);
   glEnd();
}

void dibujar_matriz(void)
{
   int i, color = 1;
   for (i = 1; i <= n; i++) {
      /* Se dibujan objetos de a uno */
      dibujar_rectangulo((GLfloat)x[i], (GLfloat)y[i], (GLfloat)w[i], (GLfloat)h[i], color);
      dibujar_contorno((GLfloat)x[i], (GLfloat)y[i], (GLfloat)w[i], (GLfloat)h[i]);
      if (color > 25) color = 1;
      else color++;
   }
}

void display(void){
   glClear(GL_COLOR_BUFFER_BIT);
   dibujar_matriz();
   dibujar_contorno(0, 0, (GLfloat)W, (GLfloat)H);
   glFlush();
}

void myReshape(int w, int h){
   glViewport(0, 0, w, h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   /* Centramos todos los poligonos */
   glOrtho(-4.0, W+4.0, -4.0, H+4.0, -4.0, 4.0);
   glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
   if (argc == 2) {
      lee_archivo(&n, w, h, &W, &H, argv[1]);
   }
   else {
      lee_archivo(&n, w, h, &W, &H, "output/sal-bb.txt");
   }

   glutInit(&argc,argv);
   glutInitDisplayMode(GLUT_RGB);
   glutInitWindowSize(500,500);
   glutCreateWindow("OpenGL: Space Planning 3D - Optimizacion Combinatoria");
   glutReshapeFunc(myReshape);
   glutDisplayFunc(display);
   glutMainLoop();
   return 0;
}

