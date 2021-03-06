#include "artificialintelligence.h"

using namespace ai;

//
//Agent::Agent() :
//    m_pos(glm::vec3(0.0, 0.0, 0.0)),
//    m_dir(glm::vec3(0.0, 0.0, -1.0)),
//    m_passive(false),
//    m_team_flag(Team0),
//    m_hostile_flags(0), // no hostiles
//    m_target(nullptr),
//    m_user(nullptr),
//    m_isPursuing(false),
//    m_isRunning(false),
//    m_randomWait(0)
//{
//
//}

Agent::Agent(glm::vec3 pos, glm::vec3 dir, bool passive) :
    m_pos(pos),
    m_dir(dir),
    m_passive(passive),
    m_target(nullptr),
    m_user(nullptr),
    m_isPursuing(false),
    m_isRunning(false),
    m_reactionTime(0.50),
    m_reactionWait(0.0)
{
    if (passive) // If this is the player set him against the world;
    {
        m_team_flag = Team1;
        m_hostile_flags = ~Team1;
    }
    else
    {
        m_team_flag = Team2;
        m_hostile_flags = ~Team2;
    }
}

void Agent::setPos(glm::vec3 pos)
{
    m_pos = pos;
}

void Agent::setDir(glm::vec3 dir)
{
    m_dir = dir;
}

glm::vec3 Agent::getPos() const
{
    return m_pos;
}

glm::vec3 Agent::getDir() const
{
    return m_dir;
}

SignalReceiver *Agent::getUserPointer()
{
    return m_user;
}

void Agent::setUserPointer(SignalReceiver* user)
{
    m_user = user;
}

void Agent::interrupt()
{
//    int spread_perc = 50 - (rand() % 100); // spread between -50 and +50 % of reaction time
//    float spread_s = m_reactionTime*((float)spread_perc)/100.f; // convert % to secs
//
//    float interruptTime = m_reactionTime + spread_s; // add up this specific interruption time
//    m_reactionWait = interruptTime; // set it

    auto range = [] (float lower_frac, float upper_frac)
                {
                    return (lower_frac + (rand()%100)/100.f * (upper_frac-lower_frac));
                };



    float timez = m_reactionTime * range(0.2, 1.2);

//    std::cout << "interrupted for: " << timez << "s\n";

    m_reactionWait = timez; // set it
}

void Agent::wait(float wait_time)
{
    m_reactionWait = wait_time; // set it
}


Agent::~Agent()
{

}

World::World()
{
    std::cout << "creating AI world\n";

    //agents.reserve(100);
}

void World::addAgent(Agent* agent)
{
    agents.push_back(agent);
}

void World::removeAgent(Agent* torem_agent)
{
    agents.remove(torem_agent);
    // remove this agent as target of all those who have this as target
    for (auto remaining_agent : agents)
    {
        if (remaining_agent->m_target==torem_agent)
        {
            remaining_agent->m_target = nullptr;
//            std::cout << "removed as target: " << (long)torem_agent << "\n";
        }
    }
}

void World::deleteAgent(Agent* todel_agent)
{
    removeAgent(todel_agent);

    delete todel_agent;
}

