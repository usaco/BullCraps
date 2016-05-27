#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

#include "driver-base.h"
#include "driver-visual.h"

#define max(a,b) \
  ({ __typeof__ (a) _a = (a); \
  __typeof__ (b) _b = (b); \
  _a > _b ? _a : _b; })

#define min(a,b) \
({ __typeof__ (a) _a = (a); \
__typeof__ (b) _b = (b); \
_a < _b ? _a : _b; })

float COLORS[][4] =
{
	{1.0, 0.0, 0.0, 0.0},
	{0.0, 1.0, 0.0, 0.0},
	{0.0, 0.0, 1.0, 0.0},
	{1.0, 1.0, 0.0, 0.0},
	{0.0, 1.0, 1.0, 0.0},
	{1.0, 0.0, 1.0, 0.0},
	{1.0, 0.5, 0.5, 0.0},
	{0.5, 1.0, 0.5, 0.0},
	{0.5, 0.5, 1.0, 0.0},
	{1.0, 1.0, 0.5, 0.0},
	{0.5, 1.0, 1.0, 0.0},
	{1.0, 0.5, 1.0, 0.0},
};

unsigned int NUMCOLORS = sizeof(COLORS)/(3 * sizeof(float));

// cow information
unsigned char TRANSPARENT[] = {255, 0, 255};
unsigned char COLORABLE[] = {0, 0, 0};

float BLACK[] = {0, 0, 0, 0};
float WHITE[] = {1, 1, 1, 0};

unsigned int WINDOW_W = 600;
unsigned int WINDOW_H = 600;

struct Image
{
	int width, height;
	unsigned char *data;
	unsigned int texID;
};

struct VisData
{
	float *color;
	struct Image image;
};

struct VisData visdata[MAXAGENTS];

struct Image read_image(char *filename)
{
	struct Image I; int i, j, k, t;
	FILE *fp = fopen(filename, "r");

	if (!fp)
	{
		printf ("Error: can't open %s\n", filename);
		exit (0);
	}

	fscanf (fp, "P6\n%d %d\n255\n", &I.width, &I.height);
	I.data = (unsigned char *) malloc(4 * I.width * I.height);
	fread (I.data, 3, I.width * I.height, fp);

	// add alpha channel, using transparent key
	for (i = I.height * I.width - 1; i >= 0; --i)
	{
		memmove(I.data + 4 * i, I.data + 3 * i, 3);
		I.data[4 * i + 3] = 255 * !memcmp(I.data + 4 * i, TRANSPARENT, 3);
	}

	#define POS(i, j, k) (((i)*I.width+(j))*4+(k))
	for (i = 0; i < I.height/2; i++) for (j = 0; j < I.width; j++) for (k = 0; k < 4; k++)
	{
		t = I.data[POS(i, j, k)];
		I.data[POS(i, j, k)] = I.data[POS(I.height-1-i, j, k)];
		I.data[POS(I.height-1-i, j, k)] = t;
	}

	fclose (fp);

	glGenTextures(1, &I.texID);
	glBindTexture(GL_TEXTURE_2D, I.texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, I.width, I.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, I.data);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return I;
}

struct Image read_image_colored(char *filename, float *color)
{
	struct Image I; int i, j, k, t;
	FILE *fp = fopen(filename, "r");

	if (!fp)
	{
		printf ("Error: can't open %s\n", filename);
		exit (0);
	}

	fscanf (fp, "P6\n%d %d\n255\n", &I.width, &I.height);
	I.data = (unsigned char *) malloc(4 * I.width * I.height);
	fread (I.data, 3, I.width * I.height, fp);

	// add alpha channel, using transparent key
	for (i = I.height * I.width - 1; i >= 0; --i)
	{
		memmove(I.data + 4 * i, I.data + 3 * i, 3);
		I.data[4 * i + 3] = 255 * !memcmp(I.data + 4 * i, TRANSPARENT, 3);

		if (!memcmp(I.data + 4 * i, COLORABLE, 3))
		{
			unsigned char newcolor[3];
			for (j = 0; j < 3; ++j)
				newcolor[j] = (unsigned char)(255 * color[j]);
			memmove(I.data + 4 * i, newcolor, 3);
		}
	}

