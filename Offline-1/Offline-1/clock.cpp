#include <GL/glut.h>
#include <cmath>
#include <ctime>
#include <cstdio>

const float PI = 3.14159265358979323846;
const int w = 600;
const int h = 600;
const float r = 0.8;

// Function to convert degrees to radians
float d2r(float d) {
    return (d * PI) / 180.0;
}

// Function to draw a circle
void drawCircle(float x, float y, float r, int n) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < n; i++) {
        float t = 2.0f * PI * float(i) / float(n);
        float x1 = r * cos(t);
        float y1 = r * sin(t);
        glVertex2f(x1 + x, y1 + y);
    }
    glEnd();
}

// Function to draw clock hour ticks
void drawHourTicks() {
    glLineWidth(2.0);
    for (int i = 0; i < 12; i++) {
        float a = i * 30.0;
        float rad = d2r(a);
        
        float ox = r * sin(rad);
        float oy = r * cos(rad);
        float ix = (r - 0.05) * sin(rad);
        float iy = (r - 0.05) * cos(rad);
        
        glBegin(GL_LINES);
        glVertex2f(ox, oy);
        glVertex2f(ix, iy);
        glEnd();
        
        // Draw hour numbers
        float tx = (r - 0.15) * sin(rad);
        float ty = (r - 0.15) * cos(rad);
        
        // Display hour number
        glRasterPos2f(tx - 0.02, ty - 0.02);
        char hr[3];
        // Convert hour to string (12 instead of 0)
        int hn = (i == 0) ? 12 : i;
        sprintf(hr, "%d", hn);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, hr[0]);
        if (hn >= 10) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, hr[1]);
    }

    // Draw minute ticks
    glLineWidth(1.0);
    for (int i = 0; i < 60; i++) {
        // Skip positions where hour ticks are drawn
        if (i % 5 == 0) continue;
        
        float a = i * 6.0;
        float rad = d2r(a);
        
        float ox = r * sin(rad);
        float oy = r * cos(rad);
        float ix = (r - 0.03) * sin(rad);
        float iy = (r - 0.03) * cos(rad);
        
        glBegin(GL_LINES);
        glVertex2f(ox, oy);
        glVertex2f(ix, iy);
        glEnd();
    }
}

void drawClockHands() {
    // Use a continuous time source for animation
    static unsigned int st = glutGet(GLUT_ELAPSED_TIME);
    unsigned int ct = glutGet(GLUT_ELAPSED_TIME);
    float es = (ct - st) / 1000.0f;
    
    // Get the initial time as a base
    static bool init = false;
    static time_t irt;
    static struct tm it;
    
    if (!init) {
        time(&irt);
        it = *localtime(&irt);
        init = true;
    }
    
    // Calculate total seconds elapsed since program start
    float ts = it.tm_sec + es;
    float tm = it.tm_min + ts / 60.0f;
    float th = (it.tm_hour % 12) + tm / 60.0f;
    
    // Normalize values to their proper ranges
    while (ts >= 60.0f) ts -= 60.0f;
    while (tm >= 60.0f) tm -= 60.0f;
    while (th >= 12.0f) th -= 12.0f;
    
    // Calculate angles for each hand (90° offset to start at 12 o'clock)
    float ha = d2r(90.0 - (th * 30));
    float ma = d2r(90.0 - (tm * 6));
    float sa = d2r(90.0 - (ts * 6));
    
    // Hour hand
    glColor3f(0.5, 0.5, 0.5);
    glLineWidth(5.0);
    glBegin(GL_LINES);
    glVertex2f(0.0, 0.0);
    glVertex2f(r * 0.5 * cos(ha), r * 0.5 * sin(ha));
    glEnd();
    
    // Minute hand
    glColor3f(0.0, 1.0, 0.0);
    glLineWidth(3.0);
    glBegin(GL_LINES);
    glVertex2f(0.0, 0.0);
    glVertex2f(r * 0.7 * cos(ma), r * 0.7 * sin(ma));
    glEnd();
    
    // Second hand
    glColor3f(0.0, 0.0, 1.0);
    glLineWidth(2.0);
    glBegin(GL_LINES);
    glVertex2f(0.0, 0.0);
    glVertex2f(r * 0.9 * cos(sa), r * 0.9 * sin(sa));
    glEnd();

    glColor3f(1.0, 0.0, 0.0);
    glPointSize(8.0);
    glBegin(GL_POINTS);
    glVertex2f(0.0, 0.0);
    glEnd();
}

// Display callback function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    

    glColor3f(1.0, 0.5, 0.0); //orange
    glLineWidth(4.0);
    drawCircle(0.0, 0.0, r, 100);
    glColor3f(0.0, 1.0, 0.0);
    drawHourTicks();   
    drawClockHands();
    
    glutSwapBuffers();
}

// Timer callback for animation
void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS for smooth animation (16ms)
}

// Reshape callback function
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Maintain aspect ratio
    if (w <= h)
        gluOrtho2D(-1.0, 1.0, -1.0 * h / w, 1.0 * h / w);
    else
        gluOrtho2D(-1.0 * w / h, 1.0 * w / h, -1.0, 1.0);
        
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(w, h);
    glutCreateWindow("Analog Clock");
    
    glClearColor(0.0, 0.0, 0.0, 1.0); // Black background
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    
    glutMainLoop();
    return 0;
}
