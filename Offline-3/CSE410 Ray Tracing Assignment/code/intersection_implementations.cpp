#include "2005062_classes.h"
#include <cmath>
#include <algorithm>

// Sphere intersection implementation
double Sphere::intersect(Ray* r, double* color, int level) {
    Vector3D oc = r->start - reference_point;
    double a = r->dir.dot(r->dir);
    double b = 2.0 * oc.dot(r->dir);
    double c = oc.dot(oc) - length * length; // length stores radius
    
    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return -1; // no intersection
    
    double t1 = (-b - sqrt(discriminant)) / (2 * a);
    double t2 = (-b + sqrt(discriminant)) / (2 * a);
    
    double t = -1;
    if (t1 > 0) t = t1;
    else if (t2 > 0) t = t2;
    
    if (t < 0 || level == 0) return t;
    
    // Calculate color using Phong lighting
    Vector3D intersectionPoint = r->start + r->dir * t;
    Vector3D normal = getNormal(intersectionPoint);
    Vector3D intersectionColor = getColorAt(intersectionPoint);
    
    // Ambient component
    color[0] = intersectionColor.x * coEfficients[0];
    color[1] = intersectionColor.y * coEfficients[0];
    color[2] = intersectionColor.z * coEfficients[0];
    
    // Process each point light
    for (const auto& light : pointLights) {
        Vector3D lightDir = (light.light_pos - intersectionPoint).normalize();
        
        // Check for shadows
        Ray shadowRay(intersectionPoint + normal * 0.001, lightDir); // slight offset
        bool inShadow = false;
        
        for (auto obj : objects) {
            if (obj != this) { // don't check intersection with self
                double shadowT = obj->intersect(&shadowRay, nullptr, 0);
                if (shadowT > 0) {
                    inShadow = true;
                    break;
                }
            }
        }
        
        if (!inShadow) {
            // Diffuse component
            double lambertValue = std::max(0.0, normal.dot(lightDir));
            color[0] += light.color[0] * coEfficients[1] * lambertValue * intersectionColor.x;
            color[1] += light.color[1] * coEfficients[1] * lambertValue * intersectionColor.y;
            color[2] += light.color[2] * coEfficients[1] * lambertValue * intersectionColor.z;
            
            // Specular component
            Vector3D viewDir = (r->start - intersectionPoint).normalize();
            Vector3D reflectDir = (lightDir * -1 + normal * (2 * normal.dot(lightDir))).normalize();
            double phongValue = std::max(0.0, viewDir.dot(reflectDir));
            phongValue = pow(phongValue, shine);
            
            color[0] += light.color[0] * coEfficients[2] * phongValue;
            color[1] += light.color[1] * coEfficients[2] * phongValue;
            color[2] += light.color[2] * coEfficients[2] * phongValue;
        }
    }
    
    // Process each spotlight
    for (const auto& spotlight : spotLights) {
        Vector3D lightDir = (spotlight.point_light.light_pos - intersectionPoint).normalize();
        
        // Check if within cutoff angle
        Vector3D lightToPoint = (intersectionPoint - spotlight.point_light.light_pos).normalize();
        double angle = acos(lightToPoint.dot(spotlight.light_direction.normalize())) * 180.0 / M_PI;
        
        if (angle <= spotlight.cutoff_angle) {
            // Check for shadows
            Ray shadowRay(intersectionPoint + normal * 0.001, lightDir);
            bool inShadow = false;
            
            for (auto obj : objects) {
                if (obj != this) {
                    double shadowT = obj->intersect(&shadowRay, nullptr, 0);
                    if (shadowT > 0) {
                        inShadow = true;
                        break;
                    }
                }
            }
            
            if (!inShadow) {
                // Diffuse component
                double lambertValue = std::max(0.0, normal.dot(lightDir));
                color[0] += spotlight.point_light.color[0] * coEfficients[1] * lambertValue * intersectionColor.x;
                color[1] += spotlight.point_light.color[1] * coEfficients[1] * lambertValue * intersectionColor.y;
                color[2] += spotlight.point_light.color[2] * coEfficients[1] * lambertValue * intersectionColor.z;
                
                // Specular component
                Vector3D viewDir = (r->start - intersectionPoint).normalize();
                Vector3D reflectDir = (lightDir * -1 + normal * (2 * normal.dot(lightDir))).normalize();
                double phongValue = std::max(0.0, viewDir.dot(reflectDir));
                phongValue = pow(phongValue, shine);
                
                color[0] += spotlight.point_light.color[0] * coEfficients[2] * phongValue;
                color[1] += spotlight.point_light.color[1] * coEfficients[2] * phongValue;
                color[2] += spotlight.point_light.color[2] * phongValue;
            }
        }
    }
    
    // Handle reflection if recursion level allows
    if (level < recursionLevel && coEfficients[3] > 0) {
        Vector3D reflectDir = (r->dir - normal * (2 * r->dir.dot(normal))).normalize();
        Ray reflectedRay(intersectionPoint + reflectDir * 0.0001, reflectDir);
        
        // Find nearest intersection for reflected ray
        double reflectT = -1;
        int nearest = -1;
        for (int i = 0; i < objects.size(); i++) {
            double t = objects[i]->intersect(&reflectedRay, nullptr, 0);
            if (t > 0 && (reflectT < 0 || t < reflectT)) {
                reflectT = t;
                nearest = i;
            }
        }
        
        if (nearest != -1) {
            double reflectedColor[3] = {0, 0, 0};
            objects[nearest]->intersect(&reflectedRay, reflectedColor, level + 1);
            
            color[0] += reflectedColor[0] * coEfficients[3];
            color[1] += reflectedColor[1] * coEfficients[3];
            color[2] += reflectedColor[2] * coEfficients[3];
        }
    }
    
    return t;
}

