#include "mesh.h"
#include "view.h"
#include "scene.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream> 
#include <string> 
#include "FreeImage.h"
#include "glew.h"
#include "light.h"
#include "glut.h"

using namespace std; 


mesh *object;
light *light1;
view *view1;
scene *scene1;

GLuint *texObject;
bool mouse_left = false;
int windowSize[2];
int mouseOriginx, mouseOriginy, movex, movey, objectNum = 0;

void material(int num);
void drag(int x, int y);
void LoadCubeTexture(char* pFilename1, char* pFilename2, char* pFilename3, char* pFilename4, char* pFilename5, char* pFilename6, int iIndex);
void LoadTexture(char* pFilename, int iIndex);
void setTex();
void display();
void reshape(GLsizei , GLsizei );
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	view1 = new view("Chess.view");
	glutInitWindowSize(view1->w, view1->h);
	//view();
	scene1 = new scene("Chess.scene");
	object = new mesh[scene1->objectNumMax];
	for(int i = 0; i < scene1->objectNumMax; i++)
	{
		object[i].mesh_init(scene1->object_scene[i].objectName);
	}
	
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Texture");

	glewInit();
	texObject = new GLuint[scene1->texObjectNum];
	glGenTextures(scene1->texObjectNum, texObject);
	
	FreeImage_Initialise();

	int temp = 0;
	for(int i = 0; i < scene1->texObjectNum; i++)
	{
		if(!strcmp(scene1->texObjectName[temp].type, "no-texture"))
		{
			cout << "nan:" << scene1->texObjectName[temp].texName << endl;
			temp++;
		}
		else if(!strcmp(scene1->texObjectName[temp].type, "single-texture"))
		{
			LoadTexture(scene1->texObjectName[temp].texName, i);
			cout << i << ":" << scene1->texObjectName[temp].texName << endl;
			temp++;
		}
		else if(!strcmp(scene1->texObjectName[temp].type, "multi-texture"))
		{
			LoadTexture(scene1->texObjectName[temp].texName, i++);
			cout << i - 1 << ":" << scene1->texObjectName[temp].texName << endl;
			LoadTexture(scene1->texObjectName[temp+1].texName, i);
			cout << i << ":" << scene1->texObjectName[temp+1].texName << endl;
			temp += 2;
		}
		else if(!strcmp(scene1->texObjectName[temp].type, "cube-map"))
		{
			/*LoadTexture(scene1->texObjectName[temp].texName, i++);
			LoadTexture(scene1->texObjectName[temp+1].texName, i++);
			LoadTexture(scene1->texObjectName[temp+2].texName, i++);
			LoadTexture(scene1->texObjectName[temp+3].texName, i++);
			LoadTexture(scene1->texObjectName[temp+4].texName, i++);*/
			LoadCubeTexture(scene1->texObjectName[temp].texName,
					scene1->texObjectName[temp+1].texName,
					scene1->texObjectName[temp+2].texName,
					scene1->texObjectName[temp+3].texName,
					scene1->texObjectName[temp+4].texName,
					scene1->texObjectName[temp+5].texName, i);
			cout << i << ":" << scene1->texObjectName[temp].texName << endl;
			i += 5;
			temp += 6;
		}

	}
	FreeImage_DeInitialise();
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
    glutMotionFunc(drag);
	glutMainLoop();

	return 0;
}

void material(int num)
{
	//弄成func
	int lastMaterial = -1;
	
	for(size_t i=0;i < object[num].fTotal;++i)
	{
		// set material property if this face used different material
		if(lastMaterial != object[num].faceList[i].m)
		{
			lastMaterial = (int)object[num].faceList[i].m;
			glMaterialfv(GL_FRONT, GL_AMBIENT  , object[num].mList[lastMaterial].Ka);
			glMaterialfv(GL_FRONT, GL_DIFFUSE  , object[num].mList[lastMaterial].Kd);
			glMaterialfv(GL_FRONT, GL_SPECULAR , object[num].mList[lastMaterial].Ks);
			glMaterialfv(GL_FRONT, GL_SHININESS, &object[num].mList[lastMaterial].Ns);

			//you can obtain the texture name by object->mList[lastMaterial].map_Kd
			//load them once in the main function before mainloop
			//bind them in display function here
		}
		
		glBegin(GL_TRIANGLES);
		for (size_t j=0;j<3;++j)
		{
			//textex corrd. object->tList[object->faceList[i][j].t].ptr
			glNormal3fv(object[num].nList[object[num].faceList[i][j].n].ptr);
			glVertex3fv(object[num].vList[object[num].faceList[i][j].v].ptr);	
		}
		glEnd();
	}
}

