/**
 * OpenGL 3D Drawing Demo with Proper Camera Controls
 */

// --- Includes ---
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// --- Global Variables ---
GLfloat eyex = 10, eyey = 20, eyez = 10;
GLfloat centerx = 0, centery = 0, centerz = 0;
GLfloat upx = 0, upy = 1, upz = 0;

GLfloat cubeSize = 6.0f;
GLfloat sphereX = 0, sphereY = 0, sphereZ = 0;
GLfloat sphereRadius = 0.3f;

GLfloat MOVE_SPEED = 0.5f;
GLfloat ROTATION_SPEED = 2.0f * M_PI / 180.0f; // 2 degrees in radians

// --- Helper Functions ---
void rotateVector(GLfloat &x, GLfloat &y, GLfloat &z, 
                 GLfloat ax, GLfloat ay, GLfloat az, 
                 GLfloat angle) {
    // Normalize the axis
    float len = sqrt(ax*ax + ay*ay + az*az);
    if (len == 0) return;
    ax /= len; ay /= len; az /= len;
    
    // Rotation matrix components
    float c = cos(angle);
    float s = sin(angle);
    float t = 1 - c;
    
    // Rotate the vector
    float nx = (t*ax*ax + c) * x + (t*ax*ay - s*az) * y + (t*ax*az + s*ay) * z;
    float ny = (t*ax*ay + s*az) * x + (t*ay*ay + c) * y + (t*ay*az - s*ax) * z;
    float nz = (t*ax*az - s*ay) * x + (t*ay*az + s*ax) * y + (t*az*az + c) * z;
    
    x = nx; y = ny; z = nz;
}

void normalizeUpVector() {
    // Recalculate up vector to ensure orthogonality with view direction
    GLfloat fx = centerx - eyex;
    GLfloat fy = centery - eyey;
    GLfloat fz = centerz - eyez;
    
    // Right vector = forward cross up
    GLfloat rx = fy * upz - fz * upy;
    GLfloat ry = fz * upx - fx * upz;
    GLfloat rz = fx * upy - fy * upx;
    
    // Recalculate up vector as right cross forward to ensure orthogonality
    upx = ry * fz - rz * fy;
    upy = rz * fx - rx * fz;
    upz = rx * fy - ry * fx;
    
    // Normalize the up vector
    float len = sqrt(upx*upx + upy*upy + upz*upz);
    if (len > 0) {
        upx /= len;
        upy /= len;
        upz /= len;
    }
}

// --- Camera Control Functions ---
void keyboardListener(unsigned char key, int x, int y) {
    // Calculate forward vector
    GLfloat fx = centerx - eyex;
    GLfloat fy = centery - eyey;
    GLfloat fz = centerz - eyez;
    float len = sqrt(fx*fx + fy*fy + fz*fz);
    fx /= len; fy /= len; fz /= len;
    
    // Calculate right vector
    GLfloat rx = fy * upz - fz * upy;
    GLfloat ry = fz * upx - fx * upz;
    GLfloat rz = fx * upy - fy * upx;
    len = sqrt(rx*rx + ry*ry + rz*rz);
    rx /= len; ry /= len; rz /= len;
    
    switch (key) {
        // Yaw controls
        case '1': // Look left (Yaw)
            rotateVector(fx, fy, fz, upx, upy, upz, ROTATION_SPEED);
            centerx = eyex + fx;
            centery = eyey + fy;
            centerz = eyez + fz;
            break;
            
        case '2': // Look right (Yaw)
            rotateVector(fx, fy, fz, upx, upy, upz, -ROTATION_SPEED);
            centerx = eyex + fx;
            centery = eyey + fy;
            centerz = eyez + fz;
            break;
            
        // Pitch controls
        case '3': // Look up (Pitch)
            rotateVector(fx, fy, fz, rx, ry, rz, ROTATION_SPEED);
            rotateVector(upx, upy, upz, rx, ry, rz, ROTATION_SPEED);
            centerx = eyex + fx;
            centery = eyey + fy;
            centerz = eyez + fz;
            break;
            
        case '4': // Look down (Pitch)
            rotateVector(fx, fy, fz, rx, ry, rz, -ROTATION_SPEED);
            rotateVector(upx, upy, upz, rx, ry, rz, -ROTATION_SPEED);
            centerx = eyex + fx;
            centery = eyey + fy;
            centerz = eyez + fz;
            break;
            
        // Roll controls
        case '5': // Tilt clockwise (Roll)
            rotateVector(upx, upy, upz, fx, fy, fz, ROTATION_SPEED);
            break;
            
        case '6': // Tilt counterclockwise (Roll)
            rotateVector(upx, upy, upz, fx, fy, fz, -ROTATION_SPEED);
            break;
            
        // Vertical movement without changing reference point
        case 'w': // Move upward
            eyey += MOVE_SPEED;
            break;
            
        case 's': // Move downward
            eyey -= MOVE_SPEED;
            break;
    }
    
    normalizeUpVector();
    glutPostRedisplay();
}

