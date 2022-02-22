#include <fstream>
#include "Vector.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include <optional>

//弧度
inline float deg2rad(const float &deg)
{ return deg * M_PI/180.0; }

// Compute reflection direction
// 计算 反射
Vector3f reflect(const Vector3f &I, const Vector3f &N)
{
    /**
     * @brief 反射光
     * I：入射光（非单位向量）
     * N：法线（单位向量）
     */
    return I - 2 * dotProduct(I, N) * N;//反射光的公式推导结论
}

// [comment]
// Compute refraction direction using Snell's law
//
// We need to handle with care the two possible situations:
//
//    - When the ray is inside the object
//
//    - When the ray is outside.
//
// If the ray is outside, you need to make cosi positive cosi = -N.I
//
// If the ray is inside, you need to invert the refractive indices and negate the normal N
// [/comment]
// 计算 折射
Vector3f refract(const Vector3f &I, const Vector3f &N, const float &ior)
{
    /**
     * @brief 折射光
     * I：入射光（非单位向量）
     * N：法线（单位向量）
     * ior：折射率
     */
    float cosi = clamp(-1, 1, dotProduct(I, N));
    float etai = 1, etat = ior;
    Vector3f n = N;
    if (cosi < 0) { cosi = -cosi; } else { std::swap(etai, etat); n= -N; }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
}

// [comment]
// Compute Fresnel equation
//
// \param I is the incident view direction
//
// \param N is the normal at the intersection point
//
// \param ior is the material refractive index
// [/comment]
// 计算 菲尼耳 方程
float fresnel(const Vector3f &I, const Vector3f &N, const float &ior)
{   /**
    * @brief 求反射光线的能量占比，全反射时为1
    * I：  入射光
    * N：  法线
    * ior：折射率
    */

    /**
     * @brief 当入射光方向接近垂直表面时，大部分的能量会被折射，所以我们能看清水底的东西，而当入射光方向接近平行表面时，大部分的能量会被反射
     * @brief 描述出在不同入射光的情况下，反射光与折射光所占的比例，这个公式就是菲涅尔方程。
     * @brief 菲涅尔方程会根据观察角度告诉我们被反射的光线所占的百分比，利用这个反射比率和能量守恒原则，我们可以直接得出光线被折射的部分以及光线剩余的能量。
     */

    //入射角度，cos值，光线从空气打到介质时 dot(I, N) 应该是负的, I默认朝下,N朝上
    float cosi = clamp(-1, 1, dotProduct(I, N));

    //etai：密度小的介质的折射率（空气）
    //etat（ior）：密度大的介质的折射率（默认目标对象）
    float etai = 1, etat = ior;

    //<0：光线从空气打到物体， >0：光线从物体内部打到空气
    if (cosi > 0) {  std::swap(etai, etat); }

    // Compute sini using Snell's law
    // sina**2 + cosa**2 = 1
    // sint：折射角
    // 注意：当etai<etat的时候,sint必然小于1
    // 注意：只有当etai>etat的时候（光线从密度大的地方射向密度小的地方的时候）sint才有可能>=1
    float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
    // Total internal reflection 
    // 全反射的情况，也就是折射角大于大于90的时候
    // sin(90) == 1
    if (sint >= 1) {
        return 1;//能量守恒，全部返回
    }
    else {
        float cost = sqrtf(std::max(0.f, 1 - sint * sint));
        cosi = fabsf(cosi);
        //s偏振光 和 p偏正光 互为垂直（正交）关系
        //任何一种输入偏振状态都可以表示为 s 和p分量的矢量和。
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        auto S = Rs * Rs;//s偏振光
        auto P = Rp * Rp;//p偏振光
        //如果我们不考虑偏振的情况，那么菲涅尔方程即是上面两者的平均值
        return (S + P) / 2;//反射能量占比
    }
    // As a consequence of the conservation of energy, transmittance is given by:
    // kt = 1 - kr;
}

// [comment]
// Returns true if the ray intersects an object, false otherwise.
//
// \param orig is the ray origin
// \param dir is the ray direction
// \param objects is the list of objects the scene contains
//
// \param[out] tNear contains the distance to the cloesest intersected object.
// \param[out] index stores the index of the intersect triangle if the interesected object is a mesh.
// \param[out] uv stores the u and v barycentric coordinates of the intersected point
// \param[out] *hitObject stores the pointer to the intersected object (used to retrieve material information, etc.)
// \param isShadowRay is it a shadow ray. We can return from the function sooner as soon as we have found a hit.
// [/comment]
// 光线追踪，如果有光线，返回 pyload，没有返回 false
// 注意：这里仅为一条光线
std::optional<hit_payload> trace(
        const Vector3f &orig, const Vector3f &dir,
        const std::vector<std::unique_ptr<Object> > &objects)
{
    float tNear = kInfinity;
    std::optional<hit_payload> payload;
    for (const auto & object : objects)
    {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        // 相交  且  更近
        if (object->intersect(orig, dir, tNearK, indexK, uvK) && tNearK < tNear)
        {
            payload.emplace();
            payload->hit_obj = object.get();//相交的对象
            payload->tNear = tNearK;//最近的 相交对象的 相交面的 距离
            payload->index = indexK;//光线 与 mesh 相交 triangle 的 index
            payload->uv = uvK;//重心坐标
            tNear = tNearK;
        }
    }

    // 最终返回的只有一个相交的 obj
    return payload;
}

