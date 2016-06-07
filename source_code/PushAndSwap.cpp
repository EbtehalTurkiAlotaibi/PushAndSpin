/****************************************************************************
Copyright (c) 2013, Ryan Luna, Athanasios Krontiris, Kostas Bekris
All rights reserved.

The Software is available for download and use subject to the terms and
conditions of this License. Access or use of the Software constitutes
acceptance and agreement to the terms and conditions of this License.

Permission to use, copy, modify, and distribute this software and its
documentation for educational, research, and non-profit purposes, without fee,
and without a written agreement is hereby granted.  Permission to incorporate
this software into commercial products may be obtained by contacting the authors.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the names of the authors nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

// Author: Ryan Luna

#include <algorithm>
#include <limits>

#include "PushAndSwap.h"
#include "Logger.h"
#include "iostream"
using namespace std;

// Constant which defines the absence of an agent
static const unsigned int NO_AGENT = std::numeric_limits<unsigned int>::max();
// Constant which defines the absence of a vertex
static const unsigned int NO_VERTEX = std::numeric_limits<unsigned int>::max();

PushAndSwap::PushAndSwap() : graph_(NULL)
{
                reversedmoves =0;
}

PushAndSwap::PushAndSwap(const std::vector<Agent>& agents, Graph* graph)
{
    reversedmoves = 0;
    agents_ = agents;
    graph_ = graph;

    computeSwapVertices();
    reorderAgents();
}

PushAndSwap::~PushAndSwap()
{
}

bool PushAndSwap::solve()
{
    // Sanity checks
    if (agents_.size() == 0)
        return true;
    if (graph_ == NULL)
    {
        std::cout<< ("Graph information is not set");
        return false;
    }
    if (graph_->numNodes() - agents_.size() < 2)
    {
        std::cout<< ("There must be at least two empty vertices in the graph");
        return false;
    }

    // Make sure that the configuration is feasible
    for (size_t i = 0; i < agents_.size(); ++i)
    {
        agents_[i].restart();

        if (agents_[i].getStart() >= graph_->numNodes())
        {
            std::cout<<  ("Agent %u has start position %d which is not on the graph", i, agents_[i].getStart());
            return false;
        }
        if (agents_[i].getGoal() >= graph_->numNodes())
        {
            std::cout<< ("Agent %u has goal position %d which is not on the graph", i, agents_[i].getGoal());
            return false;
        }
    }

    return executePushAndSwap();

}

PushAndSwap::Plan PushAndSwap::getPlan() const
{
    return actions_;
}

void PushAndSwap::addTemporaryAction(unsigned int id, const std::vector<unsigned int>& path)
{
    temp_actions_.push_back(std::make_pair(id, path));
}

// Try to get two empty vertices in the neighborhood of agt and swap_agt around swap_vertex
bool PushAndSwap::clear(unsigned int agt, unsigned int swap_agt, unsigned int swap_vertex)
{
    std::vector<unsigned int> neighbors;
    graph_->getNeighbors(swap_vertex, neighbors);

    std::vector<unsigned int> empty_neighbors;

    for (size_t i = 0; i < neighbors.size(); ++i)
    {
        if (occupied_.find(neighbors[i]) == occupied_.end())
            empty_neighbors.push_back(neighbors[i]);

        // we only need two
        if (empty_neighbors.size () == 2)
            break;
    }

    if (empty_neighbors.size() >= 2) // already have 2 empty neighbors
        return true;

    //////////////////// CLEARING ////////////////////
    int clear_start_index = temp_actions_.size ();

    std::vector <unsigned int> my_neighbors;
    unsigned int pre_push_index;
    std::vector<unsigned int> temp_path;
    std::vector<unsigned int> temp_unpushable;
    temp_unpushable.push_back(agents_[agt].getCurrent());
    temp_unpushable.push_back(agents_[swap_agt].getCurrent());

    // Mark empty neighbors as unpushable
    for (size_t i = 0; i < empty_neighbors.size (); i++) temp_unpushable.push_back (empty_neighbors[i]);

    // Going through all non-empty neighbors - Clear Phase 1
    for (size_t i = 0; i < neighbors.size() && empty_neighbors.size () < 2; i++)
    {
        // Do not try to clear from where the swapping agents are
        if (neighbors[i] == agents_[agt].getCurrent() || neighbors[i] == agents_[swap_agt].getCurrent())
            continue;

        // Finding the agent occupying this vertex
        unsigned int occupied_agt = NO_AGENT;
        for (size_t j = 0; j < agents_.size() && occupied_agt == NO_AGENT; j++)
        {
            if (agents_[j].getCurrent() == neighbors[i])
                occupied_agt = j;
        }

        if (occupied_agt != NO_AGENT)
        {
            unsigned int empty_neighbor = NO_VERTEX;
            if (empty_neighbors.size () == 1) empty_neighbor = empty_neighbors[0];

            // Attempt to push occupied_agt to any neighbor that is not swap_vtx or an empty vertex in neighborhood of swap_vtx
            graph_->getNeighbors(agents_[occupied_agt].getCurrent(), my_neighbors);
            bool successful_push = false;

            for (size_t j = 0; j < my_neighbors.size() && !successful_push; j++)
            {
                if (my_neighbors[j] != agents_[agt].getCurrent() &&
                    my_neighbors[j] != agents_[swap_agt].getCurrent() &&
                    my_neighbors[j] != empty_neighbor)
                {
                    // Push agents_[occupied_agt] to his neighbor
                    unsigned int start_position = agents_[occupied_agt].getCurrent();
                    temp_path.clear ();
                    temp_path.push_back (start_position);
                    temp_path.push_back (my_neighbors[j]);

                    pre_push_index = temp_actions_.size();
                    if (push(occupied_agt, temp_path, temp_unpushable))  // successful push away from vtx
                    {
                        empty_neighbors.push_back (start_position);
                        temp_unpushable.push_back (temp_path[0]);
                        successful_push = true;
                    }
                    else
                        deleteTempActions(pre_push_index);
                }
            }
        }
    }

    bool successful_clear = empty_neighbors.size() == 2;

    // Only one neighbor was freed - Clear Phase 2
    if (empty_neighbors.size () == 1)
    {
        // Push swapping robots away from vtx
        unsigned int agt_on_vtx, agt_on_nbr, neighbor_vtx;
        if (agents_[agt].getCurrent() == swap_vertex)
        {
            agt_on_vtx = agt;
            agt_on_nbr = swap_agt;
            neighbor_vtx = agents_[swap_agt].getCurrent();
        }
        else
        {
            agt_on_vtx = swap_agt;
            agt_on_nbr = agt;
            neighbor_vtx = agents_[agt].getCurrent();
        }

        std::vector<unsigned int> reverse_path;
        reverse_path.push_back(swap_vertex);
        reverse_path.push_back(neighbor_vtx);
        reversedmoves += reverse_path.size();
        int pre_push_index = temp_actions_.size ();
        if (push(agt_on_vtx, reverse_path, empty_neighbors))  // successfully pushed swapping agts backward one vertex.
        {
            temp_unpushable.clear ();
            temp_unpushable.push_back(swap_vertex);
            temp_unpushable.push_back(neighbor_vtx);

            // Pick one occupied neighbor and push through empty_neighbors[0]
            unsigned int occupied_agt = NO_AGENT;

            for (size_t i = 0; i < neighbors.size() && occupied_agt == NO_AGENT; ++i)
            {
                if (neighbors[i] != neighbor_vtx && neighbors[i] != empty_neighbors[0])
                {
                    for (size_t j = 0; j < agents_.size (); j++)
                    {
                        if (agents_[j].getCurrent() == neighbors[i])
                            occupied_agt = j;
                    }
                }
            }

            assert(occupied_agt != NO_AGENT);

            std::vector<unsigned int> push_path;
            push_path.push_back(agents_[occupied_agt].getCurrent());
            push_path.push_back(swap_vertex);
            push_path.push_back(empty_neighbors[0]);

            // Moving occupied robot to the empty neighbor
            addTemporaryAction(agents_[occupied_agt].getID(), push_path);
            occupied_.erase(agents_[occupied_agt].getCurrent());
            agents_[occupied_agt].setCurrent(empty_neighbors[0]);
            occupied_.insert(empty_neighbors[0]);

            // VERY IMPORTANT to add this agent's former location to the unpushable set
            // It can happen that the two neighbors are also connected, and we aren't
            // really clearing in that case
            temp_unpushable.push_back(push_path[0]);

            // Moving swapping robots back
            std::vector<unsigned int> tiny_path;
            tiny_path.push_back(agents_[agt_on_vtx].getCurrent());
            tiny_path.push_back(swap_vertex);
            occupied_.erase(agents_[agt_on_vtx].getCurrent());
            agents_[agt_on_vtx].setCurrent(swap_vertex);
            occupied_.insert(swap_vertex);
            addTemporaryAction(agents_[agt_on_vtx].getID(), tiny_path);

            tiny_path.clear ();
            tiny_path.push_back(agents_[agt_on_nbr].getCurrent());
            tiny_path.push_back(neighbor_vtx);
            occupied_.erase(agents_[agt_on_nbr].getCurrent());
            agents_[agt_on_nbr].setCurrent(neighbor_vtx);
            occupied_.insert(neighbor_vtx);
            addTemporaryAction(agents_[agt_on_nbr].getID(), tiny_path);

            //Check each neighbor of empty_neighbors[0] and try to push
            std::vector <unsigned int> my_neighbors;
            graph_->getNeighbors(empty_neighbors[0], my_neighbors);

            for (size_t i = 0; i < my_neighbors.size() && empty_neighbors.size () < 2; i++)
            {
                if (std::find(temp_unpushable.begin(), temp_unpushable.end(), my_neighbors[i]) == temp_unpushable.end())
                {
                    std::vector<unsigned int> test_path;
                    test_path.push_back (agents_[occupied_agt].getCurrent());
                    test_path.push_back (my_neighbors[i]);

                    int pre_push_index2 = temp_actions_.size ();
                    if (push(occupied_agt, test_path, temp_unpushable))
                    {
                        empty_neighbors.push_back (test_path[0]);
                        successful_clear = true;

                    }
                    else
                        deleteTempActions(pre_push_index2);
                }
            }

            if (!successful_clear)
                deleteTempActions(pre_push_index);
        }
        else
        {
            deleteTempActions(pre_push_index);
        }
    }

    // Clear part 3.  Special case of part 2 for graphs with triangles.
    if (empty_neighbors.size () == 1)
    {
        // Check if there is a neighbor that has an edge with the empty neighbor
        // If so, try to push out that way
        unsigned int empty_neighbor = empty_neighbors[0];

        for (size_t i = 0; i < neighbors.size() && !successful_clear; ++i)
        {
            if (neighbors[i] != empty_neighbor)
            {
                // A triangle exists
                if (graph_->edgeExists(neighbors[i], empty_neighbor))
                {
                    unsigned int occupied_agt = NO_AGENT;
                    for (size_t j = 0; j < agents_.size (); j++)
                    {
                        if (agents_[j].getCurrent() == neighbors[i])
                            occupied_agt = j;
                    }

                    assert(occupied_agt != NO_AGENT);

                    int pre_push_index = temp_actions_.size ();

                    // Moving the occupying agent to the other empty neighbor
                    std::vector<unsigned int> tiny_path;
                    tiny_path.push_back(agents_[occupied_agt].getCurrent());
                    tiny_path.push_back(empty_neighbor);
                    occupied_.erase(agents_[occupied_agt].getCurrent());
                    agents_[occupied_agt].setCurrent(empty_neighbor);
                    occupied_.insert(empty_neighbor);
                    addTemporaryAction(agents_[occupied_agt].getID(), tiny_path);

                    // Try to push it away
                    std::vector <unsigned int> my_neighbors;
                    graph_->getNeighbors(empty_neighbor, my_neighbors);

                    for (size_t j = 0; j < my_neighbors.size() && !successful_clear; ++j)
                    {
                        // Don't push where we just came from
                        if (my_neighbors[j] == neighbors[i]) continue;

                        // Not allowed to push to these vertices:
                        //   Where the agent just came from
                        //   The swap vertex
                        //   The vertex where the other swapping agent is
                        temp_unpushable.clear ();
                        temp_unpushable.push_back(agents_[agt].getCurrent());
                        temp_unpushable.push_back(agents_[swap_agt].getCurrent());
                        temp_unpushable.push_back(neighbors[i]);

                        std::vector<unsigned int> test_path;
                        test_path.push_back (agents_[occupied_agt].getCurrent());
                        test_path.push_back (my_neighbors[j]);

                        int pre_push_index2 = temp_actions_.size ();
                        if (push(occupied_agt, test_path, temp_unpushable))
                        {
                            empty_neighbors.push_back (test_path[0]);
                            successful_clear = true;
                        }
                        else
                            // We failed.  Revert the state of all agents to before the push
                            deleteTempActions(pre_push_index2);
                    }

                    // Move the occupying agent back where he was.  We failed.
                    if (!successful_clear)
                        deleteTempActions(pre_push_index);

                    // We only need to test one triangle.  Others will also fail.
                    break;
                }
            }
        }
    }

    // sanity check
    if (!successful_clear)
        deleteTempActions(clear_start_index);

    return successful_clear;
}

// Uses a BrFS to find the closest empty cell to index 1 in the shortest_path.
// The path to the empty cell is saved in push_path.  Obstacles indicates vertices that we
// should not push through.
void PushAndSwap::computePushPath(unsigned int rbt, const std::vector<unsigned int>& shortest_path,
                                 std::vector<unsigned int>& push_path, const std::vector<unsigned int>& _obstacles)
{
    // Nowhere to push toward...
    if (shortest_path.size () < 2)
        return;

    std::vector<unsigned int> search_obstacles (_obstacles.begin (), _obstacles.end ());
    search_obstacles.push_back (shortest_path[0]);  // Search is not allowed to expand where the push robot currently is

    graph_->BrFS(shortest_path[1], occupied_, push_path, search_obstacles, true);

    // Make sure to add start to the end of the path
    if (push_path.size () > 0)
        push_path.insert(push_path.begin(), shortest_path[0]);
}

// Uses a breadth-first search to find the closest vertex of degree 3 or more
// (excluding previous_attempts) to the start vertex.  The path to the new
// swap vertex is saved to swap_path.
void PushAndSwap::computeSwapPath (unsigned int start, std::vector <unsigned int> &swap_path,
                                   const std::vector<unsigned int> &previous_attempts)
{
    std::vector<unsigned int> possible_swap_goals(swap_vertices_.size ());
    std::vector<unsigned int>::iterator iter = possible_swap_goals.begin();
    iter = std::set_difference (swap_vertices_.begin(), swap_vertices_.end(),
                                previous_attempts.begin(), previous_attempts.end(),
                                possible_swap_goals.begin());

    std::set<unsigned int> swap_goals(possible_swap_goals.begin(), iter);
    graph_->BrFS(start, swap_goals, swap_path);
}

void PushAndSwap::computeSwapVertices(void)
{
    // Precompute all vertices with degree > 2
    swap_vertices_.clear ();
    for (size_t i = 0; i < graph_->numNodes(); i++)
    {
        if (graph_->getNumNeighbors(i) > 2)
            swap_vertices_.insert(i);
    }
}

// Removes all actions from temp_actions_ starting at start_index
void PushAndSwap::deleteTempActions(unsigned int start_index)
{
    if (temp_actions_.size() == 0)
        return;

    assert(temp_actions_.size() >= start_index);

    unsigned int ts_index = temp_actions_.size ()-1;
    while (ts_index != start_index-1)
    {
        const std::vector<unsigned int>& original_path = temp_actions_[ts_index].second;

        unsigned int agt_index = NO_AGENT;
        for (size_t i = 0; i < agents_.size() && agt_index == NO_AGENT; i++)
        {
            if (temp_actions_[ts_index].first == agents_[i].getID())
                agt_index = i;
        }

        assert(agents_[agt_index].getCurrent() == original_path.back ());
        occupied_.erase(agents_[agt_index].getCurrent());
        agents_[agt_index].setCurrent(original_path.front());
        occupied_.insert(original_path.front());
        --ts_index;
    }

    while (temp_actions_.size () != start_index)
        temp_actions_.pop_back ();

    assert(temp_actions_.size () == start_index);
}

// Push and Swap outer loop.  Iterate over all agents: push and then swap.  Repeat until at goal.
bool PushAndSwap::executePushAndSwap()
{
    occupied_.clear();
    for (size_t i = 0; i < agents_.size (); i++)
        occupied_.insert(agents_[i].getCurrent());

    actions_.clear();
    temp_actions_.clear();

    std::vector<unsigned int> shortest_path;
    for (size_t i = 0; i < agents_.size (); i++)
    {
        if (agents_[i].getCurrent() == agents_[i].getGoal ())
            unpushable_.push_back (agents_[i].getCurrent());

        // Push and Swap until agent i is at its goal
        while (agents_[i].getCurrent() != agents_[i].getGoal())
        {
            // Check if there is a path which moves through the free space first
            shortest_path.clear();
            graph_->AStar(agents_[i].getCurrent(), agents_[i].getGoal(), shortest_path, unpushable_);

            if (shortest_path.size() == 0) // No path through free space.  Check all vertices.
                graph_->AStar(agents_[i].getCurrent(), agents_[i].getGoal(), shortest_path);

            if (shortest_path.size() == 0) // No path at all for agent i to get to its goal
                return false;

            // Push
            if (!push(i, shortest_path, unpushable_))
            {
                // Ensure that agent i always moves along the same path
                while (shortest_path[0] != agents_[i].getCurrent())
                    shortest_path.erase(shortest_path.begin ());

                // Swap
                if (!swap(i, shortest_path))
                {
                    // swap failed, there is no solution
                    return false;
                }
            }
        }

        if (agents_[i].getCurrent() == agents_[i].getGoal())
        {
            unpushable_.push_back(agents_[i].getGoal());

            actions_.insert (actions_.end(), temp_actions_.begin(), temp_actions_.end());
            temp_actions_.clear ();
        }
    }
    return true;
}

// Performs a swap between agt and swap_agt at the given swap_vertex.
// This method assumes that agt and swap_agt are adjacent, one of
// them currently occupies swap_vertex, and there are at least two free
// vertices in the neighborhood of swap_vertex.
void PushAndSwap::executeSwap (unsigned int agt, unsigned int swap_agt,
                               unsigned int swap_vertex)
{
    std::vector <unsigned int> neighbors;
    graph_->getNeighbors(swap_vertex, neighbors);
    std::vector <unsigned int> empty_swap_neighbors;
    for (size_t i = 0; i < neighbors.size(); i++)
    {
        if (occupied_.find(neighbors[i]) == occupied_.end())
            empty_swap_neighbors.push_back(neighbors[i]);

        // we only need two
        if (empty_swap_neighbors.size () == 2)
            break;
    }

    assert (empty_swap_neighbors.size () >= 2);

    int agt1_start = agents_[agt].getCurrent();
    int agt2_start = agents_[swap_agt].getCurrent();

    // swap
    // moving agent 1 to an empty swap neighbor
    std::vector <unsigned int> tiny_path;
    tiny_path.push_back (agents_[agt].getCurrent()); // swap_vertex
    tiny_path.push_back (empty_swap_neighbors[0]);
    addTemporaryAction(agents_[agt].getID(), tiny_path);
    agents_[agt].setCurrent(empty_swap_neighbors[0]);

    // moving agent 2 to a different empty swap neighbor
    tiny_path.clear ();
    tiny_path.push_back (agents_[swap_agt].getCurrent());
    tiny_path.push_back (swap_vertex);
    tiny_path.push_back (empty_swap_neighbors[1]);
    addTemporaryAction(agents_[swap_agt].getID(), tiny_path);
    agents_[swap_agt].setCurrent(empty_swap_neighbors[1]);


    // Move agent 1 to agent 2's original position
    tiny_path.clear ();
    tiny_path.push_back (agents_[agt].getCurrent());
    tiny_path.push_back (swap_vertex);
    tiny_path.push_back (agt2_start);
    addTemporaryAction(agents_[agt].getID(), tiny_path);
    agents_[agt].setCurrent(agt2_start);

    // Move robot 2 to robot 1's original start (swap_vertex);
    tiny_path.clear ();
    tiny_path.push_back (agents_[swap_agt].getCurrent());
    tiny_path.push_back (swap_vertex);
    addTemporaryAction(agents_[swap_agt].getID(), tiny_path);
    agents_[swap_agt].setCurrent(agt1_start);
}

// "Pushes" agt along the "shortest_path", pushing agents that do not exist
// in 'unpushable' vertices out of the way when possible.
// tag_along is true when this is a compound push where two agents need to be pushed together.
// 'only_empty' is true if we want to push only along empty vertices.
bool PushAndSwap::push (unsigned int agt, std::vector<unsigned int> shortest_path,
                        const std::vector<unsigned int> &unpushable, unsigned int tag_along, bool only_empty)
{
    assert(shortest_path.size() > 0);
    unsigned int goal = shortest_path.back();

    // Move along the path while it is free
    unsigned int start = shortest_path[0];
    for (size_t i = 1; i < shortest_path.size(); ++i)
    {
        if (occupied_.find(shortest_path[i]) == occupied_.end())
            start = shortest_path[i];
        else
            break;
    }

    // Path was totally free
    if (start == goal)
    {
        // Adding this action to the action set
        addTemporaryAction(agents_[agt].getID(), shortest_path);
        if (tag_along != NO_AGENT)
        {
            std::vector<unsigned int> tag_along_path;
            tag_along_path.push_back (agents_[tag_along].getCurrent());
            for (size_t i = 0; i < shortest_path.size ()-1; i++)
                tag_along_path.push_back (shortest_path[i]);

            addTemporaryAction(agents_[tag_along].getID(), tag_along_path);
            occupied_.erase(agents_[tag_along].getCurrent());
            agents_[tag_along].setCurrent(tag_along_path.back());
            occupied_.insert(agents_[tag_along].getCurrent());
        }
    }
    // Add the actions that moved along the empty portion of the path
    else
    {
        std::vector <unsigned int> partial_path;
        int index = 0;
        while (shortest_path[index] != start)
            partial_path.push_back(shortest_path[index++]);
        partial_path.push_back(shortest_path[index]);

        if (partial_path.size () > 1)
        {
            addTemporaryAction(agents_[agt].getID(), partial_path);
            if (tag_along != NO_AGENT)
            {
                std::vector<unsigned int> tag_along_path;
                tag_along_path.push_back (agents_[tag_along].getCurrent());
                for (size_t i = 0; i < partial_path.size ()-1; i++)
                    tag_along_path.push_back (partial_path[i]);

                addTemporaryAction(agents_[tag_along].getID(), tag_along_path);
                occupied_.erase(agents_[tag_along].getCurrent());
                agents_[tag_along].setCurrent(tag_along_path.back());
                occupied_.insert(agents_[tag_along].getCurrent());
            }
        }
    }

    // If this is a compound push, tag_along could already be at agt position.  Check this.
    if (tag_along == NO_AGENT || agents_[tag_along].getCurrent() != agents_[agt].getCurrent())
        occupied_.erase(agents_[agt].getCurrent());
    agents_[agt].setCurrent(start);
    occupied_.insert(agents_[agt].getCurrent());

    if (start == goal || only_empty)
        return true;

    while (shortest_path[0] != agents_[agt].getCurrent())
        shortest_path.erase (shortest_path.begin());

    // Now, find the shortest path from an empty node to agt's start position
    // with NO unpushable agent along the way (these are "blocked")
    std::vector <unsigned int> shortest_push_path;
    computePushPath (agt, shortest_path, shortest_push_path, unpushable);

    // We can not push any more
    if (shortest_push_path.empty())
        return false;

    // The path must have more than one vertex in it
    assert(shortest_push_path.size () != 1);

    bool tag_along_behind = false;
    if (tag_along != NO_AGENT && agents_[tag_along].getCurrent() != shortest_push_path[1])
        tag_along_behind = true;

    // Pushing all agents along the path...
    for (int i = shortest_push_path.size()-2; i >= 0; --i)
    {
        bool found = false;
        for (size_t j = 0; j < agents_.size () && !found; j++)
        {
            if (agents_[j].getCurrent() == shortest_push_path[i])
            {
                // Move this robot to shortest_push_path[i-1]
                std::vector<unsigned int> tiny_path;
                tiny_path.push_back(agents_[j].getCurrent());
                tiny_path.push_back(shortest_push_path[i+1]);
                addTemporaryAction(agents_[j].getID(), tiny_path);

                agents_[j].setCurrent(shortest_push_path[i+1]);
                found = true;
            }
        }
    }

    // The end of the path is the empty vertex.  Add this vertex as occupied.  Removed beginning of path from occupied.
    occupied_.erase(shortest_push_path.front());
    occupied_.insert(shortest_push_path.back());

    if (tag_along_behind) // we are not pushing the compound agent; it is behind us
    {
        std::vector<unsigned int> tiny_path;
        tiny_path.push_back(agents_[tag_along].getCurrent());
        tiny_path.push_back(shortest_push_path.front());
        addTemporaryAction(agents_[tag_along].getID(), tiny_path);

        occupied_.erase(agents_[tag_along].getCurrent());
        agents_[tag_along].setCurrent(shortest_push_path.front());
        occupied_.insert(agents_[tag_along].getCurrent());
    }

    // Recurse
    if (agents_[agt].getCurrent() == goal)
        return true;
    else
    {
        while (shortest_path[0] != agents_[agt].getCurrent())
            shortest_path.erase (shortest_path.begin ());

        assert(shortest_path.size () > 0);
        return push(agt, shortest_path, unpushable, tag_along, false);
    }
}

// Pushes rbt and swap_rbt along the path
bool PushAndSwap::pushToSwapVertex (unsigned int agt, unsigned int swap_agt, std::vector<unsigned int>& path)
{
    // We are already there
    if (path.size () <= 1)
        return true;

    std::vector<unsigned int> empty;
    return push(agt, path, empty, swap_agt);
}

// Change the order of agents in the agent list so that the agents with
// goals furthest away from the root of the MST of the graph appear first
void PushAndSwap::reorderAgents(void)
{
    // Root of MST will be swap_vertices_[0]
    if (swap_vertices_.size() == 0)
        return;

    std::vector<unsigned int> goals;
    for (size_t i = 0; i < agents_.size(); ++i)
        goals.push_back(agents_[i].getGoal());

    std::vector<unsigned int> predecessors;
    graph_->getMinSpanningTree(*(swap_vertices_.begin()), predecessors);

    std::map<unsigned int, double> distancemap;
    for (size_t i = 0; i < goals.size(); ++i)
    {
        double dist = 0.0;
        unsigned int v = goals[i];
        while (predecessors[v] != v)
        {
            dist += graph_->edgeWeight(v, predecessors[v]);
            v = predecessors[v];
        }
        distancemap[goals[i]] = dist;
    }

    std::vector<unsigned int> newOrdering;
    // Insertion sort into output list
    for (size_t i = 0; i < goals.size(); ++i)
    {
        // Output will contain indices into the input vector.
        newOrdering.push_back(int(i));

        // Insertion sort:
        for (int j = 0; j < (int)i; ++j)
        {
            // Vertices with larger distance from the root come first
            if (distancemap[goals[j]] <= distancemap[goals[i]])
            {
                // Shifting all members down one:
                for (int k = newOrdering.size()-2; k >= j; --k)
                    newOrdering[k+1] = newOrdering[k];
                newOrdering[j] = (int)i;

                break;
            }
        }
    }

    std::vector<Agent> newAgents(agents_.size(), Agent(0,0,0));
    for (size_t i = 0; i < agents_.size(); ++i)
        newAgents[i] = agents_[newOrdering[i]];

    agents_ = newAgents;
}

// Reverses the set of actions used to swap agt with swap_agt.
// start_index is the index of the action set before the swap actions were performed.
void PushAndSwap::reversePaths (unsigned int agt, unsigned int swap_agt, unsigned int start_index)
{
    // If there are exactly four actions, there is nothing to reverse
    if (temp_actions_.size() == 4)
        return;

    assert(temp_actions_.size() > 4);
    unsigned int ts_index = temp_actions_.size ()-5;
    while (ts_index != start_index)
    {
        std::vector<unsigned int> original_path = temp_actions_[ts_index].second;
        std::vector<unsigned int> reverse_path (original_path.rbegin(), original_path.rend());

        assert(original_path.size() != 0);

        // Make sure to exchange paths for agt and swap_agt
        if (temp_actions_[ts_index].first == agents_[agt].getID())
        {
            addTemporaryAction(agents_[swap_agt].getID(), reverse_path);
            reversedmoves += reverse_path.size();
            occupied_.erase(agents_[swap_agt].getCurrent());
            agents_[swap_agt].setCurrent(original_path.front());
            occupied_.insert(original_path.front());
        }

        else if (temp_actions_[ts_index].first == agents_[swap_agt].getID())
        {
            addTemporaryAction(agents_[agt].getID(), reverse_path);
            reversedmoves += reverse_path.size();
            occupied_.erase(agents_[agt].getCurrent());
            agents_[agt].setCurrent(original_path.front());
            occupied_.insert(original_path.front());
        }
        else
        {
            unsigned int agt_id = temp_actions_[ts_index].first;
            unsigned int agt_index = NO_AGENT;
            for (unsigned int i = 0; i < agents_.size () && agt_index == NO_AGENT; i++)
            {
                if (agents_[i].getID() == agt_id)
                    agt_index = i;
            }
            assert(agt_index != NO_AGENT);

            addTemporaryAction(agt_id, reverse_path);
            reversedmoves += reverse_path.size();
            occupied_.erase(agents_[agt_index].getCurrent());
            agents_[agt_index].setCurrent(original_path.front());
            occupied_.insert(original_path.front());
        }
        --ts_index;
    }
}

// This resolves the condition where agt swaps with swap_agt, who was already at its goal.
bool PushAndSwap::resolve(unsigned int agt, unsigned int swap_agt,
                          std::vector <unsigned int> shortest_path,
                          std::vector <unsigned int> _unpushable)
{
    // Try a simple push
    std::vector <unsigned int> tiny_path;
    if (agents_[agt].getCurrent() == shortest_path[1]) // called from swap
    {
        tiny_path.push_back (agents_[agt].getCurrent());
        if (shortest_path.size() > 2)
            tiny_path.push_back (shortest_path[2]);
    }

    // unpushable is the set of agents at their goal except for swap_agt
    std::vector <unsigned int> temp_unpushable;
    for (size_t i = 0; i < _unpushable.size (); i++)
    {
        if (_unpushable[i] != agents_[swap_agt].getGoal ())
            temp_unpushable.push_back (_unpushable[i]);
    }

    // add back swap_agt at its current position
    if (std::find(_unpushable.begin(), _unpushable.end(), agents_[swap_agt].getCurrent()) == _unpushable.end())
        temp_unpushable.push_back (agents_[swap_agt].getCurrent());

    // Push agt one more step.  If success, move swap_agt
    unsigned int pre_push_index = temp_actions_.size ();
    if (shortest_path.size () >= 3 && push(agt, tiny_path, temp_unpushable))
    {
        tiny_path.clear ();
        tiny_path.push_back (agents_[swap_agt].getCurrent());
        tiny_path.push_back (agents_[swap_agt].getGoal());

        return push(swap_agt, tiny_path, unpushable_);
    }
    else
    {
        // swap agt along path
        deleteTempActions(pre_push_index);

        tiny_path.clear ();
        tiny_path.push_back (agents_[swap_agt].getCurrent());
        tiny_path.push_back (agents_[swap_agt].getGoal());

        while (shortest_path[0] != agents_[agt].getCurrent())
            shortest_path.erase(shortest_path.begin());

        // Swap agt one more time
        if (swap(agt, shortest_path))
        {
            if (push(swap_agt, tiny_path, unpushable_) || swap(swap_agt, tiny_path))
            {
                // At this point, swap_agt should be back at goal
                return true;
            }
        }
    }
    return false;
}

// Top level swap method.  Attempts to swap rbt with the robot adjacent to it along its
// shortest path.  Invokes the clear, multipush, and reverse actions for a successful swap.
bool PushAndSwap::swap(unsigned int agt, const std::vector <unsigned int> &shortest_path)
{
    assert(shortest_path.size () > 1);

    unsigned int swap_agent = NO_AGENT;
    for (size_t i = 0; i < agents_.size () && swap_agent == NO_AGENT; i++)
    {
        if (agents_[i].getCurrent() == shortest_path[1])
            swap_agent = i;
    }

    assert(swap_agent != NO_AGENT);

    std::vector <unsigned int> attempted_swap_vertices;
    bool successful_swap = false;

    // Iterating over all possible swap vertices
    while (!successful_swap && attempted_swap_vertices.size () != swap_vertices_.size ())
    {
        // Finding the swap vertex
        std::vector <unsigned int> swap_path;
        computeSwapPath(agents_[agt].getCurrent(), swap_path, attempted_swap_vertices);

        if (swap_path.size () == 0) // No paths to any swap vertices
            return false;

        unsigned int swap_vertex = swap_path.back();
        assert(swap_vertices_.find(swap_path.back()) != swap_vertices_.end());

        unsigned int sequence_index_start_push_swap = temp_actions_.size()-1;

        // Pushing to swap vertex and clearing two neighbors
        if (pushToSwapVertex(agt, swap_agent, swap_path) &&
            clear(agt, swap_agent, swap_vertex))
        {
            // Swapping the two agents' positions
            executeSwap(agt, swap_agent, swap_vertex);
            successful_swap = true;

            // Reversing paths to maintain invariant
            reversePaths(agt, swap_agent, sequence_index_start_push_swap);
        }
        else
        {
            // mark this swap vertex as a failure
            attempted_swap_vertices.push_back(swap_vertex);
            std::sort(attempted_swap_vertices.begin (), attempted_swap_vertices.end ());

            // Start over (reverse everybody out and delete from temporary sequence)
            deleteTempActions(sequence_index_start_push_swap+1);
        }
    }

    // if swap_agent is unmoveable, push him BACK to his goal
    if (successful_swap && std::find(unpushable_.begin(), unpushable_.end(),  agents_[swap_agent].getGoal()) != unpushable_.end())
    {
        assert(shortest_path.size () >= 2);
        assert(shortest_path[1] == agents_[agt].getCurrent());
        assert(agents_[swap_agent].getCurrent() == shortest_path[0]);
        return resolve(agt, swap_agent, shortest_path, unpushable_);
    }
    return successful_swap;
}

bool pathIsClear(const PushAndSwap::Plan& plan, int index, unsigned int endpoint, int &plan_index, int &path_index)
{
    if (index == 0)
        return false;

    // Working backwards from simulation[index][endpoint] and find the next occurrance of endpoint
    for (int i = index -1; i >= 0; i--)
    {
        std::vector<unsigned int> path = plan[i].second;
        for (size_t j = 0; j < path.size (); j++)
        {
            // Found the next occurrance
            if (path[j] == endpoint)
            {
                // and its ours!
                if (plan[i].first == plan[index].first)
                {
                    plan_index = i;
                    path_index = j;
                    return true;
                }
                // Somebody else is blocking the path.  Fail.
                else
                    return false;
            }
        }
    }

    // Never found an occurrance of endpoint - return false
    return false;
}

PushAndSwap::Plan PushAndSwap::smooth(const PushAndSwap::Plan& plan)
{
    PushAndSwap::Plan smoothed = plan;

    int plan_index, path_index, diff;
    bool pruned;

    do
    {
        pruned = false;
        for (int i = smoothed.size ()-1; i >= 0; i--)
        {
            std::vector<unsigned int> current_path = smoothed[i].second;
            unsigned int agent = smoothed[i].first;
            if (pathIsClear (smoothed, i, current_path.back(), plan_index, path_index))
            {
                diff = i - plan_index;

                // Trimming the path_index path first
                std::vector <unsigned int> new_path = smoothed[plan_index].second;

                int temp = 0;
                for (std::vector<unsigned int>::iterator iter = new_path.begin (); iter != new_path.end (); temp++)
                {
                   if (temp <= path_index)
                   {
                      ++iter;
                      continue;
                   }

                   iter = new_path.erase (iter);
                }
                smoothed[plan_index].second = new_path;

                // Removing all paths for agent smooth[i].first until path_index

                temp = 0;
                int iterated = 0;
                int deleted = 0;
                for (Plan::iterator iter = smoothed.begin (); iter != smoothed.end (); temp++)
                {
                   if (temp <= plan_index)
                   {
                      ++iter;
                      continue;
                   }

                   if (iter->first == agent)
                   {
                        iter = smoothed.erase (iter);
                        deleted++;
                   }
                   else
                       ++iter;

                   iterated++;
                   if (iterated >= diff)
                       break;
                }
                pruned = true;
                break;
            }
        }
    } while (pruned);

    return smoothed;
}
