// filepath: d:\4-1\410\Offline 2\Offline 2\code.cpp
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stack>
#include <vector>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include "bmp_image_codes\bitmap_image.hpp"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

// ========================================
// DATA STRUCTURES
// ========================================

// Homogeneous 3D point
struct Point {
    double x, y, z, w;
    Point(double x = 0, double y = 0, double z = 0, double w = 1) : x(x), y(y), z(z), w(w) {}
};

// 4x4 transformation matrix
struct Matrix {
    double m[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };
};


struct Triangle {
    Point points[3];
    int color[3];  // RGB (0-255)
};

// ========================================
// GLOBAL VARIABLES
// ========================================

stack<Matrix> matrixStack; // Matrix stack for push/pop

// ========================================
// BASIC UTILITY FUNCTIONS - VECTOR OPERATIONS
// ========================================

// Cross product of two 3D vectors
Point cross(const Point& a, const Point& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
        0
    };
}

// Normalize a 3D vector
Point normalize(const Point& p) {
    double len = sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
    if (len == 0) return {0, 0, 0, 0};
    return { p.x / len, p.y / len, p.z / len, 0 };
}

// Rodrigues' rotation formula (vector part)
Point rotateVector(const Point& v, const Point& axis, double angle) {
    double rad = angle * M_PI / 180.0;
    double cosA = cos(rad);
    double sinA = sin(rad);

    // v_rot = v*cosA + (axis × v)*sinA + axis*(axis·v)*(1-cosA)
    Point cross = {
        axis.y * v.z - axis.z * v.y,
        axis.z * v.x - axis.x * v.z,
        axis.x * v.y - axis.y * v.x,
        0
    };
    double dot = axis.x * v.x + axis.y * v.y + axis.z * v.z;

    Point result;    
    result.x = v.x * cosA + cross.x * sinA + axis.x * dot * (1 - cosA);
    result.y = v.y * cosA + cross.y * sinA + axis.y * dot * (1 - cosA);
    result.z = v.z * cosA + cross.z * sinA + axis.z * dot * (1 - cosA);
    result.w = v.w;
    return result;
}

// ========================================
// VALIDATION FUNCTIONS
// ========================================

// Check if look direction is parallel to up direction
bool validateViewParameters(const Point& eye, const Point& look, const Point& up) {
    // Compute look direction
    Point lookDir = { look.x - eye.x, look.y - eye.y, look.z - eye.z, 0 };
    lookDir = normalize(lookDir);
    Point upNorm = normalize(up);
    
    // Check if vectors are parallel using cross product
    Point crossProd = cross(lookDir, upNorm);
    double crossMag = sqrt(crossProd.x * crossProd.x + crossProd.y * crossProd.y + crossProd.z * crossProd.z);
    
    if (crossMag < 1e-6) {
        cerr << "Error: Look direction is parallel to up direction!" << endl;
        cerr << "Look direction: (" << lookDir.x << ", " << lookDir.y << ", " << lookDir.z << ")" << endl;
        cerr << "Up direction: (" << upNorm.x << ", " << upNorm.y << ", " << upNorm.z << ")" << endl;
        return false;
    }
    return true;
}

