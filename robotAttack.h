// CPS 511 Assignment 3
// Danny Khuu (500903037) and Daniel Peng (500901658)

int main(int argc, char* argv[]);
void init3DSurfaceWindow();
void reshape3D(int w, int h);
void display3D();
void drawGround();
void initVBO();
void drawCannonFromVBO();
void mouseScrollWheelHandler3D(int button, int dir, int xMouse, int yMouse);
void mouseMotionHandler3D(int x, int y);
void keyboardHandler3D(unsigned char key, int x, int y);
void specialKeyHandler(int key, int x, int y);
void animationHandler(int param);
void drawCannon();
void animationHandlerBullets(int param);
void drawBullet();
void setLatestYawAndPitch();
void animationHandlerShooting(int param);
void collisionDetection(int param);