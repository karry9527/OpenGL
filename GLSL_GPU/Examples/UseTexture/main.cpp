#include "../SharedCode/ShaderLoader.h"
#include "../SharedCode/mesh.h"


#include "FreeImage.h"
#include "view.h"
#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "light.h"
#include <GL/glut.h>


#define		TEX_NUM 2	  //the number of textures you use.
GLuint		texObject[TEX_NUM];	//texture object
GLhandleARB	MyShader;
mesh*		object;
light*		light1;
view*		view1;
scene		*scene1, *scene2;
int			WinW,WinH;

unsigned int LoadTexture(const char* filename, int iTextureId);
void LoadShaders();
void Display();
void Reshape(GLsizei , GLsizei );

int main(int argc, char** argv)
{
	object = new mesh("HighResolutionSphere.obj");

	view1 = new view("HW2.view");
	light1 = new light("HW2.light");
	scene1 = new scene("HW2DisplacementMap.scene");
	scene2 = new scene("HW2BumpMap.scene");
	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Shader Use Texture");

	GLenum glew_error;
	if((glew_error = glewInit()) != GLEW_OK)return -1;

	FreeImage_Initialise();
	texObject[0] = LoadTexture("worldColorMap.bmp", 0);
	texObject[1] = LoadTexture("worldHeightMap.bmp", 1);
	FreeImage_DeInitialise();

	LoadShaders();

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutMainLoop();

	return 0;
}

unsigned int LoadTexture(const char* pszFile, int iTextureId)
{
	FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType(pszFile, 0), pszFile);
	FIBITMAP *p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);

	glBindTexture(GL_TEXTURE_2D, iTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pImage);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return iTextureId;
}

void LoadShaders()
{
	MyShader = glCreateProgram();
	if(MyShader != 0)
	{
		ShaderLoad(MyShader, "../Examples/UseTexture/UseTexture.vs", GL_VERTEX_SHADER);
		ShaderLoad(MyShader, "../Examples/UseTexture/UseTexture.fs", GL_FRAGMENT_SHADER);
	}
}

void Display()
{
	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(view1->fovy, (GLfloat)WinW/(GLfloat)WinW, view1->dnear, view1->dfar);//fovy aspect near far

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	view1->eye[0], view1->eye[1], view1->eye[2],// eye
				view1->center[0], view1->center[1], view1->center[2],  // center
				view1->up[0], view1->up[1], view1->up[2]);    // up

	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

	glEnable(GL_LIGHTING);
	for(int i = 0; i < light1->now; i++)
	{
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, light1->lightList[i].position);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light1->lightList[i].ambient);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light1->lightList[i].diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light1->lightList[i].specular);
		//cout << "i:" << i << "  " << light1->position[i] << light1->ambient[i] << light1->diffuse[i] << light1->specular[i] << endl;
	}

	glUseProgram(MyShader);

	glActiveTexture( GL_TEXTURE0 );
	glBindTexture(GL_TEXTURE_2D, texObject[0]);
	

	glActiveTexture( GL_TEXTURE1 );
	glBindTexture(GL_TEXTURE_2D, texObject[1]);
	GLint location = glGetUniformLocation(MyShader, "colorTexture");
	if(location == -1)
		printf("Cant find texture name: colorTexture\n");
	else
		glUniform1i(location, 1);
	/*varying vec3 vertex_position;
varying vec3 vertex_light_vector;
varying vec3 vertex_light_half_vector;
varying vec3 vertex_normal;
 
uniform sampler2D displacement_texture;
	glTexCoord[0].xy = glMultiTexCoord0.xy;
	gl_Position = ftransform();*/	
	for (size_t i=0;i < object->fTotal;i++)
	{
		glBegin(GL_POLYGON);
		for (size_t j=0;j<3;j++)
		{
			glMultiTexCoord2fv(GL_TEXTURE0,	object->tList[object->faceList[i][j].t].ptr);
			glNormal3fv(object->nList[object->faceList[i][j].n].ptr);
			glVertex3fv(object->vList[object->faceList[i][j].v].ptr);	
		}
		glEnd();
	}

	glutSwapBuffers();
	glutPostRedisplay();
}

void Reshape(GLsizei w, GLsizei h)
{
	WinW = w;
	WinH = h;
	// viewport transformation
	glViewport(view1->x, view1->y, view1->w, view1->h);

}

