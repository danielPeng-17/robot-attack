#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <math.h>
#include <gl/glut.h>
#include <utility>
#include <vector>
#include "robot.h"

const float MAX_LOWER_LEG_ANGLE = 20.0;
const float MIN_LOWER_LEG_ANGLE = -20.0;

const float MAX_UPPER_LEG_ANGLE = 20.0;
const float MIN_UPPER_LEG_ANGLE = -20.0;

// Note how everything depends on robot body dimensions so that can scale entire robot proportionately
// just by changing robot body scale
float robotBodyWidth = 5.0;
float robotBodyLength = 7.0;
float robotBodyDepth = 5.0;
float headWidth = 0.4 * robotBodyWidth;
float headLength = headWidth;
float headDepth = headWidth;
float upperArmLength = robotBodyLength;
float upperArmWidth = 0.125 * robotBodyWidth;
float gunLength = upperArmLength / 4.0;
float gunWidth = upperArmWidth;
float gunDepth = upperArmWidth;
float stanchionLength = robotBodyLength;
float stanchionRadius = 0.1 * robotBodyDepth;
float baseWidth = 2 * robotBodyWidth;
float baseLength = 0.25 * stanchionLength;
float upperLegLength = robotBodyLength / 4;
float upperLegWidth = robotBodyLength / 4;


// Control Robot body rotation on base
float robotAngle = 42.0;

// Control rotation
float bodyAngle = 0.0;
float upperLegAngle = 0.0;
float lowerLegAngle = 0.0;
float gunAngle = 0.0;

// Lighting/shading and material properties for robot - upcoming lecture - just copy for now
// Robot RGBA material properties (NOTE: we will learn about this later in the semester)
GLfloat robotBody_mat_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat robotBody_mat_specular[] = { 0.45f,0.55f,0.45f,1.0f };
GLfloat robotBody_mat_diffuse[] = { 0.1f,0.35f,0.1f,1.0f };
GLfloat robotBody_mat_shininess[] = { 32.0F };


GLfloat robotArm_mat_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
GLfloat robotArm_mat_diffuse[] = { 0.5f,0.0f,0.0f,1.0f };
GLfloat robotArm_mat_specular[] = { 0.7f, 0.6f, 0.6f, 1.0f };
GLfloat robotArm_mat_shininess[] = { 32.0F };

GLfloat gun_mat_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat gun_mat_diffuse[] = { 0.01f,0.0f,0.01f,0.01f };
GLfloat gun_mat_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat gun_mat_shininess[] = { 100.0F };

GLfloat robotLowerBody_mat_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat robotLowerBody_mat_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat robotLowerBody_mat_specular[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
GLfloat robotLowerBody_mat_shininess[] = { 76.8F };

GLUquadricObj* quadratic = gluNewQuadric();

void drawRobot()
{
	glPushMatrix();
	// spin robot on base. 
	glRotatef(robotAngle, 0.0, 1.0, 0.0);

	drawBody();
	drawGun(true);
	drawGun(false);
	drawLeg(true);
	drawLeg(false);

	glPopMatrix();
}

void drawGun(bool isLeft)
{
	int x = isLeft ? -1 : 1;

	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);
	glPushMatrix();

	glRotatef(bodyAngle, 1.0, 0.0, 0.0);

	glPushMatrix();
	glTranslatef((robotBodyLength / 2.3) * x, 0.5 * (robotBodyLength / 1.7) + 0.5 * headLength, -(robotBodyLength / 1.6));
	glRotatef(gunAngle, 0.0, 0.0, 1.0);
	glScalef(3.0, 3.0, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	// cylinder
	glPushMatrix();
	glTranslatef((robotBodyLength / 2.3) * x, 0.5 * (robotBodyLength / 1.7) + 0.5 * headLength, -(robotBodyLength / 1.6));
	glRotatef(gunAngle, 0.0, 0.0, 1.0);
	glScalef(10, 10, robotBodyDepth);
	gluCylinder(quadratic, 0.14f, 0.09f, 1.8f, 32, 32);
	glPopMatrix();

	glPopMatrix();
}

void drawBody()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);

	glPushMatrix();
	glRotatef(bodyAngle, 1.0, 0.0, 0.0);

	glPushMatrix();
	glTranslatef(0, 0, robotBodyLength / 1.75);
	glScaled(2.5, 2.5, 2.5);
	glutSolidCone(0.8, 0.8, 32, 32);
	glPopMatrix();

	glPushMatrix();
	glScalef(robotBodyWidth / 1.2, robotBodyLength / 1.5, robotBodyDepth + 3);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}


void drawLeg(bool isLeft) {
	int x = isLeft ? -1 : 1;

	glMaterialfv(GL_FRONT, GL_AMBIENT, robotArm_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotArm_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotArm_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotArm_mat_shininess);

	glPushMatrix();

	//if (value == "h" || value == "k") {
	//	glRotatef(upperLegAngle, 1.0, 0.0, 0.0);
	//}
	//else if (isLeft && value == "w") {
	//	glRotatef(upperLegAngle, 1.0, 0.0, 0.0);
	//}

	glPushMatrix();
	glTranslatef((robotBodyWidth / 2.4) * x + (0.5 * upperLegWidth) * x, -(robotBodyLength / 2.4), -0.5 * robotBodyWidth);

	// build left upper leg
	glPushMatrix();
	glTranslatef(0.0, -0.8 * upperLegLength, 0.0);
	glRotatef(30.0, 1.0, 0.0, 0.0);
	glScalef(upperLegWidth * 0.5, 5.0 * upperLegLength, upperLegWidth * 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, -2.5 * upperLegLength, -upperLegWidth);
	glPushMatrix();
	glutSolidSphere(1.0, 32, 32);
	glPopMatrix();

	//left lower leg
	glPushMatrix();

	/*if (value == "k" || value == "h") {
		glRotatef(lowerLegAngle, 1.0, 0.0, 0.0);
	}
	else if (isLeft && value == "w") {
		glRotatef(lowerLegAngle, 1.0, 0.0, 0.0);
	}*/

	glPushMatrix();
	glRotatef(-20.0, 1.0, 0.0, 0.0);
	glTranslatef(0.0, -2.2 * upperLegLength, -(robotBodyWidth / 5));
	glScalef(upperLegWidth, 6.0 * upperLegLength, upperLegLength);
	glutSolidCube(1.0);
	glPopMatrix();

	// draw left foot
	glPushMatrix();
	glTranslatef(0.0, -5.0 * upperLegLength, 0.5 * robotBodyDepth);
	glScalef(0.3 * robotBodyWidth, 0.3 * robotBodyWidth, 0.9 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}