Vector3D Sphere::getNormal(Vector3D point) {
    return (point - reference_point).normalize();
}

// Floor intersection implementation
double Floor::intersect(Ray* r, double* color, int level) {
    // Floor is at z = 0 plane
    if (abs(r->dir.z) < 1e-6) return -1; // ray parallel to floor
    
    double t = -r->start.z / r->dir.z;
    if (t < 0) return -1; // intersection behind ray origin
    
    Vector3D intersectionPoint = r->start + r->dir * t;
    
    // Check if intersection is within floor bounds
    double halfWidth = floorWidth / 2.0;
    if (intersectionPoint.x < -halfWidth || intersectionPoint.x > halfWidth ||
        intersectionPoint.y < -halfWidth || intersectionPoint.y > halfWidth) {
        return -1;
    }
    
    if (level == 0) return t;
    
    // Calculate checkerboard color
    Vector3D intersectionColor = getColorAt(intersectionPoint);
    Vector3D normal = getNormal(intersectionPoint); // (0, 0, 1) for floor
    
    // Ambient component
    color[0] = intersectionColor.x * coEfficients[0];
    color[1] = intersectionColor.y * coEfficients[0];
    color[2] = intersectionColor.z * coEfficients[0];
    
    // Process each point light
    for (const auto& light : pointLights) {
        Vector3D lightDir = (light.light_pos - intersectionPoint).normalize();
        
        // Check for shadows
        Ray shadowRay(intersectionPoint + normal * 0.001, lightDir);
        bool inShadow = false;
        
        for (auto obj : objects) {
            if (obj != this) {
                double shadowT = obj->intersect(&shadowRay, nullptr, 0);
                if (shadowT > 0) {
                    inShadow = true;
                    break;
                }
            }
        }
        
        if (!inShadow) {
            // Diffuse component
            double lambertValue = std::max(0.0, normal.dot(lightDir));
            color[0] += light.color[0] * coEfficients[1] * lambertValue * intersectionColor.x;
            color[1] += light.color[1] * coEfficients[1] * lambertValue * intersectionColor.y;
            color[2] += light.color[2] * coEfficients[1] * lambertValue * intersectionColor.z;
            
            // Specular component
            Vector3D viewDir = (r->start - intersectionPoint).normalize();
            Vector3D reflectDir = (lightDir * -1 + normal * (2 * normal.dot(lightDir))).normalize();
            double phongValue = std::max(0.0, viewDir.dot(reflectDir));
            phongValue = pow(phongValue, shine);
            
            color[0] += light.color[0] * coEfficients[2] * phongValue;
            color[1] += light.color[1] * coEfficients[2] * phongValue;
            color[2] += light.color[2] * coEfficients[2] * phongValue;
        }
    }
    
    // Handle reflection if recursion level allows
    if (level < recursionLevel && coEfficients[3] > 0) {
        Vector3D reflectDir = (r->dir - normal * (2 * r->dir.dot(normal))).normalize();
        Ray reflectedRay(intersectionPoint + reflectDir * 0.0001, reflectDir);
        
        // Find nearest intersection for reflected ray
        double reflectT = -1;
        int nearest = -1;
        for (int i = 0; i < objects.size(); i++) {
            double t = objects[i]->intersect(&reflectedRay, nullptr, 0);
            if (t > 0 && (reflectT < 0 || t < reflectT)) {
                reflectT = t;
                nearest = i;
            }
        }
        
        if (nearest != -1) {
            double reflectedColor[3] = {0, 0, 0};
            objects[nearest]->intersect(&reflectedRay, reflectedColor, level + 1);
            
            color[0] += reflectedColor[0] * coEfficients[3];
            color[1] += reflectedColor[1] * coEfficients[3];
            color[2] += reflectedColor[2] * coEfficients[3];
        }
    }
    
    return t;
}

