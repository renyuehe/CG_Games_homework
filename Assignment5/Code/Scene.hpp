#pragma once

#include <vector>
#include <memory>
#include "Vector.hpp"
#include "Object.hpp"
#include "Light.hpp"

class Scene
{
public:
    // setting up options
    int width = 1280;
    int height = 960;
    double fov = 90;//视角
    // Vector3f backgroundColor = Vector3f(0.235294, 0.67451, 0.843137);
    Vector3f backgroundColor = Vector3f(0.1, 0.1, 0.0);
    int maxDepth = 10;//光线递归深度
    float epsilon = 0.00001;//一个很小的数，用于边缘位移,如果没有它,表面就会有很多模棱两可的值

    Scene(int w, int h) : width(w), height(h)
    {}

    //添加 物体、光源
    void Add(std::unique_ptr<Object> object) { objects.push_back(std::move(object)); }
    void Add(std::unique_ptr<Light> light) { lights.push_back(std::move(light)); }

    //获得 物体、光源
    [[nodiscard]] const std::vector<std::unique_ptr<Object> >& get_objects() const { return objects; }
    [[nodiscard]] const std::vector<std::unique_ptr<Light> >&  get_lights() const { return lights; }

private:
    // creating the scene (adding objects and lights)
    std::vector<std::unique_ptr<Object> > objects;
    std::vector<std::unique_ptr<Light> > lights;
};