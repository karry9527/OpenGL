#include "../SharedCode/ShaderLoader.h"
#include "../SharedCode/mesh.h"


#include "FreeImage.h"
#include "view.h"
#include "scene.h"
#include "glmath.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include "light.h"
#include <GL/glut.h>


#define		TEX_NUM 2	  //the number of textures you use.
GLuint		texObject[TEX_NUM];	//texture object
GLhandleARB	MyShader;
mesh			*object1, *object2, *object3;
light*		light1;
view*		view1;
scene		*scene1;
mat4x4		ModelMatrix, ViewMatrix, ViewMatrixInverse, ProjectionMatrix, ProjectionMatrixInverse;
mat4x4		LightViewMatrix, LightProjectionMatrix, ShadowMatrix;
int			WinW,WinH;


unsigned int LoadTexture(const char* filename, int iTextureId);
void LoadShaders();
void Display();
void keyboard(unsigned char key, int x, int y);
void Reshape(GLsizei , GLsizei );

int main(int argc, char** argv)
{
	//remember to change the path of mesh.cpp

	/*object1 = new mesh("HW3\\TestScene1\\Ground.obj");
	object2 = new mesh("HW3\\TestScene1\\SingleTriangle.obj");

	view1 = new view("HW3\\TestScene1\\TestScene1.view");
	light1 = new light("HW3\\TestScene1\\TestScene1.light");
	scene1 = new scene("HW3\\TestScene1\\TestScene1.scene");*/

	/*object1 = new mesh("HW3\\TestScene2\\Ground.obj");
	object2 = new mesh("HW3\\TestScene2\\Objects.obj");

	view1 = new view("HW3\\TestScene2\\TestScene2.view");
	light1 = new light("HW3\\TestScene2\\TestScene2.light");
	scene1 = new scene("HW3\\TestScene2\\TestScene2.scene");*/

	object1 = new mesh("HW3\\TwoToys\\Cornell_box.obj");
	object2 = new mesh("HW3\\TwoToys\\ToySit.obj");
	object3 = new mesh("HW3\\TwoToys\\ToyStand.obj");

	view1 = new view("HW3\\TwoToys\\TwoToys.view");
	light1 = new light("HW3\\TwoToys\\TwoToys.light");
	scene1 = new scene("HW3\\TwoToys\\TwoToys.scene");

	glutInit(&argc, argv);
	glutInitWindowSize(512, 512);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Shader Use Texture");

	GLenum glew_error;
	if((glew_error = glewInit()) != GLEW_OK)return -1;

	FreeImage_Initialise();
	//texObject[0] = LoadTexture("worldColorMap.bmp", 0);
	//texObject[1] = LoadTexture("worldHeightMap.bmp", 1);
	FreeImage_DeInitialise();

	LoadShaders();

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
    glutKeyboardFunc(keyboard);
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
		ShaderLoad(MyShader, "../shadowMap.vs", GL_VERTEX_SHADER);
		ShaderLoad(MyShader, "../shadowMap.fs", GL_FRAGMENT_SHADER);
	}
}

void Display()
{
	float buffer[16];

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(view1->fovy, (GLfloat)WinW/(GLfloat)WinW, view1->dnear, view1->dfar);//fovy aspect near far

	LightProjectionMatrix = perspective(view1->fovy, (GLfloat)WinW/(GLfloat)WinW, view1->dnear, view1->dfar);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//ViewMatrix
	gluLookAt(	view1->eye[0], view1->eye[1], view1->eye[2],// eye
				view1->center[0], view1->center[1], view1->center[2],  // center
				view1->up[0], view1->up[1], view1->up[2]);    // up

	LightViewMatrix = look(vec3(view1->eye[0], view1->eye[1], view1->eye[2]), vec3(view1->center[0], view1->center[1], view1->center[2]), vec3(view1->up[0], view1->up[1], view1->up[2]));

	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

	glEnable(GL_LIGHTING);

	//LightView
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
	ShadowMatrix = LightProjectionMatrix * LightViewMatrix; 
	GLint location = glGetUniformLocation(MyShader, "ShadowMatrix");
	if(location == -1)
		printf("Cant find name: ShadowMatrix\n");
	else
		glUniformMatrix4fv(location, 1, GL_FALSE, &ShadowMatrix);

	
	for (size_t i=0;i < object1->fTotal;i++)
	{
		glBegin(GL_POLYGON);
		for (size_t j=0;j<3;j++)
		{
			//glMultiTexCoord2fv(GL_TEXTURE0,	object1->tList[object1->faceList[i][j].t].ptr);
			glNormal3fv(object1->nList[object1->faceList[i][j].n].ptr);
			glVertex3fv(object1->vList[object1->faceList[i][j].v].ptr);	
		}
		glEnd();
	}
	for (size_t i=0;i < object2->fTotal;i++)
	{
		glBegin(GL_POLYGON);
		for (size_t j=0;j<3;j++)
		{
			//glMultiTexCoord2fv(GL_TEXTURE0,	object2->tList[object2->faceList[i][j].t].ptr);
			glNormal3fv(object2->nList[object2->faceList[i][j].n].ptr);
			glVertex3fv(object2->vList[object2->faceList[i][j].v].ptr);	
		}
		glEnd();
	}

	for (size_t i=0;i < object3->fTotal;i++)
	{
		glBegin(GL_POLYGON);
		for (size_t j=0;j<3;j++)
		{
			//glMultiTexCoord2fv(GL_TEXTURE0,	object3->tList[object3->faceList[i][j].t].ptr);
			glNormal3fv(object3->nList[object3->faceList[i][j].n].ptr);
			glVertex3fv(object3->vList[object3->faceList[i][j].v].ptr);	
		}
		glEnd();
	}
	glGetFloatv(GL_MODELVIEW_MATRIX, buffer);

	glutSwapBuffers();
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
		case 'w':	view1->eye[2] -= 10;break;//eye往前
        case 'a':	view1->eye[0] -= 10;break;//eye往左
        case 's':	view1->eye[2] += 10;break;//eye往後
        case 'd':	view1->eye[0] += 10;break;//eye往右
		//case '1':	object_num = 1;break;
		//case '2':	object_num = 2;break;
    }
	glutPostRedisplay();
}

void Reshape(GLsizei w, GLsizei h)
{
	WinW = w;
	WinH = h;
	// viewport transformation
	glViewport(view1->x, view1->y, view1->w, view1->h);

}

