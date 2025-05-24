/**
 * Camera Orbit Demo
 *
 * This program demonstrates camera orbiting techniques in OpenGL
 * Controls:
 * - A/D: Orbit left/right around the target
 * - W/S: Orbit up/down around the target
 * - Q/E: Roll camera clockwise/counterclockwise
 * - Arrow keys: Move camera position
 * - R: Reset camera to initial position
 * - ESC: Exit program
 */

// --- Includes ---
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// --- Global Variables ---
// Camera position and orientation
GLfloat eyex = 4, eyey = 4, eyez = 4;          // Camera position coordinates
GLfloat centerx = 0, centery = 0, centerz = 0; // Look-at point coordinates
GLfloat upx = 0, upy = 1, upz = 0;             // Up vector coordinates

// Camera orbit parameters
const float ORBIT_RADIUS = 5.0f;                // Default orbit radius
const float ORBIT_SPEED = 0.05f;                // Orbit speed
bool showOrbitPath = true;                      // Display orbit path
bool showCameraPosition = true;                 // Show current camera position

// Object visibility flags
bool showAxes = true;                          // Toggle for coordinate axes
bool showTarget = true;                        // Toggle for target object

// Function Declarations
void initGL();
void display();
void reshapeListener(GLsizei width, GLsizei height);
void keyboardListener(unsigned char key, int x, int y);
void specialKeyListener(int key, int x, int y);
void drawAxes();
void drawSphere(float radius, int slices, int stacks);
void drawOrbitPath();
void drawCamera();
void drawCube();
void printInstructions();

/**
 * Initialize OpenGL settings
 */
void initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    // Print instructions to console
    printInstructions();
}

/**
 * Main display function
 */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Position camera using eye, center and up vectors
    gluLookAt(eyex, eyey, eyez,
              centerx, centery, centerz,
              upx, upy, upz);
              
    // Draw axes if enabled
    if (showAxes)
        drawAxes();
        
    // Draw orbit path if enabled
    if (showOrbitPath)
        drawOrbitPath();
        
    // Draw camera position indicator
    if (showCameraPosition)
        drawCamera();
        
    // Draw target cube
    if (showTarget)
        drawCube();

    // Display info on screen
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Display camera position
    glColor3f(1.0f, 1.0f, 1.0f);
    char buffer[100];
    sprintf(buffer, "Camera: (%.1f, %.1f, %.1f)", eyex, eyey, eyez);
    glRasterPos2i(10, 580);
    for (char* c = buffer; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glutSwapBuffers();
}

/**
 * Window reshape callback
 */