void LoadCubeTexture(char* pFilename1, char* pFilename2, char* pFilename3, char* pFilename4, char* pFilename5, char* pFilename6, int iIndex)
{
	FIBITMAP* pImage1 = FreeImage_Load(FreeImage_GetFileType(pFilename1, 0), pFilename1);
	FIBITMAP *p32BitsImage1 = FreeImage_ConvertTo32Bits(pImage1);
	int iWidth1 = FreeImage_GetWidth(p32BitsImage1);
	int iHeight1 = FreeImage_GetHeight(p32BitsImage1);

	FIBITMAP* pImage2 = FreeImage_Load(FreeImage_GetFileType(pFilename2, 0), pFilename2);
	FIBITMAP *p32BitsImage2 = FreeImage_ConvertTo32Bits(pImage2);
	int iWidth2 = FreeImage_GetWidth(p32BitsImage2);
	int iHeight2 = FreeImage_GetHeight(p32BitsImage2);

	FIBITMAP* pImage3 = FreeImage_Load(FreeImage_GetFileType(pFilename3, 0), pFilename3);
	FIBITMAP *p32BitsImage3 = FreeImage_ConvertTo32Bits(pImage3);
	int iWidth3 = FreeImage_GetWidth(p32BitsImage3);
	int iHeight3 = FreeImage_GetHeight(p32BitsImage3);

	FIBITMAP* pImage4 = FreeImage_Load(FreeImage_GetFileType(pFilename4, 0), pFilename4);
	FIBITMAP *p32BitsImage4 = FreeImage_ConvertTo32Bits(pImage4);
	int iWidth4 = FreeImage_GetWidth(p32BitsImage4);
	int iHeight4 = FreeImage_GetHeight(p32BitsImage4);

	FIBITMAP* pImage5 = FreeImage_Load(FreeImage_GetFileType(pFilename5, 0), pFilename5);
	FIBITMAP *p32BitsImage5 = FreeImage_ConvertTo32Bits(pImage5);
	int iWidth5 = FreeImage_GetWidth(p32BitsImage5);
	int iHeight5 = FreeImage_GetHeight(p32BitsImage5);

	FIBITMAP* pImage6 = FreeImage_Load(FreeImage_GetFileType(pFilename6, 0), pFilename6);
	FIBITMAP *p32BitsImage6 = FreeImage_ConvertTo32Bits(pImage6);
	int iWidth6 = FreeImage_GetWidth(p32BitsImage6);
	int iHeight6 = FreeImage_GetHeight(p32BitsImage6);

	glBindTexture(GL_TEXTURE_CUBE_MAP, texObject[iIndex]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, iWidth1, iHeight1,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage1));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, iWidth2, iHeight2,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage2));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, iWidth3, iHeight3,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage3));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, iWidth4, iHeight4,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage4));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, iWidth5, iHeight5,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage5));
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, iWidth6, iHeight6,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage6));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	FreeImage_Unload(p32BitsImage1);
	FreeImage_Unload(pImage1);
	FreeImage_Unload(p32BitsImage2);
	FreeImage_Unload(pImage2);
	FreeImage_Unload(p32BitsImage3);
	FreeImage_Unload(pImage3);
	FreeImage_Unload(p32BitsImage4);
	FreeImage_Unload(pImage4);
	FreeImage_Unload(p32BitsImage5);
	FreeImage_Unload(pImage5);
	FreeImage_Unload(p32BitsImage6);
	FreeImage_Unload(pImage6);
}

void LoadTexture(char* pFilename, int iIndex)
{
	FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType(pFilename, 0), pFilename);
	FIBITMAP *p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);

	glBindTexture(GL_TEXTURE_2D, texObject[iIndex]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                     GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                     GL_REPEAT );
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pImage);
}

