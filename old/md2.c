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
  return thisMod; // may still be NULL, need to check when called
}

void MD2ModelInit(MD2Model_t *model){
  model->numVertices = 0;    
  model->numTriangles = 0;   
  model->numFrames = 0;      
  model->numST = 0;         
  model->frameSize = 0;      
  model->currentFrame = 0;    
  model->nextFrame = 1;    
  model->interpol = 0.0; 
  model->triIndex = NULL; 
  model->texCoordList = NULL;  
  model->vertexList = NULL; 
  model->normalList = NULL;
  model->texID = -1;
  model->modelState = MODEL_IDLE;
}    


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
  
  // open the model file
  filePtr = fopen(md2File, "rb"); 
  if (filePtr == NULL)
    return -1;
     
  // find length of file
  fseek(filePtr, 0, SEEK_END);
  fileLen = ftell(filePtr);
  fseek(filePtr, 0, SEEK_SET);
     
  // read entire file into buffer
  buffer = (char *)malloc((fileLen+1)*sizeof(char));
					 
  err = fread(buffer, sizeof(char), fileLen, filePtr);
  if(err != fileLen){
    free(buffer);
	return -1;
  }
     
  // extract model file header from buffer
  modelHeader = (MD2Header_t *)buffer;
  nPoints = modelHeader->numPoints;
  nFrames = modelHeader->numFrames; 
  frameSize = modelHeader->frameSize;
  offsetFrames = modelHeader->offsetFrames;

  // set up information in the model, allocate memory
  mod->numVertices = nPoints;
  mod->numFrames = nFrames;
  mod->frameSize = frameSize;
  mod->numST = modelHeader->numST;
  mod->numTriangles = modelHeader->numTris;
  
  // get memory
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
  mod->normalList = (vector_t *)malloc(nPoints*nFrames*sizeof(vector_t));
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

  // pull the vertices out of the file  
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
  
  // load the skin 
  mod->texID = TextureLoadSkin(&tempTex, skinFile);
  if(mod->texID == -1){
	free(buffer);
	free(mod->vertexList);
	free(mod->texCoordList);
	free(mod->triIndex);
	fclose(filePtr);
    return -1;
  }
     
  // get the scaled texture coordinates for the model 
  stPtr = (texCoordIndex_t *)(buffer + modelHeader->offsetST);
  for (i = 0; i < mod->numST; i++){
    mod->texCoordList[i].s = (float)stPtr[i].s / (float)tempTex.sizeX;
    mod->texCoordList[i].t = (float)stPtr[i].t / (float)tempTex.sizeY;
  }
     
     
  // pull the triangle indices out
  bufIndexPtr = (mesh_t *)(buffer + modelHeader->offsetTris);
     
  // create a mesh (triangle) list
  for (j = 0; j < nFrames; j++){
  
    // for all triangles in each frame
    for(i = 0; i < mod->numTriangles; i++){
      mod->triIndex[i].meshIndex[0] = bufIndexPtr[i].meshIndex[0];
      mod->triIndex[i].meshIndex[1] = bufIndexPtr[i].meshIndex[1];
      mod->triIndex[i].meshIndex[2] = bufIndexPtr[i].meshIndex[2];
      mod->triIndex[i].texCoordIndex[0] = bufIndexPtr[i].texCoordIndex[0];
      mod->triIndex[i].texCoordIndex[1] = bufIndexPtr[i].texCoordIndex[1];
      mod->triIndex[i].texCoordIndex[2] = bufIndexPtr[i].texCoordIndex[2];

    }
  
  }

  // calculate normals for all vertices for all frames

  for (j = 0; j < nFrames; j++){
    for (i = 0; i < mod->numTriangles; i++){
    
	  vList = (mod->vertexList + mod->numVertices*j);
      nList = (mod->normalList + mod->numVertices*j);
	  
      // store vertices of triangle in correct order
      tri[0] = vList[mod->triIndex[i].meshIndex[0]];
      tri[2] = vList[mod->triIndex[i].meshIndex[2]];
      tri[1] = vList[mod->triIndex[i].meshIndex[1]];
          
      // calculate the normal of the triangle
	  VectorNormal(&nList[mod->triIndex[i].meshIndex[0]],
					  &tri[0], &tri[2], &tri[1]);
	  VectorNormal(&nList[mod->triIndex[i].meshIndex[2]],
					  &tri[0], &tri[2], &tri[1]);
   	  VectorNormal(&nList[mod->triIndex[i].meshIndex[1]],
					  &tri[0], &tri[2], &tri[1]);
    }
  }
 
  // last details 
  mod->currentFrame = 0;
  mod->nextFrame = 1;
  mod->interpol = 0.0;
  
  fclose(filePtr);
  free(buffer);
     
  return 0;
}