void reshapeListener(GLsizei width, GLsizei height)
{
    if (height == 0) height = 1;
    
    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

/**
 * Keyboard input handler for standard keys
 */
void keyboardListener(unsigned char key, int x, int y)
{
    // Calculate view direction vector
    double lx = centerx - eyex;
    double ly = centery - eyey;
    double lz = centerz - eyez;
    double s;

    switch (key) {
        // --- Orbit Controls ---
        case 'a': // Orbit left around target
            eyex += ORBIT_SPEED * (upy * lz);
            eyez += ORBIT_SPEED * (-lx * upy);
            s = sqrt(eyex * eyex + eyez * eyez) / ORBIT_RADIUS;
            eyex /= s;
            eyez /= s;
            printf("Orbiting left\n");
            break;
            
        case 'd': // Orbit right around target
            eyex += ORBIT_SPEED * (-upy * lz);
            eyez += ORBIT_SPEED * (lx * upy);
            s = sqrt(eyex * eyex + eyez * eyez) / ORBIT_RADIUS;
            eyex /= s;
            eyez /= s;
            printf("Orbiting right\n");
            break;
            
        case 'w': // Orbit up
            // Need to use the right vector (cross product of view and up)
            {
                double rightx = ly * upz - lz * upy;
                double righty = lz * upx - lx * upz;
                double rightz = lx * upy - ly * upx;
                
                // Normalize right vector
                double len = sqrt(rightx*rightx + righty*righty + rightz*rightz);
                rightx /= len;
                righty /= len;
                rightz /= len;
                
                // Move eye position in the direction of right cross view
                eyex += ORBIT_SPEED * (righty * lz - rightz * ly);
                eyey += ORBIT_SPEED * (rightz * lx - rightx * lz);
                eyez += ORBIT_SPEED * (rightx * ly - righty * lx);
                
                // Normalize to maintain orbit radius
                double dist = sqrt(eyex*eyex + eyey*eyey + eyez*eyez);
                eyex = eyex * ORBIT_RADIUS / dist;
                eyey = eyey * ORBIT_RADIUS / dist;
                eyez = eyez * ORBIT_RADIUS / dist;
                printf("Orbiting up\n");
            }
            break;
            
        case 's': // Orbit down
            // Similar to orbiting up, but in opposite direction
            {
                double rightx = ly * upz - lz * upy;
                double righty = lz * upx - lx * upz;
                double rightz = lx * upy - ly * upx;
                
                double len = sqrt(rightx*rightx + righty*righty + rightz*rightz);
                rightx /= len;
                righty /= len;
                rightz /= len;
                
                eyex -= ORBIT_SPEED * (righty * lz - rightz * ly);
                eyey -= ORBIT_SPEED * (rightz * lx - rightx * lz);
                eyez -= ORBIT_SPEED * (rightx * ly - righty * lx);
                
                double dist = sqrt(eyex*eyex + eyey*eyey + eyez*eyez);
                eyex = eyex * ORBIT_RADIUS / dist;
                eyey = eyey * ORBIT_RADIUS / dist;
                eyez = eyez * ORBIT_RADIUS / dist;
                printf("Orbiting down\n");
            }
            break;
            
        case 'q': // Roll camera counterclockwise
            {
                // Rotate up vector around view direction
                double len = sqrt(lx*lx + ly*ly + lz*lz);
                double vx = lx/len;
                double vy = ly/len;
                double vz = lz/len;
                
                // Apply Rodrigues' rotation formula
                double angle = 0.05; // Small angle for smooth rotation
                double cosA = cos(angle);
                double sinA = sin(angle);
                
                double newUpx = upx * cosA + (vy * upz - vz * upy) * sinA + vx * (vx * upx + vy * upy + vz * upz) * (1 - cosA);
                double newUpy = upy * cosA + (vz * upx - vx * upz) * sinA + vy * (vx * upx + vy * upy + vz * upz) * (1 - cosA);
                double newUpz = upz * cosA + (vx * upy - vy * upx) * sinA + vz * (vx * upx + vy * upy + vz * upz) * (1 - cosA);
                
                // Normalize the new up vector
                double upLen = sqrt(newUpx*newUpx + newUpy*newUpy + newUpz*newUpz);
                upx = newUpx / upLen;
                upy = newUpy / upLen;
                upz = newUpz / upLen;
                printf("Rolling counterclockwise\n");
            }
            break;
            
        case 'e': // Roll camera clockwise
            {
                // Similar to counterclockwise roll but with negative angle
                double len = sqrt(lx*lx + ly*ly + lz*lz);
                double vx = lx/len;
                double vy = ly/len;
                double vz = lz/len;
                
                double angle = -0.05;
                double cosA = cos(angle);
                double sinA = sin(angle);
                
                double newUpx = upx * cosA + (vy * upz - vz * upy) * sinA + vx * (vx * upx + vy * upy + vz * upz) * (1 - cosA);
                double newUpy = upy * cosA + (vz * upx - vx * upz) * sinA + vy * (vx * upx + vy * upy + vz * upz) * (1 - cosA);
                double newUpz = upz * cosA + (vx * upy - vy * upx) * sinA + vz * (vx * upx + vy * upy + vz * upz) * (1 - cosA);
                
                double upLen = sqrt(newUpx*newUpx + newUpy*newUpy + newUpz*newUpz);
                upx = newUpx / upLen;
                upy = newUpy / upLen;
                upz = newUpz / upLen;
                printf("Rolling clockwise\n");
            }
            break;
            
        case 'r': // Reset camera position
            eyex = 4;
            eyey = 4;
            eyez = 4;
            centerx = 0;
            centery = 0;
            centerz = 0;
            upx = 0;
            upy = 1;
            upz = 0;
            printf("Camera reset\n");
            break;
            
        case 'p': // Toggle orbit path visibility
            showOrbitPath = !showOrbitPath;
            printf("Orbit path %s\n", showOrbitPath ? "shown" : "hidden");
            break;
            
        case 'c': // Toggle camera position indicator
            showCameraPosition = !showCameraPosition;
            printf("Camera position indicator %s\n", showCameraPosition ? "shown" : "hidden");
            break;
            
        case 'x': // Toggle axes
            showAxes = !showAxes;
            printf("Axes %s\n", showAxes ? "shown" : "hidden");
            break;
            
        case 't': // Toggle target object
            showTarget = !showTarget;
            printf("Target %s\n", showTarget ? "shown" : "hidden");
            break;
            
        case 'h': // Show help
            printInstructions();
            break;
            
        case 27: // ESC key
            exit(0);
            break;
    }
    
    glutPostRedisplay();
}

/**
 * Special key input handler (arrow keys, function keys)
 */
void specialKeyListener(int key, int x, int y)
{
    float moveSpeed = 0.25f;
    
    switch (key) {
        case GLUT_KEY_LEFT:  // Move camera left
            eyex -= moveSpeed;
            break;
            
        case GLUT_KEY_RIGHT: // Move camera right
            eyex += moveSpeed;
            break;
            
        case GLUT_KEY_UP:    // Move camera forward
            eyez -= moveSpeed;
            break;
            
        case GLUT_KEY_DOWN:  // Move camera backward
            eyez += moveSpeed;
            break;
            
        case GLUT_KEY_PAGE_UP:    // Move camera up
            eyey += moveSpeed;
            break;
            
        case GLUT_KEY_PAGE_DOWN:  // Move camera down
            eyey -= moveSpeed;
            break;
    }
    
    glutPostRedisplay();
}

/**
 * Print control instructions to console
 */
void printInstructions() {
    printf("\n=== Camera Orbit Demo Controls ===\n");
    printf("Orbit Controls:\n");
    printf("  A/D: Orbit left/right around target\n");
    printf("  W/S: Orbit up/down around target\n");
    printf("  Q/E: Roll camera counterclockwise/clockwise\n");
    printf("\nCamera Position:\n");
    printf("  Arrow keys: Move camera position\n");
    printf("  Page Up/Down: Move camera up/down\n");
    printf("  R: Reset camera to initial position\n");
    printf("\nVisibility Controls:\n");
    printf("  P: Toggle orbit path\n");
    printf("  C: Toggle camera position indicator\n");
    printf("  X: Toggle axes\n");
    printf("  T: Toggle target cube\n");
    printf("  H: Show this help message\n");
    printf("  ESC: Exit program\n");
    printf("===================================\n\n");
}

/**
 * Draw coordinate axes
 */
void drawAxes()
{
    glLineWidth(3);

    glBegin(GL_LINES);
    
    // X axis (red)
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(2, 0, 0);
    
    // Y axis (green)
    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 2, 0);
    
    // Z axis (blue)
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 2);
    
    glEnd();
    
    // Draw axis labels
    glColor3f(1, 1, 1);
    glRasterPos3f(2.1, 0, 0);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'X');
    glRasterPos3f(0, 2.1, 0);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Y');
    glRasterPos3f(0, 0, 2.1);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Z');
}