Vector3D Floor::getColorAt(Vector3D point) {
    if (useTexture && textureData) {
        if (texturePerTile) {
            // Map texture to each individual tile/square
            // Find which tile we're in
            double localX = point.x + floorWidth/2.0;
            double localY = point.y + floorWidth/2.0;
            
            // Get position within the current tile [0, tileWidth]
            double tileLocalX = fmod(localX, tileWidth);
            double tileLocalY = fmod(localY, tileWidth);
            
            // Handle negative coordinates
            if (tileLocalX < 0) tileLocalX += tileWidth;
            if (tileLocalY < 0) tileLocalY += tileWidth;
            
            // Convert to texture coordinates [0,1]
            double u = tileLocalX / tileWidth;
            double v = tileLocalY / tileWidth;
            
            return sampleTexture(u, v);
        } else {
            // Map texture to entire floor
            double u = (point.x + floorWidth/2.0) / floorWidth;
            double v = (point.y + floorWidth/2.0) / floorWidth;
            
            // Wrap coordinates if they're outside [0,1] (for repeating texture)
            u = u - floor(u); // Keep fractional part
            v = v - floor(v); // Keep fractional part
            
            return sampleTexture(u, v);
        }
    } else {
        // Original checkerboard pattern
        int tileX = (int)floor((point.x + floorWidth/2.0) / tileWidth);
        int tileY = (int)floor((point.y + floorWidth/2.0) / tileWidth);
        
        // Checkerboard pattern
        if ((tileX + tileY) % 2 == 0) {
            return Vector3D(1, 1, 1); // white
        } else {
            return Vector3D(0, 0, 0); // black
        }
    }
}

// Floor draw method
void Floor::draw() {
    glBegin(GL_QUADS);
    
    double halfWidth = floorWidth / 2.0;
    int numTiles = (int)(floorWidth / tileWidth);
    
    for (int i = 0; i < numTiles; i++) {
        for (int j = 0; j < numTiles; j++) {
            // Determine tile color
            if ((i + j) % 2 == 0) {
                glColor3f(1, 1, 1); // white
            } else {
                glColor3f(0, 0, 0); // black
            }
            
            double x1 = -halfWidth + i * tileWidth;
            double y1 = -halfWidth + j * tileWidth;
            double x2 = x1 + tileWidth;
            double y2 = y1 + tileWidth;
            
            glVertex3f(x1, y1, 0);
            glVertex3f(x2, y1, 0);
            glVertex3f(x2, y2, 0);
            glVertex3f(x1, y2, 0);
        }
    }
    
    glEnd();
}

