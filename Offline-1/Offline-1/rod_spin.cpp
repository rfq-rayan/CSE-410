#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

float angle = 0.0f;
float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;
bool rotateX = false;
bool rotateY = true; // Start with Y rotation
bool rotateZ = false;
bool showHelp = true;

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void printHelpManual() {
    printf("\n--- Keyboard Controls ---\n");
    printf("x: Toggle X-axis rotation (currently %s)\n", rotateX ? "ON" : "OFF");
    printf("y: Toggle Y-axis rotation (currently %s)\n", rotateY ? "ON" : "OFF");
    printf("z: Toggle Z-axis rotation (currently %s)\n", rotateZ ? "ON" : "OFF");
    printf("r: Reset rotation angles\n");
    printf("h: Show/hide this help menu\n");
    printf("ESC: Exit program\n");
    printf("----------------------\n\n");
}

void renderBitmapString(float x, float y, float z, void *font, const char *string) {
    const char *c;
    glRasterPos3f(x, y, z);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void displayHelpOnScreen() {
    char buffer[100];
    
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Set to orthographic projection for text rendering
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Display help text in top right corner
    renderBitmapString(550, 580, 0, GLUT_BITMAP_9_BY_15, "--- Controls ---");
    renderBitmapString(550, 560, 0, GLUT_BITMAP_9_BY_15, "x: Toggle X rotation");
    renderBitmapString(550, 540, 0, GLUT_BITMAP_9_BY_15, "y: Toggle Y rotation");
    renderBitmapString(550, 520, 0, GLUT_BITMAP_9_BY_15, "z: Toggle Z rotation");
    renderBitmapString(550, 500, 0, GLUT_BITMAP_9_BY_15, "r: Reset rotation");
    renderBitmapString(550, 480, 0, GLUT_BITMAP_9_BY_15, "h: Toggle help");
    
    // Show current rotation states
    sprintf(buffer, "X-rotation: %s", rotateX ? "ON" : "OFF");
    renderBitmapString(550, 440, 0, GLUT_BITMAP_9_BY_15, buffer);
    sprintf(buffer, "Y-rotation: %s", rotateY ? "ON" : "OFF");
    renderBitmapString(550, 420, 0, GLUT_BITMAP_9_BY_15, buffer);
    sprintf(buffer, "Z-rotation: %s", rotateZ ? "ON" : "OFF");
    renderBitmapString(550, 400, 0, GLUT_BITMAP_9_BY_15, buffer);
    
    // Restore the projection
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'x': // Toggle X-axis rotation
            rotateX = !rotateX;
            printf("X-axis rotation: %s\n", rotateX ? "ON" : "OFF");
            break;
        case 'y': // Toggle Y-axis rotation
            rotateY = !rotateY;
            printf("Y-axis rotation: %s\n", rotateY ? "ON" : "OFF");
            break;
        case 'z': // Toggle Z-axis rotation
            rotateZ = !rotateZ;
            printf("Z-axis rotation: %s\n", rotateZ ? "ON" : "OFF");
            break;
        case 'r': // Reset rotation angles
            angleX = 0.0f;
            angleY = 0.0f;
            angleZ = 0.0f;
            // reset rotation states
            rotateX = false;
            rotateY = false; // Start with Y rotation
            rotateZ = false;
            printf("Rotation angles reset\n");
            break;
        case 'h': // Show help
            showHelp = !showHelp;
            if (showHelp) {
                printHelpManual();
            } else {
                printf("Help hidden. Press 'h' to show help again.\n");
            }
            break;
        case 27: // ESC key - exit program
            printf("Exiting program\n");
            exit(0);
            break;
    }
}

void drawAxes() {
    // Draw X axis in red
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(3.0f, 0.0f, 0.0f);
    glEnd();
    
    // Draw Y axis in green
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 3.0f, 0.0f);
    glEnd();
    
    // Draw Z axis in blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 3.0f);
    glEnd();
}

void drawCylinder() {
    GLUquadricObj *quadObj = gluNewQuadric();
    
    // Set drawing style
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    gluQuadricNormals(quadObj, GLU_SMOOTH);
    
    const float radius = 0.2f;
    const float height = 2.0f;
    const int slices = 32;
    const int stacks = 10;
    
    // Move cylinder so its center is at the origin
    glPushMatrix();
    
    // Draw the white bottom cap
    glColor3f(1.0f, 1.0f, 1.0f); // White
    glPushMatrix();
    glTranslatef(0.0f, -height/2, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotate disk to face down
    gluDisk(quadObj, 0.0f, radius, slices, 1);
    glPopMatrix();
    
    // Draw the cylindrical body with half red and half green
    glPushMatrix();
    glTranslatef(0.0f, -height/2, 0.0f); // Start from bottom
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f); // Rotate to align with Y axis
    
    // Draw custom cylinder with half red and half green
    const float PI = 3.14159265358979323846f;
    const float angleStep = 2.0f * PI / slices;
    const float heightStep = height / stacks;
    
    for (int i = 0; i < stacks; i++) {
        float z0 = i * heightStep;
        float z1 = (i + 1) * heightStep;
        
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j <= slices; j++) {
            float angle = j * angleStep;
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            
            // Choose color based on angle - half red, half green
            if (j < slices / 2) {
                glColor3f(1.0f, 0.0f, 0.0f); // Red
            } else {
                glColor3f(0.0f, 1.0f, 0.0f); // Green
            }
            
            glVertex3f(x, y, z1);
            glVertex3f(x, y, z0);
        }
        glEnd();
    }
    glPopMatrix();
    
    // Draw the yellow top cap
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow
    glPushMatrix();
    glTranslatef(0.0f, height/2, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f); // Rotate disk to face up
    gluDisk(quadObj, 0.0f, radius, slices, 1);
    glPopMatrix();
    
    glPopMatrix();
    
    gluDeleteQuadric(quadObj);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Position the camera
    gluLookAt(0.0f, 0.0f, 5.0f,  // Eye position
              0.0f, 0.0f, 0.0f,  // Look at point
              0.0f, 1.0f, 0.0f); // Up vector

    // Draw axes before applying rotation
    drawAxes();

    // Rotate around axes
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);
    glRotatef(angleZ, 0.0f, 0.0f, 1.0f);

    // Draw the cylindrical rod instead of the cube
    drawCylinder();

    // Display help on screen if enabled
    if (showHelp) {
        displayHelpOnScreen();
    }

    glutSwapBuffers();
}

void update(int value) {
    // Update angles based on which rotations are enabled
    if (rotateX) {
        angleX += 2.0f;
        if (angleX > 360.0f) angleX -= 360.0f;
    }
    if (rotateY) {
        angleY += 2.0f;
        if (angleY > 360.0f) angleY -= 360.0f;
    }
    if (rotateZ) {
        angleZ += 2.0f;
        if (angleZ > 360.0f) angleZ -= 360.0f;
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0); // 60 FPS approximately
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w/(float)h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Spinning Rod");
    init();
    
    // Print initial help message
    printf("Press 'h' for help on keyboard controls\n");
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, update, 0);
    glutKeyboardFunc(keyboard);
    
    glutMainLoop();
    return 0;
}