void setTex()
{
	for(int i = 0, j = 0, k = 0; i < scene1->objectNumMax; j++)
	{
		int type;
		if(!strcmp(scene1->texObjectName[j].type, "no-texture"))
		{
			cout << scene1->texObjectName[j].type << ":";
			type = scene1->object_scene[i].type;
			for(int x = i; x < scene1->objectNumMax; x++, i++)
			{
				if(scene1->object_scene[x].type == type)
				{
					cout << scene1->object_scene[x].objectName << " ";
					glPushMatrix();
						glTranslatef(scene1->object_scene[x].Tx, scene1->object_scene[x].Ty, scene1->object_scene[x].Tz);
						glRotatef(scene1->object_scene[x].Angle, scene1->object_scene[x].Rx, scene1->object_scene[x].Ry, scene1->object_scene[x].Rz);
						glScalef(scene1->object_scene[x].Sx, scene1->object_scene[x].Sy, scene1->object_scene[x].Sz);
						material(x);
					glPopMatrix();
				}
				else
					break;
			}
			cout << endl;
			k++;
		}
		else if(!strcmp(scene1->texObjectName[j].type, "single-texture"))
		{
			cout << scene1->texObjectName[j].type << ":";
			type = scene1->object_scene[i].type;
			if(!strcmp(scene1->texObjectName[j].texName, "Room.bmp"))
			{
				/*glEnable(GL_TEXTURE_2D);
				//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0.5f);
				glBindTexture(GL_TEXTURE_2D, texObject[k]);
				glDisable(GL_CULL_FACE);*/
				/*glBegin(GL_QUADS);
				glTexCoord2f( 0.0, 0.0 ); 
				glVertex3f( -1.0, -1.0, 1.0 );
				glTexCoord2f( 1.0, 0.0 ); 
				glVertex3f( 1.0, -1.0, 1.0 );
				glTexCoord2f( 1.0, 1.0 ); 
				glVertex3f( 1.0, 1.0, 1.0 );
				glTexCoord2f( 0.0, 1.0 ); 
				glVertex3f( -1.0, 1.0, 1.0 );
				glEnd();
				glFlush();*/
				glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
				//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_TEXTURE_2D);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0.5f);


				glBindTexture(GL_TEXTURE_2D, texObject[k]);


			}
			else
			{
				float params1[] = {1.0 , 0.0 , 0.0 ,0.0};
				float params2[] = {0.0 , 1.0 , 0.0 ,0.0};
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
				glTexGenfv(GL_S , GL_OBJECT_PLANE , params1);
				glTexGenfv(GL_T , GL_OBJECT_PLANE , params2);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_ALPHA_TEST);
				glAlphaFunc(GL_GREATER, 0.5f);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texObject[k]);
				glDisable(GL_CULL_FACE);
			}
			
			
			for(int x = i; x < scene1->objectNumMax; x++, i++)
			{
				if(scene1->object_scene[x].type == type)
				{
					cout << scene1->object_scene[x].objectName << " ";
					glPushMatrix();
						glTranslatef(scene1->object_scene[x].Tx, scene1->object_scene[x].Ty, scene1->object_scene[x].Tz);
						glRotatef(scene1->object_scene[x].Angle, scene1->object_scene[x].Rx, scene1->object_scene[x].Ry, scene1->object_scene[x].Rz);
						glScalef(scene1->object_scene[x].Sx, scene1->object_scene[x].Sy, scene1->object_scene[x].Sz);
						material(x);
					glPopMatrix();
				}
				else
					break;
			}
			cout << endl;
			/*glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0);
				glVertex3f(-50.0, 0.0, 0.0);
				glTexCoord2f(1.0, 0.0);
				glVertex3f(-50.0, 0.0, 100.0);
				glTexCoord2f(1.0, 1.0);
				glVertex3f(50.0, 0.0, 100.0);
				glTexCoord2f(0.0, 1.0);
				glVertex3f(50.0, 0.0, 0.0);
			glEnd();*/
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_ALPHA_TEST);

			k++;
		}
		else if(!strcmp(scene1->texObjectName[j].type, "multi-texture"))
		{	
			cout << scene1->texObjectName[j].type << ":";
			type = scene1->object_scene[i].type;
			//bind texture 0
			glActiveTexture(GL_TEXTURE0);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texObject[k]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvf (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE );

			//bind texture 1
			glActiveTexture(GL_TEXTURE1);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texObject[k+1]);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
			glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE );
			/*glBegin(GL_QUADS);
				glMultiTexCoord2f(GL_TEXTURE0, 0.0, 0.0);
				glMultiTexCoord2f(GL_TEXTURE1, 0.0, 0.0);
				glVertex3f(-1.0, -1.0, 0.0);
	
				glMultiTexCoord2f(GL_TEXTURE0, 100.0, 0.0);
				glMultiTexCoord2f(GL_TEXTURE1, 3.0, 0.0);
				glVertex3f(1.0, -1.0, 0.0);
	
				glMultiTexCoord2f(GL_TEXTURE0, 100.0, 100.0);
				glMultiTexCoord2f(GL_TEXTURE1, 3.0, 3.0);
				glVertex3f(1.0, 1.0, 0.0);
	
				glMultiTexCoord2f(GL_TEXTURE0, 0.0, 100.0);
				glMultiTexCoord2f(GL_TEXTURE1, 0.0, 3.0);
				glVertex3f(-1.0, 1.0, 0.0);*/
			glEnd();
			for(int x = i; x < scene1->objectNumMax; x++, i++)
			{
				if(scene1->object_scene[x].type == type)
				{
					cout << scene1->object_scene[x].objectName << " ";
					glPushMatrix();
						glTranslatef(scene1->object_scene[x].Tx, scene1->object_scene[x].Ty, scene1->object_scene[x].Tz);
						glRotatef(scene1->object_scene[x].Angle, scene1->object_scene[x].Rx, scene1->object_scene[x].Ry, scene1->object_scene[x].Rz);
						glScalef(scene1->object_scene[x].Sx, scene1->object_scene[x].Sy, scene1->object_scene[x].Sz);
						material(x);
					glPopMatrix();
				}
				else
					break;
			}
			cout << endl;
			

			//unbind texture 1
			glActiveTexture(GL_TEXTURE1);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			//unbind texture 0
			glActiveTexture(GL_TEXTURE0);
			glDisable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			glFlush();
			k += 2;
			j++;
		}
		else if(!strcmp(scene1->texObjectName[j].type, "cube-map"))
		{
			cout << scene1->texObjectName[j].type << ":";
			type = scene1->object_scene[i].type;
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_GEN_R);
			glEnable(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, texObject[k]);

			for(int x = i; x < scene1->objectNumMax; x++, i++)
			{
				if(scene1->object_scene[x].type == type)
				{
					cout << scene1->object_scene[x].objectName << " ";
					glPushMatrix();
						glTranslatef(scene1->object_scene[x].Tx, scene1->object_scene[x].Ty, scene1->object_scene[x].Tz);
						glRotatef(scene1->object_scene[x].Angle, scene1->object_scene[x].Rx, scene1->object_scene[x].Ry, scene1->object_scene[x].Rz);
						glScalef(scene1->object_scene[x].Sx, scene1->object_scene[x].Sy, scene1->object_scene[x].Sz);
						material(x);
					glPopMatrix();
				}
				else
					break;
			}
			cout << endl;
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			glDisable(GL_TEXTURE_CUBE_MAP);
			k += 6;
			j += 5;
		}
	}
}

