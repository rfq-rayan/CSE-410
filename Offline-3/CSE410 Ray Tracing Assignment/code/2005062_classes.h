#ifndef CLASSES_H
#define CLASSES_H

#include <vector>
#include <cmath>
#include <GL/glut.h>
#include "bitmap_image.hpp"
#include "stb_image.h"
// Forward declarations
class Object;
class PointLight;
class SpotLight;
class Ray;

// Global vectors (declare extern here, define in main.cpp)
extern std::vector<Object*> objects;
extern std::vector<PointLight> pointLights;
extern std::vector<SpotLight> spotLights;
extern int recursionLevel;
extern int imageWidth, imageHeight;

// 3D Vector class (salvage from Assignment 1)
class Vector3D {
public:
    double x, y, z;
    
    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}
    
    // Vector operations
    Vector3D operator+(const Vector3D& v) const { return Vector3D(x + v.x, y + v.y, z + v.z); }
    Vector3D operator-(const Vector3D& v) const { return Vector3D(x - v.x, y - v.y, z - v.z); }
    Vector3D operator*(double scalar) const { return Vector3D(x * scalar, y * scalar, z * scalar); }
    
    double dot(const Vector3D& v) const { return x * v.x + y * v.y + z * v.z; }
    Vector3D cross(const Vector3D& v) const { 
        return Vector3D(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); 
    }
    
    double length() const { return sqrt(x*x + y*y + z*z); }
    Vector3D normalize() const { 
        double len = length(); 
        return len > 0 ? Vector3D(x/len, y/len, z/len) : Vector3D(0, 0, 0); 
    }
};

// Ray class for ray tracing
class Ray {
public:
    Vector3D start;
    Vector3D dir; // should be normalized
    
    Ray() {}
    Ray(Vector3D start, Vector3D dir) : start(start), dir(dir.normalize()) {}
};

// Base Object class
class Object {
public:
    Vector3D reference_point;
    double height, width, length;
    double color[3];
    double coEfficients[4]; // ambient, diffuse, specular, reflection
    int shine;
    
    Object() {
        height = width = length = 0;
        color[0] = color[1] = color[2] = 0;
        coEfficients[0] = coEfficients[1] = coEfficients[2] = coEfficients[3] = 0;
        shine = 0;
    }
    
    virtual void draw() {}
    virtual double intersect(Ray* r, double* color, int level) { return -1.0; }
    virtual Vector3D getNormal(Vector3D point) { return Vector3D(0, 0, 1); }
    virtual Vector3D getColorAt(Vector3D point) { return Vector3D(color[0], color[1], color[2]); }
    
    void setColor(double r, double g, double b) {
        color[0] = r; color[1] = g; color[2] = b;
    }
    
    void setShine(int s) { shine = s; }
    
    void setCoEfficients(double amb, double diff, double spec, double refl) {
        coEfficients[0] = amb; coEfficients[1] = diff; 
        coEfficients[2] = spec; coEfficients[3] = refl;
    }
    
    virtual ~Object() {}
};

// Sphere class
class Sphere : public Object {
public:
    Sphere(Vector3D center, double radius) {
        reference_point = center;
        length = radius; // using length to store radius
    }
    
    void draw() override {
        glPushMatrix();
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);
        glColor3f(color[0], color[1], color[2]);
        glutSolidSphere(length, 24, 24);
        glPopMatrix();
    }
    
    double intersect(Ray* r, double* color, int level) override;
    Vector3D getNormal(Vector3D point) override;
};

// Triangle class
class Triangle : public Object {
public:
    Vector3D a, b, c; // three vertices
    
    Triangle(Vector3D p1, Vector3D p2, Vector3D p3) : a(p1), b(p2), c(p3) {
        reference_point = a;
    }
    
    void draw() override {
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_TRIANGLES);
            glVertex3f(a.x, a.y, a.z);
            glVertex3f(b.x, b.y, b.z);
            glVertex3f(c.x, c.y, c.z);
        glEnd();
    }
    
    double intersect(Ray* r, double* color, int level) override;
    Vector3D getNormal(Vector3D point) override;
};

// General Quadric class
class GeneralQuadric : public Object {
public:
    double A, B, C, D, E, F, G, H, I, J; // coefficients of quadric equation
    Vector3D cube_ref_point;
    double cube_length, cube_width, cube_height;
    
    GeneralQuadric(double coeffs[10], Vector3D ref, double l, double w, double h) {
        A = coeffs[0]; B = coeffs[1]; C = coeffs[2]; D = coeffs[3]; E = coeffs[4];
        F = coeffs[5]; G = coeffs[6]; H = coeffs[7]; I = coeffs[8]; J = coeffs[9];
        cube_ref_point = ref;
        cube_length = l; cube_width = w; cube_height = h;
    }
    
