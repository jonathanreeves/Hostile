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
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <time.h>

#include "main.h"

/* some paramters you can mess with... */
int screenWidth = SCREEN_WIDTH;
int screenHeight = SCREEN_HEIGHT;
float mouseSens = MOUSE_SENS;

/* the camera */
camera_t cam;

/* physical dimensions of the screen */
GLint middleX;
GLint middleY;

/* used to test the position of the camera relative to terrain */
GLfloat curX, curY, curZ; 

GLfloat diff[4] = {0.2, 0.2, 0.2, 1.0};

projectile_t *proj = NULL;


/* initialization of everything */
void init(void){

  /* starting position and view */
  float sPosX, sPosY, sPosZ; /* player starting position */
  float sViewX, sViewY, sViewZ; /* player starting forward vector */
  
  srand(time(NULL));

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
 
  /* setup light properties */
  SetupLights();

  /* setup the fonts */
  FontBuildAll();
  
  FogPlaneSetup(40.0, 50.0);

  printf("loading terrain...\n");
  ground = TerrainLoadMap("maps/size256edge.ppm", "textures/lawn.ppm");
  
  printf("loading water...\n");
  water = TerrainMakeWater(ground->sizeX, ground->sizeZ, waterLevel, 
				                     "textures/water.ppm");
  sizeX = (GLfloat)ground->sizeX;
  sizeZ = (GLfloat)ground->sizeZ;

  /* get ready to set up the first player (you) */
  sPosX = (sizeX-1)/2.0;
  sPosZ = (sizeZ-1)/2.0;
  sViewX = 1.0;
  sViewZ = sizeZ;
  sPosY = TerrainGetHeight(ground, sPosX, sPosZ) + MOD_HEIGHT2;
  sViewY = TerrainGetHeight(ground, sViewX, sViewZ);

  printf("loading player 1...\n");
  sod = PlayerNew(); 
  PlayerLoad(sod, "models/sod/sod.md2", "models/sod/sod.ppm");
  
  PlayerSetPosition(sod, sPosX, sPosY, sPosZ);
  PlayerSetUpVector(sod, 0.0, 1.0, 0.0);
  PlayerSetForwardByAngle(sod, 0.0, 30.0);
  PlayerSetAnimation(sod, PLAYER_ANIM_IDLE0);

  printf("loading %d ogros into a flock...\n", 6);
  flocks[0] = AIFlockNew();
  AIFlockLoadAll(flocks[0], "models/ogro/ogro.md2", 
				  "models/ogro/ogro.ppm", 6);

  printf("loading %d bobafetts into a flock...\n", 6);
  flocks[1] = AIFlockNew();
  AIFlockLoadAll(flocks[1], "models/boba/bobafett.md2", 
				         "models/boba/bobafett.ppm", 6);

  /* set up the camera */
  CameraInit(&cam); 
  CameraSetUpVector(&cam, 0.0, 1.0, 0.0);
  CameraFollowPlayer(&cam, sod, camDistance, camHeight);  
  
  printf("setting up skybox...\n");
  /* arguments are width, length, height, centerX, centerZ */
  sky = SkyboxLoad(400, 400, 100, (sizeX-1)/2.0, (sizeZ-1)/2.0);
  plist = PListNew();
}


/* what happens when you resize the window */
void reshape(int w, int h){
  middleX = w >> 1; /* fast divide by 2 */
  middleY = h >> 1;
  glutWarpPointer(middleX, middleY);
 
  screenWidth = w;
  screenHeight = h; 
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
   
  /* load a fake projection matrix for the frustum culling */
  gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 0.5, 80.0);
  FrustumUpdateProjection(&frust);

  /* load the real projection matrix */
  glLoadIdentity();
  gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 0.5, 500.0);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}



/* these four globals are used only in display() to calculate and show fps  */
float lastTime = 0.0;
float curTime = 0.0;
float diffTime = 0.0;
float fps, dfps;
int count = 0;



