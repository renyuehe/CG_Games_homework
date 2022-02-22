//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_INTERSECTION_H
#define RAYTRACING_INTERSECTION_H
#include "Vector.hpp"
#include "Material.hpp"
class Object;
class Sphere;

//交点（光线打到表面的交点）hit
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
    bool happened;//是否发生
    Vector3f coords;//坐标
    Vector3f normal;//法线
    double distance;//距离（t near）

    Object* obj;//对象
    Material* m;//材质
};
#endif //RAYTRACING_INTERSECTION_H
