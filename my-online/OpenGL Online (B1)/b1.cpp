#include <GL/glut.h>
#include <string>
#include <cmath>
#include <sstream>

using namespace std;

// Window dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
string who_win = "";
// Game objects dimensions
const float PADDLE_WIDTH = 10.0f;
const float PADDLE_HEIGHT = 80.0f;
const float BALL_SIZE = 10.0f;
const float PADDLE_SPEED = 5.0f;
const float BALL_SPEED = 4.0f;

// Game state variables
float leftPaddleY = 0.0f;
float rightPaddleY = 0.0f;
float ballX = 0.0f;
float ballY = 0.0f;
float ballVelX = BALL_SPEED;
float ballVelY = BALL_SPEED;
bool isPaused = true;
int leftScore = 0;
int rightScore = 0;

// Function to draw text on screen
void drawText(const string& text, float x, float y) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

// Function to reset the game
void resetGame() {
    leftPaddleY = 0.0f;
    rightPaddleY = 0.0f;
    ballX = 0.0f;
    ballY = 0.0f;
    ballVelX = BALL_SPEED;
    ballVelY = BALL_SPEED;
    isPaused = true;
}

// Function to handle ball collision with paddles
bool checkPaddleCollision() {
    // Left paddle collision
    if (ballX - BALL_SIZE/2 <= -WINDOW_WIDTH/2 + PADDLE_WIDTH &&
        ballX - BALL_SIZE/2 >= -WINDOW_WIDTH/2 &&
        ballY >= leftPaddleY - PADDLE_HEIGHT/2 &&
        ballY <= leftPaddleY + PADDLE_HEIGHT/2) {
        return true;
    }
    // Right paddle collision
    if (ballX + BALL_SIZE/2 >= WINDOW_WIDTH/2 - PADDLE_WIDTH &&
        ballX + BALL_SIZE/2 <= WINDOW_WIDTH/2 &&
        ballY >= rightPaddleY - PADDLE_HEIGHT/2 &&
        ballY <= rightPaddleY + PADDLE_HEIGHT/2) {
        return true;
    }
    return false;
}

void update() {
    if (isPaused) return;

    // Update ball position
    ballX += ballVelX;
    ballY += ballVelY;

    // Ball collision with top and bottom walls
    if (ballY + BALL_SIZE/2 > WINDOW_HEIGHT/2 || ballY - BALL_SIZE/2 < -WINDOW_HEIGHT/2) {
        ballVelY = -ballVelY;
    }

    // Ball collision with paddles
    if (checkPaddleCollision()) {
        ballVelX = -ballVelX;
    }

    // Ball out of bounds
    if (ballX + BALL_SIZE/2 > WINDOW_WIDTH/2) {
        // leftScore++;
        who_win = "Left Player Wins";
        resetGame();
    }
    if (ballX - BALL_SIZE/2 < -WINDOW_WIDTH/2) {
        // rightScore++;
        who_win = "Right Player Wins";
        resetGame();
    }

    glutPostRedisplay();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Draw left paddle
    glPushMatrix();
    glTranslatef(-WINDOW_WIDTH/2 + PADDLE_WIDTH/2, leftPaddleY, 0);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-PADDLE_WIDTH/2, -PADDLE_HEIGHT/2);
    glVertex2f(PADDLE_WIDTH/2, -PADDLE_HEIGHT/2);
    glVertex2f(PADDLE_WIDTH/2, PADDLE_HEIGHT/2);
    glVertex2f(-PADDLE_WIDTH/2, PADDLE_HEIGHT/2);
    glEnd();
    glPopMatrix();

    // Draw right paddle
    glPushMatrix();
    glTranslatef(WINDOW_WIDTH/2 - PADDLE_WIDTH/2, rightPaddleY, 0);
    glBegin(GL_QUADS);
    glVertex2f(-PADDLE_WIDTH/2, -PADDLE_HEIGHT/2);
    glVertex2f(PADDLE_WIDTH/2, -PADDLE_HEIGHT/2);
    glVertex2f(PADDLE_WIDTH/2, PADDLE_HEIGHT/2);
    glVertex2f(-PADDLE_WIDTH/2, PADDLE_HEIGHT/2);
    glEnd();
    glPopMatrix();

    // Draw ball
    glPushMatrix();
    glTranslatef(ballX, ballY, 0);
    glBegin(GL_QUADS);
    glVertex2f(-BALL_SIZE/2, -BALL_SIZE/2);
    glVertex2f(BALL_SIZE/2, -BALL_SIZE/2);
    glVertex2f(BALL_SIZE/2, BALL_SIZE/2);
    glVertex2f(-BALL_SIZE/2, BALL_SIZE/2);
    glEnd();
    glPopMatrix();

    // Draw scores
    stringstream scoreText;
    // scoreText << leftScore << " - " << rightScore;
    // drawText(scoreText.str(), -20, WINDOW_HEIGHT/2 - 30);
    drawText(who_win, -20, WINDOW_HEIGHT/2 - 30);

    // Draw pause text if game is paused
    if (isPaused) {
        drawText("PAUSED", -30, 0);
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'e': // Move left paddle up
            if (leftPaddleY + PADDLE_HEIGHT/2 < WINDOW_HEIGHT/2)
                leftPaddleY += PADDLE_SPEED;
            break;
        case 'b': // Move left paddle down
            if (leftPaddleY - PADDLE_HEIGHT/2 > -WINDOW_HEIGHT/2)
                leftPaddleY -= PADDLE_SPEED;
            break;
        case 'p': // Move right paddle up
            if (rightPaddleY + PADDLE_HEIGHT/2 < WINDOW_HEIGHT/2)
                rightPaddleY += PADDLE_SPEED;
            break;
        case 'n': // Move right paddle down
            if (rightPaddleY - PADDLE_HEIGHT/2 > -WINDOW_HEIGHT/2)
                rightPaddleY -= PADDLE_SPEED;
            break;
        case 't': // Toggle pause
            isPaused = !isPaused;
            break;
        case 'r': // Reset game
            resetGame();
            leftScore = 0;
            rightScore = 0;
            break;
    }
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-WINDOW_WIDTH/2, WINDOW_WIDTH/2, -WINDOW_HEIGHT/2, WINDOW_HEIGHT/2, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void timer(int) {
    update();
    glutTimerFunc(1000/60, timer, 0); // 60 FPS
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Pong Game");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);

    resetGame();
    glutMainLoop();
    return 0;
}