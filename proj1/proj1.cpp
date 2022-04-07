#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

int window_width = 0, window_height = 0;
int prev_x = 0, prev_y = 0;
int rightbuttonpressed = 0;
float r = 0.7843, g = 0.6431, b = 0.7098; // Background color = habitat rose

int num = 0;
int winpos[128][2];
float openpos[128][2];
float gravity_center[2];

double angle = 3.141592 / 180;
float area = 1;

//MODES
int animation_exist = 0;
int polygon_exist = 0;

float xwin2open(int x);
float ywin2open(int y);
bool point_exist(int x, int y);
void getarea();
void getxg(float a);
void getyg(float a);

void timer(int val) {
	float tmp[2];
	for (int i = 0; i < num; i++)
	{
		tmp[0] = (openpos[i][0] - gravity_center[0]) * (float)cos(angle) - (openpos[i][1] - gravity_center[1]) * (float)sin(angle) + gravity_center[0];
		tmp[1] = (openpos[i][0] - gravity_center[0]) * (float)sin(angle) + (openpos[i][1] - gravity_center[1]) * (float)cos(angle) + gravity_center[1];
		openpos[i][0] = tmp[0];
		openpos[i][1] = tmp[1];
	}
	glutPostRedisplay();
	if (animation_exist)
		glutTimerFunc(500, timer, 1);
}

void display(void) {
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//ȸ�����̸�, �����߽� �׸���
	if (animation_exist)
	{
		glPointSize(15.0f);
		glBegin(GL_POINTS);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex2f(gravity_center[0], gravity_center[1]);
		glEnd();
	}

	glPointSize(10.0f);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 1.0f);
	for (int i = 0; i < num; i++)
		glVertex2f(openpos[i][0], openpos[i][1]);
	glEnd();

	glLineWidth(4.0f);
	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f);
	if (num > 1) {
		for (int i = 1; i < num; i++)
		{
			glVertex2f(openpos[i - 1][0], openpos[i - 1][1]);
			glVertex2f(openpos[i][0], openpos[i][1]);
		}
		if (num >= 3 && polygon_exist)
		{
			glVertex2f(openpos[num - 1][0], openpos[num - 1][1]);
			glVertex2f(openpos[0][0], openpos[0][1]);
		}
	}
	glEnd();
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'c':
		if (!animation_exist)
		{
			num = 0;
			polygon_exist = 0;
			glutPostRedisplay();
		}
		break;
	case 'p':
		if (num >= 3 && !polygon_exist) {
			polygon_exist = 1;
			getarea();
			getxg(area);
			getyg(area);
			glutPostRedisplay();
		}
		else if(num < 3)
			fprintf(stdout, "YOU HAVE TO SELECT AT LEAST THREE POINTS.\n");
		else if(polygon_exist)
			fprintf(stdout, "POLYGON IS ALREADY EXISTED.\n");
		break;
	case 'r':
		if (polygon_exist) {
			animation_exist = 1 - animation_exist;
			if (animation_exist)
				glutTimerFunc(500, timer, 0);
			else
				glutPostRedisplay();
		}
		break;
	case 'f':
		glutLeaveMainLoop();
		break;
	}
}

void special(int key, int x, int y) {
	if (polygon_exist && !animation_exist) {
		switch (key) {
		case GLUT_KEY_LEFT:
			for (int i = 0; i < num; i++)
				openpos[i][0] -= 0.1f;
			gravity_center[0] -= 0.1f;
			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			for (int i = 0; i < num; i++)
				openpos[i][0] += 0.1f;
			gravity_center[0] += 0.1f;
			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			for (int i = 0; i < num; i++)
				openpos[i][1] -= 0.1f;
			gravity_center[1] -= 0.1f;
			glutPostRedisplay();
			break;
		case GLUT_KEY_UP:
			for (int i = 0; i < num; i++)
				openpos[i][1] += 0.1f;
			gravity_center[1] += 0.1f;
			glutPostRedisplay();
			break;
		}
	}
}