void specialKeyListener(int key, int x, int y) {
    // Calculate forward vector
    GLfloat fx = centerx - eyex;
    GLfloat fy = centery - eyey;
    GLfloat fz = centerz - eyez;
    float len = sqrt(fx*fx + fy*fy + fz*fz);
    fx /= len; fy /= len; fz /= len;
    
    // Calculate right vector
    GLfloat rx = fy * upz - fz * upy;
    GLfloat ry = fz * upx - fx * upz;
    GLfloat rz = fx * upy - fy * upx;
    
    switch(key) {
        case GLUT_KEY_UP: // Move forward
            eyex += fx * MOVE_SPEED;
            eyey += fy * MOVE_SPEED;
            eyez += fz * MOVE_SPEED;
            centerx += fx * MOVE_SPEED;
            centery += fy * MOVE_SPEED;
            centerz += fz * MOVE_SPEED;
            break;
            
        case GLUT_KEY_DOWN: // Move backward
            eyex -= fx * MOVE_SPEED;
            eyey -= fy * MOVE_SPEED;
            eyez -= fz * MOVE_SPEED;
            centerx -= fx * MOVE_SPEED;
            centery -= fy * MOVE_SPEED;
            centerz -= fz * MOVE_SPEED;
            break;
            
        case GLUT_KEY_LEFT: // Move left
            eyex -= rx * MOVE_SPEED;
            eyey -= ry * MOVE_SPEED;
            eyez -= rz * MOVE_SPEED;
            centerx -= rx * MOVE_SPEED;
            centery -= ry * MOVE_SPEED;
            centerz -= rz * MOVE_SPEED;
            break;
            
        case GLUT_KEY_RIGHT: // Move right
            eyex += rx * MOVE_SPEED;
            eyey += ry * MOVE_SPEED;
            eyez += rz * MOVE_SPEED;
            centerx += rx * MOVE_SPEED;
            centery += ry * MOVE_SPEED;
            centerz += rz * MOVE_SPEED;
            break;
            
        case GLUT_KEY_PAGE_UP: // Move upward
            eyex += upx * MOVE_SPEED;
            eyey += upy * MOVE_SPEED;
            eyez += upz * MOVE_SPEED;
            centerx += upx * MOVE_SPEED;
            centery += upy * MOVE_SPEED;
            centerz += upz * MOVE_SPEED;
            break;
            
        case GLUT_KEY_PAGE_DOWN: // Move downward
            eyex -= upx * MOVE_SPEED;
            eyey -= upy * MOVE_SPEED;
            eyez -= upz * MOVE_SPEED;
            centerx -= upx * MOVE_SPEED;
            centery -= upy * MOVE_SPEED;
            centerz -= upz * MOVE_SPEED;
            break;
    }
    
    glutPostRedisplay();
}

/**
 * Draw coordinate axes
 * X axis: red, Y axis: green, Z axis: blue
 */
