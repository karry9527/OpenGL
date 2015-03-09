//CIRL GPU GLSL Geometry Shader Code
// Derek Anderson and Robert Luke
//We are using Visual Studio 2005 to compile this project for Windows XP
//We are using a NVIDIA 8800
// Graphics driver is - GeForce 8800 Series - Windows XP
// Version is Version 97.44 (December 8th 2006)
//GLEW version is 1.3.5
//I also used NVemulate!

#include <stdio.h>			//C standard IO
#include <stdlib.h>			//C standard lib
#include <string.h>			//C string lib

#include <GL/glew.h>			//GLEW lib
#include <GL/glut.h>			//GLUT lib

//Function from: http://www.evl.uic.edu/aej/594/code/ogl.cpp
//Read in a textfile (GLSL program)
// we need to pass it as a string to the GLSL driver
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

	return content;
}

//Function from: http://www.evl.uic.edu/aej/594/code/ogl.cpp
//Read in a textfile (GLSL program)
// we can use this to write to a text file
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

//Got this from http://www.lighthouse3d.com/opengl/glsl/index.php?oglinfo
// it prints out shader info (debugging!)
void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("printShaderInfoLog: %s\n",infoLog);
        free(infoLog);
	}else{
		printf("Shader Info Log: OK\n");
	}
}

//Got this from http://www.lighthouse3d.com/opengl/glsl/index.php?oglinfo
// it prints out shader info (debugging!)
void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("printProgramInfoLog: %s\n",infoLog);
        free(infoLog);
    }else{
		printf("Program Info Log: OK\n");
	}
}

//YEAH, YEAH, YEAH ... global vars, this is a freggn demo!
GLuint v,f,f2,p,g;			//Handlers for our vertex, geometry, and fragment shaders
int gw,gh;				//Keep track of window width and height

//GLUT callback fx
// called when window size changes
void changeSize(int w, int h) {
	//stores the width and height
	gw = w;
	gh = h;
	//Set the viewport (fills the entire window)
	glViewport(0,0,gw,gh);
	//Go to projection mode and load the identity
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//Orthographic projection, stretched to fit the screen dimensions 
	gluOrtho2D(0,gw,0,gh);
	//Go back to modelview and load the identity
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void renderScene(void) 
{
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

	//Set the clear color (black)
	glClearColor(1.0,1.0,1.0,1.0);
	//Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	//stretch to screen
	glViewport(0,0,gw,gh);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,gw,0,gh);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Draw a single line
	// it will stretch 1/2 the width and the full vertical
	//We will use a geometry shader to draw the same line, but with the x and y values swapped!
	// i.e. we will get a cross on the screen
	glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex2i(gw*0.2 , gh*0.2);
		glVertex2i(gw*0.4 , gh*0.8);

		glVertex2i(gw*0.4 , gh*0.8);
		glVertex2i(gw*0.6 , gh*0.4);

		glVertex2i(gw*0.6 , gh*0.4);
		glVertex2i(gw*0.8 , gh*0.5);
	glEnd();

	//swap buffers (double buffering)
	glutSwapBuffers();
	
	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////

}

//GLUT callback fx
// this is for processing key commands (setup a exit key)
void processNormalKeys(unsigned char key, int x, int y) 
{
	if (key == 27) 
		exit(0);
}

//Setup shaders
void setShaders() 
{
	//a few strings
	// will hold onto the file read in!
	char *vs = NULL, *fs = NULL, *gs = NULL;

	//First, create our shaders 
	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	g = glCreateShader(GL_GEOMETRY_SHADER_EXT);

	//Read in the programs
	vs = textFileRead("../Examples/GeometryShader/shader.vert");
	fs = textFileRead("../Examples/GeometryShader/shader.frag");
	gs = textFileRead("../Examples/GeometryShader/shader.geom");

	//Setup a few constant pointers for below
	const char * ff = fs;
	const char * vv = vs;
	const char * gg = gs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);
	glShaderSource(g, 1, &gg, NULL);

	free(vs);free(fs);free(gs);

	glCompileShader(v);
	glCompileShader(f);
	glCompileShader(g);

	p = glCreateProgram();

	glAttachShader(p,f);
	glAttachShader(p,v);
	glAttachShader(p,g);

	glProgramParameteriEXT(p,GL_GEOMETRY_INPUT_TYPE_EXT,GL_LINES);
	glProgramParameteriEXT(p,GL_GEOMETRY_OUTPUT_TYPE_EXT,GL_LINE_STRIP);

	int temp;
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT,&temp);
	glProgramParameteriEXT(p,GL_GEOMETRY_VERTICES_OUT_EXT,temp);

	glLinkProgram(p);
	glUseProgram(p);

	printShaderInfoLog(v);
	printShaderInfoLog(f);
	printShaderInfoLog(g);
	printProgramInfoLog(p);
}
	
int main(int argc, char **argv) 
{

	glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(320,320);
	glutCreateWindow("GPU");

	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processNormalKeys);

	//glEnable(GL_DEPTH_TEST);
	glClearColor(1.0,1.0,1.0,1.0);
	//glEnable(GL_CULL_FACE);

	glewInit();
	if (glewIsSupported("GL_VERSION_2_1"))
		printf("Ready for OpenGL 2.1\n");
	else {
		printf("OpenGL 2.1 not supported\n");
		exit(1);
	}
	if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader && GL_EXT_geometry_shader4)
		printf("Ready for GLSL - vertex, fragment, and geometry units\n");
	else {
		printf("Not totally ready :( \n");
		exit(1);
	}

	setShaders();

	glutMainLoop();

	// just for compatibiliy purposes
	return 0;

}
