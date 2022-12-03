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


GLdouble eyeX = 0.0, eyeY = 3.0, eyeZ = 30.0;
GLdouble radius = eyeZ;
GLdouble zNear = 0.1, zFar = 40.0;

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

double pitch = 10;
double yaw = -90;

double positionX = 0.0;
double positionY = 0.0;

const double PI = 3.14159265;
const double RADIANS = PI / 180;

int window3DSizeX = 800, window3DSizeY = 600;
GLdouble aspect = (GLdouble)window3DSizeX / window3DSizeY;

int currentButton;

Robot r1 = Robot();

vector<Robot> bots;

int delay = 30;

bool gameStart = false;

bool isAllBotsDead;

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
	//glutSetCursor(GLUT_CURSOR_NONE);
	//glutMouseWheelFunc(mouseScrollWheelHandler3D);
	//glutMotionFunc(mouseMotionHandler3D);
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
	gluLookAt(eyeX, eyeY, eyeZ, cameraFrontX * 8.0, cameraFrontY * 8.0, cameraFrontY * 8.0, 0.0, 1.0, 0.0);
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
	gluLookAt(eyeX, eyeY, eyeZ, cameraFrontX * 8.0, cameraFrontY * 8.0, cameraFrontY * 8.0, 0.0, 1.0, 0.0);
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
	gluLookAt(eyeX, eyeY, eyeZ, cameraFrontX * 8.0, cameraFrontY * 8.0, cameraFrontY * 8.0, 0.0, 1.0, 0.0);
	drawGround();
	initVBO();
	drawCannon();
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
	glutSolidCube(4.0);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0, 0, radius * 0.8);
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

void drawBots()
{

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
	glVertex3f(-50.0f, -4.0f, -50.0f);
	glVertex3f(-50.0f, -4.0f, 50.0f);
	glVertex3f(50.0f, -4.0f, 50.0f);
	glVertex3f(50.0f, -4.0f, -50.0f);
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

//void mouseMotionHandler3D(int x, int y)
//{
//	if (firstMouse) {
//		lastMouseX = x;
//		lastMouseY = y;
//		firstMouse = false;
//	}
//
//	double hypoteneuse = sqrt(pow(radius, 2) + pow(eyeY, 2));
//	int dx = x - lastMouseX;
//	int dy = y - lastMouseY;
//
//	lastMouseX = x;
//	lastMouseY = y;
//
//	//if (x < 50 || x > window3DSizeX - 50) {
//	//	lastMouseX = window3DSizeX / 2;
//	//	lastMouseY = window3DSizeY / 2;
//	//	glutWarpPointer(window3DSizeX / 2, window3DSizeY / 2);
//	//}
//	//else if (y < 50 || y > window3DSizeY - 50) {
//	//	lastMouseX = window3DSizeX / 2;
//	//	lastMouseY = window3DSizeY / 2;
//	//	glutWarpPointer(window3DSizeX / 2, window3DSizeY / 2);
//	//}
//
//	float sensitivity = 0.1f;
//	dx *= sensitivity;
//	dy *= sensitivity;
//
//	yaw += dx;
//	pitch += dy;
//
//	if (pitch > 89.0f)
//		pitch = 89.0f;
//	if (pitch < -89.0f)
//		pitch = -89.0f;
//
//	eyeX = cos(RADIANS * yaw) * cos(RADIANS * pitch) * hypoteneuse;
//	eyeY = sin(RADIANS * pitch) * hypoteneuse;
//	eyeZ = sin(RADIANS * yaw) * cos(RADIANS * pitch) * hypoteneuse;
//
//	glutPostRedisplay();
//}

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
	case 'A':
	case 'a':
		if (gameStart == false) {
			gameStart = true;
			for (int i = 0; i < 5; i++) {
				bots[i].isWalking = true;
			}
			glutTimerFunc(delay, animationHandler, 0);
		}
		break;
	case 'R':
	case 'r':
		if (bots.empty()) {
			for (int i = 0; i < 5; i++) {
				Robot r = Robot();
				r.scaleFactor = 0.3;
				r.startX = -15 + (7.5 * i);
				bots.push_back(r);
			}
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

	switch (key)
	{
	case GLUT_KEY_LEFT:
		yaw -= 10.0;
		break;
	case GLUT_KEY_RIGHT:
		yaw += 10.0;
		break;
	case GLUT_KEY_UP:
		pitch += 5.0;
		break;
	case GLUT_KEY_DOWN:
		pitch -= 5.0;
		break;
	}

	printf("%f", yaw);
	printf("   ");

	if (yaw > 0)
		yaw = 0;
	if (yaw < -180)
		yaw = -180;

	if (pitch > 90.0)
		pitch = 90.0;
	if (pitch < 10)
		pitch = 10;

	cameraFrontX = (cos(RADIANS * yaw) * cos(RADIANS * pitch));
	cameraFrontY = (sin(RADIANS * pitch));
	cameraFrontZ = (sin(RADIANS * yaw) * cos(RADIANS * pitch));

	glutPostRedisplay();
}

void animationHandler(int param) {

	for (int i = 0; i < bots.size() && bots.size() > 0; i++)
	{
		if (bots[i].life > 0)
		{
			if (bots[i].walkZ < eyeZ + 5) {
				bots[i].walkZ += 0.1;
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