// [comment]
// Implementation of the Whitted-style light transport algorithm (E [S*] (D|G) L)
//
// This function is the function that compute the color at the intersection point
// of a ray defined by a position and a direction. Note that thus function is recursive (it calls itself).
//
// If the material of the intersected object is either reflective or reflective and refractive,
// then we compute the reflection/refraction direction and cast two new rays into the scene
// by calling the castRay() function recursively. When the surface is transparent, we mix
// the reflection and refraction color using the result of the fresnel equations (it computes
// the amount of reflection and refraction depending on the surface normal, incident view direction
// and surface refractive index).
//
// If the surface is diffuse/glossy we use the Phong illumation model to compute the color
// at the intersection point.
// [/comment]
// whitted-style 的递归光线追踪方式，仅仅指一根光线
Vector3f castRay(const Vector3f &orig, 
                 const Vector3f &dir, 
                 const Scene& scene,
                 int depth)
{
    /**
     * @brief whitted-style 的递归光线追踪方式
     * orig：光线的起点
     * dir：光线的传播方向
     * sence：场景（包括场景中的物体）
     * depth：递归深度
     */

    //超过递归深度直接返回0，（递归返回条件）
    //在反射和折射中，如果深度不够，有可能返回黑色
    //如果递归深度小以至于不足以打到，DIFFUSE_AND_GLOSSY物体（mesh 和 sphere）上的时候，返回黑色
    
    if (depth > scene.maxDepth) {
        return Vector3f(0.0,0.0,0.0);
    }

    //初始化背景色
    //注意：球体中的背景色既不是折射来的，也不是反射来的
    //注意：球体中的背景色依然要经过 菲尼耳 反射和折射的比例来计算
    Vector3f hitColor = scene.backgroundColor;//
    // Vector3f hitColor = Vector3f(0.0, 0.0, 0.0);//去掉背景色

    //如果这条光线与物体相交并返回payload
    if (auto payload = trace(orig, dir, scene.get_objects()); payload)
    {
        //通过光线的起点、方向、时间、算出击中的点
        Vector3f hitPoint = orig + dir * payload->tNear;//击中点
        Vector3f N; // normal
        Vector2f st; // st coordinates

        //对象获取表面属性：
        // Sphere返回值参数 N（法线）； 会用到 hitPoint
        // Mesh返回值参数 N（法线）、st（纹理坐标）。会用到 index 和 payload->uv（在光线和三角形求交的时候就已经求出）
        payload->hit_obj->getSurfaceProperties(hitPoint, dir, payload->index, payload->uv, N, st);

        //材质的选择
        switch (payload->hit_obj->materialType) {
            case REFLECTION_AND_REFRACTION:
            {//反射和折射
                //反射和折射
                Vector3f reflectionDirection = normalize(reflect(dir, N));
                Vector3f refractionDirection = normalize(refract(dir, N, payload->hit_obj->ior));

                //反射光光源 和 折射光光源 都是 hitPoint，但是为了避免重叠（以及给表面增加一个很小的法线方向上的厚度）就是 N*epsilon
                //反射光光源 和 折射光光源
                Vector3f reflectionRayOrig = (dotProduct(reflectionDirection, N) < 0) ?
                                             hitPoint - N * scene.epsilon :
                                             hitPoint + N * scene.epsilon;
                Vector3f refractionRayOrig = (dotProduct(refractionDirection, N) < 0) ?
                                             hitPoint - N * scene.epsilon :
                                             hitPoint + N * scene.epsilon;
                //递归调用 castRay，并返回 color
                Vector3f reflectionColor = castRay(reflectionRayOrig, reflectionDirection, scene, depth + 1);
                Vector3f refractionColor = castRay(refractionRayOrig, refractionDirection, scene, depth + 1);

                float kr = fresnel(dir, N, payload->hit_obj->ior);//菲尼耳方程计算反射能量占比
                hitColor = reflectionColor * kr + refractionColor * (1 - kr);//当前以及更深 深度的 颜色和
                break;
            }
            case REFLECTION:
            {//反射
                //反射能量占比
                float kr = fresnel(dir, N, payload->hit_obj->ior);
                Vector3f reflectionDirection = reflect(dir, N);//反射
                Vector3f reflectionRayOrig = (dotProduct(reflectionDirection, N) < 0) ?
                                             hitPoint + N * scene.epsilon :
                                             hitPoint - N * scene.epsilon;
                
                hitColor = castRay(reflectionRayOrig, reflectionDirection, scene, depth + 1) * kr;
                break;
            }
            default:
            {
                // [comment]
                // We use the Phong illumation model int the default case. The phong model
                // Phong 照明 模型
                // is composed of a diffuse and a specular reflection component.
                // 漫反射 和 镜面反射 组成
                // [/comment]

                // lightAmt:环境光照， specularColor: 镜面color
                Vector3f lightAmt = 0, specularColor = 0;
                // shadow源，hitPoint
                Vector3f shadowPointOrig = (dotProduct(dir, N) < 0) ?
                                           hitPoint + N * scene.epsilon :
                                           hitPoint - N * scene.epsilon;

                // [comment]
                // Loop over all lights in the scene and sum their contribution up
                // We also apply the lambert cosine law
                // [/comment]
                for (auto& light : scene.get_lights()) {
                    Vector3f lightDir = light->position - hitPoint;
                    // square of the distance between hitPoint and the light
                    float lightDistance2 = dotProduct(lightDir, lightDir);
                    //光源 ====>> hitPoint 的方向
                    lightDir = normalize(lightDir);
                    //cos
                    float LdotN = std::max(0.f, dotProduct(lightDir, N));
                    
                    // is the point in shadow, and is the nearest occluding object closer to the object than the light itself?
                    // 光
                    auto shadow_res = trace(shadowPointOrig, lightDir, scene.get_objects());

                    // 是不是阴影
                    bool inShadow = shadow_res && (shadow_res->tNear * shadow_res->tNear < lightDistance2);

                    // 阴影：0， 不是阴影：光的强度 * cos
                    // 注意：此处的阴影是单独处理的，直接设置成了0
                    lightAmt += inShadow ? 0 : light->intensity * LdotN;

                    // 镜面反射光
                    Vector3f reflectionDirection = reflect(-lightDir, N);

                    // (-(入射光 点乘 反射光))^范围ie指数 * 光的强度
                    // 反射光 和 入射光 求 cos，在之前的课程中用的是 法线和半程向量求cos，本质上没有区别，Phong 模型只是近似。
                    // 这里入射光是向下的，反射光是向上的，所以要加上负号
                    // specularExponent 是镜面反射区域控制指数
                    // 光线的强度没有经过 r2 衰减，这里只是简单的模拟一下
                    specularColor += powf(std::max(0.f, -dotProduct(reflectionDirection, dir)),
                        payload->hit_obj->specularExponent) * light->intensity;
                }

                //漫反射*漫反射（RGB系数）*漫反射系数 + 镜面反射*镜面反射系数
                hitColor = lightAmt * payload->hit_obj->evalDiffuseColor(st) * payload->hit_obj->Kd + specularColor * payload->hit_obj->Ks;
                
                break;
            }
        }
    }

    return hitColor;
}

