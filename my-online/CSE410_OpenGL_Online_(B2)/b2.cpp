#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// --- Global Variables ---
GLfloat eyex = 4, eyey = 4, eyez = 10;
GLfloat centerx = 0, centery = 0, centerz = 0;
GLfloat upx = 0, upy = 1, upz = 0;

// Fan specific variables
GLfloat fanRotation = 0.0f;      // Rotation of entire fan
GLfloat bladeRotation = 0.0f;    // Rotation of blades
GLfloat bladeSpeed = 2.0f;       // Speed of blade rotation

GLfloat MOVE_SPEED = 0.5f;
GLfloat ROTATION_SPEED = 2.0f * M_PI / 180.0f;

// Function to draw coordinate axes
void drawAxes() {
    glLineWidth(3);
    glBegin(GL_LINES);
    
    // X axis (red)
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(3, 0, 0);
    
    // Y axis (green)
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 3, 0);
    
    // Z axis (blue)
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 3);
    
    glEnd();
}

// Function to draw fan body (cuboid with 8 faces)
void drawFanBody() {
    glPushMatrix();
    
    // Set dark grey color for fan body
    glColor3f(0.2f, 0.2f, 0.2f);
    
    // Scale to make it look like a proper fan body
    glScalef(0.8f, 0.4f, 0.6f);
    
    // Draw the rectangular faces of the cuboid
    glBegin(GL_QUADS);
    
    // Front face
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    
    // Back face
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    
    // Top face
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    
    // Bottom face
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    
    // Right face
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    
    // Left face
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    
    glEnd();
    glPopMatrix();
}

// Function to draw rod (cylindrical approximation using 8 faces)
void drawRod() {
    glPushMatrix();
    
    // Set silver color for rod
    glColor3f(0.75f, 0.75f, 0.75f);
    
    // Scale to make it look like a thin rod
    glScalef(0.1f, 0.6f, 0.1f);
    
    // Draw 8 faces to approximate a cylinder
    glBegin(GL_QUAD_STRIP);
    for(int i = 0; i <= 8; i++) {
        float angle = (i % 8) * 2.0f * M_PI / 8;
        float x = cos(angle);
        float z = sin(angle);
        
        glVertex3f(x, 1.0f, z);
        glVertex3f(x, -1.0f, z);
    }
    glEnd();
    
    // Draw top and bottom faces
    glBegin(GL_POLYGON);
    for(int i = 0; i < 8; i++) {
        float angle = i * 2.0f * M_PI / 8;
        glVertex3f(cos(angle), 1.0f, sin(angle));
    }
    glEnd();
    
    glBegin(GL_POLYGON);
    for(int i = 0; i < 8; i++) {
        float angle = i * 2.0f * M_PI / 8;
        glVertex3f(cos(angle), -1.0f, sin(angle));
    }
    glEnd();
    
    glPopMatrix();
}

// Function to draw a single blade
void drawBlade() {
    glPushMatrix();
    
    // Set light grey color for blades
    glColor3f(0.8f, 0.8f, 0.8f);
    
    // Scale and position the blade
    glScalef(0.15f, 1.0f, 0.4f);
    
    // Draw blade as a rectangle
    glBegin(GL_QUADS);
    // Front face
    glVertex3f(-1.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 2.0f, 1.0f);
    glVertex3f(-1.0f, 2.0f, 1.0f);
    
    // Back face
    glVertex3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(-1.0f, 2.0f, -1.0f);
    glVertex3f(1.0f, 2.0f, -1.0f);
    glVertex3f(1.0f, 0.0f, -1.0f);
    
    // Top face
    glVertex3f(-1.0f, 2.0f, -1.0f);
    glVertex3f(-1.0f, 2.0f, 1.0f);
    glVertex3f(1.0f, 2.0f, 1.0f);
    glVertex3f(1.0f, 2.0f, -1.0f);
    
    // Bottom face
    glVertex3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(1.0f, 0.0f, -1.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 0.0f, 1.0f);
    glEnd();
    
    glPopMatrix();
}

// Function to draw the complete fan
void drawFan() {
    glPushMatrix();
    
    // Apply overall fan rotation
    glRotatef(fanRotation, 0.0f, 1.0f, 0.0f);
    
    // Draw fan body
    drawFanBody();
    
    // Position and draw rod
    glTranslatef(0.0f, 0.0f, 0.8f);
    drawRod();
    
    // Position and rotate blades
    static float lastTime = 0;
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    bladeRotation += bladeSpeed * (currentTime - lastTime);
    lastTime = currentTime;
    
    glRotatef(bladeRotation, 0.0f, 0.0f, 1.0f);
    
    // Draw three blades at 120 degrees apart
    drawBlade();
    glRotatef(120.0f, 0.0f, 0.0f, 1.0f);
    drawBlade();
    glRotatef(120.0f, 0.0f, 0.0f, 1.0f);
    drawBlade();
    
    glPopMatrix();
}

// Keyboard control function
void keyboardListener(unsigned char key, int x, int y) {
    switch(key) {
        case 'a':  // Rotate fan left
            fanRotation += 5.0f;
            break;
        case 'd':  // Rotate fan right
            fanRotation -= 5.0f;
            break;
        case 'w':  // Increase blade speed
            bladeSpeed += 0.5f;
            break;
        case 's':  // Decrease blade speed
            if(bladeSpeed > 0.5f)
                bladeSpeed -= 0.5f;
            break;
    }
    glutPostRedisplay();
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    gluLookAt(eyex, eyey, eyez,
              centerx, centery, centerz,
              upx, upy, upz);
    
    drawAxes();
    drawFan();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

// Initialize OpenGL settings
void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

// Window reshape function
void reshapeListener(int w, int h) {
    if (h == 0) h = 1;
    float ratio = w * 1.0 / h;
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45, ratio, 0.1, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("3D Rotating Fan");
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeListener);
    glutKeyboardFunc(keyboardListener);
    
    initGL();
    glutMainLoop();
    return 0;
}