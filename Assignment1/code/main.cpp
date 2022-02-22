#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f rodrigues_rotate(float rotation_angle,  Eigen::Vector4f rotation_axis = Eigen::Vector4f(1.0, 0, 0, 0))
{
    float radian = rotation_angle/180.0 * 3.1415926;//弧度

    //旋转轴叉乘的矩阵形式
    Eigen::Matrix4f rotation_axis_mm;
    rotation_axis_mm << 0, -rotation_axis[2],  rotation_axis[1], 0,
                      rotation_axis[2], 0,  -rotation_axis[0],   0,
                      -rotation_axis[1], -rotation_axis[0], 0,   0,
                      0, 0, 0, 0; 

    //单位阵
    Eigen::Matrix4f unit_mm;
    unit_mm <<  1,0,0, 0,
                0,1,0, 0,
                0,0,1, 0,
                0,0,0, 0;
    
    //罗德里格斯旋转矩阵， 通过  旋转弧度、 旋转轴 计算旋转矩阵
    Eigen::Matrix4f rodrigues_r_matrix;
    rodrigues_r_matrix = cos(radian) * unit_mm
                        + (1-cos(radian))*rotation_axis * rotation_axis.transpose() 
                        + sin(radian) * rotation_axis_mm;
    rodrigues_r_matrix(3,3) = 1.0;//homogenous特性,这里强制设置为1,不通用

    //旋转
    return rodrigues_r_matrix;
}

