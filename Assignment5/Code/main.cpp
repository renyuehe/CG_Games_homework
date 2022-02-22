#include "Scene.hpp"
#include "Sphere.hpp"
#include "Triangle.hpp"
#include "Light.hpp"
#include "Renderer.hpp"

// In the main function of the program, we create the scene (create objects and lights)
// as well as set the options for the render (image width and height, maximum recursion
// depth, field-of-view, etc.). We then call the render function().
int main()
{
    //场景
    Scene scene(1280, 960);

    //unique 唯一指针的构造方法 make_unique
    //创建球体
    auto sph1 = std::make_unique<Sphere>(Vector3f(-1, 0, -12), 2);
    //设置材质
    sph1->materialType = DIFFUSE_AND_GLOSSY;
    sph1->diffuseColor = Vector3f(0.9, 0.9, 0.9);//RGB系数

    auto sph2 = std::make_unique<Sphere>(Vector3f(0.5, -0.5, -8), 1.5);
    sph2->ior = 1.01; //折射率,真空折射率为1,一般其他介质应该大于1
    sph2->materialType = REFLECTION_AND_REFRACTION;

    //场景添加球
    scene.Add(std::move(sph1));
    scene.Add(std::move(sph2));

    //创建 mesh
    Vector3f verts[4] = {{-5,-3,-6}, {5,-3,-6}, {5,-3,-16}, {-5,-3,-16}};//4个顶点
    uint32_t vertIndex[6] = {1, 3, 0, 3, 1, 2};//每三个一个三角形
    // Vector2f st[4] = {{0, 0}, {0.5, 0}, {0.5, 0.5}, {0, 0.5}};//4个顶点对应的纹理坐标,??纹理贴图在哪?
    Vector2f st[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};//4个顶点对应的纹理坐标,??纹理贴图在哪?
    auto mesh = std::make_unique<MeshTriangle>(verts, vertIndex, 2, st);
    mesh->materialType = DIFFUSE_AND_GLOSSY;
    // mesh->diffuseColor = Vector3f(0.5, 0.5, 0.5);

    //添加 mesh
    scene.Add(std::move(mesh));

    //添加 光源
    scene.Add(std::make_unique<Light>(Vector3f(-20, 70, 20), 0.5));
    scene.Add(std::make_unique<Light>(Vector3f(30, 50, -12), 0.5));    

    //渲染
    Renderer r;
    r.Render(scene);

    return 0;
}