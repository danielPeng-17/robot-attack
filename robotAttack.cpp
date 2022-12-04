#include <stdio.h>
#include <windows.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include <string.h>
#include <string>
#include <fstream>
#include <sstream>  
#include <vector>
#include "robotAttack.h"
#include "robot.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

 // Ground Mesh material
GLfloat groundMat_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat groundMat_specular[] = { 0.01, 0.01, 0.01, 1.0 };
GLfloat groundMat_diffuse[] = { 0.4, 0.4, 0.7, 1.0 };
GLfloat groundMat_shininess[] = { 1.0 };

GLfloat light_position0[] = { 4.0, 8.0, 8.0, 1.0 };
GLfloat light_diffuse0[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat light_position1[] = { -4.0, 8.0, 8.0, 1.0 };
GLfloat light_diffuse1[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat model_ambient[] = { 0.5, 0.5, 0.5, 1.0 };

//
// Surface of Revolution consists of vertices and quads
//
// Set up lighting/shading and material properties for surface of revolution
GLfloat quadMat_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat quadMat_specular[] = { 0.45, 0.55, 0.45, 1.0 };
GLfloat quadMat_diffuse[] = { 0.1, 0.35, 0.1, 1.0 };
GLfloat quadMat_shininess[] = { 10.0 };

GLdouble fov = 60.0;

int lastMouseX;
int lastMouseY;


GLdouble eyeX = 0.0, eyeY = 5.0, eyeZ = 30.0;
GLdouble radius = eyeZ;
GLdouble zNear = 0.1, zFar = 100.0;

GLdouble cameraFrontX = 0.0, cameraFrontY = 0.0, cameraFrontZ = 0.0;

const double degree = 3.1415 / 180;

bool allocateVBO = true;
unsigned int nId;
unsigned int vId;
unsigned int qId;

GLdouble worldLeft = -12;
GLdouble worldRight = 12;
GLdouble worldBottom = -9;
GLdouble worldTop = 9;
GLdouble worldCenterX = 0.0;
GLdouble worldCenterY = 0.0;
GLdouble wvLeft = -12;
GLdouble wvRight = 12;
GLdouble wvBottom = -9;
GLdouble wvTop = 9;

GLint glutWindowWidth = 800;
GLint glutWindowHeight = 600;
GLint viewportWidth = glutWindowWidth;
GLint viewportHeight = glutWindowHeight;

// object sizes

double cannonLength = 1.0;
double cannonWidth = 1.0;

// screen window identifiers
int window2D, window3D;

// vbo

unsigned int vboId;
unsigned int iboId;

vector<double> vertices;
vector<double> normals;
vector<unsigned int> indices;

bool vboInitialized;

// camera

bool firstMouse = true;

float pitch = 5.0f;
float yaw = -90.0f;

const double PI = 3.14159265;
const double RADIANS = PI / 180;

// cannon

bool isBulletActive = false;
bool isBulletFired = false;
bool isCannonActive = true;

float latestBulletYaw = yaw;
float latestBulletPitch = pitch;

float bulletSpeed = 2.0f;
float maxBulletActiveTime = 60.0f;

float currentBulletActiveTime = 0.0f;
float currentBulletTravel = 0.0f;

int window3DSizeX = 800, window3DSizeY = 600;
GLdouble aspect = (GLdouble)window3DSizeX / window3DSizeY;

int currentButton;

Robot r1 = Robot();

vector<Robot> bots;

int delay = 30;

bool gameStart = false;

bool isAllBotsDead;

// size

float bulletSize = 1.0f;

float gunOffset = 1.0;

int main(int argc, char* argv[])
{
	glutInit(&argc, (char**)argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(glutWindowWidth, glutWindowHeight);
	glutInitWindowPosition(50, 100);

	// The 3D Window
	window3D = glutCreateWindow("Surface of Revolution");
	glutPositionWindow(900, 100);
	glutDisplayFunc(display3D);
	glutReshapeFunc(reshape3D);
	glutKeyboardFunc(keyboardHandler3D);
	glutSpecialFunc(specialKeyHandler);
	// Initialize the 3D system
	init3DSurfaceWindow();

	glewExperimental = GL_TRUE;
	glewInit();

	// Annnd... ACTION!!
	glutMainLoop();

	return 0;
}

void init3DSurfaceWindow()
{
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, model_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, model_ambient);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear

	glViewport(0, 0, (GLsizei)window3DSizeX, (GLsizei)window3DSizeY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, cameraFrontX * 8.0, cameraFrontY * 8.0, cameraFrontZ * 8.0, 0.0, 1.0, 0.0);
}


void reshape3D(int w, int h)
{
	glutWindowWidth = (GLsizei)w;
	glutWindowHeight = (GLsizei)h;
	glViewport(0, 0, glutWindowWidth, glutWindowHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, cameraFrontX * 8.0, cameraFrontY * 8.0, cameraFrontZ * 8.0, 0.0, 1.0, 0.0);
}

void display3D()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, aspect, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// Set up the Viewing Transformation (V matrix)	
	gluLookAt(eyeX, eyeY, eyeZ, cameraFrontX * 8.0, cameraFrontY * 8.0, cameraFrontZ * 8.0, 0.0, 1.0, 0.0);
	drawGround();
	initVBO();
	drawCannon();
	if (isBulletActive == true) {
		drawBullet();
	}
	glPushMatrix();
	for (int i = 0; i < bots.size(); i++) {
		bots[i].drawRobot();
	}
	glPopMatrix();
	

	// Draw quad mesh
	glutSwapBuffers();
}


void drawCannon() {
	
	glPushMatrix();
		if (!isCannonActive) {
			glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
			glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
			glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);
		}
		glTranslatef(gunOffset, 2, radius * 0.8);
		glRotatef(-90, 0, 1, 0);
		glRotatef(-yaw, 0, 1, 0);
		glRotatef(pitch, 1, 0, 0);
		glPushMatrix();

		glPushMatrix();
		glRotatef(-90, 1, 0, 0);
		glScalef(1, 1, 1);
		drawCannonFromVBO();
		glPopMatrix();

		glPopMatrix();
	glPopMatrix();
}

