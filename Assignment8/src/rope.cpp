#include <iostream>
#include <vector>

#include "CGL/vector2D.h"

#include "mass.h"
#include "rope.h"
#include "spring.h"

namespace CGL {

    //绳子构造
    Rope::Rope(Vector2D start, Vector2D end, int num_nodes, float node_mass, float k, vector<int> pinned_nodes)
    {
        // TODO (Part 1): Create a rope starting at `start`, ending at `end`, and containing `num_nodes` nodes.
        /**
         * @brief 
         * start：绳子的起点， 绳子的终点， 
         * num_nodes：绳子的节点数，每个节点对应一个 node_mass（质量）， 
         * spring：节点数-1，每个弹簧对应一个 k（系数)
         * pinned_nodes：表示哪些节点被针别住了
         */
        num_nodes = num_nodes > 1 ? num_nodes : 2;//节点数
        Vector2D spring_distance = (end - start)/ (num_nodes - 1);//每一个 spring 的长度（每一节）
        
        //一个 mass 对应一个 node
        for(int i = 0; i < num_nodes; i++){
            masses.push_back(new Mass(start + i * spring_distance, node_mass, false));
        }
        // n个mass 对应 n-1个spring
        for(int i = 0; i < masses.size() - 1; i++){
            springs.push_back(new Spring( masses[i], masses[i + 1], k));
        }

        //pinned：被针别住,就是固定点了
        for (auto &i : pinned_nodes) {
            masses[i]->pinned = true;
        }
    }

    //模拟 Euler
    void Rope::simulateEuler(float delta_t, Vector2D gravity)
    {
        //遍历弹簧
        for (auto &s : springs)
        {
            // TODO (Part 2): Use Hooke's law to calculate the force on a node
            auto m1tom2 = s->m2->position - s->m1->position;
            //相对位置（被拉开的距离）
            auto raletive_position =  m1tom2.norm() - s->rest_length; //use for calculating elastic force
            //在当前方向上的相对速度
            auto raletive_velocity =dot(s->m1->velocity - s->m2->velocity, m1tom2.unit()) * m1tom2.unit(); //use for calculating damp
            //扭矩 阻尼 方向上的速度
            auto raletive_velocity_t = raletive_velocity - (s->m1->velocity - s->m2->velocity);   //simulate torque damp

            // //限制过大,这个违背物理的写法肯定屏蔽了呀
            // if (raletive_position > 0.2f * s->rest_length){
            //     raletive_position = s->rest_length;
            // }

            //更新（增加弹簧力） spring 上 m1 和 m2 的受力
            //如果 m1 没有被钉上
            if(!s->m1->pinned){
                s->m1->forces += 
                                 s->k * m1tom2.unit() * raletive_position //弹簧力
                               - 0.01 * raletive_velocity //阻尼
                               + 0.01 * raletive_velocity_t //阻尼
                               ;
            }
            
            //如果 m2 没有被钉上
            if(!s->m2->pinned){
                s->m2->forces += 
                               - s->k * m1tom2 / m1tom2.norm() * raletive_position //弹簧力
                               + 0.01 * raletive_velocity //阻尼
                               - 0.01 * raletive_velocity_t //阻尼
                               ;
            }

        }

        //遍历 mass
        for (auto &m : masses)
        {
            //更新（增加重力）
            //没有被针别住
            if (!m->pinned)
            {
                // TODO (Part 2): Add the force due to gravity, then compute the new velocity and position
                // TODO (Part 2): Add global damping
                m->forces += gravity * m -> mass;//重力

                //节点速度 和 位置 基于上一次的状态更新
                //更新节点速度
                m->velocity += delta_t * (m->forces / m->mass);     //implicit euler method (order 1)
                //更新节点位置
                m->position += delta_t * m->velocity ;               //local truncation error:O(h2)
            }

            //每一次节点受力都需要重新计算
            // Reset all forces on each mass
            m->forces = Vector2D(0, 0);
        }
    }

    //模拟 Verlet
    void Rope::simulateVerlet(float delta_t, Vector2D gravity)
    {
        for (auto &s : springs)
        {
            // TODO (Part 2): Use Hooke's law to calculate the force on a node
            auto m1tom2 = s->m2->position - s->m1->position;
            auto raletive_position =  m1tom2.norm() - s->rest_length; //use for calculating elastic force
            auto raletive_velocity =dot(s->m1->velocity - s->m2->velocity, m1tom2.unit()) * m1tom2.unit(); //use for calculating damp
            auto raletive_velocity_t = raletive_velocity - (s->m1->velocity - s->m2->velocity);   //simulate torque damp

            // //限制过大，违背物理了呀，为什么要这么写
            // if (raletive_position > 0.2f * s->rest_length){
            //     raletive_position = s->rest_length;
            // }

            if(!s->m1->pinned){
                s->m1->forces += s->k * m1tom2.unit() * raletive_position 
                                - raletive_velocity * 0.05 
                                + raletive_velocity_t * 0.05;
            }
                
            if(!s->m2->pinned){
                s->m2->forces += -s->k * m1tom2.unit() * raletive_position 
                                + raletive_velocity * 0.05 
                                - raletive_velocity_t * 0.05;
            }
        }

        for (auto &m : masses)
        {
            if (!m->pinned)
            {
                // TODO (Part 3.1): Set the new position of the rope mass
                auto temp = m->position;
                m->forces += gravity * m -> mass;
                m->velocity += delta_t * (m->forces / m->mass);
                m->position = 2 * m->position 
                            - m->last_position 
                            + (m->forces / m->mass) * delta_t * delta_t;
                m->last_position = temp;
                // TODO (Part 4): Add global Verlet dampings

            }

            m->forces = Vector2D(0, 0);
        }

    }
}
