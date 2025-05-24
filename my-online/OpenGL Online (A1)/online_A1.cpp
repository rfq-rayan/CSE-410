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
GLfloat eyex = 4, eyey = 2, eyez = 10;
GLfloat centerx = 0, centery = 0, centerz = 0;
GLfloat upx = 0, upy = 1, upz = 0;

// Windmill specific variables
GLfloat windmillRotation = 0.0f;    // Rotation of entire windmill
GLfloat bladeRotation = 0.0f;       // Rotation of blades
GLfloat bladeSpeed = 2.0f;          // Speed of blade rotation

GLfloat MOVE_SPEED = 0.5f;
GLfloat ROTATION_SPEED = 2.0f * M_PI / 180.0f;

// --- Function Declarations ---
void drawAxes();
void rotateVector(GLfloat &x, GLfloat &y, GLfloat &z, 
                 GLfloat ax, GLfloat ay, GLfloat az, 
                 GLfloat angle);
void normalizeUpVector();
void keyboardListener(unsigned char key, int x, int y);
void specialKeyListener(int key, int x, int y);
void drawBase();
void drawPole();
void drawBlade();
void drawWindmill();
void display();
void initGL();
void reshapeListener(int w, int h);

// --- Coordinate Axes Drawing Function ---
/**
 * Draw coordinate axes
 * X axis: red, Y axis: green, Z axis: blue
 */
void drawAxes()
{
    glLineWidth(3);  // Set line thickness

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

// --- Helper Functions from stepbystep.cpp ---
void rotateVector(GLfloat &x, GLfloat &y, GLfloat &z, 
                 GLfloat ax, GLfloat ay, GLfloat az, 
                 GLfloat angle) {
    // ...existing code...
}

void normalizeUpVector() {
    // ...existing code...
}

// Modified keyboard controls to include windmill specific controls
void keyboardListener(unsigned char key, int x, int y) {
    // Calculate vectors for camera movement
    GLfloat fx = centerx - eyex;
    GLfloat fy = centery - eyey;
    GLfloat fz = centerz - eyez;
    float len = sqrt(fx*fx + fy*fy + fz*fz);
    fx /= len; fy /= len; fz /= len;
    
    GLfloat rx = fy * upz - fz * upy;
    GLfloat ry = fz * upx - fx * upz;
    GLfloat rz = fx * upy - fy * upx;
    len = sqrt(rx*rx + ry*ry + rz*rz);
    rx /= len; ry /= len; rz /= len;
    
    switch (key) {
        // Windmill specific controls
        case 'a':  // Rotate windmill left
            windmillRotation += 5.0f;
            break;
        case 'd':  // Rotate windmill right
            windmillRotation -= 5.0f;
            break;
        case 'w':  // Increase blade speed
            bladeSpeed += 0.5f;
            break;
        case 's':  // Decrease blade speed
            if(bladeSpeed > 0.5f) bladeSpeed -= 0.5f;
            break;
            
        // Camera controls from original code
        case '1': 
            rotateVector(fx, fy, fz, upx, upy, upz, ROTATION_SPEED);
            centerx = eyex + fx;
            centery = eyey + fy;
            centerz = eyez + fz;
            break;
        // ...rest of the camera controls...
    }
    
    normalizeUpVector();
    glutPostRedisplay();
}

void specialKeyListener(int key, int x, int y) {
    // ...existing code...
}

// --- Windmill Drawing Functions ---
void drawBase() {
    glPushMatrix();
    
    // Set brown color for base
    glColor3f(0.545f, 0.271f, 0.075f);
      // Scale to make it look like a proper base
    glScalef(0.5f, 0.4f, 0.5f);
    
    // Draw the rectangular faces of the base
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
    
    // Left face
    glVertex3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(-1.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 2.0f, 1.0f);
    glVertex3f(-1.0f, 2.0f, -1.0f);
    
    // Right face
    glVertex3f(1.0f, 0.0f, -1.0f);
    glVertex3f(1.0f, 2.0f, -1.0f);
    glVertex3f(1.0f, 2.0f, 1.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    
    glEnd();
    
    glPopMatrix();
}

void drawPole() {
    glPushMatrix();
    
    // Set grey color for pole
    glColor3f(0.5f, 0.5f, 0.5f);
      // Scale to make it look like a proper pole
    glScalef(0.15f, 1.2f, 0.15f);
    
    // Draw the rectangular faces of the pole
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
    
    // Left face
    glVertex3f(-1.0f, 0.0f, -1.0f);
    glVertex3f(-1.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 2.0f, 1.0f);
    glVertex3f(-1.0f, 2.0f, -1.0f);
    
    // Right face
    glVertex3f(1.0f, 0.0f, -1.0f);
    glVertex3f(1.0f, 2.0f, -1.0f);
    glVertex3f(1.0f, 2.0f, 1.0f);
    glVertex3f(1.0f, 0.0f, 1.0f);
    
    glEnd();
    
    glPopMatrix();
}

void drawBlade() {
    glPushMatrix();
    
    // Set white color for blades
    glColor3f(1.0f, 1.0f, 1.0f);
      // Draw triangular blade
    glBegin(GL_TRIANGLES);
    glVertex3f(0.0f, 0.0f, 0.0f);    // Center point
    glVertex3f(0.0f, 1.2f, 0.0f);    // Top point
    glVertex3f(0.3f, 0.0f, 0.0f);    // Right point
    glEnd();
    
    glPopMatrix();
}

void drawWindmill() {
    glPushMatrix();
    
    // Apply overall windmill rotation
    glRotatef(windmillRotation, 0.0f, 1.0f, 0.0f);
    
    // Draw base
    drawBase();
      // Move up to draw pole
    glTranslatef(0.0f, 0.8f, 0.0f);
    drawPole();
    
    // Move to top of pole to draw blades
    glTranslatef(0.0f, 2.4f, 0.0f);
    
    // Rotate blades continuously
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

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Set up the camera
    gluLookAt(eyex, eyey, eyez,
              centerx, centery, centerz,
              upx, upy, upz);
    
    // Draw coordinate axes
    drawAxes();
    
    // Draw the windmill
    drawWindmill();
    
    glutSwapBuffers();
    
    // Request continuous animation
    glutPostRedisplay();
}

// --- Main function and initialization ---
void initGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

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
    glutCreateWindow("3D Windmill Simulation");
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeListener);
    glutSpecialFunc(specialKeyListener);
    glutKeyboardFunc(keyboardListener);
    
    initGL();
    glutMainLoop();
    return 0;
}