void drawBullet() {
	glPushMatrix();
		glTranslatef(gunOffset, 2, radius * 0.8);
		glRotatef(-90, 0, 1, 0);
		glRotatef(-latestBulletYaw, 0, 1, 0);
		glRotatef(latestBulletPitch, 1, 0, 0);
	if (isCannonActive == true) {
		glPushMatrix();
		glTranslatef(0, 0, -currentBulletTravel); // probably replace camera front with a varaible that does not change when moving cannon
		glutSolidCylinder(0.5, bulletSize, 30, 30);
		glPopMatrix();
	}
	glPopMatrix();
}

void drawGround()
{
	glPushMatrix();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, groundMat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, groundMat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, groundMat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, groundMat_shininess);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(-60.0f, -4.0f, -60.0f);
	glVertex3f(-60.0f, -4.0f, 60.0f);
	glVertex3f(60.0f, -4.0f, 60.0f);
	glVertex3f(60.0f, -4.0f, -60.0f);
	glEnd();
	glPopMatrix();
}

void initVBO()
{
	if (!vboInitialized) {
		glGenBuffers(1, &vboId);
		glGenBuffers(1, &iboId);

		string line;
		ifstream MyFile("mesh.obj");
		while (getline(MyFile, line))
		{
			double x, y, z = 0.0;
			int f1, f2, f3, f4 = 0;
			istringstream iss(line);
			string context;
			iss >> context;

			if (context == "v") {
				iss >> x >> y >> z;
				vertices.push_back(x);
				vertices.push_back(y);
				vertices.push_back(z);
			}
			else if (context == "vn") {
				iss >> x >> y >> z;
				normals.push_back(x);
				normals.push_back(y);
				normals.push_back(z);
			}
			else if (context == "f") {
				iss >> f1 >> f2 >> f3 >> f4;
				indices.push_back(f1);
				indices.push_back(f2);
				indices.push_back(f3);
				indices.push_back(f4);
			}
		}
		MyFile.close();

		vboInitialized = true;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() + normals.size() * sizeof(double), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(double), &vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(double), normals.size() * sizeof(double), &normals[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void drawCannonFromVBO()
{
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_DOUBLE, 0, (void*)0);
	glNormalPointer(GL_DOUBLE, 0, (void*)0);

	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, (void*)0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void mouseScrollWheelHandler3D(int button, int dir, int xMouse, int yMouse)
{
	// Fill in this code for zooming in and out
	fov -= (GLdouble)(dir);
	if (fov < 1.0)
		fov = 1.0;
	if (fov > 175.0)
		fov = 175.0;
	glutPostRedisplay();
}

void keyboardHandler3D(unsigned char key, int x, int y)
{

	switch (key)
	{
	case 'q':
	case 'Q':
	case 27:
		// Esc, q, or Q key = Quit 
		exit(0);
		break;
	case 'R':
	case 'r':
		if (bots.size() > 0) {
			bots.clear();
		}
		if (bots.empty()) {
			for (int i = 0; i < 4; i++) {
				Robot r = Robot();
				r.cameraX = eyeX;
				r.cameraY = eyeY;
				r.cameraZ = eyeZ;
				r.scaleFactor = 0.3;
				float x = -25 + (15 * i);
				if (x > 45) {
					x = 45;
				}
				r.startX = x;
				bots.push_back(r);
			}
		}
		if (!gameStart || !isCannonActive) {
			gameStart = true;
			isCannonActive = true;
			for (int i = 0; i < bots.size(); i++) {
				bots[i].isWalking = true;
				bots[i].calcBulletPatch();
			}
			glutTimerFunc(delay, collisionDetection, 0);
			glutTimerFunc(delay, animationHandler, 0);
			glutTimerFunc(delay, animationHandlerShooting, 0);
		}
		break;
	case ' ':
		if (isBulletActive == false && isCannonActive && gameStart == true) {
			isBulletActive = true;
			setLatestYawAndPitch();
			glutTimerFunc(delay, animationHandlerBullets, 0);
		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void specialKeyHandler(int key, int x, int y)
{
	double length = sqrt(pow(radius, 2) + pow(eyeY, 2));

	if (!isCannonActive) return;

	switch (key)
	{
	case GLUT_KEY_LEFT:
		yaw -= 3.0f;
		break;
	case GLUT_KEY_RIGHT:
		yaw += 3.0f;
		break;
	case GLUT_KEY_UP:
		pitch += 3.0f;
		break;
	case GLUT_KEY_DOWN:
		pitch -= 3.0f;
		break;
	}

	if (yaw > -10)
		yaw = -10;
	if (yaw < -180.0f)
		yaw = -180;

	if (pitch > 90.0f)
		pitch = 90.0f;
	if (pitch < -8.0f)
		pitch = -8.0f;

	cameraFrontX = (cos(RADIANS * yaw) * cos(RADIANS * pitch));
	cameraFrontY = (sin(RADIANS * pitch));
	cameraFrontZ = (sin(RADIANS * yaw) * cos(RADIANS * pitch));

	glutPostRedisplay();
}

void collisionDetection(int param) {
	for (int i = 0; i < bots.size(); i++) {
		float botBulletsCollisionDistance = sqrt(pow(gunOffset - bots[i].bulletX, 2) + pow(2 - bots[i].bulletY, 2) + pow((radius * 0.8) - bots[i].bulletZ, 2));
		if (botBulletsCollisionDistance < 1.8) {
			isCannonActive = false;
		}

		float cannonBulletCollisionDistance = sqrt(pow(bots[i].startX - latestBulletYaw - 90 - gunOffset, 2) + pow(-latestBulletPitch, 2) + pow(bots[i].walkZ - radius * 0.8 - currentBulletTravel, 2));
		if (i == 3) {
			printf("cannonBulletCollisionDistance for bot %d is %f\n", i, cannonBulletCollisionDistance);
		}
		if (cannonBulletCollisionDistance < 5) {
			bots[i].life -= 1;
			bots[i].isShooting = false;
			currentBulletTravel = 0.0f;
			currentBulletActiveTime = 0.0f;
			isBulletActive = false;
			setLatestYawAndPitch();
		}
	}
	glutPostRedisplay();
	glutTimerFunc(delay, collisionDetection, 0);
}

void animationHandler(int param) {

	for (int i = 0; i < bots.size() && bots.size() > 0; i++)
	{
		if (bots[i].life > 0)
		{
			if (bots[i].walkZ < eyeZ + 5) {
				bots[i].walkZ += 0.05;
			}
			else {
				bots[i].life = 0;
				gameStart = false;
				bots.clear();
				break;
			}
			

			// walking animation
			if (bots[i].legMoveBack) {
				if (bots[i].upperLegAngle < bots[i].MAX_UPPER_LEG_ANGLE) {
					bots[i].upperLegAngle += 2.0;
				}
				else {
					if (bots[i].lowerLegAngle > bots[i].MIN_LOWER_LEG_ANGLE) {
						bots[i].lowerLegAngle -= 2.0;
					}
					else {
						bots[i].legMoveBack = false;
						bots[i].legMoveForward = true;
					}
				}
			}
			else if (bots[i].legMoveForward) {
				if (bots[i].upperLegAngle > bots[i].MIN_UPPER_LEG_ANGLE) {
					bots[i].upperLegAngle -= 2.0;
				}
				else {
					if (bots[i].lowerLegAngle < bots[i].MAX_LOWER_LEG_ANGLE) {
						bots[i].lowerLegAngle += 2.0;
					}
					else {
						bots[i].legMoveForward = false;
						bots[i].legMoveBack = true;
						bots[i].movingLeftLeg = !bots[i].movingLeftLeg;
					}
				}
			}
		}
		else if (bots[i].life == 0 && bots[i].scaleFactor > 0) {
			if (bots[i].legMoveForward != false || bots[i].legMoveBack != false) {
				bots[i].legMoveForward = false;
				bots[i].legMoveBack = false;
			}

			bots[i].deathRotation += 0.1;
			bots[i].scaleFactor -= 0.1;
		}
	}
	if (gameStart == true) {
		//glutSetWindow(window3D);
		glutPostRedisplay();
		glutTimerFunc(delay, animationHandler, 0);
	}
}

void animationHandlerBullets(int param) {
	if (isBulletActive == true && currentBulletActiveTime < maxBulletActiveTime && isCannonActive == true) {
		currentBulletTravel += bulletSpeed;
		currentBulletActiveTime++;
		glutPostRedisplay();
		glutTimerFunc(delay, animationHandlerBullets, 0);
	}
	else {
		currentBulletTravel = 0.0f;
		currentBulletActiveTime = 0.0f;
		isBulletActive = false;
		setLatestYawAndPitch();
	}
	glutPostRedisplay();
}

void setLatestYawAndPitch() {
	latestBulletYaw = yaw;
	latestBulletPitch = pitch;
}

void animationHandlerShooting(int param)
{
	srand((unsigned int)time(NULL));
	for (int i = 0; i < bots.size(); i++)
	{
		if (bots[i].isShooting)
		{
			bots[i].shoot();
		}
		if (rand() % 10 < 4 && bots[i].life > 0 && !bots[i].isShooting)
		{
			bots[i].shoot();
			bots[i].isShooting = true;
		}
	}
	if (gameStart == true)
	{
		glutPostRedisplay();
		glutTimerFunc(delay, animationHandlerShooting, 0);
	}
}