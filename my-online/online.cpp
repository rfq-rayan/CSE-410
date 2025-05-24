// --- Includes ---
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
using namespace std;
x
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// --- Tic Tac Toe Variables ---
char board[3][3]; // 'X', 'O', or '\0'
bool xTurn = true;
bool gameOver = false;
string gameMessage = "";

// Helper: Draw text at (x, y)
void drawText(const string& text, float x, float y) {
    glRasterPos2f(x, y);
    for (char c : text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

void resetGame() {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            board[i][j] = '\0';
    xTurn = true;
    gameOver = false;
    gameMessage = "";
}

void drawGrid() {
    glColor3f(1, 1, 1);
    glLineWidth(4);
    glBegin(GL_LINES);
    // Vertical lines
    glVertex2f(-0.33f, 1.0f); glVertex2f(-0.33f, -1.0f);
    glVertex2f(0.33f, 1.0f); glVertex2f(0.33f, -1.0f);
    // Horizontal lines
    glVertex2f(-1.0f, 0.33f); glVertex2f(1.0f, 0.33f);
    glVertex2f(-1.0f, -0.33f); glVertex2f(1.0f, -0.33f);
    glEnd();
}

void drawX(float cx, float cy) {
    glColor3f(1, 0, 0);
    glLineWidth(6);
    glBegin(GL_LINES);
    glVertex2f(cx - 0.25f, cy - 0.25f);
    glVertex2f(cx + 0.25f, cy + 0.25f);
    glVertex2f(cx - 0.25f, cy + 0.25f);
    glVertex2f(cx + 0.25f, cy - 0.25f);
    glEnd();
}

void drawO(float cx, float cy) {
    glColor3f(0, 0.7f, 1);
    glLineWidth(6);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / 100.0f;
        float x = 0.25f * cosf(theta);
        float y = 0.25f * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawBoard() {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            float cx = -0.66f + j * 0.66f;
            float cy = 0.66f - i * 0.66f;
            if (board[i][j] == 'X') drawX(cx, cy);
            else if (board[i][j] == 'O') drawO(cx, cy);
        }
    }
}

char checkWinner() {
    // Rows, columns, diagonals
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return board[i][0];
        if (board[0][i] && board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return board[0][i];
    }
    if (board[0][0] && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return board[0][0];
    if (board[0][2] && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return board[0][2];
    return '\0';
}

bool isDraw() {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (!board[i][j]) return false;
    return true;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawGrid();
    drawBoard();
    if (gameOver) {
        drawText(gameMessage, -0.3f, -0.9f);
    } else {
        string turnMsg = xTurn ? "Player X's Turn" : "Player O's Turn";
        drawText(turnMsg, -0.3f, -0.9f);
    }
    glutSwapBuffers();
}

void reshapeListener(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

void keyboardListener(unsigned char key, int x, int y) {
    if (gameOver && key == 'r') {
        resetGame();
        glutPostRedisplay();
        return;
    }
    if (gameOver) return;
    if (key >= '1' && key <= '9') {
        int idx = key - '1';
        int row = idx / 3;
        int col = idx % 3;
        if (!board[row][col]) {
            board[row][col] = xTurn ? 'X' : 'O';
            char winner = checkWinner();
            if (winner) {
                gameOver = true;
                gameMessage = string("Player ") + winner + " Wins! Press r to restart.";
            } else if (isDraw()) {
                gameOver = true;
                gameMessage = "Draw! Press r to restart.";
            } else {
                xTurn = !xTurn;
            }
        }
        glutPostRedisplay();
    }
}

void initGL() {
    glClearColor(0, 0, 0, 1);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tic Tac Toe - CSE 410");
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeListener);
    glutKeyboardFunc(keyboardListener);
    initGL();
    resetGame();
    glutMainLoop();
    return 0;
}
