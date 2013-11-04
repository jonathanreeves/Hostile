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

#ifndef __FOGPLANE_H__
#define __FOGPLANE_H__
/* nothing like a header file with 2 lines of real stuff */
void FogPlaneSetup(float near, float far);

void FogPlaneDraw(float yaw, float x, float y, float z);

#endif
