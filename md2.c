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

#include "md2.h"

MD2Model_t *MD2ModelAlloc(void){
  MD2Model_t *thisMod;
  thisMod = (MD2Model_t *)malloc(sizeof(MD2Model_t));
  if(thisMod != NULL){
    MD2ModelInit(thisMod);
  }
  return thisMod; /* may still be NULL, need to check when called */
}

/* just set everything clear */
void MD2ModelInit(MD2Model_t *model){
  model->numVertices = 0;    
  model->numTriangles = 0;   
  model->numFrames = 0;      
  model->numST = 0;         
  model->frameSize = 0;      
  model->triIndex = NULL; 
  model->texCoordList = NULL;  
  model->vertexList = NULL; 
  model->normalList = NULL;
  model->texID = -1;
  model->usageCount = 0;
}    

/* load the model data (vertices, texture coordinates, etc...) */
int MD2ModelLoad(MD2Model_t *mod, const char *md2File, const char *skinFile){

  FILE *filePtr;                         
  int fileLen;                           
  char *buffer;                       
  MD2Header_t *modelHeader;    
  texCoordIndex_t *stPtr;    
  frame_t *frame;         
  vector_t *vertexListPtr; 
  mesh_t *bufIndexPtr;  
  int i, j; 
  int nFrames;
  int nPoints; 
  int offsetFrames;
  int frameSize;
  int err;
  texture_t tempTex;
  vector_t tri[3];
  vector_t *vList;
  vector_t *nList;
  
  /* open the model file */
  filePtr = fopen(md2File, "rb"); 
  if (filePtr == NULL)
    return -1;
     
  /* find length of file */
  fseek(filePtr, 0, SEEK_END);
  fileLen = ftell(filePtr);
  fseek(filePtr, 0, SEEK_SET);
     
  /* read entire file into buffer */
  buffer = (char *)malloc((fileLen+1)*sizeof(char));
					 
  err = fread(buffer, sizeof(char), fileLen, filePtr);
  if(err != fileLen){
    free(buffer);
	return -1;
  }
     
  /* extract model file header from buffer */
  modelHeader = (MD2Header_t *)buffer;
  nPoints = modelHeader->numPoints;
  nFrames = modelHeader->numFrames; 
  frameSize = modelHeader->frameSize;
  offsetFrames = modelHeader->offsetFrames;

  /* set up information in the model, allocate memory */
  mod->numVertices = nPoints;
  mod->numFrames = nFrames;
  mod->frameSize = frameSize;
  mod->numST = modelHeader->numST;
  mod->numTriangles = modelHeader->numTris;
  
  /* get memory */
  mod->texCoordList = (texCoord_t *)malloc(mod->numST*sizeof(texCoord_t));
  if(mod->texCoordList == NULL){
	fclose(filePtr);
	free(buffer);
    return -1;
  }
  mod->vertexList = (vector_t *)malloc(nPoints*nFrames*sizeof(vector_t));
  if(mod->vertexList == NULL){
	fclose(filePtr);
	free(buffer);
    free(mod->texCoordList);
    return -1;
  }
  mod->normalList = (vector_t *)malloc(mod->numTriangles*nFrames*sizeof(vector_t));
  if(mod->normalList == NULL){
	fclose(filePtr);
	free(buffer);
	free(mod->texCoordList);
	free(mod->vertexList);
    return -1;
  }

  mod->triIndex = (mesh_t *)malloc(mod->numTriangles*sizeof(mesh_t));
  if(mod->triIndex == NULL){
	fclose(filePtr);
	free(buffer);
	free(mod->normalList);
	free(mod->texCoordList);
	free(mod->vertexList);
    return -1;
  }

  /* pull the vertices out of the file  */
  for (j = 0; j < nFrames; j++){
 		  
	frame = (frame_t *)(buffer + offsetFrames + frameSize * j);
    vertexListPtr = (mod->vertexList + nPoints * j);
    
	for (i = 0; i < nPoints; i++){
      vertexListPtr[i].x = 
			  frame->scale[0] * frame->fp[i].v[0] + frame->translate[0];
      vertexListPtr[i].y = 
			  frame->scale[1] * frame->fp[i].v[1] + frame->translate[1];
      vertexListPtr[i].z = 
			  frame->scale[2] * frame->fp[i].v[2] + frame->translate[2];
    }
  }
  
  /* load the skin  */
  mod->texID = TextureLoadSkin(&tempTex, skinFile);
  if(mod->texID == -1){
	free(buffer);
	free(mod->vertexList);
	free(mod->texCoordList);
	free(mod->triIndex);
	fclose(filePtr);
    return -1;
  }
     
  /* get the scaled texture coordinates for the model  */
  stPtr = (texCoordIndex_t *)(buffer + modelHeader->offsetST);
  for (i = 0; i < mod->numST; i++){
    mod->texCoordList[i].s = (float)stPtr[i].s / (float)tempTex.sizeX;
    mod->texCoordList[i].t = (float)stPtr[i].t / (float)tempTex.sizeY;
  }
     
     
  /* pull the triangle indices out */
  bufIndexPtr = (mesh_t *)(buffer + modelHeader->offsetTris);
     
  /* create a mesh (triangle) list */
  for (j = 0; j < nFrames; j++){
  
    /* for all triangles in each frame */
    for(i = 0; i < mod->numTriangles; i++){
      mod->triIndex[i].meshIndex[0] = bufIndexPtr[i].meshIndex[0];
      mod->triIndex[i].meshIndex[1] = bufIndexPtr[i].meshIndex[1];
      mod->triIndex[i].meshIndex[2] = bufIndexPtr[i].meshIndex[2];
      mod->triIndex[i].texCoordIndex[0] = bufIndexPtr[i].texCoordIndex[0];
      mod->triIndex[i].texCoordIndex[1] = bufIndexPtr[i].texCoordIndex[1];
      mod->triIndex[i].texCoordIndex[2] = bufIndexPtr[i].texCoordIndex[2];

    }
  
  }

  /* calculate normals for all vertices for all frames */

  for (j = 0; j < nFrames; j++){
    for (i = 0; i < mod->numTriangles; i++){
    
	  vList = (mod->vertexList + mod->numVertices*j);
      nList = (mod->normalList + i + mod->numTriangles*j);
	  
      /* store vertices of triangle in correct order */
      tri[0] = vList[mod->triIndex[i].meshIndex[0]];
      tri[2] = vList[mod->triIndex[i].meshIndex[2]];
      tri[1] = vList[mod->triIndex[i].meshIndex[1]];
          
      /* calculate the normal of the triangle */
	  VectorNormal(nList, &tri[0], &tri[2], &tri[1]);
    }
  }
 
  fclose(filePtr);
  free(buffer);
  mod->usageCount = 1; /* after loading, it's the first time it's been used */
  
  return 0;
}