    // void draw() override {} // No OpenGL drawing for general quadrics
    void draw() override {
        glPushMatrix();
        glTranslatef(reference_point.x, reference_point.y, reference_point.z);
        glColor3f(color[0], color[1], color[2]);
        glutSolidSphere(length, 24, 24);
        glPopMatrix();
    }
    
    double intersect(Ray* r, double* color, int level) override;
    Vector3D getNormal(Vector3D point) override;
    bool isWithinBounds(Vector3D point); // Check if point is within bounding box
};

// Floor class
// Floor class with texture support
class Floor : public Object {
public:
    double floorWidth, tileWidth;
    bool useTexture;
    bool texturePerTile; // New: whether to map texture to each tile or entire floor
    unsigned char* textureData;
    int textureWidth, textureHeight, textureChannels;
    
    Floor(double fw, double tw) : floorWidth(fw), tileWidth(tw), useTexture(false), texturePerTile(false),
                                  textureData(nullptr), textureWidth(0), textureHeight(0), textureChannels(0) {
        reference_point = Vector3D(-fw/2, -fw/2, 0);
        length = tw;
    }
    
    ~Floor() {
        if (textureData) {
            stbi_image_free(textureData);
        }
    }
    
    // Load texture from file
    bool loadTexture(const char* filename) {
        if (textureData) {
            stbi_image_free(textureData);
        }
        
        textureData = stbi_load(filename, &textureWidth, &textureHeight, &textureChannels, 0);
        if (textureData) {
            useTexture = true;
            std::cout << "Texture loaded: " << filename << " (" << textureWidth << "x" << textureHeight << ", " << textureChannels << " channels)" << std::endl;
            return true;
        } else {
            std::cout << "Failed to load texture: " << filename << std::endl;
            useTexture = false;
            return false;
        }
    }
    
    // Switch between texture and checkerboard
    void setUseTexture(bool use) { useTexture = use; }
    
    // Switch between texture per tile and texture per floor
    void setTexturePerTile(bool perTile) { texturePerTile = perTile; }
    
    // Sample texture color at (u,v) coordinates
    Vector3D sampleTexture(double u, double v) {
        if (!textureData || textureWidth <= 0 || textureHeight <= 0) {
            return Vector3D(0.5, 0.5, 0.5); // Gray fallback
        }
        
        // Clamp u and v to [0,1]
        u = std::max(0.0, std::min(1.0, u));
        v = std::max(0.0, std::min(1.0, v));
        
        // Normalized -> pixel coords
        int pixel_x = (int)(u * (textureWidth - 1));
        int pixel_y = (int)((1.0 - v) * (textureHeight - 1)); // Flip Y
        
        // Safety clamp
        pixel_x = std::max(0, std::min(textureWidth - 1, pixel_x));
        pixel_y = std::max(0, std::min(textureHeight - 1, pixel_y));
        
        // Compute array index
        int index = (pixel_y * textureWidth + pixel_x) * textureChannels;
        int max_index = textureWidth * textureHeight * textureChannels;
        if (index < 0 || index + 2 >= max_index) {
            return Vector3D(1.0, 0.0, 1.0); // Magenta = error
        }
        
        Vector3D color;
        color.x = textureData[index] / 255.0; // r
        
        if (textureChannels >= 2) {
            color.y = textureData[index + 1] / 255.0; // g
        } else {
            color.y = color.x; // Grayscale
        }
        
        if (textureChannels >= 3) {
            color.z = textureData[index + 2] / 255.0; // b
        } else {
            color.z = color.x; // Grayscale
        }
        
        return color;
    }
    
    void draw() override;
    double intersect(Ray* r, double* color, int level) override;
    Vector3D getNormal(Vector3D point) override { return Vector3D(0, 0, 1); }
    Vector3D getColorAt(Vector3D point) override;
};

// Point Light class
class PointLight {
public:
    Vector3D light_pos;
    double color[3];
    
    PointLight() {
        color[0] = color[1] = color[2] = 1.0;
    }
    
    PointLight(Vector3D pos, double r, double g, double b) : light_pos(pos) {
        color[0] = r; color[1] = g; color[2] = b;
    }
    
    void draw() {
        glPointSize(5);
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_POINTS);
            glVertex3f(light_pos.x, light_pos.y, light_pos.z);
        glEnd();
    }
};

// Spot Light class
class SpotLight {
public:
    PointLight point_light;
    Vector3D light_direction;
    double cutoff_angle;
    
    SpotLight(PointLight pl, Vector3D dir, double angle) 
        : point_light(pl), light_direction(dir.normalize()), cutoff_angle(angle) {}
    
    void draw() {
        point_light.draw();
    }
};

// Function declarations
void loadData();
void capture();
double clamp(double value, double min_val, double max_val);

#endif // CLASSES_H