int MD2ModelAnimate(MD2Model_t *model, int startFrame, int endFrame, float percent){

  vector_t *vList, *nList;             
  vector_t *nextVList, *nextNList;      
  int i;                
  float x1, y1, z1;   
  float x2, y2, z2;  
  float nx1, ny1, nz1;   
  float nx2, ny2, nz2;  
     
  vector_t vertex[3]; // interpolated vertices 
  vector_t normal[3]; // interpolated normals
    
  if ((startFrame > model->currentFrame)){
    model->currentFrame = startFrame;
  }
     
  if ((startFrame < 0) || (endFrame < 0)){
    return -1;
  }
     
  if ((startFrame >= model->numFrames) || (endFrame >= model->numFrames)){
    return -1;
  }
     
  if (model->interpol >= 1.0){
    model->interpol = 0.0f;
    model->currentFrame++;
    
	if (model->currentFrame >= endFrame){
      model->currentFrame = startFrame;
	}
          
    model->nextFrame = model->currentFrame + 1;
          
    if (model->nextFrame >= endFrame){
      model->nextFrame = startFrame;
	}
  }
     
  vList = (model->vertexList + model->numVertices*model->currentFrame);
  nList = (model->normalList + model->numVertices*model->currentFrame);
  nextVList = (model->vertexList + model->numVertices*model->nextFrame);
  nextNList = (model->normalList + model->numVertices*model->nextFrame);
  
  glBindTexture(GL_TEXTURE_2D, model->texID);
  glBegin(GL_TRIANGLES);
  for (i = 0; i < model->numTriangles; i++){
		  
    // get first points of each frame and first normals of each frame
    x1 = vList[model->triIndex[i].meshIndex[0]].x;
    y1 = vList[model->triIndex[i].meshIndex[0]].y;
    z1 = vList[model->triIndex[i].meshIndex[0]].z;
    x2 = nextVList[model->triIndex[i].meshIndex[0]].x;
    y2 = nextVList[model->triIndex[i].meshIndex[0]].y;
    z2 = nextVList[model->triIndex[i].meshIndex[0]].z;
    
	nx1 = nList[model->triIndex[i].meshIndex[0]].x;
    ny1 = nList[model->triIndex[i].meshIndex[0]].y;
    nz1 = nList[model->triIndex[i].meshIndex[0]].z;
    nx2 = nextNList[model->triIndex[i].meshIndex[0]].x;
    ny2 = nextNList[model->triIndex[i].meshIndex[0]].y;
    nz2 = nextNList[model->triIndex[i].meshIndex[0]].z;
          
    // store first interpolated vertex of triangle
    vertex[0].x = x1 + model->interpol * (x2 - x1);
    vertex[0].y = y1 + model->interpol * (y2 - y1);
    vertex[0].z = z1 + model->interpol * (z2 - z1);
        
    // store first interpolated normal of this vertex
	normal[0].x = nx1 + model->interpol * (nx2 - nx1);
    normal[0].y = ny1 + model->interpol * (ny2 - ny1);
    normal[0].z = nz1 + model->interpol * (nz2 - nz1);
    VectorNormalize(&normal[0]); 
    
	// get second points and normals of each frame
    x1 = vList[model->triIndex[i].meshIndex[2]].x;
    y1 = vList[model->triIndex[i].meshIndex[2]].y;
    z1 = vList[model->triIndex[i].meshIndex[2]].z;
    x2 = nextVList[model->triIndex[i].meshIndex[2]].x;
    y2 = nextVList[model->triIndex[i].meshIndex[2]].y;
    z2 = nextVList[model->triIndex[i].meshIndex[2]].z;
    
	nx1 = nList[model->triIndex[i].meshIndex[2]].x;
    ny1 = nList[model->triIndex[i].meshIndex[2]].y;
    nz1 = nList[model->triIndex[i].meshIndex[2]].z;
    nx2 = nextNList[model->triIndex[i].meshIndex[2]].x;
    ny2 = nextNList[model->triIndex[i].meshIndex[2]].y;
    nz2 = nextNList[model->triIndex[i].meshIndex[2]].z;
          
    // store second interpolated vertex and normal of triangle
    vertex[2].x = x1 + model->interpol * (x2 - x1);
    vertex[2].y = y1 + model->interpol * (y2 - y1);
    vertex[2].z = z1 + model->interpol * (z2 - z1);   
 
	normal[2].x = nx1 + model->interpol * (nx2 - nx1);
    normal[2].y = ny1 + model->interpol * (ny2 - ny1);
    normal[2].z = nz1 + model->interpol * (nz2 - nz1);   
    VectorNormalize(&normal[2]); 

    // get third points and normal of each frame
    x1 = vList[model->triIndex[i].meshIndex[1]].x;
    y1 = vList[model->triIndex[i].meshIndex[1]].y;
    z1 = vList[model->triIndex[i].meshIndex[1]].z;
    x2 = nextVList[model->triIndex[i].meshIndex[1]].x;
    y2 = nextVList[model->triIndex[i].meshIndex[1]].y;
    z2 = nextVList[model->triIndex[i].meshIndex[1]].z;
 
	nx1 = nList[model->triIndex[i].meshIndex[1]].x;
    ny1 = nList[model->triIndex[i].meshIndex[1]].y;
    nz1 = nList[model->triIndex[i].meshIndex[1]].z;
    nx2 = nextNList[model->triIndex[i].meshIndex[1]].x;
    ny2 = nextNList[model->triIndex[i].meshIndex[1]].y;
    nz2 = nextNList[model->triIndex[i].meshIndex[1]].z;
         
    // store third interpolated vertex and normal of triangle
    vertex[1].x = x1 + model->interpol * (x2 - x1);
    vertex[1].y = y1 + model->interpol * (y2 - y1);
    vertex[1].z = z1 + model->interpol * (z2 - z1);
    
	normal[1].x = nx1 + model->interpol * (nx2 - nx1);
    normal[1].y = ny1 + model->interpol * (ny2 - ny1);
    normal[1].z = nz1 + model->interpol * (nz2 - nz1);
    VectorNormalize(&normal[1]);   
	
    // render properly textured and normaled triangle
	glNormal3fv((float*)(&normal[0]));
    glTexCoord2f(model->texCoordList[model->triIndex[i].texCoordIndex[0]].s,
                 model->texCoordList[model->triIndex[i].texCoordIndex[0]].t);
    glVertex3fv((float *)(&vertex[0]));
      
   	
	glNormal3fv((float*)(&normal[2]));
    glTexCoord2f(model->texCoordList[model->triIndex[i].texCoordIndex[2]].s ,
                 model->texCoordList[model->triIndex[i].texCoordIndex[2]].t);
    glVertex3fv((float *)(&vertex[2]));
         
	glNormal3fv((float*)(&normal[1]));
    glTexCoord2f(model->texCoordList[model->triIndex[i].texCoordIndex[1]].s,
                 model->texCoordList[model->triIndex[i].texCoordIndex[1]].t);
    glVertex3fv((float *)(&vertex[1]));
  }
  glEnd();
     
  model->interpol += percent;
     
  return 0;
}