/* used for animation. Does linear interpolation between keyframes */
int MD2ModelAnimate(MD2Model_t *model, int currentFrame, int nextFrame, float interpol){

  vector_t *vList, *nList;             
  vector_t *nextVList, *nextNList;      
  int i;                
  float x1, y1, z1;   
  float x2, y2, z2;  
  float nx1, ny1, nz1;   
  float nx2, ny2, nz2;  
     
  vector_t vertex[3]; /* interpolated vertices  */
  vector_t normal; /* interpolated normals */
    
  vList = (model->vertexList + model->numVertices*currentFrame);
  nList = (model->normalList + model->numTriangles*currentFrame);
  nextVList = (model->vertexList + model->numVertices*nextFrame);
  nextNList = (model->normalList + model->numTriangles*nextFrame);
  
  glBindTexture(GL_TEXTURE_2D, model->texID);
  glBegin(GL_TRIANGLES);
  for (i = 0; i < model->numTriangles; i++){
		  
    /* get first points of each frame and first normals of each frame */
    x1 = vList[model->triIndex[i].meshIndex[0]].x;
    y1 = vList[model->triIndex[i].meshIndex[0]].y;
    z1 = vList[model->triIndex[i].meshIndex[0]].z;
    x2 = nextVList[model->triIndex[i].meshIndex[0]].x;
    y2 = nextVList[model->triIndex[i].meshIndex[0]].y;
    z2 = nextVList[model->triIndex[i].meshIndex[0]].z;
    
    /* store first interpolated vertex of triangle */
    vertex[0].x = x1 + interpol * (x2 - x1);
    vertex[0].y = y1 + interpol * (y2 - y1);
    vertex[0].z = z1 + interpol * (z2 - z1);
        
	/* get second points and normals of each frame */
    x1 = vList[model->triIndex[i].meshIndex[2]].x;
    y1 = vList[model->triIndex[i].meshIndex[2]].y;
    z1 = vList[model->triIndex[i].meshIndex[2]].z;
    x2 = nextVList[model->triIndex[i].meshIndex[2]].x;
    y2 = nextVList[model->triIndex[i].meshIndex[2]].y;
    z2 = nextVList[model->triIndex[i].meshIndex[2]].z;
    
    /* store second interpolated vertex and normal of triangle */
    vertex[2].x = x1 + interpol * (x2 - x1);
    vertex[2].y = y1 + interpol * (y2 - y1);
    vertex[2].z = z1 + interpol * (z2 - z1);   
 
    /* get third points and normal of each frame */
    x1 = vList[model->triIndex[i].meshIndex[1]].x;
    y1 = vList[model->triIndex[i].meshIndex[1]].y;
    z1 = vList[model->triIndex[i].meshIndex[1]].z;
    x2 = nextVList[model->triIndex[i].meshIndex[1]].x;
    y2 = nextVList[model->triIndex[i].meshIndex[1]].y;
    z2 = nextVList[model->triIndex[i].meshIndex[1]].z;
 
    /* store third interpolated vertex and normal of triangle */
    vertex[1].x = x1 + interpol * (x2 - x1);
    vertex[1].y = y1 + interpol * (y2 - y1);
    vertex[1].z = z1 + interpol * (z2 - z1);
 
	nx1 = nList[i].x;
    ny1 = nList[i].y;
    nz1 = nList[i].z;
    nx2 = nextNList[i].x;
    ny2 = nextNList[i].y;
    nz2 = nextNList[i].z;
       
	normal.x = nx1 + interpol * (nx2 - nx1);
    normal.y = ny1 + interpol * (ny2 - ny1);
    normal.z = nz1 + interpol * (nz2 - nz1);
    VectorNormalize(&normal);   
	
    /* render properly textured and normaled triangle */
	glNormal3fv((float*)(&normal));
    glTexCoord2f(model->texCoordList[model->triIndex[i].texCoordIndex[0]].s,
                 model->texCoordList[model->triIndex[i].texCoordIndex[0]].t);
    glVertex3fv((float *)(&vertex[0]));
      
   	
	glNormal3fv((float*)(&normal));
    glTexCoord2f(model->texCoordList[model->triIndex[i].texCoordIndex[2]].s ,
                 model->texCoordList[model->triIndex[i].texCoordIndex[2]].t);
    glVertex3fv((float *)(&vertex[2]));
         
	glNormal3fv((float*)(&normal));
    glTexCoord2f(model->texCoordList[model->triIndex[i].texCoordIndex[1]].s,
                 model->texCoordList[model->triIndex[i].texCoordIndex[1]].t);
    glVertex3fv((float *)(&vertex[1]));
  }
  glEnd();
     
  return 0;
}

