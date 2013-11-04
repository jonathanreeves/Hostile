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
 
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "font.h"


GLuint base0; /* Base Display List For first Font Set */
GLuint base1; /* Base Display List For second Font Set */

/* function to recover memory form our list of characters */
GLvoid FontDestroyAll(GLvoid){
  glDeleteLists(base0, 96);
  glDeleteLists(base1, 96);
  return;
}

GLvoid FontDestroySmall(GLvoid){
  glDeleteLists(base0, 96);
  return;
}

GLvoid FontDestroyLarge(GLvoid){
  glDeleteLists(base1, 96);
  return;
}

GLvoid FontBuildAll(GLvoid){
  FontBuildSmall();
  FontBuildLarge();
}

/* function to build our font list */
GLvoid FontBuildSmall(GLvoid){

  Display *dpy;          /* Our current X display */
  XFontStruct *fontInfo; /* Our font info */

  /* Storage for 96 characters */
  base0 = glGenLists(96);

  /* Get our current display long enough to get the fonts */
  dpy = XOpenDisplay(NULL);

  /* Get the font information */
  fontInfo = XLoadQueryFont( dpy, 
			"-adobe-helvetica-medium-r-normal--18-*-*-*-p-*-iso8859-1" );

  /* If the above font didn't exist try one that should */
  if (fontInfo == NULL){
	fontInfo = XLoadQueryFont( dpy, "fixed" );
	/* If that font doesn't exist, something is wrong */
	if ( fontInfo == NULL ){
      fprintf( stderr, "no X font available?\n" );
	}
  }

  /* generate the list */
  glXUseXFont(fontInfo->fid, 32, 96, base0);

  /* Recover some memory */
  XFreeFont( dpy, fontInfo );

  /* close the display now that we're done with it */
  XCloseDisplay( dpy );
  return;
}

/* function to build our font list */
GLvoid FontBuildLarge(GLvoid){

  Display *dpy;          /* Our current X display */
  XFontStruct *fontInfo; /* Our font info */

  /* Storage for 96 characters */
  base1 = glGenLists(96);

  /* Get our current display long enough to get the fonts */
  dpy = XOpenDisplay(NULL);

  /* Get the font information */
  fontInfo = XLoadQueryFont( dpy, 
			"-adobe-helvetica-bold-r-normal--60-*-*-*-p-*-iso8859-1" );

  /* If the above font didn't exist try one that should */
  if (fontInfo == NULL){
	fontInfo = XLoadQueryFont( dpy, "fixed" );
	/* If that font doesn't exist, something is wrong */
	if ( fontInfo == NULL ){
      fprintf( stderr, "no X font available?\n" );
	}
  }

  /* generate the list */
  glXUseXFont(fontInfo->fid, 32, 96, base1);

  /* Recover some memory */
  XFreeFont( dpy, fontInfo );

  /* close the display now that we're done with it */
  XCloseDisplay( dpy );
  return;
}


/* Print our GL text to the screen */
GLvoid glPrint(int fontNum, const char *fmt, ... ){

  char text[256]; /* Holds our string */
  va_list ap;     /* Pointer to our list of elements */

  /* If there's no text, do nothing */
  if (fmt == NULL)
    return;

  /* Parses The String For Variables */
  va_start(ap, fmt);
    /* Converts Symbols To Actual Numbers */
  vsprintf(text, fmt, ap);
  va_end(ap);

  /* Pushes the Display List Bits */
  glPushAttrib(GL_LIST_BIT);

  /* Sets base character to 32 */
  if(fontNum == 0){
    glListBase(base0 - 32);
  }
  else{
    glListBase(base1 - 32);
  }

  /* Draws the text */
  glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);

  /* Pops the Display List Bits */
  glPopAttrib();
}

