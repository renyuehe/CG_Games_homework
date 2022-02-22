#include <algorithm>
#include <cassert>
#include "BVH.hpp"

//BVH 构造函数
BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;

    //递归构建这个 BVH 树
    root = recursiveBuild(primitives);

    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf("\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
    hrs, mins, secs);
}

//递归构造 BVH 树
BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;

    //并集所有的 objects,即为最原始（最大的）那个boundsBox
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());

    //当只有一个节点的时候（叶子节点），表示是物体的节点
    //当还有两个节点的时候（非叶子节点）但是左右分别都确定是叶子节点了，则左节点和右节点都设置为物体节点
    //
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;

        for (int i = 0; i < objects.size(); ++i)
            centroidBounds = Union(centroidBounds, objects[i]->getBounds().Centroid());

        //排序
        //获取(xyz)中最长的轴
        int dim = centroidBounds.maxExtent();
        //通过最长的轴，对 objects boundBox 做一个排序
        switch (dim) {
        case 0://x轴最长
            std::sort(objects.begin(), objects.end(), 
                        //根据每个object的中心点在x轴的位置来排序
                        [](auto f1, auto f2) 
                        {
                            return f1->getBounds().Centroid().x <
                                f2->getBounds().Centroid().x;
                        }
                    );
            break;
        case 1://y轴最长
            std::sort(objects.begin(), objects.end(), 
                    //根据每个object的中心点在y轴的位置来排序
                    [](auto f1, auto f2) {
                        return f1->getBounds().Centroid().y <
                               f2->getBounds().Centroid().y;
            });
            break;
        case 2://z轴最长
            std::sort(objects.begin(), objects.end(), 
                    //根据每个object的中心点在y轴的位置来排序
                    [](auto f1, auto f2) {
                        return f1->getBounds().Centroid().z <
                               f2->getBounds().Centroid().z;
            });
            break;
        }

        //取中值
        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        //划分左子树， 和右子树
        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        //左+右 == 总
        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        //递归
        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        //当前节点的 bounds
        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}

//与整个 BVH 判断那些是相交的
Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)//根节点为空
        return isect;
    //找到相交的 节点们
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

//递归遍历 BVHTree 找到与光线相交的叶子节点，但是最后返回一个最近的 hit
Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // TODO Traverse the BVH to find intersection

    Intersection inter;
    //invdir = 1 / D; bounds3.hpp中会用到。
    Vector3f invdir(1 / ray.direction.x, 1 / ray.direction.y, 1 / ray.direction.z);
    //判断射线的方向正负，如果负，为1；bounds3.hpp中会用到。
    std::array<int, 3> dirIsNeg;
    dirIsNeg[0] = ray.direction.x < 0;
    dirIsNeg[1] = ray.direction.y < 0;
    dirIsNeg[2] = ray.direction.z < 0;
    
    //------------当前节点的情况
    //没有交点
    if(!node -> bounds.IntersectP(ray, invdir, dirIsNeg))
        return inter;
    //有交点，且该点为叶子节点，去和三角形求交
    if(node -> left == nullptr && node -> right == nullptr)
        return node -> object -> getIntersection(ray);//obj 和 光线求交

    //------------子节点的情况
    //该点为中间节点，继续判断，并返回最近的包围盒交点
    Intersection hit1 = getIntersection(node -> left,  ray);
    Intersection hit2 = getIntersection(node -> right, ray);

    return hit1.distance < hit2.distance ? hit1 : hit2;
}