int MD2ModelDrawFrame(MD2Model_t *model, int keyFrame){
  
  vector_t *vList;
  vector_t *nList;
  int i;
     
  // create a pointer to the frame we want to show
  vList = (model->vertexList + model->numVertices * keyFrame);
  nList = (model->normalList + model->numVertices * keyFrame);
  
  // set the texture
  glBindTexture(GL_TEXTURE_2D, model->texID);
     
  // display the textured model with proper lighting normals
  glBegin(GL_TRIANGLES);
  for(i = 0; i < model->numTriangles; i++){

    glNormal3fv((float *)(&nList[model->triIndex[i].meshIndex[0]]));
	glTexCoord2f(model->texCoordList[model->triIndex[i].texCoordIndex[0]].s,
			     model->texCoordList[model->triIndex[i].texCoordIndex[0]].t);
    glVertex3fv((float *)(&vList[model->triIndex[i].meshIndex[0]]));
          
    glNormal3fv((float *)(&nList[model->triIndex[i].meshIndex[0]]));
	glTexCoord2f(model->texCoordList[model->triIndex[i].texCoordIndex[2]].s,
				 model->texCoordList[model->triIndex[i].texCoordIndex[2]].t);
    glVertex3fv((float *)(&vList[model->triIndex[i].meshIndex[2]]));
          
    glNormal3fv((float *)(&nList[model->triIndex[i].meshIndex[0]]));
	glTexCoord2f(model->texCoordList[model->triIndex[i].texCoordIndex[1]].s,
				 model->texCoordList[model->triIndex[i].texCoordIndex[1]].t);
    glVertex3fv((float *)(&vList[model->triIndex[i].meshIndex[1]]));
  }

  glEnd();
     
  return 0;
}


int MD2ModelDestroy(MD2Model_t *model){
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


int MD2ModelSetState(MD2Model_t *model, int state){
  model->modelState = state;
  return 0;
}

int MD2ModelGetState(MD2Model_t *model){
  return model->modelState;
}


