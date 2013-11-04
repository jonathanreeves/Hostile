/* 
 * Hostile Takeover: 
 * CS248 Final Project, Fall 2004
 * 
 * Written by Jonathan Reeves
 * jrreeves@stanford.edu
 *
 * A simple 3D video game demonstrating the use of OpenGL for graphics 
 * rendering with a number of advanced techniques and optimizations to make
 * it interesting. See the README file which came with this package for more
 * details. It also includes a list of sources which were consulted while
 * building this software package.
 *
 */

#ifndef __FONT_H__
#define __FONT_H__

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

/* these are pretty self explanatory...small and large refer to actual font
 * size. I'm only using 2 */
GLvoid FontDestroyAll(GLvoid);

GLvoid FontDestroySmall(GLvoid);

GLvoid FontDestroyLarge(GLvoid);

GLvoid FontBuildSmall(GLvoid);

GLvoid FontBuildLarge(GLvoid);

GLvoid FontBuildAll(GLvoid);

/* Print our GL text to the screen */
GLvoid glPrint(int fontNum, const char *fmt, ... );

#endif