// Validate triangles after stage2 (view transformation)
bool validateTrianglesInViewSpace(const string& stage2File, double nearPlane) {
    ifstream in(stage2File);
    if (!in.is_open()) {
        cerr << "Error: Cannot open " << stage2File << " for validation" << endl;
        return false;
    }
    
    Point p1, p2, p3;
    int triangleCount = 0;
    bool allValid = true;
    
    while (in >> p1.x >> p1.y >> p1.z >> p2.x >> p2.y >> p2.z >> p3.x >> p3.y >> p3.z) {
        triangleCount++;
        
        // Check if any vertex is behind near plane
        // In view space, camera looks down -Z axis, so near plane is at z = -nearPlane
        if (p1.z > -nearPlane || p2.z > -nearPlane || p3.z > -nearPlane) {
            cerr << "Warning: Triangle " << triangleCount << " has vertices behind near plane!" << endl;
            cerr << "  Vertex 1 Z: " << p1.z << " (near plane at Z = " << -nearPlane << ")" << endl;
            cerr << "  Vertex 2 Z: " << p2.z << " (near plane at Z = " << -nearPlane << ")" << endl;
            cerr << "  Vertex 3 Z: " << p3.z << " (near plane at Z = " << -nearPlane << ")" << endl;
            allValid = false;
        }
    }
    
    in.close();
    
    // if (allValid) {
    //     cout << "✓ All " << triangleCount << " triangles are in front of near plane" << endl;
    // }
    
    return allValid;
}

// ========================================
// MATRIX OPERATIONS
// ========================================

// Matrix multiplication: A * B
Matrix multiply(const Matrix& a, const Matrix& b) {
    Matrix result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return result;
}

// Transform a point using a matrix
Point transformPoint(const Matrix& mat, const Point& p) {
    Point result;
    result.x = mat.m[0][0] * p.x + mat.m[0][1] * p.y + mat.m[0][2] * p.z + mat.m[0][3] * p.w;
    result.y = mat.m[1][0] * p.x + mat.m[1][1] * p.y + mat.m[1][2] * p.z + mat.m[1][3] * p.w;
    result.z = mat.m[2][0] * p.x + mat.m[2][1] * p.y + mat.m[2][2] * p.z + mat.m[2][3] * p.w;
    result.w = mat.m[3][0] * p.x + mat.m[3][1] * p.y + mat.m[3][2] * p.z + mat.m[3][3] * p.w;

    // Homogenize if w != 1
    if (result.w != 1.0 && result.w != 0.0) {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
        result.w = 1.0;
    }
    return result;
}

// ========================================
// TRANSFORMATION MATRIX GENERATORS
// ========================================

// Generate translation matrix
Matrix translateMatrix(double tx, double ty, double tz) {
    Matrix mat;
    mat.m[0][3] = tx;
    mat.m[1][3] = ty;
    mat.m[2][3] = tz;
    return mat;
}

// Generate scaling matrix
Matrix scaleMatrix(double sx, double sy, double sz) {
    Matrix mat;
    mat.m[0][0] = sx;
    mat.m[1][1] = sy;
    mat.m[2][2] = sz;
    return mat;
}

// Generate rotation matrix
Matrix rotateMatrix(double angle, double ax, double ay, double az) {
    Point axis = normalize({ax, ay, az, 0});

    Point i = {1, 0, 0, 0};
    Point j = {0, 1, 0, 0};
    Point k = {0, 0, 1, 0};

    Point c1 = rotateVector(i, axis, angle);
    Point c2 = rotateVector(j, axis, angle);
    Point c3 = rotateVector(k, axis, angle);

    Matrix mat;
    mat.m[0][0] = c1.x; mat.m[0][1] = c2.x; mat.m[0][2] = c3.x;
    mat.m[1][0] = c1.y; mat.m[1][1] = c2.y; mat.m[1][2] = c3.y;
    mat.m[2][0] = c1.z; mat.m[2][1] = c2.z; mat.m[2][2] = c3.z;
    return mat;
}

