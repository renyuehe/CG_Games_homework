//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_INTERSECTION_H
#define RAYTRACING_INTERSECTION_H
#include "Vector.hpp"
#include "Material.hpp"
class Object;
class Sphere;

struct Intersection
{
    Intersection(){
        happened=false;
        coords=Vector3f();
        normal=Vector3f();
        distance= std::numeric_limits<double>::max();
        obj =nullptr;
        m=nullptr;
    }
    bool happened; //是否发生碰撞
    Vector3f coords; //碰撞发生的坐标
    Vector3f tcoords;
    Vector3f normal; //相交三角形的法线
    Vector3f emit; //光源光
    double distance; //碰撞点距光源的距离

    Object* obj;
    Material* m;
};
#endif //RAYTRACING_INTERSECTION_H
