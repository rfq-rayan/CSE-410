#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glut.h>
#include "stb_image.h"
#include "2005062_classes.h"
using namespace std;
// Global variables
vector<Object*> objects;
vector<PointLight> pointLights;
vector<SpotLight> spotLights;

// Camera variables (salvage from Assignment 1)
Vector3D eye(100, 100, 100);    // camera position
Vector3D look(-1, -1, -1);      // look direction
Vector3D up(0, 0, 1);           // up direction
Vector3D rightV;                 // rightV direction (calculated)

// Ray tracing parameters
int recursionLevel = 5;
int imageWidth = 4096, imageHeight = 4096;  // Increased from 768x768
double viewAngle = 80.0; // in degrees
int windowWidth = 500, windowHeight = 500;

// Function to initialize camera vectors
void updateCameraVectors() {
    look = look.normalize();
    rightV = look.cross(up).normalize();
    up = rightV.cross(look).normalize();
}

// Camera control functions (salvage from Assignment 1)
void moveForward() {
    eye = eye + look;
    glutPostRedisplay();
}

void moveBackward() {
    eye = eye - look;
    glutPostRedisplay();
}

void moveLeft() {
    eye = eye - rightV;
    glutPostRedisplay();
}

void moveRight() {
    eye = eye + rightV;
    glutPostRedisplay();
}

void moveUp() {
    eye = eye + up;
    glutPostRedisplay();
}

void moveDown() {
    eye = eye - up;
    glutPostRedisplay();
}

void lookLeft() {
    look = look * cos(0.05) + rightV * sin(0.05);
    updateCameraVectors();
    glutPostRedisplay();
}

void lookRightV() {
    look = look * cos(-0.05) + rightV * sin(-0.05);
    updateCameraVectors();
    glutPostRedisplay();
}

void lookUp() {
    look = look * cos(0.05) + up * sin(0.05);
    updateCameraVectors();
    glutPostRedisplay();
}

void lookDown() {
    look = look * cos(-0.05) + up * sin(-0.05);
    updateCameraVectors();
    glutPostRedisplay();
}

void tiltClockwise() {
    up = up * cos(-0.05) + rightV * sin(-0.05);
    updateCameraVectors();
    glutPostRedisplay();
}

void tiltCounterclockwise() {
    up = up * cos(0.05) + rightV * sin(0.05);
    updateCameraVectors();
    glutPostRedisplay();
}