/* render a single key frame */
int MD2ModelDrawFrame(MD2Model_t *model, int keyFrame){
  
  vector_t *vList;
  vector_t *normal;
  int i;
     
  vList = (model->vertexList + model->numVertices * keyFrame);
  normal = (model->normalList + model->numTriangles * keyFrame);
  
  glBindTexture(GL_TEXTURE_2D, model->texID);
     
  /* display the textured model with proper lighting normals */
  glBegin(GL_TRIANGLES);
  for(i = 0; i < model->numTriangles; i++){

    glNormal3fv((float *)(normal));

	glTexCoord2f(model->texCoordList[model->triIndex[i].texCoordIndex[0]].s,
			     model->texCoordList[model->triIndex[i].texCoordIndex[0]].t);
    glVertex3fv((float *)(&vList[model->triIndex[i].meshIndex[0]]));
          
	glTexCoord2f(model->texCoordList[model->triIndex[i].texCoordIndex[2]].s,
				 model->texCoordList[model->triIndex[i].texCoordIndex[2]].t);
    glVertex3fv((float *)(&vList[model->triIndex[i].meshIndex[2]]));
          
	glTexCoord2f(model->texCoordList[model->triIndex[i].texCoordIndex[1]].s,
				 model->texCoordList[model->triIndex[i].texCoordIndex[1]].t);
    glVertex3fv((float *)(&vList[model->triIndex[i].meshIndex[1]]));
  }

  glEnd();
     
  return 0;
}


int MD2ModelDestroy(MD2Model_t *model){
  /* if other people are using this model, don't delete it */
  if(model->usageCount > 1){
    model->usageCount--;
	return 0;
  }
  if (model->triIndex != NULL)
    free(model->triIndex);
  if (model->vertexList != NULL)
    free(model->vertexList);
  if (model->texCoordList != NULL)
    free(model->texCoordList);
  if (model->normalList != NULL)
    free(model->normalList);
   
  free(model); 
  return 0;
}


int MD2ModelCleanup(MD2Model_t *model){
  if(model->usageCount > 1){
    model->usageCount--;
	return 0;
  }
  if (model->triIndex != NULL)
    free(model->triIndex);
  if (model->vertexList != NULL)
    free(model->vertexList);
  if (model->texCoordList != NULL)
    free(model->texCoordList);
  if (model->normalList != NULL)
    free(model->normalList);
    
  return 0;
}


