/**
 * OpenGL 3D Camera and Bouncing Ball Simulation
 * - Fully controllable 3D camera
 * - Bouncing ball physics in a cube
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>



#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =====================
// Constants & Globals
// =====================
const float GRAVITY = -9.8f;
const float COLLISION = 0.75f;
const float MOVEMENT_SPEED = 0.5f;
const float ROTATION_SPEED = 2.0f;
const float MIN_VELOCITY = 0.01f;
const float CUBE_SIZE = 6.0f;

bool showVelocityArrow = true;
bool simulationRunning = false;
float ballRadius = 0.3f;

// =====================
// Vector3 Struct & Math
// =====================
struct Vector3 {
    float x, y, z;
    Vector3(float _x=0, float _y=0, float _z=0) : x(_x), y(_y), z(_z) {}
    Vector3 operator+(const Vector3& v) const { return Vector3(x+v.x, y+v.y, z+v.z); }
    Vector3 operator-(const Vector3& v) const { return Vector3(x-v.x, y-v.y, z-v.z); }
    Vector3 operator*(float s) const { return Vector3(x*s, y*s, z*s); }
    Vector3& normalize() { float l = length(); if(l>0) { x/=l; y/=l; z/=l; } return *this; }
    float length() const { return sqrt(x*x + y*y + z*z); }
    Vector3 cross(const Vector3& v) const {
        return Vector3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
    }
    Vector3 normalized() const {
        float l = length();
        if (l > 0) return Vector3(x / l, y / l, z / l);
        return *this;
    }
    float dot(const Vector3& v) const { return x*v.x + y*v.y + z*v.z; }
};

// =====================
// Camera State
// =====================
Vector3 eye(10, 20, 10);
Vector3 center(0, 0, 0);
Vector3 up(0, 1, 0);

// =====================
// Ball State
// =====================
Vector3 ballPos, ballVel, ballAngularVel, ballRotationAxis(0, 1, 0);
float ballRotationAngle = 0;


// Helper Functions 
void rotateVector(Vector3 &v, const Vector3 &axis, float angle) {
    float rad = angle * M_PI / 180.0f;
    float c = cos(rad);
    float s = sin(rad);
    float t = 1 - c;
    
    Vector3 a = axis;
    a.normalize();
    
    Vector3 rotated;
    rotated.x = (t*a.x*a.x + c) * v.x + (t*a.x*a.y - s*a.z) * v.y + (t*a.x*a.z + s*a.y) * v.z;
    rotated.y = (t*a.x*a.y + s*a.z) * v.x + (t*a.y*a.y + c) * v.y + (t*a.y*a.z - s*a.x) * v.z;
    rotated.z = (t*a.x*a.z - s*a.y) * v.x + (t*a.y*a.z + s*a.x) * v.y + (t*a.z*a.z + c) * v.z;
    
    v = rotated;
}

void normalizeUpVector() {
    Vector3 forward = center - eye;
    forward.normalize();
    
    Vector3 right = forward.cross(up);
    right.normalize();
    
    up = right.cross(forward);
    up.normalize();
}

// =====================
// Physics Simulation
// =====================
void updatePhysics(int value) {
    if (!simulationRunning) {
        glutTimerFunc(16, updatePhysics, 0);
        return;
    }

    float dt = 0.016f; // Approximate 60 FPS
    
    // Apply gravity
    ballVel.y += GRAVITY * dt;
    
    // Update position
    ballPos = ballPos + ballVel * dt;
    
    // Check collisions with cube walls
    float margin = CUBE_SIZE - ballRadius;
    
    // Floor collision (y = 0)
    if (ballPos.y < ballRadius) {
        ballPos.y = ballRadius;
        ballVel.y = -ballVel.y * COLLISION;
        
        // Stop if velocity is very small
        if (fabs(ballVel.y) < MIN_VELOCITY && fabs(ballVel.x) < MIN_VELOCITY && fabs(ballVel.z) < MIN_VELOCITY) {
            ballVel = Vector3(0, 0, 0);
        }
    }
    
    // Other walls
    if (ballPos.x < -margin) { ballPos.x = -margin; ballVel.x = -ballVel.x * COLLISION; }
    if (ballPos.x > margin) { ballPos.x = margin; ballVel.x = -ballVel.x * COLLISION; }
    if (ballPos.z < -margin) { ballPos.z = -margin; ballVel.z = -ballVel.z * COLLISION; }
    if (ballPos.z > margin) { ballPos.z = margin; ballVel.z = -ballVel.z * COLLISION; }
    if (ballPos.y > 2*CUBE_SIZE - ballRadius) { ballPos.y = 2*CUBE_SIZE - ballRadius; ballVel.y = -ballVel.y * COLLISION; }
    
    // Update rolling rotation
    if (ballVel.length() > 0.1f) {
        ballRotationAxis = Vector3(-ballVel.z, 0, ballVel.x).normalize();
        ballRotationAngle += ballVel.length() * dt * 180.0f / (M_PI * ballRadius);
        if (ballRotationAngle > 360.0f) ballRotationAngle -= 360.0f;
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, updatePhysics, 0);
}

// =====================
// Camera Controls
// =====================
void keyboardListener(unsigned char key, int x, int y) {
    Vector3 forward = center - eye;
    forward.normalize();
    Vector3 right = forward.cross(up);
    right.normalize();

    switch (key) {
        case '1':
            rotateVector(forward, up, ROTATION_SPEED);
            center = eye + forward;
            printf("1: Yaw left\n");
            break;
        case '2':
            rotateVector(forward, up, -ROTATION_SPEED);
            center = eye + forward;
            printf("2: Yaw right\n");
            break;
        case '3':
            rotateVector(forward, right, ROTATION_SPEED);
            rotateVector(up, right, ROTATION_SPEED);
            center = eye + forward;
            printf("3: Pitch up\n");
            break;
        case '4':
            rotateVector(forward, right, -ROTATION_SPEED);
            rotateVector(up, right, -ROTATION_SPEED);
            center = eye + forward;
            printf("4: Pitch down\n");
            break;
        case '5':
            rotateVector(up, forward, ROTATION_SPEED);
            printf("5: Roll clockwise\n");
            break;
        case '6':
            rotateVector(up, forward, -ROTATION_SPEED);
            printf("6: Roll counterclockwise\n");
            break;
         case 'w': // Move upward without changing reference point
{
    // Calculate current distance
    Vector3 viewDir = center - eye;
    float distance = viewDir.length();
    viewDir.normalize();
    
    // Move along up vector (but subtract any forward component to maintain distance)
    Vector3 moveDir = up - viewDir * (up.x * viewDir.x + up.y * viewDir.y + up.z * viewDir.z);
    moveDir.normalize();
    
    eye = eye + moveDir * MOVEMENT_SPEED;
    
    // Recalculate center to maintain exact distance
    Vector3 newViewDir = center - eye;
    newViewDir.normalize();
    center = eye + newViewDir * distance;
    break;
}

case 's': // Move downward without changing reference point
{
    // Calculate current distance
    Vector3 viewDir = center - eye;
    float distance = viewDir.length();
    viewDir.normalize();
    
    // Move along negative up vector (but subtract any forward component)
    Vector3 moveDir = Vector3(-up.x, -up.y, -up.z) - viewDir * (-up.x * viewDir.x + -up.y * viewDir.y + -up.z * viewDir.z);

    moveDir.normalize();
    
    eye = eye + moveDir * MOVEMENT_SPEED;
    
    // Recalculate center to maintain exact distance
    Vector3 newViewDir = center - eye;
    newViewDir.normalize();
    center = eye + newViewDir * distance;
    break;
}
        case ' ':
            simulationRunning = !simulationRunning;
            printf("Space: Simulation %s\n", simulationRunning ? "ON" : "OFF");
            break;
        case 'r':
            ballPos = Vector3((rand()%100)/100.0f * 2*CUBE_SIZE - CUBE_SIZE, 
                             ballRadius,
                             (rand()%100)/100.0f * 2*CUBE_SIZE - CUBE_SIZE);
            ballVel = Vector3((rand()%100)/100.0f - 0.5f, 
                             (rand()%100)/100.0f + 2.0f, 
                             (rand()%100)/100.0f - 0.5f);
            printf("r: Reset ball to random position. Ball position: (%.2f, %.2f, %.2f)\n", ballPos.x, ballPos.y, ballPos.z);
            printf("  Ball velocity: (%.2f, %.2f, %.2f)\n", ballVel.x, ballVel.y, ballVel.z);
            break;
        case 'v':
            showVelocityArrow = !showVelocityArrow;
            printf("v: Toggle velocity arrow display\n");
            break;
        case '+':
            if (!simulationRunning) {
                ballVel = ballVel * 1.1f;
                printf("+: Ball speed: %.2f\n", ballVel.length());
            }
            break;
        case '-':
            if (!simulationRunning) {
                ballVel = ballVel * 0.9f;
                printf("-: Ball speed: %.2f\n", ballVel.length());
            }
            break;
        case 'x':
            eye = Vector3(10, 20, 10);
            center = Vector3(0, 0, 0);
            up = Vector3(0, 1, 0);
            printf("x: Reset camera to default position\n");
            break;
        default:
            printf("Unknown key\n");
            break;
    }

    normalizeUpVector();
    glutPostRedisplay();
}

void specialKeyListener(int key, int x, int y) {
    Vector3 forward = center - eye;
    forward.normalize();
    
    Vector3 right = forward.cross(up);
    right.normalize();
    
    switch(key) {
        case GLUT_KEY_UP:
            eye = eye + forward * MOVEMENT_SPEED;
            center = center + forward * MOVEMENT_SPEED;
            printf("Arrow Up: Move camera forward\n");
            break;
        case GLUT_KEY_DOWN:
            eye = eye - forward * MOVEMENT_SPEED;
            center = center - forward * MOVEMENT_SPEED;
            printf("Arrow Down: Move camera backward\n");
            break;
        case GLUT_KEY_LEFT:
            eye = eye - right * MOVEMENT_SPEED;
            center = center - right * MOVEMENT_SPEED;
            printf("Arrow Left: Move camera left\n");
            break;
        case GLUT_KEY_RIGHT:
            eye = eye + right * MOVEMENT_SPEED;
            center = center + right * MOVEMENT_SPEED;
            printf("Arrow Right: Move camera right\n");
            break;
        case GLUT_KEY_PAGE_UP:
            eye = eye + up * MOVEMENT_SPEED;
            center = center + up * MOVEMENT_SPEED;
            printf("Page Up: Move camera up\n");
            break;
        case GLUT_KEY_PAGE_DOWN:
            eye = eye - up * MOVEMENT_SPEED;
            center = center - up * MOVEMENT_SPEED;
            printf("Page Down: Move camera down\n");
            break;
        // else
        default:
            printf("Unknown key\n");
            break;
    }
    
    glutPostRedisplay();
}

// =====================
// Drawing Functions
// =====================
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

void drawCube() {
    glBegin(GL_QUADS);


    // Top face (y = top) - Color #8080CC
    glColor3f(0.25f, 0.5f, 0.8f);
    glVertex3f( CUBE_SIZE, 2*CUBE_SIZE, -CUBE_SIZE);
    glVertex3f(-CUBE_SIZE, 2*CUBE_SIZE, -CUBE_SIZE);
    glVertex3f(-CUBE_SIZE, 2*CUBE_SIZE,  CUBE_SIZE);
    glVertex3f( CUBE_SIZE, 2*CUBE_SIZE,  CUBE_SIZE);

    glEnd();

    // Bottom face (checkerboard)
    int numSquares = 20;
    float squareSize = 2 * CUBE_SIZE / numSquares;
    for (int i = 0; i < numSquares; i++) {
        for (int j = 0; j < numSquares; j++) {
            glBegin(GL_QUADS);
            if ((i + j) % 2 == 0) {
                glColor3f(0.9f, 0.9f, 0.9f);
            } else {
                glColor3f(0.2f, 0.2f, 0.2f);
            }
            float x1 = -CUBE_SIZE + i * squareSize;
            float x2 = x1 + squareSize;
            float z1 = -CUBE_SIZE + j * squareSize;
            float z2 = z1 + squareSize;
            glVertex3f(x1, 0, z1);
            glVertex3f(x2, 0, z1);
            glVertex3f(x2, 0, z2);
            glVertex3f(x1, 0, z2);
            glEnd();
        }
    }

    // Front face (cyan)
    glBegin(GL_QUADS);
    glColor3f(0.3f, 0.8f, 0.8f);
    glVertex3f( CUBE_SIZE, 2*CUBE_SIZE,  CUBE_SIZE);
    glVertex3f(-CUBE_SIZE, 2*CUBE_SIZE,  CUBE_SIZE);
    glVertex3f(-CUBE_SIZE, 0,            CUBE_SIZE);
    glVertex3f( CUBE_SIZE, 0,            CUBE_SIZE);

    // Back face (yellow)
    glColor3f(0.8f, 0.8f, 0.3f);
    glVertex3f( CUBE_SIZE, 0,           -CUBE_SIZE);
    glVertex3f(-CUBE_SIZE, 0,           -CUBE_SIZE);
    glVertex3f(-CUBE_SIZE, 2*CUBE_SIZE, -CUBE_SIZE);
    glVertex3f( CUBE_SIZE, 2*CUBE_SIZE, -CUBE_SIZE);

    // Left face (red)
    glColor3f(0.8f, 0.3f, 0.3f);
    glVertex3f(-CUBE_SIZE, 2*CUBE_SIZE,  CUBE_SIZE);
    glVertex3f(-CUBE_SIZE, 2*CUBE_SIZE, -CUBE_SIZE);
    glVertex3f(-CUBE_SIZE, 0,           -CUBE_SIZE);
    glVertex3f(-CUBE_SIZE, 0,            CUBE_SIZE);

    // Right face (green)
    glColor3f(0.3f, 0.8f, 0.3f);
    glVertex3f( CUBE_SIZE, 2*CUBE_SIZE, -CUBE_SIZE);
    glVertex3f( CUBE_SIZE, 2*CUBE_SIZE,  CUBE_SIZE);
    glVertex3f( CUBE_SIZE, 0,            CUBE_SIZE);
    glVertex3f( CUBE_SIZE, 0,           -CUBE_SIZE);

    glEnd();
}


void drawVerticalAlternatingStripedSphere(float radius, int slices, int stacks) {
    for (int i = 0; i < slices; ++i) {
        float theta1 = (2.0f * M_PI * i) / slices;
        float theta2 = (2.0f * M_PI * (i + 1)) / slices;

        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= stacks; ++j) {
            float phi = M_PI * j / stacks;
            float y = radius * cos(phi);
            float r = radius * sin(phi);

            // Determine if we are in the top or bottom hemisphere
            bool topHemisphere = (j <= stacks / 2);

            // Color logic: invert color in bottom hemisphere
            bool isRed = (i % 2 == 0);
            if (!topHemisphere) isRed = !isRed;

            if (isRed)
                glColor3f(1.0f, 0.0f, 0.0f); // Red
            else
                glColor3f(0.0f, 1.0f, 0.0f); // Green

            // First point (on theta1)
            float x1 = r * cos(theta1);
            float z1 = r * sin(theta1);
            glVertex3f(x1, y, z1);

            // Second point (on theta2)
            float x2 = r * cos(theta2);
            float z2 = r * sin(theta2);
            glVertex3f(x2, y, z2);
        }
        glEnd();
    }
}

void drawBall() {
    glPushMatrix();
    glTranslatef(ballPos.x, ballPos.y, ballPos.z);
    glRotatef(ballRotationAngle, ballRotationAxis.x, ballRotationAxis.y, ballRotationAxis.z);
    
    drawVerticalAlternatingStripedSphere(ballRadius, 24, 24); 
    glPopMatrix(); 

    if (showVelocityArrow && ballVel.length() > 0.1f) {
        glColor3f(1, 1, 0); // Yellow arrow
        glLineWidth(2);
        
        // Draw the arrow shaft
        glBegin(GL_LINES);
        glVertex3f(ballPos.x, ballPos.y, ballPos.z);
        Vector3 arrowEnd = ballPos + ballVel * 0.5f;
        glVertex3f(arrowEnd.x, arrowEnd.y, arrowEnd.z);
        glEnd();

        // Draw the cone head
        glPushMatrix();
        glTranslatef(arrowEnd.x, arrowEnd.y, arrowEnd.z);
        
        // Calculate rotation to point cone in velocity direction
        Vector3 zAxis(0, 0, 1);
        Vector3 rotAxis = zAxis.cross(ballVel.normalized());
        float rotAngle = acos(zAxis.dot(ballVel.normalized())) * 180.0f / M_PI;
        
        if (rotAxis.length() > 0.001f) {
            glRotatef(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);
        }
        
        // Draw cone
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, 0.1f); // Cone tip
        for(int i = 0; i <= 8; i++) {
            float angle = i * 2.0f * M_PI / 8.0f;
            glVertex3f(0.05f * cos(angle), 0.05f * sin(angle), 0);
        }
        glEnd();
        
        glPopMatrix();
    }
}

// =====================
// Main Functions
// =====================
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    gluLookAt(eye.x, eye.y, eye.z,
              center.x, center.y, center.z,
              up.x, up.y, up.z);
    
    drawAxes();
    drawCube();
    drawBall();
    
    glutSwapBuffers();
}

void reshapeListener(GLsizei width, GLsizei height) {
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void initGL() {
    glClearColor(0.18f, 0.20f, 0.22f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    srand(time(NULL));
    
    // Initial ball velocity
    ballVel = Vector3((rand()%100)/100.0f - 0.5f, 
                     (rand()%100)/100.0f + 2.0f, 
                     (rand()%100)/100.0f - 0.5f);
}

void printControls() {
    printf("\n--- 3D Bouncing Ball Controls ---\n");
    printf("Space: Toggle simulation on/off\n");
    printf("v: Toggle velocity arrow display\n");
    printf("+/-: Increase/decrease ball speed (when paused)\n");
    printf("r: Reset ball to random position (on floor)\n");
    printf("\n--- Camera Controls ---\n");
    printf("Arrow keys: Move camera forward/backward/left/right\n");
    printf("Page Up/Down: Move camera up/down\n");
    printf("w/s: Move camera up/down (with up vector)\n");
    printf("1/2: Yaw (look left/right)\n");
    printf("3/4: Pitch (look up/down)\n");
    printf("5/6: Roll (tilt camera clockwise/counterclockwise)\n");
    printf("x: Reset camera to default position\n");
    printf("---------------------------------\n\n");
}

int main(int argc, char **argv) {
    printControls();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutCreateWindow("OpenGL 3D Camera & Physics Demo");
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeListener);
    glutSpecialFunc(specialKeyListener);
    glutKeyboardFunc(keyboardListener);
    glutTimerFunc(16, updatePhysics, 0);
    
    initGL();
    glutMainLoop();
    return 0;
}