// Compute view matrix V = R * T
Matrix computeViewMatrix(const Point& eye, const Point& look, const Point& up) {
    // Step 1: Compute l, r, u
    Point l = { look.x - eye.x, look.y - eye.y, look.z - eye.z, 0 };
    l = normalize(l);
    Point r = cross(l, up);
    r = normalize(r);
    Point u = cross(r, l);

    // Step 2: Translation matrix (move eye to origin)
    Matrix T;
    T.m[0][3] = -eye.x;
    T.m[1][3] = -eye.y;
    T.m[2][3] = -eye.z;

    // Step 3: Rotation matrix (align axes)
    Matrix R;
    R.m[0][0] = r.x; R.m[0][1] = r.y; R.m[0][2] = r.z; R.m[0][3] = 0;
    R.m[1][0] = u.x; R.m[1][1] = u.y; R.m[1][2] = u.z; R.m[1][3] = 0;
    R.m[2][0] = -l.x; R.m[2][1] = -l.y; R.m[2][2] = -l.z; R.m[2][3] = 0;
    R.m[3][0] = 0; R.m[3][1] = 0; R.m[3][2] = 0; R.m[3][3] = 1;

    // V = R * T
    return multiply(R, T);
}

// Compute projection matrix //detail bujhinaa :)
Matrix computeProjectionMatrix(double fovY, double aspect, double near, double far) {
    double fovX = fovY * aspect;
    double t = near * tan(fovY * M_PI / 360.0);  // tan(fovY/2)
    double r = near * tan(fovX * M_PI / 360.0);  // tan(fovX/2)

    Matrix P;
    P.m[0][0] = near / r;
    P.m[1][1] = near / t;
    P.m[2][2] = -(far + near) / (far - near);
    P.m[2][3] = -2 * far * near / (far - near);
    P.m[3][2] = -1;
    P.m[3][3] = 0;
    return P;
}

// ========================================
// FILE I/O FUNCTIONS
// ========================================

// Read config.txt
/** 
1. The first Line of file contains two integers, representing Screen_Width and Screen_Height
respectively.
2. The second line contains a -ve number which specifies the left limit of X. You can get the right
limit of X by negating this value.
3. The third line contains a -ve number which specifies the bottom limit of Y. You can get the top
limit of Y by negating this value.
4. The fourth line contains two real numbers denoting front and rear limits of Z respectively
*/

void readConfig(const string& filename, int& width, int& height, 
               double& left, double& right, double& bottom, double& top, 
               double& front, double& rear) {
    ifstream in(filename);
    in >> width >> height;
    in >> left; right = -left;
    in >> bottom; top = -bottom;
    in >> front >> rear;
    in.close();
}

// Read stage3.txt and assign random colors
vector<Triangle> readTriangles(const string& filename) {
    ifstream in(filename);
    vector<Triangle> triangles;
    Point p1, p2, p3;
    
    // Fixed seed
    srand(1);
    
    while (in >> p1.x >> p1.y >> p1.z >> p2.x >> p2.y >> p2.z >> p3.x >> p3.y >> p3.z) {
        Triangle tri;
        tri.points[0] = p1; tri.points[1] = p2; tri.points[2] = p3;
        // Assign random color
        tri.color[0] = rand() % 256;
        tri.color[1] = rand() % 256;
        tri.color[2] = rand() % 256;
        triangles.push_back(tri);
    }
    in.close();
    return triangles;
}

// ========================================
// RASTERIZATION FUNCTIONS
// ========================================

// Check if point (x, y) is inside triangle using barycentric coordinates
bool pointInTriangle(double x, double y, const Triangle& tri, double& z) {
    Point A = tri.points[0], B = tri.points[1], C = tri.points[2];
    double denom = (B.y - C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y);
    if (abs(denom) < 1e-10) return false;  // Degenerate triangle with tolerance

    double alpha = ((B.y - C.y) * (x - C.x) + (C.x - B.x) * (y - C.y)) / denom;
    double beta  = ((C.y - A.y) * (x - C.x) + (A.x - C.x) * (y - C.y)) / denom;
    double gamma = 1 - alpha - beta;

    // Use small epsilon for boundary inclusion
    const double eps = 1e-10;
    if (alpha >= -eps && beta >= -eps && gamma >= -eps) {
        z = alpha * A.z + beta * B.z + gamma * C.z;  // Interpolate Z
        return true;
    }
    return false;
}

// ========================================
// STAGE PROCESSING FUNCTIONS
// ========================================

