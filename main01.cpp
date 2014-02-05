#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

#include "GL/glew.h"

#ifdef __linux__
#include <GL/glut.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <glut.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include "GL/glut.h"
#endif

///////////////////////////////////////////////////////////////////////
//Error checking functions
void CheckOpenGLError(const char* stmt, const char* fname, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
		exit(EXIT_FAILURE);
	}
}

#define _DEBUG 1
#if _DEBUG
#define GL_CHECK(stmt) do { \
	stmt; \
	CheckOpenGLError(#stmt, __FILE__, __LINE__); \
} while (0)
#else
#define GL_CHECK(stmt) stmt
#endif

///////////////////////////////////////////////////////////////////////
//Text file IO for shader files
char *textFileRead(char *fn)
{
	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) {
		fp = fopen(fn,"rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}

	if (content == NULL)
	{
		fprintf(stderr, "ERROR: could not load in file %s\n", fn);
		exit(1);
	}
	return content;
}

int textFileWrite(char *fn, char *s)
{
	FILE *fp;
	int status = 0;

	if (fn != NULL) {
		fp = fopen(fn,"w");

		if (fp != NULL) {

			if (fwrite(s,sizeof(char),strlen(s),fp) == strlen(s))
				status = 1;
			fclose(fp);
		}
	}
	return(status);
}

///////////////////////////////////////////////////////////////////////
//shaders and light pos variables
GLuint v,f,p,g;
float lpos[4] = {15.0, 0.5, 15.0, 0.0};
int subdivLevel;
GLuint tex;

// mouse controls
/////////////////////////////////////////////////
//scene interaction variables
int mouse_old_x, mouse_old_y;
int mouse_buttons = 0;
float rotate_x = 0.0, rotate_y = 0.0, rotate_composite=0.0;
float rotate_axis_x=1.0, rotate_axis_y=0.0, rotate_axis_z=0.0;
float move_x = 0.0, move_y = 0.0;
float win_width = 128.0, win_height = 128.0;
float translate_z = -1.0;
bool moving = false, rotating = false, zooming = false;
/////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//adapt viewport when window size changes
void changeSize(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	float ratio = 1.0* w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45, ratio, 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
	/////////////////////////////////////////////////
	//Exercise 1 TODO: add scene interaction code here
	win_width = w;
	win_height = h;
	/////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////
//the actual render function, which is called for each frame
void renderScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(0.0,0.0,1.0,0.0,0.0,-1.0,0.0f,1.0f,0.0f);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glPushMatrix();
	/////////////////////////////////////////////////
	//Exercise 1 TODO: add scene interaction code here
	// use glTranslatef and glRotatef
	//e.g.,
	glTranslatef(0.0, 0.0, translate_z);
	glTranslatef(move_x, move_y, 0.0);
	// glRotatef(rotate_composite, rotate_axis_x, rotate_axis_y, rotate_axis_z); 
	glRotatef(rotate_x, 1.0, 0.0, 0.0); 
	glRotatef(rotate_y, 0.0, 1.0, 0.0); 
	/////////////////////////////////////////////////
	glutSolidTeapot(0.5);

	glPopMatrix();
	GL_CHECK(glutSwapBuffers());
}

///////////////////////////////////////////////////////////////////////
//keyboard functions
void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27)
		exit(0);
}

///////////////////////////////////////////////////////////////////////
// mouse interaction functions
void mouseClick(int button, int state, int x, int y)
{
	/////////////////////////////////////////////////
	//Exercise 1 TODO: add scene interaction code here
	// use GLUT_UP and GLUT_DOWN to evaluate the current
	// "state" of the mouse.
  switch(button)
    {
    case GLUT_LEFT_BUTTON:
      if(state == GLUT_DOWN)
	{
	  rotating = true;
	  mouse_old_x = x;
	  mouse_old_y = y;
	}
      else rotating = false;
      break;

    case GLUT_RIGHT_BUTTON:
      if(state == GLUT_DOWN)
	{
	  zooming = true;
	  mouse_old_x = x;
	  mouse_old_y = y;
	}
      else zooming = false;
      break;

    case GLUT_MIDDLE_BUTTON:
      if(state == GLUT_DOWN)
	{
	  moving = true;
	  mouse_old_x = x;
	  mouse_old_y = y;
	}
      else moving = false;
      break;

    default:
      break;
    }

      

	/////////////////////////////////////////////////
}

// void setRotation() // unnecessary complication. Do not mark.
// {
//   float alpha = M_PI/180 * rotate_x; // convert to radians
//   float beta = M_PI/180 * rotate_y;

//   float q1 = sin(alpha/2)*cos(beta/2);
//   float q2 = cos(alpha/2)*sin(beta/2);
//   float q3 = sin(alpha/2)*sin(beta/2);
//   float q4 = cos(alpha/2)*cos(beta/2);
  
//   float theta = 2*acos(q4);
//   float denominator = sin( theta/2 );
//   float x = q1 / denominator;
//   float y = q2 / denominator;
//   float z = q3 / denominator;

//   printf("angle = %f, axis = (%f, %f, %f)\n", theta, x, y, z);

//   rotate_composite = 180/M_PI * theta; // convert to degrees
//   rotate_axis_x = 180/M_PI * x;
//   rotate_axis_y = 180/M_PI * y;
//   rotate_axis_z = 180/M_PI * z;
// }

