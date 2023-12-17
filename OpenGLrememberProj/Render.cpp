#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include <cmath>
#include <vector>
#include <algorithm>
#include <iterator>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"


bool textureMode = true;
bool lightMode = true;

double kvadrouravX(double y, int vpravo) {
	double x1 = (10. + std::sqrt(100. - 4. * ( y * y - 2 * y + 21))) / 2.;
	double x2 = (10. - std::sqrt(100. - 4. * ( y * y - 2 * y + 21))) / 2.;
	if (vpravo == 1 && x1 >= 5)
		return x1;
	else return x2;
}

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}

Vector3 GetNormal(double* start, double* end1, double* end2)
{
	Vector3 a = Vector3(start[0] - end1[0], start[1] - end1[1], start[2] - end1[2]);
	Vector3 b = Vector3(start[0] - end2[0], start[1] - end2[1], start[2] - end2[2]);
	Vector3 normal = Vector3(a.Y() * b.Z() - b.Y() * a.Z(), -a.X() * b.Z() + b.X() * a.Z(), a.X() * b.Y() - a.Y() * b.X());
	return normal;
}



void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  
	double A[3]{ 0,12,0 };
	double B[3]{ 7,12 ,0};
	double C[3]{ 8, 7 ,0};
	double D[3]{ 15,5 ,0};
	double E[3]{ 7,4 ,0};
	double F[3]{ 7,0 ,0};
	double G[3]{ 3,2 ,0};
	double H[3]{ 5,6 ,0};

	double A1[3]{ 0,12,10 };
	double B1[3]{ 7,12 ,10 };
	double C1[3]{ 8, 7 ,10 };
	double D1[3]{ 15,5 ,10 };
	double E1[3]{ 7,4 ,10 };
	double F1[3]{ 7,0 ,10 };
	double G1[3]{ 3,2 ,10 };
	double H1[3]{ 5,6 ,10 };
	Vector3 normal;








	glBindTexture(GL_TEXTURE_2D, texId);
	glBegin(GL_QUADS);
	normal = GetNormal(A, A1, B);
	glNormal3d(normal.X(), normal.Y(), normal.Z());

	glColor3d(1, 0, 1);
	glTexCoord2d(1, 0);
	glVertex3dv(A);
	glTexCoord2d(1, 1);
	glVertex3dv(B);
	glTexCoord2d(0, 1);
	glVertex3dv(B1);
	glTexCoord2d(0, 0);
	glVertex3dv(A1);

	glEnd();








	glBegin(GL_QUADS);

	normal = GetNormal(B, B1, C);
	glNormal3d(normal.X(), normal.Y(), normal.Z());


	glColor3f(1, 1, 0);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(C1);
	glVertex3dv(B1);

	glEnd();

	glBegin(GL_QUADS);

	normal = GetNormal(C, C1, D);
	glNormal3d(normal.X(), normal.Y(), normal.Z());

	glColor3f(0, 1, 0);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(D1);
	glVertex3dv(C1);

	glEnd();

	glBegin(GL_QUADS);

	normal = GetNormal(D, D1, E);
	glNormal3d(normal.X(), normal.Y(), normal.Z());

	glColor3f(1, 0, 0);

	glVertex3dv(D);
	glVertex3dv(E);
	glVertex3dv(E1);
	glVertex3dv(D1);
	glEnd();

	glBegin(GL_QUADS);

	normal = GetNormal(E, E1, F);
	glNormal3d(normal.X(), normal.Y(), normal.Z());

	glColor3f(0, 0, 1);

	glVertex3dv(E);
	glVertex3dv(F);
	glVertex3dv(F1);
	glVertex3dv(E1);

	glEnd();








	glBegin(GL_QUADS);

	normal = GetNormal(F, F1, G);
	glNormal3d(normal.X(), normal.Y(), normal.Z());

	glColor3d(0, 1, 1);
	glVertex3dv(F);
	glVertex3dv(G);
	glVertex3dv(G1);
	glVertex3dv(F1);

	glEnd();






	glBegin(GL_QUADS);

	normal = GetNormal(G, G1, H);
	glNormal3d(normal.X(), normal.Y(), normal.Z());

	glColor3d(0.5, 0, 1);
	glVertex3dv(G);
	glVertex3dv(H);
	glVertex3dv(H1);
	glVertex3dv(G1);


	glEnd();


	glBegin(GL_QUADS);

	glColor3d(1, 0.5, 0);

	normal = GetNormal(H, H1, A);
	glNormal3d(normal.X(), normal.Y(), normal.Z());

	glVertex3dv(H);
	glVertex3dv(A);
	glVertex3dv(A1);
	glVertex3dv(H1);
	glEnd();



	glBegin(GL_QUADS);

	normal = GetNormal(C, E, B);
	glNormal3d(normal.X(), normal.Y(), normal.Z());

	glColor3d(1, 0.7, 0.7);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(H);

	glVertex3dv(H);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(E);

	glVertex3dv(G);
	glVertex3dv(H);
	glVertex3dv(E);
	glVertex3dv(F);



	glEnd();



	glBegin(GL_QUADS);

	normal = GetNormal(B1, E1, C1);
	glNormal3d(normal.X(), normal.Y(), normal.Z());

	glColor3d(1, 0, 0.5);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(H1);

	glVertex3dv(H1);
	glVertex3dv(C1);
	glVertex3dv(D1);
	glVertex3dv(E1);

	glVertex3dv(G1);
	glVertex3dv(H1);
	glVertex3dv(E1);
	glVertex3dv(F1);


	glEnd();

	std::vector<double*>krug;
	std::vector<double*>krug2;


		double Midl[3]{ 5, 1 , 0};
		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_TRIANGLE_FAN);
		{
			glColor3d(0, 1, 1);
			glNormal3d(0, 0, -1);
			glVertex3dv(Midl);
			double Last[3]{ 3, 2, 0};
			for (double y = 1 - std::sqrt(5); y <= 2; y += 0.01) {
				double* dot;
				dot = new double[3] { kvadrouravX(y, 0), y, 0 };	
				krug.push_back(dot);//
				glVertex3dv(dot);
			}
			//krug.push_back(Last);
			glVertex3dv(Last);
		}
		glEnd();
		std::reverse(krug.begin(), krug.end());

		glBindTexture(GL_TEXTURE_2D, texId);
		glBegin(GL_TRIANGLE_FAN);
		{
			glNormal3d(0, 0, -1);
			glVertex3dv(Midl);
			for (double y = 1 - std::sqrt(5); y <= 0; y += 0.01) {

				double* dot;
				dot = new double[3] { kvadrouravX(y, 1), y, 0 };
				krug.push_back(dot);
				glVertex3dv(dot);

			}
			double Last[3]{ 7,0, 0 };
			krug.push_back(Last);
			glVertex3dv(Last);

		}
		glEnd();

		for (double* i : krug)
		{
			i[2] = 10;
		}
		glBegin(GL_TRIANGLE_FAN);
		{
			glNormal3d(0, 0, 1);
			Midl[2] = 10;
			glVertex3dv(Midl);
			for (double* i : krug)
			{
				glVertex3dv(i);
			}
		}
		glEnd();

		glBegin(GL_QUADS);
		{
			for (int i = 0; i < krug.size() - 1; i++)
			{
				double point1[3] = {krug[i][0],krug[i][1], 0};
				double point2[3] = {krug[i+1][0],krug[i+1][1], 0 };
				Vector3 normal;
				normal = GetNormal(point2, krug[i+1], krug[i]);
				glNormal3d(normal.X(), normal.Y(), normal.Z());

				glVertex3dv(point1);
				glVertex3dv(krug[i]);
				glVertex3dv(krug[i+1]);
				glVertex3dv(point2);
			}
		}
		glEnd();

   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}