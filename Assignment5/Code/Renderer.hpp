#pragma once
#include "Scene.hpp"

struct hit_payload
{
    float tNear;//击中的最近的time
    uint32_t index;//第几个三角形
    Vector2f uv;//重心坐标
    Object* hit_obj;//击中的对象
};

class Renderer
{
public:
    void Render(const Scene& scene);

private:
};