// Process Stage 1: Modeling Transformation
void processStage1(const string& inputFile, const string& outputFile) {
    ifstream in(inputFile);
    ofstream out(outputFile);

    // Initialize stack with identity matrix
    while (!matrixStack.empty()) matrixStack.pop();
    matrixStack.push(Matrix()); // Identity matrix

    string command;
    while (in >> command) {
        if (command == "triangle") {
            Point p1, p2, p3;
            in >> p1.x >> p1.y >> p1.z;
            in >> p2.x >> p2.y >> p2.z;
            in >> p3.x >> p3.y >> p3.z;

            // Transform points using top of stack
            p1 = transformPoint(matrixStack.top(), p1);
            p2 = transformPoint(matrixStack.top(), p2);
            p3 = transformPoint(matrixStack.top(), p3);

            // Write to stage1.txt with fixed precision
            out << fixed << setprecision(7);
            out << p1.x << " " << p1.y << " " << p1.z << endl;
            out << p2.x << " " << p2.y << " " << p2.z << endl;
            out << p3.x << " " << p3.y << " " << p3.z << endl;
            out << endl; // Separate triangles
        }
        else if (command == "translate") {
            double tx, ty, tz;
            in >> tx >> ty >> tz;
            Matrix T = translateMatrix(tx, ty, tz);
            Matrix newTop = multiply(matrixStack.top(), T);
            matrixStack.pop();
            matrixStack.push(newTop);
        }
        else if (command == "scale") {
            double sx, sy, sz;
            in >> sx >> sy >> sz;
            Matrix S = scaleMatrix(sx, sy, sz);
            Matrix newTop = multiply(matrixStack.top(), S);
            matrixStack.pop();
            matrixStack.push(newTop);
        }
        else if (command == "rotate") {
            double angle, ax, ay, az;
            in >> angle >> ax >> ay >> az;
            Matrix R = rotateMatrix(angle, ax, ay, az);
            Matrix newTop = multiply(matrixStack.top(), R);
            matrixStack.pop();
            matrixStack.push(newTop);
        }
        else if (command == "push") {
            matrixStack.push(matrixStack.top()); // Duplicate top
        }
        else if (command == "pop") {
            if (matrixStack.size() > 1) {
                matrixStack.pop();
            } else {
                cerr << "Warning: Cannot pop the identity matrix." << endl;
            }
        }
        else if (command == "end") {
            break;
        }
    }
    in.close();
    out.close();
}

// Process Stage 2: View Transformation
void processStage2(const string& inputFile, const string& outputFile, 
                   const Point& eye, const Point& look, const Point& up) {
    
    // Validate view parameters before processing
    if (!validateViewParameters(eye, look, up)) {
        throw runtime_error("Invalid view parameters: look direction is parallel to up direction");
    }
    
    ifstream in(inputFile);
    ofstream out(outputFile);

    Matrix V = computeViewMatrix(eye, look, up);

    Point p1, p2, p3;
    while (in >> p1.x >> p1.y >> p1.z >> p2.x >> p2.y >> p2.z >> p3.x >> p3.y >> p3.z) {
        p1.w = p2.w = p3.w = 1.0;  // Homogeneous coordinate

        // Transform points by view matrix
        p1 = transformPoint(V, p1);
        p2 = transformPoint(V, p2);
        p3 = transformPoint(V, p3);

        // Write to stage2.txt
        out << fixed << setprecision(7);
        out << p1.x << " " << p1.y << " " << p1.z << endl;
        out << p2.x << " " << p2.y << " " << p2.z << endl;
        out << p3.x << " " << p3.y << " " << p3.z << endl;
        out << endl;  // Separate triangles
    }

    in.close();
    out.close();
}

