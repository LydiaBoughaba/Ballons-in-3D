#include <GL\freeglut.h>
#include <cmath>
#include <stdlib.h>

//Les couleurs
GLfloat WHITE[] = { 1, 1, 1 };
GLfloat BLACK[] = { 0, 0, 0 };
GLfloat BLUE[] = { 0, 0, 1 };
GLfloat RED[] = { 1, 0, 0 };
GLfloat GREEN[] = { 0, 1, 0 };
float x = 0;
float y = 0;
float z = 0;

//La caméra
class Camera {
	double theta;
	double dTheta; 
	double dy; 
	double x,y,z; 
	
public:
	Camera(): y(5), x(20), z(4), theta(0.42), dTheta(0.04), dy(0.2){}
	double getX() {
		return 20 * cos(theta);;
	}
	double getY() {
		return y;
	}
	double getZ() {
		return 10 * sin(theta);
	}
	
	void moveRight() { theta += dTheta; }
	void moveLeft() { theta -= dTheta; }
	void moveUp() { y += dy; }
	void moveDown() { if (y > dy) y -= dy; }
};

//Ballon
class Ball {
	double radius;
	GLfloat*color;
	double maximumHeight;
	double x;
	double y;
	double z;
	int direction;

public:
	Ball(double r, GLfloat* c, double h, double x, double z):
		radius(r), color(c), maximumHeight(h), direction(-1), y(h), x(x), z(z) {}

	void update() {
		y += direction * 0.1;
		if (y > maximumHeight) {
			y = maximumHeight;
			direction = -1;
		}
		else if (y < radius) {
			y = radius;
			direction = 1;
		}
		glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
		glTranslated(x, y, z);
		glutSolidSphere(radius, 30, 30);
		glPopMatrix();
	}
	void drawSphereReflexion()
	{
    	glScalef(1.0, -1.0, 1.0);
    	
    	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
		glTranslated(x, y, z);
		glutSolidSphere(radius, 30, 30);
		glPopMatrix();
		glDisable(GL_NORMALIZE);
    	glCullFace(GL_BACK);
	}
};

class Checkerboard {
	int displayListId;
	int width;
	int depth;
public:
	Checkerboard(int width, int depth) : width(width), depth(depth) {}
	double centerx() { return width / 2; }
	double centerz() { return depth / 2; }
	
	void create() {
		GLfloat rectangle[] ={0.9, 0.9, 0.9, 0.2};
		displayListId = glGenLists(1);
		glNewList(displayListId, GL_COMPILE);
		GLfloat lightPosition[] = { 4, 3, 7, 1 };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		glBegin(GL_QUADS);
		glNormal3d(0, 1, 0);
		/*for (int x = 0; x < width - 1; x++) {
			for (int z = 0; z < depth - 1; z++) {
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,rectangle);
				glVertex3d(x, 0, z);
				glVertex3d(x + 1, 0, z);
				glVertex3d(x + 1, 0, z + 1);
				glVertex3d(x, 0, z + 1);
			}
		}*/
		glEnd();
		glEndList();
	}
	void draw()
	{	
		glCallList(displayListId);	
	}
};


Checkerboard checkerboard(10, 9);
Camera camera;
Ball balls[] = {
	Ball(0.5, GREEN, 3, 4.2, 3.2),
	Ball(0.5, BLUE, 3.2, 4, 6),
	Ball(0.5, RED, 3.3, 3.9, 4),

};


void init() {
	glClearColor( 0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, WHITE);
	glLightfv(GL_LIGHT0, GL_SPECULAR, WHITE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, WHITE);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	checkerboard.create();
}


void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef( x, y, z ); 
	gluLookAt(camera.getX(), camera.getY(), camera.getZ(), checkerboard.centerx(), 0.0, checkerboard.centerz(), 0.0, 1.0, 0.0);
	
	checkerboard.draw();

	for (int i = 0; i < sizeof balls / sizeof(Ball); i++) {
		balls[i].update();
		balls[i].drawSphereReflexion();
	}
	glFlush();
	glutSwapBuffers();
}


void reshape(GLint M, GLint b) {
	glViewport(0, 0, M, b);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, GLfloat(M) / GLfloat(b), 1.0, 150.0);
	glMatrixMode(GL_MODELVIEW);
}


void timer(int v)
{
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, timer, v);
}


void special1( int key, int, int )
{
    const float step = 0.4;
    if( GLUT_KEY_LEFT == key )
        x -= step;
    if( GLUT_KEY_RIGHT == key )
        x += step;
    if( GLUT_KEY_UP == key )
        y += step;
    if( GLUT_KEY_DOWN == key )
        y -= step;
    glutPostRedisplay();
}


void special2(int key, int, int){
	switch (key)
	{
	case GLUT_KEY_LEFT: camera.moveLeft(); break;
	case GLUT_KEY_RIGHT: camera.moveRight(); break;
	case GLUT_KEY_UP: camera.moveUp(); break;
	case GLUT_KEY_DOWN: camera.moveDown(); break;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv) { 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(80, 80);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Scène");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special1); 
	glutSpecialFunc(special2); 
	glutTimerFunc(100, timer, 0);
	init();
	glutMainLoop();
	return 0;
}