/* the display function... */
void display(void){

  curTime = (float)clock()/CLOCKS_PER_SEC;
  diffTime = curTime - lastTime;
  fps = 1.0/diffTime;
  lastTime = curTime;
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* needs to be done for mouse look to work correctly */
  glutWarpPointer(middleX, middleY);

  /* up date the protagonist's position and state */
  PlayerUpdate(sod);
  
  /* set the camera on the protagonist */
  CameraFollowPlayer(&cam, sod, camDistance, camHeight);  

  /* protagonist is set, now process the antagonists */
  if(flocks[0]->nBots != 0){
    AIFlockUpdate(flocks[0], sod);
  }
  if(flocks[1]->nBots != 0){
    AIFlockUpdate(flocks[1], sod);
  }
	
  /* run through the projectile list and update */
  PListUpdate(plist, sod, flocks, 2);

  /* preprocessing is done, now update the screen  */
  glPushMatrix();
    CameraUpdateView(&cam);
	FrustumUpdateView(&frust);
    
	/* draw the sky box now that blending is disabled */
	glColor3f(1.0, 1.0, 1.0);
    SkyboxDraw(sky);
  
    glEnable(GL_LIGHTING);	
    glLightfv(GL_LIGHT0, GL_POSITION, position0);
	glEnable(GL_LIGHT0);
	
	/* draw the protagonist	 */
	glPushMatrix();
      glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);	
#ifdef NOFRUST
	  PlayerAnimate(sod, 0.12);
#else
	  PlayerAnimateCulled(sod, &frust, 0.2);
#endif
	glPopMatrix();

	glEnable(GL_FOG);
	/* draw the antagonist */
#ifdef NOFRUST
    AIFlockDraw(flocks[0]);
    AIFlockDraw(flocks[1]);
#else
    AIFlockDrawCulled(flocks[0]);
    AIFlockDrawCulled(flocks[1]);
#endif
 	
	glDisable(GL_LIGHT0);
    glDisable(GL_BLEND);

	/* draw the terrain	 */
	glDisable(GL_LIGHTING);
#ifdef NOFRUST
    TerrainDrawShaded(ground);
#else
    TerrainDrawShadedCulled(ground, &frust);
#endif
	FogPlaneDraw(sod->yaw, cam.position.x, waterLevel-5.0, cam.position.z);
	
	/* draw the projectiles	 */
    PListDraw(plist);
   	
    glDisable(GL_CULL_FACE);
	
	
	glEnable(GL_BLEND);
    /* draw the water */
	glColor4f(0.5, 0.5, 1.0, 0.6);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

#ifdef NOFRUST
    TerrainDraw(water);
#else
    TerrainDrawCulled(water, &frust);
#endif

  glPopMatrix();
  
  glDisable(GL_FOG);
  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);

  /* display frames per second, and enemies */  
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glPushMatrix();
   if(!(count % 20)){
	  dfps = fps;
	  count = 0;
	} count++;
    glColor4f(0.0, 0.5, 1.0, 1.0);
    glTranslatef(0.0, 0.0, -1.0);
	glRasterPos2f(-0.45, -0.3);
	glPrint(0, "total enemies: %d", flocks[0]->nBots + flocks[1]->nBots);
	glRasterPos2f(-0.45, -0.34);
    glPrint(0, "fps: %f", dfps);	
  glPopMatrix(); 

  /* display your health */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
    glLoadIdentity();
	glOrtho(0.0, screenWidth, screenHeight, 0.0, 0.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
	
	glPushMatrix();
	  glLoadIdentity();
	  glBegin(GL_QUADS);
	    glColor4f(1.0, 1.0, 0.0, 0.6);
	    glVertex2f(55.0, 55.0);
	    glVertex2f(55.0 + ((sod->health)*IMAX_HEALTH*340.0), 55.0);
	    glVertex2f(55.0 + ((sod->health)*IMAX_HEALTH*340.0), 35.0);
	    glVertex2f(55.0, 35.0);  
		
		glColor4f(1.0, 1.0, 1.0, 0.3);
	    glVertex2f(50.0, 60.0);
	    glVertex2f(400.0, 60.0);
	    glVertex2f(400.0, 30.0);
	    glVertex2f(50.0, 30.0);
	  glEnd();
	glPopMatrix();
	
	glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
	
  /* all done */
  glutSwapBuffers();
}


