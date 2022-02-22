//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_normals(const std::vector<Eigen::Vector3f>& normals)
{
    auto id = get_next_id();
    nor_buf.emplace(id, normals);

    normal_id = id;

    return {id};
}


auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}

static bool insideTriangle(int x, int y, const Vector4f* _v){
    Vector3f v[3];
    for(int i=0;i<3;i++)
        v[i] = {_v[i].x(),_v[i].y(), 1.0};
    Vector3f f0,f1,f2;
    f0 = v[1].cross(v[0]);
    f1 = v[2].cross(v[1]);
    f2 = v[0].cross(v[2]);
    Vector3f p(x,y,1.);
    if((p.dot(f0)*f0.dot(v[2])>0) && (p.dot(f1)*f1.dot(v[0])>0) && (p.dot(f2)*f2.dot(v[1])>0))
        return true;
    return false;
}

//计算重心坐标系数
static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector4f* v){
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(std::vector<Triangle *> &TriangleList) {

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    //因为重心坐标在投影坐标下会发生变换，没有projection是为了后面三角形内部重心坐标与纹理重心坐标对应
    Eigen::Matrix4f mv = view * model;
    for (const auto& t:TriangleList)
    {
        Triangle newtri = *t;

        //mv 空间坐标
        std::array<Eigen::Vector4f, 3> mm {
            (mv * t->v[0]),
            (mv * t->v[1]),
            (mv * t->v[2])
        };

        std::array<Eigen::Vector3f, 3> viewspace_pos;

        // vector4f 转 vector3f
        std::transform(mm.begin(), mm.end(), viewspace_pos.begin(), 
            [](auto& v) { return v.template head<3>();}
        );

        //mvp 坐标
        Eigen::Vector4f v[] = {
                mvp * t->v[0],
                mvp * t->v[1],
                mvp * t->v[2]
        };
        
        //Homogeneous division
        for (auto& vec : v) {
            vec.x()/=vec.w();
            vec.y()/=vec.w();
            vec.z()/=vec.w();
        }

        Eigen::Matrix4f inv_trans = (view * model).inverse().transpose();
        Eigen::Vector4f n[] = {
                inv_trans * to_vec4(t->normal[0], 0.0f),
                inv_trans * to_vec4(t->normal[1], 0.0f),
                inv_trans * to_vec4(t->normal[2], 0.0f)
        };

        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        //设置三角形顶点坐标
        for (int i = 0; i < 3; ++i)
        {
            //screen space coordinates
            newtri.setVertex(i, v[i]);
        }

        //设置三角形顶点法线
        for (int i = 0; i < 3; ++i)
        {
            //view space normal
            newtri.setNormal(i, n[i].head<3>());
        }

        //设置三角形顶点颜色
        newtri.setColor(0, 148,121.0,92.0);
        newtri.setColor(1, 148,121.0,92.0);
        newtri.setColor(2, 148,121.0,92.0);


        //三角形  以及   视图空间坐标
        // Also pass view space vertice position
        rasterize_triangle(newtri, viewspace_pos);
    }
}

//重心坐标计算插值
static Eigen::Vector3f interpolate(float alpha, float beta, float gamma, const Eigen::Vector3f& vert1, const Eigen::Vector3f& vert2, const Eigen::Vector3f& vert3, float weight)
{
    // auto x = (alpha * vert1[0] + beta * vert2[0] + gamma * vert3[0]);
    // auto y = (alpha * vert1[1] + beta * vert2[1] + gamma * vert3[1]);
    // auto z = (alpha * vert1[2] + beta * vert2[2] + gamma * vert3[2]);
    // x /= weight;
    // y /= weight;
    // z /= weight;
    // return Eigen::Vector3f(x, y, z);

    return (alpha * vert1 + beta * vert2 + gamma * vert3) / weight;
}

//重心坐标计算uv空间插值
static Eigen::Vector2f interpolate(float alpha, float beta, float gamma, const Eigen::Vector2f& vert1, const Eigen::Vector2f& vert2, const Eigen::Vector2f& vert3, float weight)
{
    // auto u = (alpha * vert1[0] + beta * vert2[0] + gamma * vert3[0]);
    // auto v = (alpha * vert1[1] + beta * vert2[1] + gamma * vert3[1]);
    // u /= weight;
    // v /= weight;
    // return Eigen::Vector2f(u, v);

    return (alpha * vert1 + beta * vert2 + gamma * vert3) / weight;
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t, const std::array<Eigen::Vector3f, 3>& view_pos) 
{
    // TODO: From your HW3, get the triangle rasterization code.
    // TODO: Inside your rasterization loop:
    //    * v[i].w() is the vertex view space depth value z.
    //    * Z is interpolated view space depth for the current pixel
    //    * zp is depth between zNear and zFar, used for z-buffer

    // float Z = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    // float zp = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    // zp *= Z;

    // TODO: Interpolate the attributes:
    // auto interpolated_color
    // auto interpolated_normal
    // auto interpolated_texcoords
    // auto interpolated_shadingcoords

    // Use: fragment_shader_payload payload( interpolated_color, interpolated_normal.normalized(), interpolated_texcoords, texture ? &*texture : nullptr);
    // Use: payload.view_pos = interpolated_shadingcoords;
    // Use: Instead of passing the triangle's color directly to the frame buffer, pass the color to the shaders first to get the final color;
    // Use: auto pixel_color = fragment_shader(payload);

    //返回 3 个顶点
    auto v = t.toVector4();

    // bounding box
    float min_x = std::min(v[0][0], std::min(v[1][0], v[2][0]));
    float max_x = std::max(v[0][0], std::max(v[1][0], v[2][0]));
    float min_y = std::min(v[0][1], std::min(v[1][1], v[2][1]));
    float max_y = std::max(v[0][1], std::max(v[1][1], v[2][1]));

    int x_min = std::floor(min_x);
    int x_max = std::ceil(max_x);
    int y_min = std::floor(min_y);
    int y_max = std::ceil(max_y);

    //遍历 bouding box
    for(int i = x_min; i < x_max;i++){
        for(int j = y_min; j < y_max;j++){
            //如果在三角形内
            if(insideTriangle(i+0.5,j+0.5,t.v)){

                //depth interpolated，三个顶点的重心坐标系数
                auto [alpha, beta, gamma] = computeBarycentric2D(i+0.5,j+0.5,t.v);
                
                //该项目中并没有什么实际的意义,因为 w() 就是 1.f
                //alpha + beta + gamma == 1
                float Z = 1.0 / (alpha / v[0].w() 
                                 + beta / v[1].w() 
                                 + gamma / v[2].w());    
                // zp 是在zNear和zFar之间的深度，用于z-缓冲区
                float zp =  alpha * v[0].z() / v[0].w() 
                            + beta * v[1].z() / v[1].w() 
                            + gamma * v[2].z() / v[2].w();
                zp *= Z;

                //如果 当前像素z值 小于 深度缓冲像素z值 
                if(zp < depth_buf[get_index(i,j)]){ 
                    //重心坐标插值
                	//分别对应颜色、法向量、纹理坐标、viewpos坐标(没用到)进行插值
                    //颜色插值（没有采用双线性插值）
                    auto interpolated_color = interpolate(alpha,beta,gamma,t.color[0], t.color[1], t.color[2],1);
                    auto interpolated_normal = interpolate(alpha,beta,gamma,t.normal[0],t.normal[1],t.normal[2],1).normalized();
                    //纹理插值
                    auto interpolated_texcoords = interpolate(alpha,beta,gamma,t.tex_coords[0],t.tex_coords[1],t.tex_coords[2],1);
                    //view_pos[]是三角形顶点在view space中的坐标，插值是为了还原在camera space中的坐标
                    //mv空间插值
                    auto interpolated_shadingcoords = interpolate(alpha,beta,gamma,view_pos[0],view_pos[1],view_pos[2],1);  

                    //插值结果传到payload里，渲染时根据这些值确定发现，uv坐标
                    //fragment_shader_payload类型的payload是用来传递插值结果的结构体
                    //在fragment_shader_payload中, ViewPos(x,y,z)和纹理坐标(u,v)就映射好了
                    fragment_shader_payload payload(interpolated_color,interpolated_normal,interpolated_texcoords,texture ? &*texture : nullptr);
                    payload.view_pos = interpolated_shadingcoords;
                    //通过 颜色、法线、纹理坐标、view_pos 共同着色（shader由用户指定实现）
                    auto pixel_color = fragment_shader(payload);

					//更新深度
                    depth_buf[get_index(i,j)] = zp; 
                    //设置颜色
                    set_pixel(Eigen::Vector2i(i,j),pixel_color);   
                }
            }
        }
    }
}


void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);

    texture = std::nullopt;
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-y)*width + x;
}

void rst::rasterizer::set_pixel(const Vector2i &point, const Eigen::Vector3f &color)
{
    //old index: auto ind = point.y() + point.x() * width;
    int ind = (height-point.y())*width + point.x();
    frame_buf[ind] = color;
}

void rst::rasterizer::set_vertex_shader(std::function<Eigen::Vector3f(vertex_shader_payload)> vert_shader)
{
    vertex_shader = vert_shader;
}

void rst::rasterizer::set_fragment_shader(std::function<Eigen::Vector3f(fragment_shader_payload)> frag_shader)
{
    fragment_shader = frag_shader;
}

