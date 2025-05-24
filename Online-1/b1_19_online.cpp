#ifdef __linux__
#include <GL/glut.h>
#elif defined(_WIN32)
#include <windows.h>
#include <GL/glut.h>
#elif defined(__APPLE__)
#include <GLUT/glut.h>
#endif

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdio>
#include <chrono>
using namespace std;

#define PI 3.14159265f

void axes();
void circleOutline(double x, double y, double r);
void drawLine(double x1, double y1, double x2, double y2);
void square(double a);
void squareOutline(double a);
void drawFilledCircle(float x, float y, float radius, int segments);
void drawTriangleFilled(float x1, float y1, float x2, float y2, float x3, float y3);
void drawTriangleOutline(float x1, float y1, float x2, float y2, float x3, float y3);

double line1 = 10;
double line2 = 10;
double line3 = 0;

double speed1 = .01;
double speed2 = .01;
double speed3 = .01;

void display()
{
    // glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT);

    // axes();
    glColor3f(1, 0, 0);
    circleOutline(0, 0, 0.5);
    glPushMatrix();
    glRotatef(line1, 0, 0, 1);
    drawLine(0, 0, 0, 0.5);
    glTranslatef(0, 0.5, 0);
    glColor3f(0, 1, 0);
    circleOutline(0, 0, 0.2);
    glRotatef(line2, 0, 0, 1);
    drawLine(0, 0, 0, 0.2);
    glTranslatef(0, 0.2, 0);
    glColor3f(0, 0, 1);
    circleOutline(0, 0, 0.05);
    glRotatef(line3, 0, 0, 1);
    drawLine(0, 0, 0, 0.05);
    glPopMatrix();

    // glFlush();
    glutSwapBuffers();
}

void idle()
{
    // Redraw the display
    line1 = fmod(speed1 + line1, 360.0);
    line2 = fmod(speed2 + line2, 360.0);
    line3 = fmod(speed3 + line3, 360.0);
    glutPostRedisplay();
}

void init()
{
    glClearColor(0.0f, .0f, 0.0f, 1.0f); // Set background color to black and opaque
}

int main(int argc, char **argv)
{
    printf("Hello World\n");
    glutInit(&argc, argv);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("OpenGL Demo");

    init();

    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutMainLoop();

    return 0;
}

void axes()
{
    glColor3f(0, 0, 1.0);
    glBegin(GL_LINES);
    {
        glVertex3f(100, 0, 0);
        glVertex3f(-100, 0, 0);

        glVertex3f(0, -100, 0);
        glVertex3f(0, 100, 0);
    }
    glEnd();
}

void drawFilledCircle(float x, float y, float radius, int segments)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center of the circle
    for (int i = 0; i <= segments; ++i)
    {
        float angle = 2.0f * PI * i / segments;
        float dx = radius * cosf(angle);
        float dy = radius * sinf(angle);
        glVertex2f(x + dx, y + dy);
    }
    glEnd();
}

void circleOutline(double x, double y, double r)
{

    glPushMatrix();
    int segments = 100;
    glBegin(GL_LINE_LOOP); // Use line loop for border only
    for (int i = 0; i < segments; i++)
    {
        double theta = 2.0f * PI * double(i) / double(segments);
        double x = r * cosf(theta);
        double y = r * sinf(theta);
        glVertex2f(x, y);
    }
    glEnd();
    glPopMatrix();
}

void drawLine(double x1, double y1, double x2, double y2)
{
    glBegin(GL_LINES);
    {
        glVertex3f(x1, y1, 0);
        glVertex3f(x2, y2, 0);
    }
    glEnd();
}

void square(double a)
{

    glBegin(GL_QUADS);
    {
        glVertex3f(a, a, 0);
        glVertex3f(a, -a, 0);
        glVertex3f(-a, -a, 0);
        glVertex3f(-a, a, 0);
    }
    glEnd();
}

void squareOutline(double a)
{

    glBegin(GL_LINE_LOOP);
    {
        glVertex3f(a, a, 0);
        glVertex3f(a, -a, 0);
        glVertex3f(-a, -a, 0);
        glVertex3f(-a, a, 0);
    }
    glEnd();
}

void drawTriangleFilled(float x1, float y1, float x2, float y2, float x3, float y3)
{
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void drawTriangleOutline(float x1, float y1, float x2, float y2, float x3, float y3)
{
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}
