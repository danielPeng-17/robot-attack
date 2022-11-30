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

using namespace std;

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

double pitch = 0;
double yaw = 0;

double positionX = 0.0;
double positionY = 0.0;

const double PI = 3.14159265;
const double RADIANS = PI / 180;

int window3DSizeX = 800, window3DSizeY = 600;
GLdouble aspect = (GLdouble)window3DSizeX / window3DSizeY;

int currentButton;

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

/************************************************************************************
 *
 *
 * 3D Window and Surface of Revolution Code
 *
 * Fill in the code in the empty functions
 ************************************************************************************/
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

const double degree = 3.1415 / 180;

bool allocateVBO = true;
unsigned int nId;
unsigned int vId;
unsigned int qId;


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
	gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
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
	gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
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
	gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	drawGround();
	initVBO();

	glPushMatrix();
	glRotatef(pitch, 0, 1, 0);
	glRotatef(-yaw, 1, 0, 0);
	drawQuads();
	glPopMatrix();

	// Draw quad mesh
	glutSwapBuffers();
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

void drawQuads()
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

void mouseMotionHandler3D(int x, int y)
{
	if (firstMouse) {
		lastMouseX = x;
		lastMouseY = y;
		firstMouse = false;
	}

	double hypoteneuse = sqrt(pow(radius, 2) + pow(eyeY, 2));
	int dx = x - lastMouseX;
	int dy = y - lastMouseY;

	lastMouseX = x;
	lastMouseY = y;	

	//if (x < 50 || x > window3DSizeX - 50) {
	//	lastMouseX = window3DSizeX / 2;
	//	lastMouseY = window3DSizeY / 2;
	//	glutWarpPointer(window3DSizeX / 2, window3DSizeY / 2);
	//}
	//else if (y < 50 || y > window3DSizeY - 50) {
	//	lastMouseX = window3DSizeX / 2;
	//	lastMouseY = window3DSizeY / 2;
	//	glutWarpPointer(window3DSizeX / 2, window3DSizeY / 2);
	//}

	float sensitivity = 0.1f;
	dx *= sensitivity;
	dy *= sensitivity;

	yaw += dx;
	pitch += dy;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	eyeX = cos(RADIANS * yaw) * cos(RADIANS * pitch) * hypoteneuse;
	eyeY = sin(RADIANS * pitch) * hypoteneuse;
	eyeZ = sin(RADIANS * yaw) * cos(RADIANS * pitch) * hypoteneuse;

	glutPostRedisplay();
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
	default:
		break;
	}
	glutPostRedisplay();
}

void specialKeyHandler(int key, int x, int y)
{
	//double hypoteneuse = sqrt(pow(radius, 2) + pow(eyeY, 2));

	switch (key)
	{
	case GLUT_KEY_LEFT:
		yaw -= 5.0;
		break;
	case GLUT_KEY_RIGHT:
		yaw += 5.0;
		break;
	case GLUT_KEY_UP:
		pitch -= 2.0;
		break;
	case GLUT_KEY_DOWN:
		pitch += 2.0;
		break;
	}

	if (yaw > 60.0)
		yaw = 60.0;
	if (yaw < 0.0)
		yaw = 0;

	if (pitch > 20.0)
		pitch = 20.0;
	if (pitch < 0)
		pitch = 0;

	eyeX = cos(RADIANS * yaw) * cos(RADIANS * pitch) * radius;
	eyeY = sin(RADIANS * pitch) * radius;
	eyeZ = sin(RADIANS * yaw) * cos(RADIANS * pitch) * radius;
	glutPostRedisplay();
}