#ifndef WORLD_H
#define WORLD_H

#include <list>
#include <memory.h>

#include "worldobject.h"
#include "creature.h"
// #include "resourcemanager.h"
#include "camera.h"
#include "physicsworld.h"
#include "light.h"
#include "terrain.h"
#include "foliage.h"
#include "global.h"
//#include "smallvisual.h"


using std::list;
using std::vector;
using std::shared_ptr;
using std::weak_ptr;
using std::string;

enum class yRelativeTo {Ground, World};

class World : public PhysicsWorld // Consider merging Culling functionality into World Class
{
    public:
        World();
        virtual ~World();


        list<shared_ptr<WorldObject>>::iterator addStaticObject(string mesh_key,
        //void                                    addStaticObject(string mesh_key,
                                                                 string tex_key,
                                                                 glm::vec3 pos,
                                                                 float scale,
                                                                 float rotate_left_deg, float rotate_up_deg,
                                                                 yRelativeTo y_rel = yRelativeTo::World,
                                                                 btCollisionShape* shape = nullptr);

        list<shared_ptr<WorldObject>>::iterator addDynamicObject(string mesh_key,
                                                                 string tex_key,
                                                                 glm::vec3 pos,
                                                                 btCollisionShape* shape = nullptr);

        void delWorldObject(list<shared_ptr<WorldObject>>::iterator obst_it_in);

        list<shared_ptr<Creature>>::iterator addCreature(string mesh_key,
                                                         string tex_key,
                                                         glm::vec3 pos);

        void delCreature(list<shared_ptr<Creature>>::iterator creature_it_in);
        void mainLight(glm::vec3 dir, glm::vec4 color );
        void addTerrain();
        void updateObserver(const glm::vec3 &observer_pos);
        void startMusic(string soundKey);
//        // Step
//        void step(float dt_in);

        //vector<shared_ptr<Prop>> instanced_objects;

//        void addSmallVisuals(string mesh_key,
//                             string tex_key,
//                             glm::vec4 wind_params,
//                             glm::vec3 center,
//                             float radius,
//                             float density);

        //std::vector<SmallVisual> small_visuals;

        // Safe pointers are for wimps...
        Camera *freecam;    // Owning pointer
        Camera *chasecam;   // Owning pointer
        Camera const *active_cam; // Non-owning pointer (points to either above) // ORDER IMPORTANT!

        // lights
        DirLight* main_light;
        static const int MAX_NUM_POINT_LIGHTS = 5;
        int num_point_lights;
        PointLight* point_lights;

        // music
        sf::Music* music;

        // foliage
        Foliage foliage;

        // Late initialized

//        void startAmbient(string soundKey);
//        sf::Sound* ambient;

        // "Physical" Contents (could with benefit be private?)
        list<shared_ptr<WorldObject>>    worldobjects;
        list<shared_ptr<Creature>>       creatures;
        // list<shared_ptr<Light>>       lights; // For future
        Terrain                          terrain;



        //shared_ptr<Camera> camera; // Shared pointer here, because we do not want other shared
        // pointers to accidentally delete the camera.




        static constexpr btCollisionShape* NO_COLLISION = (btCollisionShape*)(-12345);


        // void forAllWorldObjects( void (*f) (WorldObject&) );

    protected:
    private:

};

#endif // WORLD_H