void mousepress(int button, int state, int x, int y) {
	if (!animation_exist) {
		if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN) && (glutGetModifiers() == GLUT_ACTIVE_SHIFT)) {
			if (!point_exist(x, y) && num < 128 && !polygon_exist)
			{
				winpos[num][0] = x; winpos[num][1] = y;
				openpos[num][0] = xwin2open(winpos[num][0]);
				openpos[num][1] = ywin2open(winpos[num][1]);
				num++;
				glutPostRedisplay();
			}
			else if (polygon_exist) 
				fprintf(stdout, "***There is already a POLYGON!!!!\n");
			else if (point_exist(x, y))
				fprintf(stdout, "***There is already a point at (%d, %d).\n", x, y);
			else if (num == 128)
				fprintf(stdout, "***Maximum point number is 128.\n");
		}
		else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
			rightbuttonpressed = 1;
			prev_x = x, prev_y = y;
		}
		else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP))
			rightbuttonpressed = 0;
	}
}

void mousemove(int x, int y) {
	if (rightbuttonpressed) {
		if (!animation_exist && polygon_exist) {
			float del_x = xwin2open(x) - xwin2open(prev_x);
			float del_y = ywin2open(y) - ywin2open(prev_y);
			for (int i = 0; i < num; i++)
			{
				openpos[i][0] += del_x;
				openpos[i][1] += del_y;
			}
			gravity_center[0] += del_x;
			gravity_center[1] += del_y;
			glutPostRedisplay();
			prev_x = x, prev_y = y;
		}
	}
}

void reshape(int width, int height) {
	fprintf(stdout, "Window size is changed! ->  RESHAPE is excuted.\n");
	window_width = width, window_height = height;
	glViewport(0, 0, width,height);
}

void close(void) {
	fprintf(stdout, "\n^^^ The control is at the close callback function now.\n\n");
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMotionFunc(mousemove);
	glutReshapeFunc(reshape);
	glutCloseFunc(close);
}

void initialize_renderer(void) {
	register_callbacks();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = TRUE;
	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "***************************************************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "***************************************************************************************\n\n");
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "***************************************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n***************************************************************************************\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 4
void main(int argc, char* argv[]) {
	char program_name[80] = "Sogang CSE4170 HW1 : GLUT ��Ŷ�� ����� ������ �ٰ��� ���� ��� ����";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'p', 'c', 'r', 'f'",
		"    - Special keys used: LEFT, RIGHT, UP, DOWN",
		"    - Mouse used: L-click with SHIFT, R-click and move",
		"    - Other operations: window size change"
	};
	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE); // <-- Be sure to use this profile for this example code!
 //	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(750, 300);
	glutCreateWindow(program_name);
	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();
	// glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT); // default
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
	fprintf(stdout, "^^^ The control is at the end of main function now.\n\n");
}

void getarea()
{
	float res = 0;
	for (int i = 0; i < num - 1; i++)
		res += openpos[i][0] * openpos[i + 1][1] - openpos[i + 1][0] * openpos[i][1];
	res += openpos[num - 1][0] * openpos[0][1] - openpos[0][0] * openpos[num - 1][1];
	area = res / 2;
}

void getxg(float a)
{
	float res = 0;
	for (int i = 0; i < num - 1; i++)
		res += (openpos[i][0] + openpos[i + 1][0]) * (openpos[i][0] * openpos[i + 1][1] - openpos[i + 1][0] * openpos[i][1]);
	res += (openpos[num - 1][0] + openpos[0][0]) * (openpos[num - 1][0] * openpos[0][1] - openpos[0][0] * openpos[num - 1][1]);
	gravity_center[0] = res / (6 * a);
}

void getyg(float a)
{
	float res = 0;
	for (int i = 0; i < num - 1; i++)
		res += (openpos[i][1] + openpos[i + 1][1]) * (openpos[i][0] * openpos[i + 1][1] - openpos[i + 1][0] * openpos[i][1]);
	res += (openpos[num - 1][1] + openpos[0][1]) * (openpos[num - 1][0] * openpos[0][1] - openpos[0][0] * openpos[num - 1][1]);
	gravity_center[1] = res / (6 * a);
}

float xwin2open(int x)
{
	int w = window_width;
	return (float)(x - (float)w / 2.0) * (float)(1.0 / (float)(w / 2.0));
}

float ywin2open(int y)
{
	int h = window_height;
	return -(float)(y - (float)h / 2.0) * (float)(1.0 / (float)(h / 2.0));
}

bool point_exist(int x, int y)
{
	for (int i = 0; i < num; i++)
		if (openpos[i][0] == xwin2open(x) && openpos[i][1] == ywin2open(y))
			return true;
	return false;
}