Eigen::Matrix3f rodrigues_rotate(float rotation_angle,  Eigen::Vector3f rotation_axis = Eigen::Vector3f(1.0, 0, 0))
{
    float radian = rotation_angle/180.0 * 3.1415926;//弧度

    //旋转轴叉乘的矩阵形式
    Eigen::Matrix3f rotation_axis_mm;
    rotation_axis_mm << 0,               -rotation_axis[2],  rotation_axis[1],
                      rotation_axis[2],                  0,  -rotation_axis[0], 
                      -rotation_axis[1], -rotation_axis[0],  0; 

    //单位阵
    Eigen::Matrix3f unit_mm;
    unit_mm <<  1,0,0,
                0,1,0,
                0,0,1;
    
    //罗德里格斯旋转矩阵， 通过  旋转弧度、 旋转轴 计算旋转矩阵
    Eigen::Matrix3f rodrigues_r_matrix;
    rodrigues_r_matrix = cos(radian) * unit_mm
                        + (1-cos(radian))*rotation_axis * rotation_axis.transpose() 
                        + sin(radian) * rotation_axis_mm;

    //旋转
    return rodrigues_r_matrix;
}


Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos, Eigen::Matrix3f &eye_direction)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    //去中心化位移
    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 
                 0, 1, 0, -eye_pos[1], 
                 0, 0, 1, -eye_pos[2], 
                 0, 0, 0, 1;

    //ratation
    /*  
     * Rotate g to -Z, t to Y, gxt to X
     *
     *  Xgxt  Ygxt  Zgxt  0
     *  Xt    Yt    Zt    0
     *  Xg    Yg    Zg    0
     *  0     0     0     1
    **/
    //功能还未实现,难点在鼠标键盘如何操作相机,以及相机的 g 和 t 如何存储
    Eigen::Matrix4f rotate_matrix = Eigen::Matrix4f::Identity();
    
    // View ： Rotate Matrix
    rotate_matrix << eye_direction(0,0), eye_direction(1,0), eye_direction(2,0), 0,
                     eye_direction(0,1), eye_direction(1,1), eye_direction(2,1), 0,
                     eye_direction(0,2), eye_direction(1,2), eye_direction(2,2), 0,
                     0, 0, 0, 1;

    view = rotate_matrix * translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle, Eigen::Vector4f rotation_axis = Eigen::Vector4f(1.0, 0, 0, 0),
                                 Eigen::Vector3f distance = Eigen::Vector3f(0, 0, 0))
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();

    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    
    {//普通的 x、y、z 轴的旋转矩阵
        // Eigen::Matrix4f rotate_x, rotate_y, rotate_z;
        // float radian = rotation_angle/180.0 * MY_PI;
        // rotate_x << 1, 0, 0, 0,
        //             0, cos(radian), -1*sin(radian), 0,
        //             0, sin(radian),    cos(radian), 0,
        //             0,           0,              0, 1; //only for complete rotate metrix;   

        // rotate_y << cos(radian), 0, -1*sin(radian), 0,
        //             0,           1,              0, 0,
        //             sin(radian),    0, cos(radian), 0,
        //             0,                        0, 0, 1; //only for complete rotate metrix;

        // rotate_z << cos(radian), -1*sin(radian), 0, 0,
        //             sin(radian),    cos(radian), 0, 0,
        //             0,                        0, 1, 0,
        //             0,                        0, 0, 1; //only for complete rotate metrix;


        // model = rotate_y * model;
    }

    //罗德里格斯 旋转矩阵
    Eigen::Matrix4f rodrigues_r_matrix;
    rodrigues_r_matrix = rodrigues_rotate(rotation_angle, rotation_axis);

    //位移：未实现
    rodrigues_r_matrix(0,3) = distance(0);
    rodrigues_r_matrix(1,3) = distance(1);
    rodrigues_r_matrix(2,3) = distance(2);

    model = rodrigues_r_matrix * model;

    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    /* eye_fov：视角, 视野的大小
     * aspect_ratio：宽高比
     * zNear：近，屏幕位置，z轴坐标
     * zFar：远，目标位置，z轴坐标
     * 
     * Students will implement this function
     * TODO: Implement this function
     * Create the projection matrix for the given parameters.
     * Then return it.
     **/
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();//投影矩阵
    Eigen::Matrix4f P2O = Eigen::Matrix4f::Identity();//将透视投影转换为正交投影的矩阵

    //核心：透视投影 to 正交投影
    P2O << zNear, 0, 0, 0,
           0, zNear, 0, 0,
           0, 0, zNear+zFar,(-1)*zFar*zNear,
           0, 0, 1, 0;

    float halfEyeAngelRadian = eye_fov/2.0/180.0*MY_PI;//half视角（弧度）
    
    float t = zNear*std::tan(halfEyeAngelRadian);//top
    float r=t*aspect_ratio;//right
    float l=(-1)*r;//left
    float b=(-1)*t;//bottom

    Eigen::Matrix4f ortho1=Eigen::Matrix4f::Identity();
    ortho1<<2/(r-l),0,0,0,
            0,2/(t-b),0,0,
            0,0,2/(zNear-zFar),0,
            0,0,0,1;//缩放：进行一定的缩放使之成为一个标准的长度为2的正方体

    Eigen::Matrix4f ortho2 = Eigen::Matrix4f::Identity();
    ortho2<<1,0,0,(-1)*(r+l)/2,
            0,1,0,(-1)*(t+b)/2,
            0,0,1,(-1)*(zNear+zFar)/2,
            0,0,0,1;//位移：把一个长方体的中心移动到原点

    Eigen::Matrix4f Matrix_ortho = ortho1 * ortho2;//first bias， second decentor

    projection = Matrix_ortho * P2O;

    return projection;
}