	#define POS(i, j, k) (((i)*I.width+(j))*4+(k))
	for (i = 0; i < I.height/2; i++) for (j = 0; j < I.width; j++) for (k = 0; k < 4; k++)
	{
		t = I.data[POS(i, j, k)];
		I.data[POS(i, j, k)] = I.data[POS(I.height-1-i, j, k)];
		I.data[POS(I.height-1-i, j, k)] = t;
	}

	fclose (fp);

	glGenTextures(1, &I.texID);
	glBindTexture(GL_TEXTURE_2D, I.texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, I.width, I.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, I.data);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return I;
}

void gr_draw_image(float x, float y, float s, struct Image I)
{
	glPushMatrix();
	glLoadIdentity();

	float w = I.width, h = I.height;
	glTranslatef(x, y, 0.0);
	glScalef(s, s, 1.0);
	glColor4f(1, 1, 1, 1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, I.texID);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0, 0); glVertex3f(0, 0, 0.0);
	glTexCoord2f(0, 1); glVertex3f(0, h, 0.0);
	glTexCoord2f(1, 0); glVertex3f(w, 0, 0.0);
	glTexCoord2f(1, 1); glVertex3f(w, h, 0.0);
	glEnd();

	glPopMatrix();
}

void gr_change_size(int w, int h)
{
	/* Avoid divide by zero */
	if(h == 0) h = 1;

	/* Reset the coordinate system before modifying */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/* Set the viewport to be the entire window */
	glViewport(0, 0, w, h);

	WINDOW_W = w;
	WINDOW_H = h;
}

void gr_set_orthographic_projection(void)
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, WINDOW_W, 0, WINDOW_H);
	glMatrixMode(GL_MODELVIEW);
}

int gr_textlen(char *text)
{
	void *font = GLUT_BITMAP_HELVETICA_18;
	int i, L = strlen(text), x = 0;
	for( i=0; i<L && text[i]; i++ )
		x += glutBitmapWidth(font, text[i] );
	return x;
}

void gr_print_font(int x, int y, char *text, float* color, void* font)
{
	int i, L = strlen(text);
	glColor4fv(color);

	for( i=0; i<L && text[i]; i++ )
	{
		glRasterPos2f(x, y);
		glutBitmapCharacter(font, text[i]);
		x += glutBitmapWidth(font, text[i] );
	}

	glColor4f(1, 1, 1, 0);
}

void gr_print(int x, int y, char *text, float* color)
{ gr_print_font(x, y, text, color, GLUT_BITMAP_HELVETICA_18); }

void gr_print_centered(int x, int y, char *text, float* color)
{ gr_print(x - gr_textlen(text)/2, y, text, color); }

void gr_rect(float x, float y, float w, float h)
{
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(x, y, 0.0);

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(0, 0, 0.0);
	glVertex3f(0, h, 0.0);
	glVertex3f(w, 0, 0.0);
	glVertex3f(w, h, 0.0);
	glEnd();

	glPopMatrix();
}

void draw_object(int coord, struct Image image)
{
}

int _numagents;
struct agent_t *_agents;
int _turn;

#define SCALE(x) log(x)
int draw_screen(int numagents, struct agent_t *agents, const int turn)
{
	int i, j;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gr_set_orthographic_projection();

	return 1;
}

int setup_bcb_vis(int numagents, struct agent_t *agents, int *argc, char ***argv)
{
	glutInit(argc, *argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(WINDOW_W, WINDOW_H);
	glutCreateWindow("");

	glutReshapeFunc( gr_change_size );
	gr_change_size(WINDOW_W, WINDOW_H);
	glClearColor(1.0, 1.0, 1.0, 0.0);

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

	glutMainLoopEvent();
	draw_screen(numagents, agents, 0);

	glutSwapBuffers();
	glutMainLoopEvent();
	usleep(3000000L);

	return 1;
}

int update_bcb_vis(int numagents, struct agent_t *agents, const int turn)
{
	//if (turn > 10 && (turn % (int)(2 * log(turn)))) return 1;
	draw_screen(numagents, agents, turn);

	_numagents = numagents;
	_agents = agents;
	_turn = turn;

	glutSwapBuffers();
	glutMainLoopEvent();
	usleep(1000000L / (10 + turn));

	return 1;
}

void close_bcb_vis()
{
	draw_screen(_numagents, _agents, _turn);

	glutSwapBuffers();
	glutMainLoopEvent();

	usleep(3000000L);
}

