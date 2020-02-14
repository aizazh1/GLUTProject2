/*********
   CTIS164 - Template Source Program
----------
STUDENT : Aizaz Hassan
SECTION :
HOMEWORK: 2
----------
PROBLEMS: some overlapping sometimes
----------
ADDITIONAL FEATURES: different states available
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  4 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

#define MAX 5 //max birds on screen
#define COUNTER 20 //timer counter limit
#define LOAD 0 //states
#define RUN 1
#define END 2
enum isVisible { VISIBLE, NOTVISIBLE }; //to show if bird is visible or not

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
int totalScore = 0; //total score of user
int state=LOAD;
bool moveBullet = false; //status of bullet
int yGun = 0; //y coordinate of gun
int counter = 0, temp=0; //temp increases and counter adjusts to increase at 1 second of real time
typedef struct { //coordinates
	int x, y;
}point_t;
typedef struct { //for bird
	point_t point;
	int score, r, g, b;
	enum isVisible visible;
}bird_t;
typedef struct { //for bullet
	point_t point;
}bullet_t;

bird_t *bird;
bullet_t bullet = { 0,0 }; //initialization

void initializeStruct(bird_t *bird) //to initialize the max number of birds to be visible and have a diferent y vertex 
{
	int k,
		y = -300;
	for (k = 0; k < MAX; k++)
	{
		(bird + k)->point.y = y;
		(bird + k)->visible = VISIBLE;
		y += 100;
	}
}


//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
	float angle;
	glBegin(GL_POLYGON);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r * cos(angle), y + r * sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char *string, void *font)
{
	int len, i;

	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void *font, const char *string, ...)
{
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);

	int len, i;
	glRasterPos2f(x, y);
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, str[i]);
	}
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, const char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(size, size, 1);

	int len, i;
	len = (int)strlen(str);
	for (i = 0; i < len; i++)
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}
	glPopMatrix();
}
void drawGun()
{
	//draw gun
	glColor3f(0, 0, 0);
	glRectf(-400, 20 + yGun, -350, 10 + yGun);
	glRectf(-400, 20 + yGun, -390, 0 + yGun);
} //to draw gun
void drawLoad() //to draw loading screen
{
	glColor3f(0, 0, 0);
	vprint2(-100, 100, 0.3, "HOMEWORK 2");
	vprint2(-80, 0, 0.2, "By Aizaz Hassan");
	vprint2(-50, -50, 0.2, "21801429");
	vprint(-100, -250, GLUT_BITMAP_8_BY_13, "Press Spacebar to continue");
}
void drawEnd() //to display the end of game
{
	glColor3f(0, 0, 0);
	vprint2(-100, 100, 0.3, "GAME OVER");
	vprint2(-80, 0, 0.2, "Your Score: %d", totalScore);
	vprint(-100, -250, GLUT_BITMAP_8_BY_13, "Press F1 to Restart");
}
void drawBird(int xB, int yB, int r, int g, int b)
{
	glColor3ub(r, g, b);
	//body
	circle(300 + xB, 0 + yB, 30);
	//nose
	glBegin(GL_TRIANGLES);
	glVertex2f(295 + xB, 30 + yB);
	glVertex2f(305 + xB, 30 + yB);
	glVertex2f(290 + xB, 60 + yB);
	glEnd();
	//leg1
	glBegin(GL_TRIANGLES);
	glVertex2f(310 + xB, -20 + yB);
	glVertex2f(315 + xB, -40 + yB);
	glVertex2f(320 + xB, -38 + yB);
	glEnd();
	//leg2
	glBegin(GL_TRIANGLES);
	glVertex2f(320 + xB, -15 + yB);
	glVertex2f(325 + xB, -35 + yB);
	glVertex2f(330 + xB, -33 + yB);
	glEnd();
	//eye
	glColor3f(0, 0, 0);
	circle(310 + xB, 15 + yB, 5);
	//wing1
	glColor3f(1, 1, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(325 + xB, -5 + yB);
	glVertex2f(360 + xB, -20 + yB);
	glVertex2f(363 + xB, -10 + yB);
	glEnd();
	//wing 2
	glColor3f(1, 1, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(325 + xB, 0 + yB);
	glVertex2f(360 + xB, 0 + yB);
	glVertex2f(363 + xB, 10 + yB);
	glEnd();

} //to draw bird
void drawBullet()
{
	//bullet
	glColor3f(0, 0, 0);
	glRectf(-350 + bullet.point.x, 14 + bullet.point.y, -340 + bullet.point.x, 16 + bullet.point.y);
}// to draw bullet
void drawBg() //to draw background when game is running
{
	//sun
	glColor3f(1, 1, 0);
	circle(340, 240, 50);
	int x = 0;

	glColor3ub(8, 56, 246);
	x = 0;
	//to draw clouds
	for (int k = 0; k < 5; k++) {
		circle(-300 + x, 200, 30);
		circle(-275 + x, 200, 30);
		circle(-240 + x, 200, 30);
		x += 150;
	}
}
//
// To display onto window using OpenGL commands
//
void display() {
	//
	// clear window to black
	//
	glClearColor(0.4, 0.4, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	switch (state) // to switch states
	{
	case LOAD: drawLoad(); break;
	case RUN: drawBg();
		drawGun();
		drawBullet();
		for (int k = 0; k < MAX; k++)
		{
			if ((bird + k)->visible == VISIBLE)
			{
				drawBird(((bird + k)->point.x), (bird + k)->point.y, (bird + k)->r, (bird + k)->g, (bird + k)->b);
				glColor3f(0, 0, 1);
				vprint(295 + ((bird + k)->point.x), -5 + (bird + k)->point.y, GLUT_BITMAP_8_BY_13, "%d", (bird + k)->score);
			}
		}
		glColor3f(1, 0, 0);
		vprint(-300, -290, GLUT_BITMAP_8_BY_13, "Total Score: %d", totalScore);
		vprint(-300, -270, GLUT_BITMAP_8_BY_13, "Counter: %d", counter);
		break;
	case END: drawEnd();
	}
	
	glutSwapBuffers();
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);
	if (key == ' ' && state == RUN) // if spacebar is pressed when game is running, bullet moves
		moveBullet = true;
	if (key == ' ' && state == LOAD) //if spacebar is pressed when game is loading, game starts
		state = RUN;

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
	// exit when ESC is pressed.
	if (key == 27)
		exit(0);

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	//to move gun on y-axis
	if (key == GLUT_KEY_DOWN && state==RUN) {
		yGun -= 5;
		if (moveBullet == false)
			bullet.point.y = yGun;
	}

	if (key == GLUT_KEY_UP && state == RUN) {
		yGun += 5;
		if (moveBullet == false)
			bullet.point.y = yGun;
	}

	if (key == GLUT_KEY_F1 && state == END) //to restart game on end
	{
		state = RUN;
		temp = 0;
		totalScore = 0;
		moveBullet = false;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
	winWidth = w;
	winHeight = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {

	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	// Write your codes here.
	
	temp++;
	counter = temp / 250; //counter of seconds
	if (state == RUN && counter == COUNTER) // to end at 20 seconds
	{
		state = END;
	}
	if (bullet.point.x <= 750 && moveBullet == true && state==RUN) // to move bullet
		(bullet.point.x) += 2;
	if (bullet.point.x >= 751 && moveBullet == true && state == RUN) //to bring bullet back to original position
	{
		(bullet.point.x) = 0;
		moveBullet = false; // to stop bullet movement
		bullet.point.y = yGun;

	}
	for (int count_t = 0; count_t < MAX; count_t++) //movement of each bird
	{
		if ((bird + count_t)->point.y <= 340)
			(((bird + count_t)->point).y)++;
		if ((bird + count_t)->point.y > 340)
		{
			(bird + count_t)->point.y = -350;
			(bird + count_t)->point.x = rand() % 301 - 300;
			(bird + count_t)->score = rand() % 4 + 1;
			(bird + count_t)->r = rand() % 256;
			(bird + count_t)->g = rand() % 256;
			(bird + count_t)->b = rand() % 256;
		}
	}
	for (int count_k = 0; count_k < MAX; count_k++) { //check if bird gets hit
		if (((bird + count_k)->point.x + 300 <= bullet.point.x - 340) && (((bird + count_k)->point.x + 393 >= bullet.point.x - 340)) && ((bird + count_k)->point.y + 60 >= (bullet.point.y + 15) && (bird + count_k)->point.y - 40 <= (bullet.point.y + 15)))
		{
			(bird + count_k)->visible = NOTVISIBLE;
			(bullet.point.x) = 0;
			moveBullet = false;
			bullet.point.y = yGun;
			totalScore += (bird + count_k)->score;
			if ((bird + count_k)->visible == NOTVISIBLE) {
				(bird + count_k)->visible = VISIBLE;

				(bird + count_k)->point.y = -350;
				(bird + count_k)->point.x = rand() % 301 - 300;
				(bird + count_k)->score = rand() % 4 + 1;
				(bird + count_k)->r = rand() % 256;
				(bird + count_k)->g = rand() % 256;
				(bird + count_k)->b = rand() % 256;
			}
		}
	}

	// to refresh the window it calls display() function
	glutPostRedisplay(); // display()

}
#endif

void Init() {

	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	initializeStruct(bird);


}

void main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//glutInitWindowPosition(100, 100);
	glutCreateWindow("Aizaz Hassan");

	int k;
	bird = (bird_t *)malloc(sizeof(bird_t)*MAX); //dynamic allocation
	srand(time(0));
	for (k = 0; k < MAX; k++) //random allocation of variables
	{
		(bird + k)->score = rand() % 4 + 1;
		(bird + k)->point.x = rand() % 301 - 300;
		(bird + k)->r = rand() % 256;
		(bird + k)->g = rand() % 256;
		(bird + k)->b = rand() % 256;
	}


	glutDisplayFunc(display);
	glutReshapeFunc(onResize);

	//
	// keyboard registration
	//
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//
	// mouse registration
	//
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
	// timer event
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

	Init();

	glutMainLoop();
}