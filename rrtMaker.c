#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "obstacles.h"
#include "display.h"


// This is a helper function that determines whether or not line segment (v1---v2) intersects line segment (v3---v4)
unsigned char linesIntersect(short v1x, short v1y, short v2x, short v2y, short v3x, short v3y, short v4x, short v4y) {
  	float uA = ((v4x-v3x)*(v1y-v3y) - (v4y-v3y)*(v1x-v3x)) / (float)(((v4y-v3y)*(v2x-v1x) - (v4x-v3x)*(v2y-v1y)));
  	float uB = ((v2x-v1x)*(v1y-v3y) - (v2y-v1y)*(v1x-v3x)) / (float)(((v4y-v3y)*(v2x-v1x) - (v4x-v3x)*(v2y-v1y)));

  	// If uA and uB are between 0-1, there is an intersection
  	if (uA > 0 && uA < 1 && uB > 0 && uB < 1)
		return 1;

	return 0;
}

void createNode(Environment *env, unsigned short x, unsigned short y, TreeNode *parent){
  TreeNode *treeNode = (TreeNode *) malloc(sizeof(TreeNode));
  env->rrt[env->numNodes] = treeNode;
  env->rrt[env->numNodes]->x = x;
  env->rrt[env->numNodes]->y = y;
  env->rrt[env->numNodes]->parent = parent;
  env->rrt[env->numNodes]->firstChild = NULL;
  env->numNodes += 1;
}

int inObstacle(Environment *env, unsigned short  x, unsigned short  y){
  for(unsigned short i=0; i<env->numObstacles; i++){
    if(
      x <= (env->obstacles[i].x + env->obstacles[i].w)
      &&
      x >= env->obstacles[i].x
      &&
      y <= env->obstacles[i].y
      &&
      y >= (env->obstacles[i].y - env->obstacles[i].h)
    ){
      return 1;
    }
  }
  return 0;
}

void randomCoordGen(Environment *env, unsigned short *x, unsigned short *y){
  *x = rand()%env->maximumX;
  *y = rand()%env->maximumY;
  while(inObstacle(env,*x,*y) == 1){
    *x = rand()%env->maximumX;
    *y = rand()%env->maximumY;
  }
}

void findClosestNode(Environment *env, unsigned short x, unsigned short y, TreeNode **closestNode){
  int shortestDist = 0;
  int currDist = 0;
  for(int i=0;i<env->numNodes;i++){
    currDist = sqrt(pow((env->rrt[i]->x - x),2) + pow((env->rrt[i]->y - y),2));
    if(shortestDist == 0){
      shortestDist = currDist;
      *closestNode = env->rrt[i];
    }
    else{
      if(currDist < shortestDist){
        shortestDist = currDist;
        *closestNode = env->rrt[i];
      }
    }
  }
}

int intersectChecker(Environment *env, unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2){
  for(int i=0;i<env->numObstacles;i++){
    unsigned short a = env->obstacles[i].x;
    unsigned short b = env->obstacles[i].y;
    unsigned short w = env->obstacles[i].w;
    unsigned short h = env->obstacles[i].h;

    if(linesIntersect(x1, y1, x2, y2, a, b, a+w, b) == 1)
      return 1;
    else if(linesIntersect(x1, y1, x2, y2, a, b, a, b-h) == 1)
      return 1;
    else if(linesIntersect(x1, y1, x2, y2, a+w, b-h, a+w, b) == 1)
      return 1;
    else if(linesIntersect(x1, y1, x2, y2, a+w, b-h, a, b-h) == 1)
      return 1;
  }
  return 0;
}

void addToRRT(Environment *env, TreeNode *closestNode, unsigned short cX, unsigned short cY){
  createNode(env, cX, cY, closestNode);
  if(closestNode->firstChild == NULL){
    Child *child = (Child *) malloc(sizeof(Child));
    closestNode->firstChild = child;
    closestNode->firstChild->node = env->rrt[env->numNodes-1];
    closestNode->firstChild->nextSibling = NULL;
  }
  else if(closestNode->firstChild->nextSibling == NULL){
    Child *child = (Child *) malloc(sizeof(Child));
    closestNode->firstChild->nextSibling = child;
    closestNode->firstChild->nextSibling->node = env->rrt[env->numNodes-1];
    closestNode->firstChild->nextSibling->nextSibling = NULL;
  }
}

// Create a rrt using the growthAmount and maximumNodes parameters in the given environment.
void createRRT(Environment *env) {
  env->rrt = (TreeNode **) malloc((env->maximumNodes) * sizeof(TreeNode*));
  env->numNodes = 0;
  createNode(env, env->startX, env->startY, NULL);
  srand(time(0));
  while(env->numNodes < env->maximumNodes){

    //Generating random point that is not within any obstacles
    unsigned short randX;
    unsigned short randY;
    randomCoordGen(env, &randX, &randY);

    //Locating closest node to random point
    TreeNode *closestNode;
    findClosestNode(env, randX, randY, &closestNode);

    //Finding the coordinates for c
    double angle = atan2(randY-closestNode->y,randX-closestNode->x);
    unsigned short cX = closestNode->x + cos(angle)*env->growthAmount;
    unsigned short cY = closestNode->y + sin(angle)*env->growthAmount;

    //If the line between c and the node does not intersect any obstacles then add it to the array of nodes
    if(intersectChecker(env,closestNode->x,closestNode->y,cX,cY) == 0 && inObstacle(env,cX,cY) == 0){
      addToRRT(env, closestNode, cX, cY);
    }

  }
}


// Trace the path back from the node that is closest to the given (x,y) coordinate to the root
void tracePath(Environment *env, unsigned short x, unsigned short y) {
  unsigned short pathSize = 0;
  TreeNode *closestNode;
  findClosestNode(env, x, y, &closestNode);
  pathSize += 1;
  TreeNode *nextNode = closestNode->parent;
  pathSize += 1;
  while(nextNode->parent != NULL){
    nextNode = nextNode->parent;
    pathSize += 1;
  }
  env->path = (TreeNode **) malloc((pathSize) * sizeof(TreeNode*));
  env->path[0] = closestNode;
  for(int i=1;i<pathSize;i++){
    env->path[i] = env->path[i-1]->parent;
  }
}


// This procedure cleans up everything by creeing all alocated memory
void cleanupEverything(Environment *env) {
	free(env->obstacles);
  for(int i=0;i<env->numNodes;i++){
    if(env->rrt[i]->firstChild != NULL){
      if(env->rrt[i]->firstChild->nextSibling != NULL){
        free(env->rrt[i]->firstChild->nextSibling);
      }
      free(env->rrt[i]->firstChild);
    }
    free(env->rrt[i]);
  }
  free(env->rrt);
  free(env->path);
}