void mouseMotion(int x, int y)
{
	/////////////////////////////////////////////////
	// Exercise 1 TODO: add scene interaction code here
	// add code to handle mouse move events
	// and calculate reasonable values for object
	// rotations
  const float zoom_sens = 1.0;
  const float x_move_sens = 0.85;
  const float y_move_sens = 0.85;
  const float x_rot_sens = 1;
  const float y_rot_sens = 1;

  int dx = x - mouse_old_x;
  int dy = y - mouse_old_y;

  mouse_old_x = x;
  mouse_old_y = y;

  float depth = 1-translate_z;
  
  // printf("%d, %d\n", dx, dy);
  // printf("%.2f, %.2f, %.2f\n", move_x, move_y, translate_z);
  // printf("%.2f, %.2f\n", rotate_x, rotate_y);

  // printf("\t%.2f, %.2f,  %.2f\n", cos(M_PI*rotate_y/180), 0.0, -sin(M_PI*rotate_y/180));
  // printf("\t%.2f, %.2f, %.2f\n\n", 0.0, cos(M_PI*rotate_x/180), -sin(M_PI*rotate_x/180));

  if(moving)
    {
      move_x += x_move_sens * depth * dx / win_height; // viewport width seems to follow window height
      move_y -= y_move_sens * depth * dy / win_height;
      // printf("(%d, %d) -> (%.2f, %.2f, %.2f)\n", x, y, move_x, move_y, translate_z);
    }
  if(rotating)
    {
      rotate_x += x_rot_sens * dy * depth / 2;
      rotate_y += y_rot_sens * dx * depth / 2;

      rotate_x = std::max(std::min(90.0f, rotate_x), -90.0f);

      // setRotation();
    }
  if(zooming)
    translate_z -= zoom_sens * depth * dy /win_height;
  
	/////////////////////////////////////////////////
}

///////////////////////////////////////////////////////////////////////
//load, compile and set the shaders
void setShaders()
{
	char *vs,*fs,*gs;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	g = glCreateShader(GL_GEOMETRY_SHADER);

	vs = textFileRead("./shader01.vert");
	fs = textFileRead("./shader01.frag");
	gs = textFileRead("./shader01.geom");

	const char * ff = fs;
	const char * vv = vs;
	const char * gg = gs;

	GL_CHECK(glShaderSource(v, 1, &vv,NULL));
	GL_CHECK(glShaderSource(f, 1, &ff,NULL));
	GL_CHECK(glShaderSource(g, 1, &gg,NULL));

	free(vs);free(fs);free(gs);

	GL_CHECK(glCompileShader(v));
	GL_CHECK(glCompileShader(f));
	GL_CHECK(glCompileShader(g));

	GLint blen = 0;
	GLsizei slen = 0;

	glGetShaderiv(v, GL_INFO_LOG_LENGTH , &blen);
	if (blen > 1)
	{
		GLchar* compiler_log = (GLchar*)malloc(blen);
		glGetShaderInfoLog(v, blen, &slen, compiler_log);
		std::cout << "compiler_log vertex shader:\n" << compiler_log << std::endl;
		free (compiler_log);
	}
	blen = 0;
	slen = 0;
	glGetShaderiv(f, GL_INFO_LOG_LENGTH , &blen);
	if (blen > 1)
	{
		GLchar* compiler_log = (GLchar*)malloc(blen);
		glGetShaderInfoLog(f, blen, &slen, compiler_log);
		std::cout << "compiler_log fragment shader:\n" << compiler_log << std::endl;
		free (compiler_log);
	}
	blen = 0;
	slen = 0;
	glGetShaderiv(g, GL_INFO_LOG_LENGTH , &blen);
	if (blen > 1)
	{
		GLchar* compiler_log = (GLchar*)malloc(blen);
		glGetShaderInfoLog(g, blen, &slen, compiler_log);
		std::cout << "compiler_log geometry shader:\n" << compiler_log << std::endl;
		free (compiler_log);
	}

	p = glCreateProgram();

	GL_CHECK(glAttachShader(p,f));
	GL_CHECK(glAttachShader(p,v));
	GL_CHECK(glAttachShader(p,g));

	GL_CHECK(glLinkProgram(p));
	//comment out this line to not use the shader
	GL_CHECK(glUseProgram(p));
}



void initialize ()
{
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, 320, 320);
	GLfloat aspect = (GLfloat) 320 / 320;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, aspect, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glShadeModel( GL_SMOOTH );
	glClearColor( 0.0f, 0.0f, 0.0f, 0.5f );
	glClearDepth( 1.0f );
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	GLfloat amb_light[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat diffuse[] = { 0.6, 0.6, 0.6, 1 };
	GLfloat specular[] = { 0.7, 0.7, 0.3, 1 };
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, amb_light );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, specular );
	glEnable( GL_LIGHT0 );
	glEnable( GL_COLOR_MATERIAL );
	glShadeModel( GL_SMOOTH );
	glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE );
	glDepthFunc( GL_LEQUAL );
	glEnable( GL_DEPTH_TEST );
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

///////////////////////////////////////////////////////////////////////
//main, setup and execution of environment
int main(int argc, char **argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("Computer Graphics");

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processNormalKeys);

	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0,0.0,0.0,1.0);

	glewInit();
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
		printf("Ready for GLSL\n");
	else {
		printf("No GLSL support\n");
		exit(1);
	}

	if (glewIsSupported("GL_VERSION_3_1"))
		printf("Ready for OpenGL 3.1\n");
	else {
		printf("OpenGL 3.1 not supported\n");
		exit(1);
	}
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GL_EXT_geometry_shader4)
		printf("Ready for GLSL - vertex, fragment, and geometry units\n");
	else {
		printf("Not totally ready :( \n");
		exit(1);
	}
	initialize();
	setShaders();

	glutMainLoop();
	return EXIT_SUCCESS;
}