// Triangle intersection implementation using barycentric coordinates
double Triangle::intersect(Ray* r, double* color, int level) {
    // Triangle vertices are a, b, c
    Vector3D edge1 = b - a;
    Vector3D edge2 = c - a;
    Vector3D h = r->dir.cross(edge2);
    double det = edge1.dot(h);
    
    if (det > -1e-6 && det < 1e-6) return -1; // ray parallel to triangle
    
    double inv_det = 1.0 / det;
    Vector3D s = r->start - a;
    double u = inv_det * s.dot(h);
    
    if (u < 0.0 || u > 1.0) return -1;
    
    Vector3D q = s.cross(edge1);
    double v = inv_det * r->dir.dot(q);
    
    if (v < 0.0 || u + v > 1.0) return -1;
    
    double t = inv_det * edge2.dot(q);
    
    if (t <= 1e-6) return -1; // intersection behind ray origin
    
    if (level == 0) return t;
    
    // Calculate color using similar lighting as sphere
    Vector3D intersectionPoint = r->start + r->dir * t;
    Vector3D normal = getNormal(intersectionPoint);
    Vector3D intersectionColor = getColorAt(intersectionPoint);
    
    // Ambient component
    color[0] = intersectionColor.x * coEfficients[0];
    color[1] = intersectionColor.y * coEfficients[0];
    color[2] = intersectionColor.z * coEfficients[0];
    
    // Process each point light (similar to sphere implementation)
    for (const auto& light : pointLights) {
        Vector3D lightDir = (light.light_pos - intersectionPoint).normalize();
        
        // Check for shadows
        Ray shadowRay(intersectionPoint + normal * 0.001, lightDir);
        bool inShadow = false;
        
        for (auto obj : objects) {
            if (obj != this) {
                double shadowT = obj->intersect(&shadowRay, nullptr, 0);
                if (shadowT > 0) {
                    inShadow = true;
                    break;
                }
            }
        }
        
        if (!inShadow) {
            // Diffuse component
            double lambertValue = std::max(0.0, normal.dot(lightDir));
            color[0] += light.color[0] * coEfficients[1] * lambertValue * intersectionColor.x;
            color[1] += light.color[1] * coEfficients[1] * lambertValue * intersectionColor.y;
            color[2] += light.color[2] * coEfficients[1] * lambertValue * intersectionColor.z;
            
            // Specular component
            Vector3D viewDir = (r->start - intersectionPoint).normalize();
            Vector3D reflectDir = (lightDir * -1 + normal * (2 * normal.dot(lightDir))).normalize();
            double phongValue = std::max(0.0, viewDir.dot(reflectDir));
            phongValue = pow(phongValue, shine);
            
            color[0] += light.color[0] * coEfficients[2] * phongValue;
            color[1] += light.color[1] * coEfficients[2] * phongValue;
            color[2] += light.color[2] * phongValue;
        }
    }
    
    // Handle reflection
    if (level < recursionLevel && coEfficients[3] > 0) {
        Vector3D reflectDir = (r->dir - normal * (2 * r->dir.dot(normal))).normalize();
        Ray reflectedRay(intersectionPoint + reflectDir * 0.0001, reflectDir);
        
        // Find nearest intersection for reflected ray
        double reflectT = -1;
        int nearest = -1;
        for (int i = 0; i < objects.size(); i++) {
            double t = objects[i]->intersect(&reflectedRay, nullptr, 0);
            if (t > 0 && (reflectT < 0 || t < reflectT)) {
                reflectT = t;
                nearest = i;
            }
        }
        
        if (nearest != -1) {
            double reflectedColor[3] = {0, 0, 0};
            objects[nearest]->intersect(&reflectedRay, reflectedColor, level + 1);
            
            color[0] += reflectedColor[0] * coEfficients[3];
            color[1] += reflectedColor[1] * coEfficients[3];
            color[2] += reflectedColor[2] * coEfficients[3];
        }
    }
    
    return t;
}

Vector3D Triangle::getNormal(Vector3D point) {
    // Normal = cross product of two edges: (b - a) × (c - a)
    Vector3D edge1 = b - a;
    Vector3D edge2 = c - a;
    return edge1.cross(edge2).normalize();
}