// Process Stage 3: Projection Transformation
void processStage3(const string& inputFile, const string& outputFile,
                   double fovY, double aspect, double near, double far) {
    ifstream in(inputFile);
    ofstream out(outputFile);

    Matrix P = computeProjectionMatrix(fovY, aspect, near, far);

    Point p1, p2, p3;
    while (in >> p1.x >> p1.y >> p1.z >> p2.x >> p2.y >> p2.z >> p3.x >> p3.y >> p3.z) {
        p1.w = p2.w = p3.w = 1.0;  // Homogeneous coordinate

        // Transform points by projection matrix
        p1 = transformPoint(P, p1);
        p2 = transformPoint(P, p2);
        p3 = transformPoint(P, p3);

        // Write to stage3.txt (homogenized)
        out << fixed << setprecision(7);
        out << p1.x << " " << p1.y << " " << p1.z << endl;
        out << p2.x << " " << p2.y << " " << p2.z << endl;
        out << p3.x << " " << p3.y << " " << p3.z << endl;
        out << endl;  // Separate triangles
    }

    in.close();
    out.close();
}

// Process Stage 4: Z-Buffer and Rasterization
void processStage4(const string& configFile, const string& inputFile, 
                  const string& zbufferFile, const string& imageFile) {
    // Read config
    int width, height;
    double left, right, bottom, top, front, rear;
    readConfig(configFile, width, height, left, right, bottom, top, front, rear);

    // Read triangles
    vector<Triangle> triangles = readTriangles(inputFile);

    // Initialize Z-buffer and image
    vector<vector<double>> zbuffer(height, vector<double>(width, rear));
    bitmap_image image(width, height);
    image.set_all_channels(0, 0, 0);  // Black background

    // Pixel mapping
    double dx = (right - left) / width;
    double dy = (top - bottom) / height;
    double Top_Y = top - dy / 2;
    double Left_X = left + dx / 2;

    // Rasterize each triangle
    for (const Triangle& tri : triangles) {
        // Find bounding box (clipped to screen)
        double min_x = min({tri.points[0].x, tri.points[1].x, tri.points[2].x});
        double max_x = max({tri.points[0].x, tri.points[1].x, tri.points[2].x});
        double min_y = min({tri.points[0].y, tri.points[1].y, tri.points[2].y});
        double max_y = max({tri.points[0].y, tri.points[1].y, tri.points[2].y});

        // Clip to screen bounds
        min_x = max(min_x, left); max_x = min(max_x, right);
        min_y = max(min_y, bottom); max_y = min(max_y, top);

        // Convert to pixel coordinates with proper bounds checking
        int start_x = max(0, static_cast<int>((min_x - Left_X) / dx));
        int end_x   = min(width - 1, static_cast<int>((max_x - Left_X) / dx));
        int start_y = max(0, static_cast<int>((Top_Y - max_y) / dy));
        int end_y   = min(height - 1, static_cast<int>((Top_Y - min_y) / dy));

        // Scan bounding box
        for (int y = start_y; y <= end_y; y++) {
            for (int x = start_x; x <= end_x; x++) {
                double pixel_x = Left_X + x * dx;
                double pixel_y = Top_Y - y * dy;
                double z;
                if (pointInTriangle(pixel_x, pixel_y, tri, z)) {
                    // Check depth with epsilon tolerance for Z-buffer
                    const double z_eps = 1e-6;
                    if (z >= front && z < (zbuffer[y][x] - z_eps)) {
                        zbuffer[y][x] = z;
                        image.set_pixel(x, y, tri.color[0], tri.color[1], tri.color[2]);
                    }
                }
            }
        }
    }

    // Save outputs
    image.save_image(imageFile);
    ofstream zbout(zbufferFile);
    zbout << fixed << setprecision(6);
    for (int y = 0; y < height; y++) {
        bool firstInRow = true;
        for (int x = 0; x < width; x++) {
            if (zbuffer[y][x] < rear) {  // Only output pixels that were actually rendered
                if (!firstInRow) {
                    zbout << "\t";  // Tab separator between all values
                }
                zbout << zbuffer[y][x];
                firstInRow = false;
            }
        }
        if (!firstInRow) {  // print a tab character at the end of the line
            zbout<< "\t";
        }
        zbout << endl;
    }    zbout.close();
}