/* yay mouse look... */
void mouseMotionFunc(int x, int y){
  
  vector_t xaxis = {1.0, 0.0, 0.0};
  
  sod->yaw += ((float)(middleX - x)) * mouseSens;
  sod->pitch -= ((float)(middleY - y)) * mouseSens;

  /* put a maximum span on the pitch so we can't flip over */
  if(sod->pitch >= 70.0)
    sod->pitch = 70.0;
  else if(sod->pitch <= -70.0)
	sod->pitch = -70.0;

  /* put a maximum span on the yaw so we don't overflow the angles */
  if(sod->yaw >= 360.0) 
	sod->yaw = sod->yaw - 360.0;
  else if(sod->yaw <= 0.0)
	sod->yaw = sod->yaw + 360.0;

  /* use fast rotations for canonical basis rotations */
  VectorRotateZ(&xaxis, sod->pitch);
  VectorRotateY(&xaxis, sod->yaw);
  PlayerSetForward(sod, xaxis.x, xaxis.y, xaxis.z);

}


/* yay mouse click... */
void mouseFunc(int button, int state, int x, int y){
  
  if(sod->state != PLAYER_DEAD){
    if(button == GLUT_LEFT_BUTTON){
      switch(state){
        case GLUT_DOWN:
		  proj = ProjectileNew(&sod->forward, &sod->position, PROJ_SPEED, 0);
		  PListInsertAfter(plist, proj);
	    break;
	  }
    }
  }	
}


/* yay keyboard.. */
void keyboardDownFunc(unsigned char key, int x, int y){
  switch(key){
    case 27:
	  /* all done, cleanup */
	  Cleanup();
	  break;
	case 'w':
	  sod->fFlag = 1;
	  break;
	case 's':
	  sod->bFlag = 1;
	  break;
	case 'a':
	  sod->lFlag = 1;
	  break;
	case 'd':
	  sod->rFlag = 1;
	  break;
	case 'q':
	  sod->jFlag = 1;
	  break;
	default:
	  break;
  }

  if(sod->rFlag || sod->lFlag || sod->fFlag || sod->bFlag)
    PlayerProcessEvent(sod, PLAYER_RUNNING);
  else if(sod->jFlag)
    PlayerProcessEvent(sod, PLAYER_JUMPING);
  else
	PlayerProcessEvent(sod, PLAYER_IDLE);
}


/* yay keyboard up... */
void keyboardUpFunc(unsigned char key, int x, int y){
  switch(key){
	case 'w':
	  sod->fFlag = 0;
	  break;
	case 's':
	  sod->bFlag = 0;
	  break;
	case 'a':
	  sod->lFlag = 0;
	  break;
	case 'd':
	  sod->rFlag = 0;
	  break;
    case 'q':
	  sod->jFlag = 0;
	  break;
	default:
	  break;
  }	

  if(sod->rFlag || sod->lFlag || sod->fFlag || sod->bFlag)
    PlayerProcessEvent(sod, PLAYER_RUNNING);
  else if(sod->jFlag)
    PlayerProcessEvent(sod, PLAYER_JUMPING);
  else
	PlayerProcessEvent(sod, PLAYER_IDLE);

}

/* yay main... */
int main(int argc, char **argv){
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(screenWidth, screenHeight);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutReshapeFunc(reshape);
  glutPassiveMotionFunc(mouseMotionFunc);
  glutMotionFunc(mouseMotionFunc);
  glutMouseFunc(mouseFunc);
  glutKeyboardFunc(keyboardDownFunc);
  glutKeyboardUpFunc(keyboardUpFunc);
  glutSetCursor(GLUT_CURSOR_NONE);
  init();
  glutMainLoop();
  
  return 0;
}