// Utility function
double clamp(double value, double min_val, double max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

// Global variable for scene file
string sceneFile = "scene.txt"; // Default scene file

// Load scene data
void loadData() {
    ifstream file(sceneFile);
    if (!file.is_open()) {
        cout << "Error: Cannot open " << sceneFile << endl;
        return;
    }
    
    // Read recursion level and image dimensions
    file >> recursionLevel;
    file >> imageWidth;
    imageHeight = imageWidth; // square image
    
    // Read number of objects
    int numObjects;
    file >> numObjects;
    
    string objectType;
    for (int i = 0; i < numObjects; i++) {
        file >> objectType;
        
        if (objectType == "sphere") {

            double cx, cy, cz, radius;
            double r, g, b;
            double amb, diff, spec, refl;
            int shine;
            
            file >> cx >> cy >> cz >> radius;
            file >> r >> g >> b;
            file >> amb >> diff >> spec >> refl;
            file >> shine;
            
            Sphere* sphere = new Sphere(Vector3D(cx, cy, cz), radius);
            sphere->setColor(r, g, b);
            sphere->setCoEfficients(amb, diff, spec, refl);
            sphere->setShine(shine);
            objects.push_back(sphere);
        }
        else if (objectType == "triangle") {
            double x1, y1, z1, x2, y2, z2, x3, y3, z3;
            double r, g, b;
            double amb, diff, spec, refl;
            int shine;
            
            file >> x1 >> y1 >> z1;
            file >> x2 >> y2 >> z2;
            file >> x3 >> y3 >> z3;
            file >> r >> g >> b;
            file >> amb >> diff >> spec >> refl;
            file >> shine;
            
            Triangle* triangle = new Triangle(Vector3D(x1, y1, z1), 
                                            Vector3D(x2, y2, z2), 
                                            Vector3D(x3, y3, z3));
            triangle->setColor(r, g, b);
            triangle->setCoEfficients(amb, diff, spec, refl);
            triangle->setShine(shine);
            objects.push_back(triangle);
        }
        else if (objectType == "general") {
            double coeffs[10];
            double ref_x, ref_y, ref_z, length, width, height;
            double r, g, b;
            double amb, diff, spec, refl;
            int shine;
            
            for (int j = 0; j < 10; j++) {
                file >> coeffs[j];
            }
            file >> ref_x >> ref_y >> ref_z >> length >> width >> height;
            file >> r >> g >> b;
            file >> amb >> diff >> spec >> refl;
            file >> shine;
            
            GeneralQuadric* quad = new GeneralQuadric(coeffs, Vector3D(ref_x, ref_y, ref_z), 
                                                     length, width, height);
            quad->setColor(r, g, b);
            quad->setCoEfficients(amb, diff, spec, refl);
            quad->setShine(shine);
            objects.push_back(quad);
        }
    }
    
    // Add floor
    Floor* floor = new Floor(1000, 20);
    floor->setColor(1, 1, 1); // will be overridden by checkerboard pattern or texture
    floor->setCoEfficients(0.4, 0.2, 0.2, 0.2);
    floor->setShine(1);
    
    // Try to load texture for floor - you can change this filename
    // Available sample textures: sample_texture.bmp, sample_texture2.bmp, sample.bmp
    if (floor->loadTexture("PowerToys_Paste_20250626080047.png")) {
        cout << "Floor texture loaded successfully. Press 'T' to toggle between texture and checkerboard." << endl;
    } else {
        cout << "Failed to load floor texture, using checkerboard pattern." << endl;
    }
    
    objects.push_back(floor);
    
    // Read point lights
    int numPointLights;
    file >> numPointLights;
    
    for (int i = 0; i < numPointLights; i++) {
        double px, py, pz, r, g, b;
        file >> px >> py >> pz;
        file >> r >> g >> b;
        
        pointLights.push_back(PointLight(Vector3D(px, py, pz), r, g, b));
    }
    
    // Read spotlights
    int numSpotLights;
    file >> numSpotLights;
    
    for (int i = 0; i < numSpotLights; i++) {
        double px, py, pz, r, g, b;
        double dx, dy, dz;
        double cutoff;
        
        file >> px >> py >> pz;
        file >> r >> g >> b;
        file >> dx >> dy >> dz;
        file >> cutoff;
        
        PointLight pl(Vector3D(px, py, pz), r, g, b);
        spotLights.push_back(SpotLight(pl, Vector3D(dx, dy, dz), cutoff));
    }
    int sp, t, q = 0; // counters for  sphere, triangle, and quadric objects
    for (auto obj : objects) {
        if (dynamic_cast<Sphere*>(obj)) {
            sp++;
        } else if (dynamic_cast<Triangle*>(obj)) {
            t++;
        } else if (dynamic_cast<GeneralQuadric*>(obj)) {
            q++;
        }
        
    }
    // for all the objects;  
    file.close();
    // cout << "Loaded " << objects.size() << " objects, " 
            //   << pointLights.size() << " point lights, " 
            //   << spotLights.size() << " spotlights" << endl;
    cout << "Loaded " << sp << " spheres, " 
         << t << " triangles, " 
         << q << " quadrics, "
         << pointLights.size() << " point lights, " 
         << spotLights.size() << " spotlights" << endl;
}

// Progress bar function
void showProgress(int percentage) {
    // Clear the current lin
    printf(" \r%d%%", percentage);
    fflush(stdout);
}


// Capture function for ray tracing (basic structure)
void capture() {
    // cout << "Starting ray tracing..." << endl;
    
    bitmap_image image(imageWidth, imageHeight);
    
    // Set background color

    for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageHeight; j++) {
            image.set_pixel(i, j, 0, 0, 0); // black background
        }
    }
    

    // Calculate plane distance and setup
    double planeDistance = (windowHeight / 2.0) / tan((viewAngle * M_PI / 180.0) / 2.0);
    Vector3D topleft = eye + look * planeDistance - rightV * (windowWidth / 2.0) + up * (windowHeight / 2.0);
    
    double du = (double)windowWidth / imageWidth;
    double dv = (double)windowHeight / imageHeight;
    
    topleft = topleft + rightV * (0.5 * du) - up * (0.5 * dv);
    
    // Ray tracing loop
    for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageHeight; j++) {
            // Calculate current pixel position
            Vector3D curPixel = topleft + rightV * (i * du) - up * (j * dv);
            Vector3D rayDir = (curPixel - eye).normalize();
            
            Ray ray(eye, rayDir);
            
            // Find nearest intersection
            double tMin = -1;
            int nearest = -1;
            
            for (int k = 0; k < objects.size(); k++) {
                double t = objects[k]->intersect(&ray, nullptr, 0);
                if (t > 0 && (tMin < 0 || t < tMin)) {
                    tMin = t;
                    nearest = k;
                }
            }
            
            // Color the pixel
            if (nearest != -1) {
                double color[3] = {0.0, 0.0, 0.0};
                objects[nearest]->intersect(&ray, color, 1);
                
                // Clamp colors to [0,1] and convert to [0,255]
                int r = (int)(clamp(color[0], 0.0, 1.0) * 255);
                int g = (int)(clamp(color[1], 0.0, 1.0) * 255);
                int b = (int)(clamp(color[2], 0.0, 1.0) * 255);
                
                image.set_pixel(i, j, r, g, b);
            }
        }
        
        // Progress indicator
        if (i % 50 == 0) {
            showProgress(i * 100 / imageWidth);
        }
    }
    
    // Ensure 100% is shown at completion
    showProgress(100);
    
    // Save image
    static int imageCount = 1;
    // find how many images are in the directory
    string filename = "Output_" + to_string(imageCount) + ".bmp";
    image.save_image(filename);
    imageCount++;
    
    // printf("\nRay tracing completed!\n");
    cout << "\nImage saved as " << filename << endl;
}