// General Quadric Surface intersection implementation
double GeneralQuadric::intersect(Ray* r, double* color, int level) {
    // Quadric equation: Ax² + By² + Cz² + Dxy + Exz + Fyz + Gx + Hy + Iz + J = 0
    // Ray: P = r->start + t * r->dir
    
    Vector3D ro = r->start; // ray origin
    Vector3D rd = r->dir;   // ray direction
    
    // Substitute ray equation into quadric equation
    double aq = A * rd.x * rd.x + B * rd.y * rd.y + C * rd.z * rd.z +
               D * rd.x * rd.y + E * rd.x * rd.z + F * rd.y * rd.z;
    
    double bq = 2 * A * ro.x * rd.x + 2 * B * ro.y * rd.y + 2 * C * ro.z * rd.z +
               D * (ro.x * rd.y + ro.y * rd.x) + E * (ro.x * rd.z + ro.z * rd.x) +
               F * (ro.y * rd.z + ro.z * rd.y) + G * rd.x + H * rd.y + I * rd.z;
    
    double cq = A * ro.x * ro.x + B * ro.y * ro.y + C * ro.z * ro.z +
               D * ro.x * ro.y + E * ro.x * ro.z + F * ro.y * ro.z +
               G * ro.x + H * ro.y + I * ro.z + J;
    
    double discriminant = bq * bq - 4 * aq * cq;
    if (discriminant < 0) return -1; // no intersection
    
    double t1 = (-bq - sqrt(discriminant)) / (2 * aq);
    double t2 = (-bq + sqrt(discriminant)) / (2 * aq);
    
    double t = -1;
    
    // Check which intersection is valid and within bounding box
    if (t1 > 0) {
        Vector3D point1 = r->start + r->dir * t1;
        if (isWithinBounds(point1)) {
            t = t1;
        }
    }
    if (t < 0 && t2 > 0) {
        Vector3D point2 = r->start + r->dir * t2;
        if (isWithinBounds(point2)) {
            t = t2;
        }
    }
    
    if (t < 0 || level == 0) return t;
    
    // Calculate lighting (similar to other objects)
    Vector3D intersectionPoint = r->start + r->dir * t;
    Vector3D normal = getNormal(intersectionPoint);
    Vector3D intersectionColor = getColorAt(intersectionPoint);
    
    // Ambient component
    color[0] = intersectionColor.x * coEfficients[0];
    color[1] = intersectionColor.y * coEfficients[0];
    color[2] = intersectionColor.z * coEfficients[0];
    
    // Process each point light
    for (const auto& light : pointLights) {
        Vector3D lightDir = (light.light_pos - intersectionPoint).normalize();
        
        // Check for shadows
        Ray shadowRay(intersectionPoint + normal * 0.001, lightDir);
        bool inShadow = false;
        
        for (auto obj : objects) {
            if (obj != this) {
                double shadowT = obj->intersect(&shadowRay, nullptr, 0);
                if (shadowT > 0) {
                    inShadow = true;
                    break;
                }
            }
        }
        
        if (!inShadow) {
            // Diffuse component
            double lambertValue = std::max(0.0, normal.dot(lightDir));
            color[0] += light.color[0] * coEfficients[1] * lambertValue * intersectionColor.x;
            color[1] += light.color[1] * coEfficients[1] * lambertValue * intersectionColor.y;
            color[2] += light.color[2] * coEfficients[1] * lambertValue * intersectionColor.z;
            
            // Specular component
            Vector3D viewDir = (r->start - intersectionPoint).normalize();
            Vector3D reflectDir = (lightDir * -1 + normal * (2 * normal.dot(lightDir))).normalize();
            double phongValue = std::max(0.0, viewDir.dot(reflectDir));
            phongValue = pow(phongValue, shine);
            
            color[0] += light.color[0] * coEfficients[2] * phongValue;
            color[1] += light.color[1] * coEfficients[2] * phongValue;
            color[2] += light.color[2] * coEfficients[2] * phongValue;
        }
    }
    
    // Process each spotlight
    for (const auto& spotlight : spotLights) {
        Vector3D lightDir = (spotlight.point_light.light_pos - intersectionPoint).normalize();
        
        // Check if within cutoff angle
        Vector3D lightToPoint = (intersectionPoint - spotlight.point_light.light_pos).normalize();
        double angle = acos(lightToPoint.dot(spotlight.light_direction.normalize())) * 180.0 / M_PI;
        
        if (angle <= spotlight.cutoff_angle) {
            // Check for shadows
            Ray shadowRay(intersectionPoint + normal * 0.001, lightDir);
            bool inShadow = false;
            
            for (auto obj : objects) {
                if (obj != this) {
                    double shadowT = obj->intersect(&shadowRay, nullptr, 0);
                    if (shadowT > 0) {
                        inShadow = true;
                        break;
                    }
                }
            }
            
            if (!inShadow) {
                // Diffuse component
                double lambertValue = std::max(0.0, normal.dot(lightDir));
                color[0] += spotlight.point_light.color[0] * coEfficients[1] * lambertValue * intersectionColor.x;
                color[1] += spotlight.point_light.color[1] * coEfficients[1] * lambertValue * intersectionColor.y;
                color[2] += spotlight.point_light.color[2] * coEfficients[1] * lambertValue * intersectionColor.z;
                
                // Specular component
                Vector3D viewDir = (r->start - intersectionPoint).normalize();
                Vector3D reflectDir = (lightDir * -1 + normal * (2 * normal.dot(lightDir))).normalize();
                double phongValue = std::max(0.0, viewDir.dot(reflectDir));
                phongValue = pow(phongValue, shine);
                
                color[0] += spotlight.point_light.color[0] * coEfficients[2] * phongValue;
                color[1] += spotlight.point_light.color[1] * coEfficients[2] * phongValue;
                color[2] += spotlight.point_light.color[2] * coEfficients[2] * phongValue;
            }
        }
    }
    
    // Handle reflection if recursion level allows
    if (level < recursionLevel && coEfficients[3] > 0) {
        Vector3D reflectDir = (r->dir - normal * (2 * r->dir.dot(normal))).normalize();
        Ray reflectedRay(intersectionPoint + reflectDir * 0.0001, reflectDir);
        
        // Find nearest intersection for reflected ray
        double reflectT = -1;
        int nearest = -1;
        for (int i = 0; i < objects.size(); i++) {
            double t = objects[i]->intersect(&reflectedRay, nullptr, 0);
            if (t > 0 && (reflectT < 0 || t < reflectT)) {
                reflectT = t;
                nearest = i;
            }
        }
        
        if (nearest != -1) {
            double reflectedColor[3] = {0, 0, 0};
            objects[nearest]->intersect(&reflectedRay, reflectedColor, level + 1);
            
            color[0] += reflectedColor[0] * coEfficients[3];
            color[1] += reflectedColor[1] * coEfficients[3];
            color[2] += reflectedColor[2] * coEfficients[3];
        }
    }
    
    
    return t;
}

Vector3D GeneralQuadric::getNormal(Vector3D point) {
    // Normal = gradient of F(x,y,z) = (∂F/∂x, ∂F/∂y, ∂F/∂z)
    double nx = 2 * A * point.x + D * point.y + E * point.z + G;
    double ny = 2 * B * point.y + D * point.x + F * point.z + H;
    double nz = 2 * C * point.z + E * point.x + F * point.y + I;
    
    return Vector3D(nx, ny, nz).normalize();
}

bool GeneralQuadric::isWithinBounds(Vector3D point) {
    // Check if point is within the reference cube (bounding box)
    if (cube_length > 0) {
        if (point.x < cube_ref_point.x || point.x > cube_ref_point.x + cube_length)
            return false;
    }
    if (cube_width > 0) {
        if (point.y < cube_ref_point.y || point.y > cube_ref_point.y + cube_width)
            return false;
    }
    if (cube_height > 0) {
        if (point.z < cube_ref_point.z || point.z > cube_ref_point.z + cube_height)
            return false;
    }
    return true;
}