// [comment]
// The main render function. This where we iterate over all pixels in the image, generate
// primary rays and cast these rays into the scene. The content of the framebuffer is
// saved to a file.
// [/comment]
// 主要的 着色 函数
void Renderer::Render(const Scene& scene)
{
    // 帧缓存
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    // 视角
    float scale = std::tan(deg2rad(scene.fov * 0.5f));
    float imageAspectRatio = scene.width / (float)scene.height;

    // Use this variable as the eye position to start your rays.
    //眼睛设置为世界坐标的 (0,0,0)位置, 屏幕是 z轴方向 值为-1 的平面上
    Vector3f eye_pos(0);

    int m = 0;
    for (int j = 0; j < scene.height; ++j)
    {
        for (int i = 0; i < scene.width; ++i)
        {
            // generate primary ray direction
            // 分别对应每一个像素
            float x;
            float y;
            // TODO: Find the x and y positions of the current pixel to get the direction
            // 找到当前 x 和 y 的像素位置 给 感受射线用
            // vector that passes through it.
            // Also, don't forget to multiply both of them with the variable *scale*, and
            // x (horizontal) variable with the *imageAspectRatio*            

            //像素映射,因为眼睛之在中间,而不是左上角
            //把 0 - width 映射到 -1 ---- 1 之间， 再 * scale
            x = (((i + 0.5) / ((float)scene.width) * 2)-1);
            x = x * imageAspectRatio * scale;
            //把 0 - height 映射到 1 ---- -1 之间， 再 * scale
            y = (1 - (j + 0.5) / (float)scene.height * 2 );
            y = y * scale;

            //感受光线
            // z轴-1平面上，每个感受光线的向量，需要作归一化 normalize
            Vector3f dir = Vector3f(x, y, -1); // Don't forget to normalize this direction!

            //对每个像素的感受光线进行光线追踪,返回结果进行着色
            framebuffer[m++] = castRay(eye_pos, dir, scene, 0);
        }

        //着色进度条
        UpdateProgress(j / (float)scene.height);
    }

    // save framebuffer to file
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        color[0] = (char)(255 * clamp(0, 1, framebuffer[i].x));
        color[1] = (char)(255 * clamp(0, 1, framebuffer[i].y));
        color[2] = (char)(255 * clamp(0, 1, framebuffer[i].z));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);    
}