int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;

    rst::rasterizer r(300, 300);//改大小 显示就不正确了

    Eigen::Vector3f eye_pos = {0, 0, 10};//摄像机位置

    Eigen::Matrix3f eye_direction;//眼睛朝向,朝-Z看
    eye_direction << 1,0,0,  0,1,0,  0,0,-1;


    Eigen::Vector3f model_distance = {0.0, 0.0, 0.0};//model tanslate


    //三角形的三个 point
    std::vector<Eigen::Vector3f> pos{{1,  0, -1}, 
                                     {0,  1, -1}, 
                                     {-1, 0, -1}};

    //三角形 point index
    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};
    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    //再画一个三角形
    std::vector<Eigen::Vector3f> pos2{{1.0,  0.0,  -0.5}, 
                                      {0.5,  0.6,  0.0}, 
                                      {-1.0, 0.0,  1.0}};
    std::vector<Eigen::Vector3i> ind2{{0, 1, 2}};
    auto pos_id2 = r.load_positions(pos2);
    auto ind_id2 = r.load_indices(ind2);


    //
    int frame_count = 0;

    int key = 0;
    while (key != 27) {
        //
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        Eigen::Vector4f rotation_axis(0, 0, 1, 0);//model 旋转轴
        r.set_model(get_model_matrix(angle, rotation_axis, model_distance));
        r.set_view(get_view_matrix(eye_pos, eye_direction));
        

        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        r.draw(pos_id2, ind_id2, rst::Primitive::Triangle);

        //通过 frame_buffer 更新这张图
        cv::Mat image(300, 300, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        // std::cout << "frame count: " << frame_count++ << '\n';

        //摄像机 gazze（-z）、top（y）、gt（x）
        Eigen::Vector3f gt(eye_direction(0,0), eye_direction(0,1), eye_direction(0,2));
        Eigen::Vector3f  t(eye_direction(1,0), eye_direction(1,1), eye_direction(1,2));
        Eigen::Vector3f  g(eye_direction(2,0), eye_direction(2,1), eye_direction(2,2));

        if (key == 'q') {
            // model rotate
            angle += 10;
        }
        else if (key == 'e') {
            // model rotate
            angle -= 10;
        }
        if (key == 'w') {
            // view z-- close
            eye_pos(2) -= 1;
        }
        else if (key == 's') {
            // view z++ far
            eye_pos(2) += 1;
        }
        else if (key == 'i') {
            // view turn gt, 沿着 gt 顺时针,  初始化时对应 x轴
            Eigen::Matrix3f rodrigues_matrix = rodrigues_rotate(1, gt);
            std::cout << "rodrigues matrix = :\n" << rodrigues_matrix << std::endl;
            eye_direction = rodrigues_matrix * eye_direction;
        }
        else if (key == 'k') {
            Eigen::Matrix3f rodrigues_matrix = rodrigues_rotate(-1, gt);
            std::cout << "rodrigues matrix = :\n" << rodrigues_matrix << std::endl;
            eye_direction = rodrigues_matrix * eye_direction;
        }
        else if (key == 'j') {
            // view turn t, 沿着 t 顺时针,  初始化时对应 y轴
            Eigen::Matrix3f rodrigues_matrix = rodrigues_rotate(-1, t);
            std::cout << "rodrigues matrix = :\n" << rodrigues_matrix << std::endl;
            eye_direction = rodrigues_matrix * eye_direction;
        }
        else if (key == 'l') {
            Eigen::Matrix3f rodrigues_matrix = rodrigues_rotate(1, t);
            std::cout << "rodrigues matrix = :\n" << rodrigues_matrix << std::endl;
            eye_direction = rodrigues_matrix * eye_direction;
        }
        else if (key == 'u') {
            // view turn g, 沿着 g 顺时针, 初始化时对应 z 轴
            Eigen::Matrix3f rodrigues_matrix = rodrigues_rotate(1, g);
            std::cout << "rodrigues matrix = :\n" << rodrigues_matrix << std::endl;
            eye_direction = rodrigues_matrix * eye_direction;
        }
        else if (key == 'o') {
            Eigen::Matrix3f rodrigues_matrix = rodrigues_rotate(-1, g);
            std::cout << "rodrigues matrix = :\n" << rodrigues_matrix << std::endl;
            eye_direction = rodrigues_matrix * eye_direction;
        }
        else if (key == '1') {
            model_distance(0)++;//model: x轴++
        }
        else if (key == '2') {
            model_distance(0)--;
        }
        else if (key == '3') {
            model_distance(1)++;//model: y轴++
        }
        else if (key == '4') {
            model_distance(1)--;
        }
        else if (key == '5') {
            model_distance(2)++;//model: z轴++
        }
        else if (key == '6') {
            model_distance(2)--;
        }
        // std::cout << "model z distance = " << model_distance(2) << std::endl;
    } 

    return 0;
}