/**
 * Draw a sphere with given parameters
 */
void drawSphere(float radius, int slices, int stacks) {
    glutSolidSphere(radius, slices, stacks);
}

/**
 * Draw the orbit path
 */
void drawOrbitPath() {
    glColor3f(0.5, 0.5, 0.5);  // Gray color for orbit
    
    // Draw a circle in the XZ plane at y=eyey
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++) {
        float angle = 2.0f * M_PI * i / 100;
        float x = ORBIT_RADIUS * cos(angle);
        float z = ORBIT_RADIUS * sin(angle);
        glVertex3f(x, eyey, z);
    }
    glEnd();
    
    // Draw a circle in the XY plane at z=eyez
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++) {
        float angle = 2.0f * M_PI * i / 100;
        float x = ORBIT_RADIUS * cos(angle);
        float y = ORBIT_RADIUS * sin(angle);
        glVertex3f(x, y, eyez);
    }
    glEnd();
}

/**
 * Draw the camera position indicator
 */
void drawCamera() {
    glPushMatrix();
    
    // Temporarily reset the camera transformation
    glTranslatef(eyex, eyey, eyez);
    
    // Draw a small sphere to represent the camera
    glColor3f(1.0f, 1.0f, 0.0f);  // Yellow
    glutSolidSphere(0.1, 10, 10);
    
    // Draw camera direction line (from eye to center)
    glColor3f(1.0f, 1.0f, 1.0f);  // White
    glBegin(GL_LINES);
    glVertex3f(0, 0, 0);
    glVertex3f(centerx - eyex, centery - eyey, centerz - eyez);
    glEnd();
    
    glPopMatrix();
}

/**
 * Draw a colored cube representing the target
 */
void drawCube()
{
    glBegin(GL_QUADS);
    
    // Top face (y = 1.0f) - Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    
    // Bottom face (y = -1.0f) - Orange
    glColor3f(1.0f, 0.5f, 0.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    
    // Front face  (z = 1.0f) - Red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    
    // Back face (z = -1.0f) - Yellow
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    
    // Left face (x = -1.0f) - Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glVertex3f(-1.0f, -1.0f, 1.0f);
    
    // Right face (x = 1.0f) - Magenta
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.0f, 1.0f, -1.0f);
    glVertex3f(1.0f, 1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, 1.0f);
    glVertex3f(1.0f, -1.0f, -1.0f);
    
    glEnd();
}

/**
 * Main function: Program entry point
 */
int main(int argc, char **argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);
    
    // Configure display mode and window
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Camera Orbit Demo");
    
    // Register callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeListener);
    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    
    // Initialize OpenGL settings
    initGL();
    
    // Enter the GLUT event loop
    glutMainLoop();
    
    return 0;
}
