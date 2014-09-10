#ifndef FOLIAGE_H
#define FOLIAGE_H

#include "smallvisual.h"
#include "global.h"
#include "terrain.h"
#include "threadingwrapper.h"
#include "quadtree.hpp"
#include "camera.h"

struct FolSpec
{
    public:
        enum Type  {
            Spruce = 0,
            SpruceBB = 1,
            GrassSpring = 2
        };

        FolSpec(glm::vec4 pos_in, Type type_in) :
            pos(pos_in), type(type_in),
            x(pos.x), z(pos.z)
             {};
        glm::vec4 pos;
        Type type;
        float x;
        float z;
    private:
        FolSpec();
};

class Foliage
{
    public:
        Foliage(Terrain* in_terrain);
        virtual ~Foliage();

        //std::vector<SmallVisual> const * const getVisuals() const;

        //void addSmallVisuals(string mesh_key,
//                             string tex_key,
//                             glm::vec4 wind_params,
//                             glm::vec3 center,
//                             float radius,
//                             float density);

        struct BG_Thread
        {
            BG_Thread() : qt_trees(QuadAABB({-500, 500, -500, 500})) {};

            static const unsigned int NUM_SMALL_VISUAL_TYPES = 3;
            SmallVisual sm_types[NUM_SMALL_VISUAL_TYPES];

//            enum unsigned int Type {
//                Spruce = 0,
//                SpruceBB = 1,
//                GrassSpring = 2
//            };

            // temporary storage for sm_types
            int sm_type_counter[NUM_SMALL_VISUAL_TYPES];

            void prepareBG_Foliage()
            {
                sm_types[FolSpec::Type::Spruce].init("spruce_wbranch", "spruce_wbranch", glm::vec4(180.f, 20.f, 2.f, 0.0));
                sm_types[FolSpec::Type::SpruceBB].init("spruce_bb", "spruce_bb", glm::vec4(180.f, 20.f, 2.f, 0.0));
                sm_types[FolSpec::Type::GrassSpring].init("grass_spring", "grass_spring", glm::vec4(10.f, 10.f, 2.f, 0.0));

            };

            void updateFoliage(Camera &cam)
            {
                LockGuard lock_qt_trees(sm_mutex);

                for (int i = 0; i<NUM_SMALL_VISUAL_TYPES; i++)
                {
                    sm_type_counter[i] = 0;
                    sm_types[i].updated = true;
                }

                qt_trees.for_all_in(cam.get2dViewFrustum(1.15, 0.5),
                //qt_trees.for_all_in(QuadAABB({-100, 100, -100, 100}),
                                    [&] (FolSpec &fol_dat)
                                    {
                                        SmallVisual &sm_type = sm_types[fol_dat.type];
                                        int &counter = sm_type_counter[fol_dat.type];

                                        if (sm_type_counter[fol_dat.type]<SmallVisual::MAX_NUM_SMVIS)
                                        {

                                            if (sm_type.updated && sm_type.sm_buffer[counter]!=fol_dat.pos)
                                                sm_type.updated = false;

                                            sm_type.sm_buffer[counter] = fol_dat.pos;
                                            counter++;
                                        } // if (i<SmallVisual::MAX_NUM_SMVIS)
                                    });

                for (int i = 0; i<NUM_SMALL_VISUAL_TYPES; i++)
                {
                    sm_types[i].num_smvis = sm_type_counter[i];
                }
            }

            Mutex sm_mutex;
            QuadTree<FolSpec, 5> qt_trees;

        } bg_thread;

    protected:
    private:
        //std::vector<SmallVisual> small_visuals;



        Terrain* terrain;
};

#endif // FOLIAGE_H