// OpenGL display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Set camera
    gluLookAt(eye.x, eye.y, eye.z,
              eye.x + look.x, eye.y + look.y, eye.z + look.z,
              up.x, up.y, up.z);
    
    // Draw all objects
    for (auto obj : objects) {
        obj->draw();
    }
    
    // Draw lights
    for (auto light : pointLights) {
        light.draw();
    }
    
    for (auto light : spotLights) {
        light.draw();
    }
    
    glutSwapBuffers();
}

// Keyboard handler
void keyboardHandler(unsigned char key, int x, int y) {
    switch (key) {
        case '0': capture(); break;
        case '1': lookLeft(); break;
        case '2': lookRightV(); break;
        case '3': lookUp(); break;
        case '4': lookDown(); break;
        case '5': tiltCounterclockwise(); break;
        case '6': tiltClockwise(); break;
        case 't':
        case 'T': {
            // Toggle texture on floor
            for (auto obj : objects) {
                Floor* floor = dynamic_cast<Floor*>(obj);
                if (floor) {
                    floor->setUseTexture(!floor->useTexture);
                    cout << "Floor texture " << (floor->useTexture ? "enabled" : "disabled") << endl;
                    glutPostRedisplay();
                    break;
                }
            }
            break;
        }
        case 'y':
        case 'Y': {
            // Toggle texture mapping mode (per tile vs per floor)
            for (auto obj : objects) {
                Floor* floor = dynamic_cast<Floor*>(obj);
                if (floor) {
                    floor->setTexturePerTile(!floor->texturePerTile);
                    cout << "Texture mapping: " << (floor->texturePerTile ? "per tile" : "entire floor") << endl;
                    glutPostRedisplay();
                    break;
                }
            }
            break;
        }
        case 'v':
        case 'V': {
            // Set image quality to 768x768
            imageWidth = imageHeight = 768;
            cout << "Image resolution set to " << imageWidth << "x" << imageHeight << endl;
            break;
        }
        case 'b':
        case 'B': {
            // Set image quality to 1024x1024
            imageWidth = imageHeight = 1024;
            cout << "Image resolution set to " << imageWidth << "x" << imageHeight << endl;
            break;
        }
        case 'n':
        case 'N': {
            // Set image quality to 1536x1536
            imageWidth = imageHeight = 1536;
            cout << "Image resolution set to " << imageWidth << "x" << imageHeight << endl;
            break;
        }
        case 'm':
        case 'M': {
            // Set image quality to 2048x2048
            imageWidth = imageHeight = 2048;
            cout << "Image resolution set to " << imageWidth << "x" << imageHeight << endl;
            break;
        }
        case 27: exit(0); break; // ESC key
    }
}

// Special keys handler
void specialKeyHandler(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP: moveForward(); break;
        case GLUT_KEY_DOWN: moveBackward(); break;
        case GLUT_KEY_LEFT: moveLeft(); break;
        case GLUT_KEY_RIGHT: moveRight(); break;
        case GLUT_KEY_PAGE_UP: moveUp(); break;
        case GLUT_KEY_PAGE_DOWN: moveDown(); break;
    }
}

// Initialize OpenGL
void init() {
    glClearColor(0, 0, 0, 0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(viewAngle, 1, 1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    
    updateCameraVectors();
}

int main(int argc, char** argv) {
    // Check for command line argument for scene file
    if (argc > 1) {
        sceneFile = argv[1];
        cout << "Using scene file: " << sceneFile << endl;
    } else {
        cout << "Using default scene file: " << sceneFile << endl;
    }
    
    glutInit(&argc, argv);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutCreateWindow("Offline 3 - Ray Tracing ");
    
    init();
    loadData();
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardHandler);
    glutSpecialFunc(specialKeyHandler);
    
    glutMainLoop();
    
    // Clean up
    for (auto obj : objects) {
        delete obj;
    }
    
    return 0;
}