void display()
{
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer
	
	//glColor3f(1.0, 1.0, 1.0);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(	view1->eye[0], view1->eye[1], view1->eye[2],// eye
				view1->center[0], view1->center[1], view1->center[2],  // center
				view1->up[0], view1->up[1], view1->up[2]);    // up

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(view1->fovy, (GLfloat)windowSize[0]/(GLfloat)windowSize[1], view1->dnear, view1->dfar);//fovy aspect near far

	
	// viewport transformation
	glViewport(view1->x, view1->y, view1->w, view1->h);

	//注意light位置的設定，要在gluLookAt之後
	//light();
	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	light1 = new light("Chess.light");
	for(int i = 0; i < light1->now; i++)
	{
		glEnable(GL_LIGHT0 + i);
		glLightfv(GL_LIGHT0 + i, GL_POSITION, light1->lightList[i].position);
		glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light1->lightList[i].ambient);
		glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light1->lightList[i].diffuse);
		glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light1->lightList[i].specular);
		//cout << "i:" << i << "  " << light1->position[i] << light1->ambient[i] << light1->diffuse[i] << light1->specular[i] << endl;
	}
	//cout << light1->lightList[1].position;
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light1->total_ambient);

	/*scene1->object_scene[scene1->objectNum].Tx += movex;
	scene1->object_scene[scene1->objectNum].Ty -= movey;
	movex = 0;
	movey = 0;*/
	glMatrixMode(GL_MODELVIEW);
	setTex();
		
	glutSwapBuffers();
}

/*
	glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(0, 0, 0);
		glRotatef(0, 0, 1, 0);
		glScalef(10, 1, 1);
		//加上繪圖的func
		glPopMatrix();
*/

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
		case 'w':	view1->eye[2] -= 20;break;//eye往前
        case 'a':	view1->eye[0] -= 20;break;//eye往左
        case 's':	view1->eye[2] += 20;break;//eye往後
        case 'd':	view1->eye[0] += 20;break;//eye往右
		case '1':	scene1->objectNum = 0;break;
		case '2':	scene1->objectNum = 1;break;
		case '3':	scene1->objectNum = 2;break;
    }
	glutPostRedisplay();
}
void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
	{
		mouse_left = true;
		mouseOriginx = x;
		mouseOriginy = y;
		printf("mouse_left: true\n");
    }
	if(state == GLUT_UP)
	{
		mouse_left = false;
		printf("mouse_left: false\n");
	}
}
void drag(int x, int y)
{
	if(mouse_left)
	{
		printf("%d\n", x - mouseOriginx);
		movex = x - mouseOriginx;
		movey = y - mouseOriginy;
		mouseOriginx = x;
		mouseOriginy = y;
		glutPostRedisplay();
	}
		//printf("mouse x: %d y: %d", x, y);
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