void World::stepAI(float dt) // as it stands O(n^2), where n is number of ai agents
{
    //int counter = 0;
    for (auto agent : agents)
    {
        if (!(agent->m_passive))
        {
            if (!(agent->m_target)) // if no target
            { // search for target
                for (auto agent_inner : agents)
                {
                    if (agent_inner->m_team_flag & agent->m_hostile_flags) // we have a hostile
                    {
                        agent->m_target = agent_inner;
//                        std::cout << "assinging target\n";
                    }
                }
            }

            if (agent->m_user)
            {
                // check if I have a target
                //if (agent->m_target && agent->m_randomWait==0)
                if (agent->m_target && agent->m_reactionWait<=0.0)
                {
                    { // do this:
                        glm::vec3 from_me_2_target = (agent->m_target)->getPos() - (agent)->getPos();
                        glm::vec3 from_me_2_target_dir = glm::normalize(from_me_2_target);

                        //agent->m_user->faceDirection(from_me_2_target_dir); // Not implemented yet

                        glm::vec3 from_me_2_target_dir_xz = glm::normalize(glm::vec3(
                                                                from_me_2_target_dir.x,
                                                                0.f,
                                                                from_me_2_target_dir.z
                                                                           ));

                        float from_me_2_target_dist = glm::length(from_me_2_target);

    //
                        glm::vec3 this_dir = agent->getDir();
                        glm::vec3 this_dir_xz = glm::normalize(glm::vec3(this_dir.x, 0.f, this_dir.z));
    ////
                        float dot_prod = glm::dot(from_me_2_target_dir_xz, this_dir_xz);
                        float safe_dot = (dot_prod > 1.0) ? 1.0 : dot_prod;
                        float diff_angle = acosf(safe_dot);
    ////
    //                    std::cout << "diff_angle: " << diff_angle << "\n";
    //
    //                    agent->m_user->rotateLeft(diff_angle, 1.0); // second argument unused
                        if (std::abs(safe_dot) < 0.9999) // They are not parallell
                        {
                            glm::vec3 cross_prod = glm::cross(this_dir_xz, from_me_2_target_dir_xz);
                            if (glm::dot(cross_prod, glm::vec3(0.0, 1.0, 0.0))>0.0)
                            {
                                agent->m_user->rotateLeft(diff_angle*180.f/3.15, 0.0);
                            }
                            else
                            {
                                agent->m_user->rotateLeft(-diff_angle*180.f/3.15, 0.0);
                            }
                        }

                        // turning should not be affect by waiting (either that, or make a lag between lookdir and actual dir)

//                        std::cout << "target distance: " << from_me_2_target_dist << "\n";

                        if (!(agent->m_user->isDodging()))
                        { // Pursuit
                            if ((from_me_2_target_dist > Agent::pursuit_start_dist) && !(agent->m_isPursuing))
                            {
                                agent->m_isPursuing = true;
                            }

                            if ((from_me_2_target_dist < Agent::pursuit_stop_dist) && (agent->m_isPursuing))
                            {
                                agent->m_isPursuing = false;
                            }

                            if (agent->m_isPursuing)
                            {
                                agent->m_user->moveForward(1.0, 0.0);
                            }
                        }

                        { // Run or slow down
                            if ((from_me_2_target_dist > Agent::run_start_dist) && !(agent->m_isRunning))
                            {
                                agent->m_isRunning = true;
                            }

                            if ((from_me_2_target_dist < Agent::run_stop_dist) && (agent->m_isRunning))
                            {
                                agent->m_isRunning = false;
                            }

                            if (agent->m_isRunning)
                            {
                                agent->m_user->shift();
                            }
                        }

                        if (!(agent->m_user->isInCombat()))
                        {
                            agent->m_user->stance();
                        }

                        if (from_me_2_target_dist < Agent::interact_dist)
                        {
                            // attack
                            if (!(agent->m_user->isAttacking() || agent->m_user->isBlocking() || agent->m_user->isDodging()))
                            {
                                agent->m_user->attack(); // brutal AI
                            }

                            // dodge
                            if (agent->m_target->m_user->isAttacking())// if there is need
                            {
//                                std::cout << "I am getting attacked\n";
                                if (!(agent->m_user->isDodging() || agent->m_user->isBlocking())) // if opportunity
                                {
//                                    std::cout << "therefore I dodge\n";
                                    // either block or dodge
                                    if (rand()%10 >=4 ) // 6/10 chance for dodge
                                    {
                                        agent->m_user->dodge();
                                    }
                                    else                // 4/10 chance for parry
                                    {
                                        agent->m_user->attack();
                                    }
                                    agent->wait(0.90); // wait while dodge completes
                                }

                            }
                        }

                    } // face target
                } // if not has a target or is randomly waiting
                else
                {
                    // relax (wait till next tick)
                }

//                if (agent->m_target==nullptr)
//                    std::cout << "no target, doing nothing\n";

//                if (agent->m_randomWait > 0) agent->m_randomWait--;
//                else
//                {
//                    // randomly wait
//                    int res = rand()%10000;
//                    if (res > 9940) // 0.6% chance per tick
//                    {
//                        agent->m_randomWait = rand() % 70; // wait for between 1 and 70 ticks
////                        std::cout << "randomwaiting\n";
//                    }
//                }
                if (agent->m_reactionWait > 0.0) agent->m_reactionWait-=dt;
                else
                {
                    // randomly wait
                    int res = rand()%10000;
                    float prob = dt*0.36; // assuming 36% chance of interupt per second or one interrupt every ~3 secs on avg.
                    if (res > (1-prob)*10000) // 0.6% chance per tick
                    {
//                        int spread_perc = 50 - (rand() % 100); // spread between -50 and +50 % of reaction time
//                        float spread_s = agent->m_reactionTime*((float)spread_perc)/100.f; // convert % to secs
//
//                        float interruptTime = agent->m_reactionTime + spread_s; // add up this specific interruption time
//                        agent->m_reactionWait = interruptTime; // set it
                        agent->interrupt();
                    }
                }
            } // if agent has user
            else
            {
                std::cerr << "error: ai agent has no user\n";
            }
        }


        //
    }
}

World::~World()
{
    for (auto agent : agents)
    {
        delete agent;
    }
}
