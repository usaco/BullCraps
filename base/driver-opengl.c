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

#define THIS_FONT GLUT_BITMAP_HELVETICA_12

unsigned int NUMCOLORS = sizeof(COLORS)/(3 * sizeof(float));

// cow information
unsigned char TRANSPARENT[] = {255, 0, 255};
unsigned char COLORABLE[] = {0, 0, 0};

float BLACK[] = {0, 0, 0, 0};
float RED[] = {1, 0, 0, 0};
float WHITE[] = {1, 1, 1, 0};

unsigned int WINDOW_W = 800;
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

void gr_draw_image_centered(float x, float y, float h, struct Image I)
{
	float s = h/(float)I.height;
	gr_draw_image(x - (int)(I.width*s/2), y - (int)(I.height*s/2), s, I);
}

void gr_change_size(int w, int h)
{
	/* Avoid divide by zero */
	if (h == 0) h = 1;

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
	void *font = THIS_FONT;
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
{ gr_print_font(x, y, text, color, THIS_FONT); }

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

void gr_box(float x, float y, float w, float h)
{
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(x, y, 0.0);

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINE_LOOP);
	glVertex3f(0, 0, 0.0);
	glVertex3f(0, h, 0.0);
	glVertex3f(w, h, 0.0);
	glVertex3f(w, 0, 0.0);
	glEnd();

	glPopMatrix();
}

int _numagents;
struct agent_t *_agents;
int _turn;

extern unsigned int claims[];
extern int totals[];

void draw_histogram(int x, int y, int w, int h)
{
	int i;
	char text[256];
	
	// add labels and bars
	for (i = 0; i < NUMSIDES; ++i)
	{
		int xoff = (i+0.5)*w/NUMSIDES;
		sprintf(text, "%d", i+1);
		gr_print_centered(x+xoff, y-20, text, BLACK);
		
		glColor4fv(RED);
		int cutoff = (h*totals[i]/(NUMDICE*NUMAGENTS));
		gr_rect(x + i*w/NUMSIDES, y+cutoff, w/NUMSIDES, 2);
		
		glColor4fv(claims[i]>totals[i] ? RED : BLACK);
		gr_rect(x + i*w/NUMSIDES, y, w/NUMSIDES, (h*claims[i]/(NUMDICE*NUMAGENTS)));
	}
	
	// draw the boundary box
	glColor4fv(BLACK);
	gr_box(x, y, w, h);
}

int scoresort(const void* ap, const void* bp)
{
	struct agent_t* a = (struct agent_t*)ap;
	struct agent_t* b = (struct agent_t*)bp;
	return a->score - b->score;
}

#define SCALE(x) log(x)
int draw_screen(int numagents, struct agent_t *agents, const int turn)
{
	int i;
	struct agent_t *a;
	char text[256];
	
	struct agent_t sortedagents[MAXAGENTS];
	memcpy(sortedagents, agents, sizeof(sortedagents));
	qsort(sortedagents, numagents, sizeof(struct agent_t), scoresort);
	
	int row_height = WINDOW_H / numagents;
	int xbuffer = 75;
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gr_set_orthographic_projection();

	for (i = 0, a = sortedagents; i < numagents; ++i, ++a)
	{
		struct VisData* vis = a->vis;
		int x = xbuffer;
		int y = row_height * i + (row_height / 2);
		gr_draw_image_centered(x, y, row_height-40, vis->image);
		
		sprintf(text, "%.2lf", a->score);
		gr_print_centered(x, row_height*(i+1)-25, a->name, BLACK);
		gr_print_centered(x, row_height*(i+1)-15, text, BLACK);
	}
	
	draw_histogram(150, 40, WINDOW_W-160, WINDOW_H-50);

	return 1;
}

int setup_bcb_vis(int numagents, struct agent_t *agents, int *argc, char ***argv)
{
	glutInit(argc, *argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(WINDOW_W, WINDOW_H);
	glutCreateWindow("Bull Craps");

	glutReshapeFunc( gr_change_size );
	gr_change_size(WINDOW_W, WINDOW_H);
	glClearColor(1.0, 1.0, 1.0, 0.0);

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
	
	int i;
	for (i = 0; i < numagents; ++i)
	{
		visdata[i].color = COLORS[i % NUMCOLORS];
		visdata[i].image = read_image_colored("images/cow.ppm", visdata[i].color);
		
		agents[i].vis = &visdata[i];
	}

	glutMainLoopEvent();
	draw_screen(numagents, agents, 0);

	glutSwapBuffers();
	glutMainLoopEvent();
	usleep(5000000L);

	return 1;
}

int update_bcb_vis(int numagents, struct agent_t *agents, const int turn)
{
	//if (turn > 10 && !(turn % (int)(2 * log(turn)))) return 1;
	draw_screen(numagents, agents, turn);

	_numagents = numagents;
	_agents = agents;
	_turn = turn;

	glutSwapBuffers();
	glutMainLoopEvent();
	
	long sleeptime = 10000000L / (10 + turn);
	int i, shorten = 1;
	for (i = 0; i < NUMSIDES; ++i)
		if (claims[i] <= totals[i]) shorten = 0;
	if (shorten) sleeptime /= 8L;
	
	usleep(sleeptime);

	return 1;
}

void close_bcb_vis()
{
	int i;
	draw_screen(_numagents, _agents, _turn);
	
	struct agent_t sortedagents[MAXAGENTS];
	memcpy(sortedagents, _agents, sizeof(sortedagents));
	qsort(sortedagents, _numagents, sizeof(struct agent_t), scoresort);
	
	for (i = 0; i < _numagents; ++i)
	{
		struct agent_t *a = &sortedagents[_numagents-1-i];
		printf("#%02d %s %5.2lf\n", i+1, a->name, a->score);
	}

	glutSwapBuffers();
	glutMainLoopEvent();

	usleep(5000000L);
}