void drawAxes()
{
    glLineWidth(3); // Set line thickness

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

/**
 * Draw a colored cube centered at the origin
 * Each face has a different color
 */
void drawCube()
{
    // Store cube vertices based on size
    GLfloat s = cubeSize;  // shorthand for cubeSize
    int numSquares = 20;    // Number of squares in each dimension for chessboard
    GLfloat squareSize = 2 * s / numSquares; // Size of each square in the chessboard
    
    // Adjust y-coordinates to place bottom face at origin
    GLfloat bottom = 0;     // y-coordinate of bottom face (at origin)
    GLfloat top = 2 * s;    // y-coordinate of top face (2*s above origin)
    
    glBegin(GL_QUADS);

    // Top face (y = top) - Color #8080CC
    glColor3f(0.25f, 0.5f, 0.8f);
    glVertex3f(s, top, -s);
    glVertex3f(-s, top, -s);
    glVertex3f(-s, top, s);
    glVertex3f(s, top, s);

    // Bottom face (y = bottom) - Chessboard pattern
    glEnd();
    for (int i = 0; i < numSquares; i++) {
        for (int j = 0; j < numSquares; j++) {
            // Determine color based on position (alternating black and white)
            if ((i + j) % 2 == 0) {
                glColor3f(0.9f, 0.9f, 0.9f); // Light gray
            } else {
                glColor3f(0.2f, 0.2f, 0.2f); // Dark gray
            }
            
            // Calculate position of this square
            GLfloat x1 = -s + i * squareSize;
            GLfloat x2 = x1 + squareSize;
            GLfloat z1 = -s + j * squareSize;
            GLfloat z2 = z1 + squareSize;
            
            // Draw the square on the bottom face
            glBegin(GL_QUADS);
            glVertex3f(x1, bottom, z1);
            glVertex3f(x2, bottom, z1);
            glVertex3f(x2, bottom, z2);
            glVertex3f(x1, bottom, z2);
            glEnd();
        }
    }

    // Front face (z = s) - Color #4CCCCC
    glBegin(GL_QUADS);
    glColor3f(0.3f, 0.8f, 0.8f);
    glVertex3f(s, top, s);
    glVertex3f(-s, top, s);
    glVertex3f(-s, bottom, s);
    glVertex3f(s, bottom, s);

    // Back face (z = -s) - Yellow
    glColor3f(0.8f, 0.8f, 0.3f);
    glVertex3f(s, bottom, -s);
    glVertex3f(-s, bottom, -s);
    glVertex3f(-s, top, -s);
    glVertex3f(s, top, -s);

    // Left face (x = -s) - Red
    glColor3f(0.8f, 0.3f, 0.3f);
    glVertex3f(-s, top, s);
    glVertex3f(-s, top, -s);
    glVertex3f(-s, bottom, -s);
    glVertex3f(-s, bottom, s);

    // Right face (x = s) - Color #4CCC4C
    glColor3f(0.3f, 0.8f, 0.3f);
    glVertex3f(s, top, -s);
    glVertex3f(s, top, s);
    glVertex3f(s, bottom, s);
    glVertex3f(s, bottom, -s);

    glEnd();
}



/**
 * Draw a sphere with alternating red and green stripes
 */
void drawPatternedSphere(GLfloat radius, int slices, int stacks) {
    // Draw the sphere manually to create the striped pattern
    
    GLfloat x, y, z;
    GLfloat s, t;
    
    // Define the number of stripes
    int numStripes = 8;
    
    for(int i = 0; i < stacks; i++) {
        // t runs from 0 to 1 (from top to bottom)
        t = (GLfloat)i / stacks;
        
        // Stripe pattern is determined by the y position
        int stripeIndex = (int)(t * numStripes);
        bool isRedStripe = (stripeIndex % 2 == 0);
        
        // Calculate angles for current and next stack
        GLfloat phi1 = M_PI * t;
        GLfloat phi2 = M_PI * (t + 1.0/stacks);
        
        // Start drawing quad strips
        glBegin(GL_QUAD_STRIP);
        
        for(int j = 0; j <= slices; j++) {
            // s runs from 0 to 1 (around the sphere)
            s = (GLfloat)j / slices;
            
            // Calculate angles for current slice
            GLfloat theta = 2.0 * M_PI * s;
            
            // Calculate points on the sphere for both stacks at this slice
            // For first point (on current stack)
            x = radius * sin(phi1) * cos(theta);
            y = radius * cos(phi1);
            z = radius * sin(phi1) * sin(theta);
            
            // Set color - alternate between red and green
            if(isRedStripe) {
                glColor3f(1.0f, 0.0f, 0.0f); // Red
            } else {
                glColor3f(0.0f, 1.0f, 0.0f); // Green
            }
            
            glVertex3f(x, y, z);
            
            // For second point (on next stack)
            x = radius * sin(phi2) * cos(theta);
            y = radius * cos(phi2);
            z = radius * sin(phi2) * sin(theta);
            
            // Set color - alternate between red and green
            if(isRedStripe) {
                glColor3f(1.0f, 0.0f, 0.0f); // Red
            } else {
                glColor3f(0.0f, 1.0f, 0.0f); // Green
            }
            
            glVertex3f(x, y, z);
        }
        
        glEnd();
    }
}
/**
 * Draw a striped red and green sphere at the specified position
 */
void drawSphere() {
    glPushMatrix();
    glTranslatef(sphereX, sphereY, sphereZ);
    drawPatternedSphere(sphereRadius, 20, 16);
    glPopMatrix();
}
// --- Function Declarations ---
void display();
void reshapeListener(int w, int h);
void initGL();

// --- Function Definitions ---
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

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Set up the camera
    gluLookAt(eyex, eyey, eyez,
              centerx, centery, centerz,
              upx, upy, upz);
    
    // Draw the scene
    drawAxes();
    drawCube();
    drawSphere();
    
    glutSwapBuffers();
}

/**
 * Main function
 */
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("OpenGL 3D Camera Demo");

    glutDisplayFunc(display);
    glutReshapeFunc(reshapeListener);
    glutSpecialFunc(specialKeyListener);
    glutKeyboardFunc(keyboardListener);

    initGL();
    glutMainLoop();
    return 0;
}