// ========================================
// MEMORY CLEANUP FUNCTIONS
// ========================================

// Clean up memory before program exit
void cleanupMemory() {
    // Clear matrix stack
    while (!matrixStack.empty()) {
        matrixStack.pop();
    }
    

    // No dynamic memory allocation in this code, but if there were, we would free it here
    // For example, if we had allocated triangles dynamically:
    // for (Triangle* tri : triangles) {
    //     delete tri;
    // }
    // triangles.clear(); // Clear vector if it was used



}

// ========================================
// MAIN FUNCTION
// ========================================

int main(int argc, char* argv[]) {
    string sceneFile, configFile, outputDir;
    
    // Parse command line arguments
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <scene_file_path> <config_file_path> [output_directory]" << endl;
        cout << "Example: ./code.exe \"Test Cases/1/scene.txt\" \"Test Cases/1/config.txt\" \"output\"" << endl;
        return 1;
    }
    
    sceneFile = argv[1];
    configFile = argv[2];
    outputDir = (argc > 3) ? argv[3] : "output";
    
    // Create output directory if it doesn't exist
    system(("mkdir \"" + outputDir + "\" 2>nul").c_str());
    
    // Read scene file parameters
    ifstream scene(sceneFile);
    if (!scene.is_open()) {
        cerr << "Error: Cannot open scene file: " << sceneFile << endl;
        return 1;
    }
    
    Point eye, look, up;
    double fovY, aspect, near, far;
    scene >> eye.x >> eye.y >> eye.z;
    scene >> look.x >> look.y >> look.z;
    scene >> up.x >> up.y >> up.z;
    scene >> fovY >> aspect >> near >> far;
    scene.close();
    
    // Define output file paths
    string stage1File = outputDir + "/stage1.txt";
    string stage2File = outputDir + "/stage2.txt";
    string stage3File = outputDir + "/stage3.txt";
    string zbufferFile = outputDir + "/z_buffer.txt";
    string imageFile = outputDir + "/out.bmp";
    
    cout << "Processing with:" << endl;
    cout << "  Scene file: " << sceneFile << endl;
    cout << "  Config file: " << configFile << endl;
    cout << "  Output directory: " << outputDir << endl;
    cout << "----------------------------------------" << endl;

    // Process all stages
    try {
        // Stage 1: Modeling Transformation
        processStage1(sceneFile, stage1File);
        cout << "Stage 1 completed -> " << stage1File << endl;
        
        // Stage 2: View Transformation
        processStage2(stage1File, stage2File, eye, look, up);
        cout << "Stage 2 completed -> " << stage2File << endl;
        
        // Validate triangles are in front of near plane
        // cout << "Validating triangles are in front of near plane..." << endl;
        if (!validateTrianglesInViewSpace(stage2File, near)) {
            cerr << "Warning: Some triangles are behind the near plane after view transformation!" << endl;
            // Continue processing but warn user
        }
        
        // Stage 3: Projection Transformation
        processStage3(stage2File, stage3File, fovY, aspect, near, far);
        cout << "Stage 3 completed -> " << stage3File << endl;
        
        // Stage 4: Z-Buffer and Rasterization
        processStage4(configFile, stage3File, zbufferFile, imageFile);
        cout << "Stage 4 completed -> " << zbufferFile << ", " << imageFile << endl;
        
        cout << "----------------------------------------" << endl;
        cout << "All stages completed successfully!" << endl;
        
        // Clean up memory before exit
        cleanupMemory();
        
    } catch (const exception& e) {
        cerr << "Error during processing: " << e.what() << endl;
        cleanupMemory(); // Clean up on error too
        return 1;
    }

    return 0;
}
