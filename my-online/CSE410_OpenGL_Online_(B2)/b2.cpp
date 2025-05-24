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

// Function to draw coordinate axes (white)
void drawAxes() {
    glLineWidth(2);
    glColor3f(1, 1, 1); // White
    glBegin(GL_LINES);
    // X axis
    glVertex3f(0, 0, 0);
    glVertex3f(3, 0, 0);
    // Y axis
    glVertex3f(0, 0, 0);
    glVertex3f(0, 3, 0);
    // Z axis
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 3);
    glEnd();
}

// Function to draw fan body as octagonal prism
void drawFanBody() {
    float R = 1.5f; // Radius of octagon
    float H = 1.0f; // Height
    for (int i = 0; i < 8; i++) {
        float theta1 = i * M_PI / 4.0f;
        float theta2 = (i + 1) * M_PI / 4.0f;
        // Alternate colors: yellow and cyan
        if (i % 2 == 0)
            glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        else
            glColor3f(0.0f, 1.0f, 1.0f); // Cyan
        glBegin(GL_QUADS);
        glVertex3f(R * cos(theta1), -H, R * sin(theta1));
        glVertex3f(R * cos(theta2), -H, R * sin(theta2));
        glVertex3f(R * cos(theta2), H, R * sin(theta2));
        glVertex3f(R * cos(theta1), H, R * sin(theta1));
        glEnd();
    }
}

// Function to draw rod as octagonal prism
void drawRod() {
    float r = 0.15f; // Rod radius
    float h = 1.0f;  // Rod height
    // Draw 8 faces
    glColor3f(1.0f, 1.0f, 0.7f); // Light yellowish
    for (int i = 0; i < 8; i++) {
        float theta1 = i * M_PI / 4.0f;
        float theta2 = (i + 1) * M_PI / 4.0f;
        glBegin(GL_QUADS);
        glVertex3f(r * cos(theta1), h, r * sin(theta1));
        glVertex3f(r * cos(theta2), h, r * sin(theta2));
        glVertex3f(r * cos(theta2), -h, r * sin(theta2));
        glVertex3f(r * cos(theta1), -h, r * sin(theta1));
        glEnd();
    }
}

// Function to draw a single blade (magenta, flat)
void drawBlade() {
    glColor3f(1.0f, 0.0f, 1.0f); // Magenta
    glBegin(GL_QUADS);
    // Draw blade in X-Z plane (spread horizontally)
    glVertex3f(0.0f, 0.0f, -0.4f);  // Bottom-left
    glVertex3f(0.0f, 0.0f, 0.4f);   // Bottom-right
    glVertex3f(5.0f, 0.0f, 0.4f);   // Top-right
    glVertex3f(5.0f, 0.0f, -0.4f);  // Top-left
    glEnd();
}

// Timer function for blade animation
void update(int value) {
    bladeRotation += bladeSpeed;
    if (bladeRotation > 360.0f) {
        bladeRotation -= 360.0f;
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);  // 16ms = ~60fps
}

// Function to draw the complete fan
void drawFan() {
    glPushMatrix();
    // Apply overall fan rotation
    glRotatef(fanRotation, 1.0f, 0.0f, 0.0f);
    // Draw fan body (octagonal prism)
    drawFanBody();
    // Draw rod (octagonal prism)
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    drawRod();
    glPopMatrix();
    // Draw blades (magenta, 3 at 120 deg apart)
    glPushMatrix();
    glTranslatef(0.0f, 1.0f, 0.0f); // Move to top of rod
    glRotatef(bladeRotation, 0.0f, 1.0f, 0.0f); // Rotate around Y-axis for spinning
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        glRotatef(i * 120.0f, 0.0f, 1.0f, 0.0f); // Spread blades 120° apart around Y-axis
        drawBlade();
        glPopMatrix();
    }
    glPopMatrix();
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
    glutTimerFunc(0, update, 0);  // Start the timer
    
    initGL();
    glutMainLoop();
    return 0;
}