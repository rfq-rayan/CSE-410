#include <iostream>
#include <fstream>
#include <sstream>
#include "stb_image.h"
#include "2005062_classes.h"
using namespace std;

// Global variables
vector<Object*> objects;
vector<PointLight> pointLights;
vector<SpotLight> spotLights;
string sceneFile = "scene.txt";

// Camera variables
Vector3D eye(100, 100, 100);
Vector3D look(-1, -1, -1);
Vector3D up(0, 0, 1);
Vector3D rightV;

// Ray tracing parameters
int recursionLevel = 5;
int imageWidth = 768, imageHeight = 768;
double viewAngle = 80.0;
int windowWidth = 500, windowHeight = 500;

// Function to initialize camera vectors
void updateCameraVectors() {
    look = look.normalize();
    rightV = look.cross(up).normalize();
    up = rightV.cross(look).normalize();
}

// Utility function
double clamp(double value, double min_val, double max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}

// Load scene data
void loadData() {
    ifstream file(sceneFile);
    if (!file.is_open()) {
        cout << "Error: Cannot open " << sceneFile << endl;
        return;
    }
    
    // Clear existing data
    objects.clear();
    pointLights.clear();
    spotLights.clear();
    
    // Read recursion level and image dimensions
    file >> recursionLevel;
    file >> imageWidth;
    imageHeight = imageWidth;
    
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
    floor->setColor(1, 1, 1);
    floor->setCoEfficients(0.4, 0.2, 0.2, 0.2);
    floor->setShine(1);
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
    
    file.close();
    
    int sp = 0, t = 0, q = 0;
    for (auto obj : objects) {
        if (dynamic_cast<Sphere*>(obj)) sp++;
        else if (dynamic_cast<Triangle*>(obj)) t++;
        else if (dynamic_cast<GeneralQuadric*>(obj)) q++;
    }
    
    cout << "Loaded " << sp << " spheres, " << t << " triangles, " << q << " quadrics, "
         << pointLights.size() << " point lights, " << spotLights.size() << " spotlights" << endl;
}

// Progress bar function
void showProgress(int percentage) {
    printf("\r%d%%", percentage);
    fflush(stdout);
}

// Capture function for ray tracing
void capture(string outputFile = "") {
    bitmap_image image(imageWidth, imageHeight);
    
    // Set background color
    for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageHeight; j++) {
            image.set_pixel(i, j, 0, 0, 0);
        }
    }
    
    updateCameraVectors();
    
    // Calculate plane distance and setup
    double planeDistance = (windowHeight / 2.0) / tan((viewAngle * M_PI / 180.0) / 2.0);
    Vector3D topleft = eye + look * planeDistance - rightV * (windowWidth / 2.0) + up * (windowHeight / 2.0);
    
    double du = (double)windowWidth / imageWidth;
    double dv = (double)windowHeight / imageHeight;
    
    topleft = topleft + rightV * (0.5 * du) - up * (0.5 * dv);
    
    // Ray tracing loop
    for (int i = 0; i < imageWidth; i++) {
        for (int j = 0; j < imageHeight; j++) {
            Vector3D curPixel = topleft + rightV * (i * du) - up * (j * dv);
            Vector3D rayDir = (curPixel - eye).normalize();
            
            Ray ray(eye, rayDir);
            
            double tMin = -1;
            int nearest = -1;
            
            for (int k = 0; k < objects.size(); k++) {
                double t = objects[k]->intersect(&ray, nullptr, 0);
                if (t > 0 && (tMin < 0 || t < tMin)) {
                    tMin = t;
                    nearest = k;
                }
            }
            
            if (nearest != -1) {
                double color[3] = {0.0, 0.0, 0.0};
                objects[nearest]->intersect(&ray, color, 1);
                
                int r = (int)(clamp(color[0], 0.0, 1.0) * 255);
                int g = (int)(clamp(color[1], 0.0, 1.0) * 255);
                int b = (int)(clamp(color[2], 0.0, 1.0) * 255);
                
                image.set_pixel(i, j, r, g, b);
            }
        }
        
        if (i % 50 == 0) {
            showProgress(i * 100 / imageWidth);
        }
    }
    
    showProgress(100);
    
    // Save image
    if (outputFile.empty()) {
        static int imageCount = 1;
        outputFile = "Output_" + to_string(imageCount) + ".bmp";
        imageCount++;
    }
    
    image.save_image(outputFile);
    cout << "\nImage saved as " << outputFile << endl;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <scene_file> [output_file]" << endl;
        cout << "Example: " << argv[0] << " scene.txt output.bmp" << endl;
        return 1;
    }
    
    sceneFile = argv[1];
    string outputFile = "";
    
    if (argc > 2) {
        outputFile = argv[2];
    }
    
    cout << "Loading scene: " << sceneFile << endl;
    loadData();
    
    cout << "Starting ray tracing..." << endl;
    capture(outputFile);
    
    // Clean up
    for (auto obj : objects) {
        delete obj;
    }
    
    return 0;
}
