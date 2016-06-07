/* Pavel Surynek, 2008 */
/* pavel.surynek@mff.cuni.cz */
/* bibox.cpp */

/*
   This is an implementation of the multi-robot path planning algorithm for biconnected environments with
   at least two free places. The code is not commented however it is simple enough to be understood
   without comments.

   The problem is given as an undirected graph, where each vertex of the graph is either free or
   occupied by a robot. A robot can move to a neighboring unoccupied vertex supposed that no other
   robot is simultaneously entering the target vertex. The task is to move robots preserving this
   condition to their final positions.

   This program is an implementation of the algorithm for solving the problem where the graph is
   biconnected and threre are at least two unoccupied vertices in the graph. The program runs in
   polynomial time with respect to the size of the input graph. More precisely it runs in O(|V|^3)
   where G=(V,E) is the input biconnected graph.
 */


#define UNUSED(X)
#include <vector>
#include <list>
#include <set>
#include <map>
#include <string>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <climits>
#include <cstdlib>
#include <qmessagebox.h>
#include <iostream>
#include "version.h"
#include "DataStructures.h"
#include "string"
typedef std::list<int> IDS_list;
typedef std::set<int, std::less<int> > IDS_set;

int MOVE_COUNT = 0;
int STEP_COUNT = 0;
string GraphRec;
bool INDEPENDENT;
int CURRENT_CYCLE;

class CYCLE
{
public:
    int m_x_id, m_y_id;
    IDS_list m_vertices;
    IDS_list m_loop;
};


typedef std::vector<CYCLE> CYCLES_vector;


class VERTEX
{
public:
    bool operator==(const VERTEX &vertex) const
    {
        return (m_id == vertex.m_id);
    }

    bool operator<(const VERTEX &vertex) const
    {
        return (m_id < vertex.m_id);
    }

    void print(void) const
    {
        printf("(%d:%d)[%d:%d:%d] ", m_id, m_cyc_id, m_robot, m_final, m_goal);

        for (IDS_list::const_iterator neighbor = m_neighbors.begin(); neighbor != m_neighbors.end(); ++neighbor)
        {
            printf("%d ", *neighbor);
        }
        printf("\n");
    }
public:
    int m_id;
    IDS_list m_neighbors;

// Shortest path specific members
    bool m_active;
    int m_val;
    int m_prev_id;

// Cycle decomposition specific members
    int m_cyc_id;

// Robots specific members
    int m_robot;
    int m_final;
    int m_goal;

// Algorithm specific members
    int m_affected;
};


class EDGE
{
public:
    bool operator==(const EDGE &edge) const
    {
        return (m_U_id == edge.m_U_id && m_V_id == edge.m_V_id);
    }

    bool operator<(const EDGE &edge) const
    {
        return ((m_U_id < edge.m_U_id) ? true : ((m_U_id > edge.m_U_id) ? false : ((m_V_id < edge.m_V_id) ? true : ((m_V_id > edge.m_V_id) ? false : false))));
    }

    void print(void) const
    {
        printf("{%d,%d}\n", m_U_id, m_V_id);
    }

public:
    int m_U_id;
    int m_V_id;

// 	Cycle decomposition specific members
    int m_cyc_id;
};


typedef std::map<int, VERTEX, std::less<int> > VERTICES_map;
typedef std::set<EDGE, std::less<EDGE> > EDGES_set;

typedef std::set<int, std::less<int> > VERTICES_set;


enum ORIENTATION
{
    UN,
    CW,
    CCW
};


class GRAPH
{
public:
    void add_vertex(int id)
    {
        VERTEX &vertex = m_vertices[id];

        vertex.m_id = id;
        vertex.m_cyc_id = 0;
        vertex.m_robot = 0;
        vertex.m_final = 0;
        vertex.m_affected = -1;
    }


    void add_vertex(int id, int cyc_id)
    {
        VERTEX &vertex = m_vertices[id];

        vertex.m_id = id;
        vertex.m_cyc_id = cyc_id;
        vertex.m_robot = 0;
        vertex.m_final = 0;
        vertex.m_goal = 0;
        vertex.m_affected = -1;
    }

    void add_vertex(int id, int cyc_id, int robot, int final)
    {
        VERTEX &vertex = m_vertices[id];

        vertex.m_id = id;
        vertex.m_cyc_id = cyc_id;
        vertex.m_robot = robot;
        vertex.m_final = final;
        vertex.m_goal = final;
        vertex.m_affected = -1;
    }

    void add_edge(int U_id, int V_id)
    {
        EDGE e;
        e.m_U_id = U_id;
        e.m_V_id = V_id;
        e.m_cyc_id = 0;

        m_edges.insert(e);
        m_vertices[U_id].m_neighbors.push_back(V_id);
        m_vertices[V_id].m_neighbors.push_back(U_id);
    }

    void add_edge(int U_id, int V_id, int cyc_id)
    {
        EDGE e;
        e.m_U_id = U_id;
        e.m_V_id = V_id;
        e.m_cyc_id = cyc_id;

        m_edges.insert(e);
        m_vertices[U_id].m_neighbors.push_back(V_id);
        m_vertices[V_id].m_neighbors.push_back(U_id);
    }

    void print(void) const
    {
        printf("V = \n");
        for (VERTICES_map::const_iterator vertex = m_vertices.begin(); vertex != m_vertices.end(); ++vertex)
        {
            vertex->second.print();
        }

        printf("E = \n");
        for (EDGES_set::const_iterator edge = m_edges.begin(); edge != m_edges.end(); ++edge)
        {
            edge->print();
        }

        printf("C = \n");
        unsigned int i;
        for (i = 0; i < m_cycles.size(); ++i)
        {
            printf("%d (%d,%d): ", i, m_cycles[i].m_x_id, m_cycles[i].m_y_id);

            for (std::list<int>::const_iterator vertex = m_cycles[i].m_vertices.begin(); vertex != m_cycles[i].m_vertices.end(); ++vertex)
            {
                printf("%d ", *vertex);
            }
            printf(" [");
            for (std::list<int>::const_iterator vertex = m_cycles[i].m_loop.begin(); vertex != m_cycles[i].m_loop.end(); ++vertex)
            {
                printf("%d ", *vertex);
            }
            printf("]\n");
        }
    }

public:
    VERTICES_map m_vertices;
    EDGES_set m_edges;

// Cycle decomposition specific members
    CYCLES_vector m_cycles;
};


std::list<std::pair<int, int> > Independent;

void swap_robots(int a_id, int b_id, GRAPH *graph)
{
    int temp;

    VERTEX *a = &graph->m_vertices[a_id];
    VERTEX *b = &graph->m_vertices[b_id];

    if (a->m_affected == CURRENT_CYCLE + 1 || b->m_affected == CURRENT_CYCLE + 1)
    {
        INDEPENDENT = false;
    }

    a->m_affected = CURRENT_CYCLE;
    b->m_affected = CURRENT_CYCLE;

    temp = a->m_robot;
    a->m_robot = b->m_robot;
    b->m_robot = temp;

    if (a->m_robot > 0 || b->m_robot > 0)
    {
        bool independent = true;

        for (std::list<std::pair<int, int> >::const_iterator ind = Independent.begin(); ind != Independent.end(); ++ind)
        {
            if (ind->first == a_id || ind->first == b_id || ind->second == a_id || ind->second == b_id)
            {
                independent = false;
                break;
            }
        }
        if (!independent)
        {
            Independent.clear();
            ++STEP_COUNT;
        }

        printf("%d --> %d\n", b_id, a_id);
        FILE *f ;
        f = fopen(GraphRec.c_str(), "ab");
        fprintf(f, "%d ---> %d [0 ---> 0] (%d)\n", b_id, a_id, MOVE_COUNT);
        fclose(f);
        ++MOVE_COUNT;

        Independent.push_back(std::pair<int, int>(a_id, b_id));
    }

    assert(a->m_robot <= 0 || b->m_robot <= 0);
}


void swap_finals(int a_id, int b_id, GRAPH *graph)
{
    int temp;

    temp = graph->m_vertices[a_id].m_final;
    graph->m_vertices[a_id].m_final = graph->m_vertices[b_id].m_final;
    graph->m_vertices[b_id].m_final = temp;
}


void swap_goals(int a_id, int b_id, GRAPH *graph)
{
    int temp;

    temp = graph->m_vertices[a_id].m_goal;
    graph->m_vertices[a_id].m_goal = graph->m_vertices[b_id].m_goal;
    graph->m_vertices[b_id].m_goal = temp;
}


VERTICES_map::const_iterator find_robot(int robot, GRAPH *graph)
{
    VERTICES_map::const_iterator r = graph->m_vertices.end();

    for (r = graph->m_vertices.begin(); r != graph->m_vertices.end(); ++r)
    {
        if (r->second.m_robot == robot)
        {
            break;
        }
    }

    return r;
}


VERTICES_map::const_iterator find_final(int final, GRAPH *graph)
{
    VERTICES_map::const_iterator r = graph->m_vertices.end();

    for (r = graph->m_vertices.begin(); r != graph->m_vertices.end(); ++r)
    {
        if (r->second.m_final == final)
        {
            break;
        }
    }

    return r;
}


void rotate_cycle_CW(int free, CYCLE *cycle, GRAPH *graph)
{
    IDS_list::reverse_iterator f, p1, p2;

    for (f = cycle->m_vertices.rbegin(); f != cycle->m_vertices.rend(); ++f)
    {
        if (graph->m_vertices[*f].m_robot == free)
        {
            break;
        }
    }

    p1 = f;
    p2 = p1;
    ++p2;

    while (p2 != cycle->m_vertices.rend())
    {
        swap_robots(*p1, *p2, graph);
        ++p1;
        ++p2;
    }
    p2 = cycle->m_vertices.rbegin();
    swap_robots(*p1, *p2, graph);

    p1 = cycle->m_vertices.rbegin();
    ++p2;

    while (p1 != f)
    {
        swap_robots(*p1, *p2, graph);
        ++p1;
        ++p2;
    }
}


void rotate_cycle_CW(CYCLE *cycle, GRAPH *graph)
{
    rotate_cycle_CW(0, cycle, graph);
}


void rotate_cycle_CCW(int free, CYCLE *cycle, GRAPH *graph)
{
    IDS_list::iterator f, p1, p2;

    for (f = cycle->m_vertices.begin(); f != cycle->m_vertices.end(); ++f)
    {
        if (graph->m_vertices[*f].m_robot == free)
        {
            break;
        }
    }
    assert(f != cycle->m_vertices.end());

    p2 = p1 = f;
    ++p2;

    while (p2 != cycle->m_vertices.end())
    {
        swap_robots(*p1, *p2, graph);
        ++p1;
        ++p2;
    }
    p2 = cycle->m_vertices.begin();
    swap_robots(*p1, *p2, graph);

    p1 = cycle->m_vertices.begin();
    ++p2;

    while (p1 != f)
    {
        swap_robots(*p1, *p2, graph);
        ++p1;
        ++p2;
    }
}


void rotate_cycle_CCW(CYCLE *cycle, GRAPH *graph)
{
    rotate_cycle_CCW(0, cycle, graph);
}


void half_rotate_cycle_CW(int robot, int id, int free, CYCLE *cycle, GRAPH *graph)
{
    IDS_list::reverse_iterator f, p1, p2;

    for (f = cycle->m_vertices.rbegin(); f != cycle->m_vertices.rend(); ++f)
    {
        if (graph->m_vertices[*f].m_robot == free)
        {
            break;
        }
    }

    p1 = f;
    p2 = p1;
    ++p2;

    while (p2 != cycle->m_vertices.rend())
    {
        swap_robots(*p1, *p2, graph);

        if (graph->m_vertices[id].m_robot == robot)
        {
            return;
        }

        ++p1;
        ++p2;
    }
    p2 = cycle->m_vertices.rbegin();
    swap_robots(*p1, *p2, graph);

    if (graph->m_vertices[id].m_robot == robot)
    {
        return;
    }

    p1 = cycle->m_vertices.rbegin();
    ++p2;

    while (p1 != f)
    {
        swap_robots(*p1, *p2, graph);

        if (graph->m_vertices[id].m_robot == robot)
        {
            return;
        }

        ++p1;
        ++p2;
    }
}


void half_rotate_cycle_CW(int robot, int id, CYCLE *cycle, GRAPH *graph)
{
    half_rotate_cycle_CW(robot, id, 0, cycle, graph);
}


void half_rotate_cycle_CCW(int robot, int id, int free, CYCLE *cycle, GRAPH *graph)
{
    IDS_list::iterator f, p1, p2;

    for (f = cycle->m_vertices.begin(); f != cycle->m_vertices.end(); ++f)
    {
        if (graph->m_vertices[*f].m_robot == free)
        {
            break;
        }
    }
    assert(f != cycle->m_vertices.end());

    p2 = p1 = f;
    ++p2;

    while (p2 != cycle->m_vertices.end())
    {
        swap_robots(*p1, *p2, graph);

        if (graph->m_vertices[id].m_robot == robot)
        {
            return;
        }

        ++p1;
        ++p2;
    }
    p2 = cycle->m_vertices.begin();
    swap_robots(*p1, *p2, graph);

    if (graph->m_vertices[id].m_robot == robot)
    {
        return;
    }

    p1 = cycle->m_vertices.begin();
    ++p2;

    while (p1 != f)
    {
        swap_robots(*p1, *p2, graph);

        if (graph->m_vertices[id].m_robot == robot)
        {
            return;
        }

        ++p1;
        ++p2;
    }
}


void half_rotate_cycle_CCW(int robot, int id, CYCLE *cycle, GRAPH *graph)
{
    half_rotate_cycle_CCW(robot, id, 0, cycle, graph);
}


ORIENTATION calc_orientation(int U_id, int V_id, CYCLE *cycle)
{
    IDS_list::const_iterator u, v;

    for (u = cycle->m_vertices.begin(); u != cycle->m_vertices.end(); ++u)
    {
        if (U_id == *u)
        {
            break;
        }
    }

    v = u;
    ++v;

    if (v == cycle->m_vertices.end())
    {
        v = cycle->m_vertices.begin();
    }

    if (V_id == *v)
    {
        return CW;
    }
    else
    {
        return CCW;
    }
}


void find_shortest_path(int source_id, int sink_id, GRAPH *graph, IDS_set &fvertices, IDS_set &fcycles)
{
    VERTICES_map::iterator v;

    for (v = graph->m_vertices.begin(); v != graph->m_vertices.end(); ++v)
    {
        v->second.m_val = INT_MAX;
        v->second.m_active = true;
    }

    VERTEX &source_vertex = graph->m_vertices[source_id];
    source_vertex.m_val = 0;
    source_vertex.m_prev_id = -1;

    VERTICES_set unfinished;
    unfinished.insert(source_id);

    while (!unfinished.empty())
    {
        VERTICES_set::iterator lowest, u;

        lowest = u = unfinished.begin();

        for (++u; u != unfinished.end(); ++u)
        {
            if (graph->m_vertices[*u].m_val < graph->m_vertices[*lowest].m_val)
            {
                lowest = u;
            }
        }

        VERTEX &lowest_vertex = graph->m_vertices[*lowest];
        int lowest_val = lowest_vertex.m_val;
        lowest_vertex.m_active = false;
        unfinished.erase(lowest);

        IDS_list &neighbors = lowest_vertex.m_neighbors;
        IDS_list::iterator n;

        for (n = neighbors.begin(); n != neighbors.end(); ++n)
        {
            VERTEX &n_vertex = graph->m_vertices[*n];

            if (fvertices.find(n_vertex.m_id) == fvertices.end() && fcycles.find(n_vertex.m_cyc_id) == fcycles.end())
            {
                if (n_vertex.m_active)
                {
                    if (n_vertex.m_val > lowest_val + 1)
                    {
                        n_vertex.m_val = lowest_val + 1;
                        n_vertex.m_prev_id = lowest_vertex.m_id;
                        unfinished.insert(n_vertex.m_id);
                    }
                }
            }
        }

        if (lowest_vertex.m_id == sink_id)
        {
            return;
        }
    }
    if (sink_id != INT_MIN)
    {
        assert(false);
    }
}


int find_nearest_outsider(int source_id, CYCLE *cycle, GRAPH *graph, IDS_set &fvertices, IDS_set &fcycles)
{
    VERTICES_map::iterator v;

    for (v = graph->m_vertices.begin(); v != graph->m_vertices.end(); ++v)
    {
        v->second.m_val = INT_MAX;
        v->second.m_active = true;
    }

    VERTEX &source_vertex = graph->m_vertices[source_id];
    source_vertex.m_val = 0;
    source_vertex.m_prev_id = -1;

    VERTICES_set unfinished;
    unfinished.insert(source_id);

    while (!unfinished.empty())
    {
        VERTICES_set::iterator lowest, u;

        lowest = u = unfinished.begin();

        for (++u; u != unfinished.end(); ++u)
        {
            if (graph->m_vertices[*u].m_val < graph->m_vertices[*lowest].m_val)
            {
                lowest = u;
            }
        }

        VERTEX &lowest_vertex = graph->m_vertices[*lowest];
        int lowest_val = lowest_vertex.m_val;
        lowest_vertex.m_active = false;
        unfinished.erase(lowest);

        IDS_list &neighbors = lowest_vertex.m_neighbors;
        IDS_list::iterator n;

        for (n = neighbors.begin(); n != neighbors.end(); ++n)
        {
            VERTEX &n_vertex = graph->m_vertices[*n];

            if (fvertices.find(n_vertex.m_id) == fvertices.end() && fcycles.find(n_vertex.m_cyc_id) == fcycles.end())
            {
                if (n_vertex.m_active)
                {
                    if (n_vertex.m_val > lowest_val + 1)
                    {
                        n_vertex.m_val = lowest_val + 1;
                        n_vertex.m_prev_id = lowest_vertex.m_id;
                        unfinished.insert(n_vertex.m_id);
                    }
                }
            }
        }

        bool outsider = true;

        for (IDS_list::const_iterator c = cycle->m_vertices.begin(); c != cycle->m_vertices.end(); ++c)
        {
            if (lowest_vertex.m_id == *c || fcycles.find(graph->m_vertices[lowest_vertex.m_id].m_cyc_id) != fcycles.end())
            {
                outsider = false;
                break;
            }
        }

        if (outsider)
        {
            assert(lowest_vertex.m_val != INT_MAX);
            return lowest_vertex.m_id;
        }
    }

    assert(false);
}


void find_shortest_path(int source_id, int sink_id, GRAPH *graph)
{
    IDS_set vdummy, cdummy;

    find_shortest_path(source_id, sink_id, graph, vdummy, cdummy);
}


int find_nearest_outsider(int source_id, CYCLE *cycle, GRAPH *graph)
{
    IDS_set vdummy, cdummy;

    return find_nearest_outsider(source_id, cycle, graph, vdummy, cdummy);
}


int find_nearest_free(int source_id, GRAPH *graph, IDS_set &fvertices, IDS_set &fcycles)
{
    find_shortest_path(source_id, INT_MIN, graph, fvertices, fcycles);

    VERTICES_map::iterator v, n;
    n = graph->m_vertices.end();

    for (v = graph->m_vertices.begin(); v != graph->m_vertices.end(); ++v)
    {
        if (v->second.m_robot <= 0)
        {
            if (n == graph->m_vertices.end())
            {
                n = v;
            }
            else
            {
                if (v->second.m_val < n->second.m_val)
                {
                    n = v;
                }
            }
        }
    }

    return n->second.m_id;
}


int find_nearest_free(int source_id, GRAPH *graph, IDS_set &fvertices, IDS_set &fcycles, IDS_list &floop)
{
    find_shortest_path(source_id, INT_MIN, graph, fvertices, fcycles);

    VERTICES_map::iterator v, n;
    n = graph->m_vertices.end();

    for (v = graph->m_vertices.begin(); v != graph->m_vertices.end(); ++v)
    {
        bool allowed_free = true;

        IDS_list::const_iterator l;

        for (l = floop.begin(); l != floop.end(); ++l)
        {
            if (v->second.m_robot == graph->m_vertices[*l].m_final)
            {
                allowed_free = false;
                break;
            }
        }

        if (v->second.m_robot <= 0 && allowed_free)
        {
            if (n == graph->m_vertices.end())
            {
                n = v;
            }
            else
            {
                if (v->second.m_val < n->second.m_val)
                {
                    n = v;
                }
            }
        }
    }

    return n->second.m_id;
}


void move_free(int init_id, int goal_id, GRAPH *graph, IDS_set &fvertices, IDS_set &fcycles)
{
    IDS_list path;

//	graph->print();

    find_shortest_path(init_id, goal_id, graph, fvertices, fcycles);
    int z = goal_id;

    while (z >= 0)
    {
        path.push_front(z);
        z = graph->m_vertices[z].m_prev_id;
    }

    assert(graph->m_vertices[goal_id].m_val != INT_MAX);

    IDS_list::iterator p1 = path.begin();
    IDS_list::iterator p2 = p1;
    ++p2;

    while (p2 != path.end())
    {
        swap_robots(*p1, *p2, graph);
        ++p1;
        ++p2;
    }
}


void move_free(int init_id, int goal_id, GRAPH *graph)
{
    IDS_set vdummy, cdummy;

    move_free(init_id, goal_id, graph, vdummy, cdummy);
}


int find_nearest(CYCLE *cycle, GRAPH *graph)
{
    IDS_list::const_iterator v, min;
    min = v = cycle->m_vertices.begin();

    for (++v; v != cycle->m_vertices.end(); ++v)
    {
        if (graph->m_vertices[*v].m_val < graph->m_vertices[*min].m_val)
        {
            min = v;
        }
    }

    assert(graph->m_vertices[*min].m_val != INT_MAX);

    return *min;
}


EDGES_set::iterator find_edge(int U_id, int V_id, GRAPH *graph)
{
    for (EDGES_set::iterator edge = graph->m_edges.begin(); edge != graph->m_edges.end(); ++edge)
    {
        if ((edge->m_U_id == U_id && edge->m_V_id == V_id) || (edge->m_U_id == V_id && edge->m_V_id == U_id))
        {
            return edge;
        }
    }

    return graph->m_edges.end();
}


void move_robot(int robot, int where_id, int free, GRAPH *graph, IDS_set &fvertices, IDS_set &fcycles)
{
    VERTICES_map::const_iterator r, f;

    r = find_robot(robot, graph);
    assert(r != graph->m_vertices.end());

    IDS_list path;

    find_shortest_path(r->second.m_id, where_id, graph, fvertices, fcycles);
    int z = where_id;

    while (z >= 0)
    {
        path.push_front(z);
        z = graph->m_vertices[z].m_prev_id;
    }

    IDS_list::iterator p1 = path.begin();
    IDS_list::iterator p2 = p1;
    ++p2;

    while (p2 != path.end())
    {
        f = find_robot(free, graph);

        assert(f != graph->m_vertices.end());

        fvertices.clear();
        fvertices.insert(*p1);
//		fvertices.insert(*p2);
        find_shortest_path(f->second.m_id, INT_MIN, graph, fvertices, fcycles);

        EDGES_set::iterator path_edge = find_edge(*p1, *p2, graph);
        assert(fcycles.find(path_edge->m_cyc_id) == fcycles.end());

        assert(path_edge != graph->m_edges.end());
        CYCLE *rcycle = &graph->m_cycles[path_edge->m_cyc_id];

        int n = find_nearest(rcycle, graph);
        move_free(f->second.m_id, n, graph, fvertices, fcycles);

        ORIENTATION o = calc_orientation(*p1, *p2, rcycle);

        switch (o)
        {
        case CW:
        {
            half_rotate_cycle_CW(robot, *p2, free, rcycle, graph);
            break;
        }
        case CCW:
        {
            half_rotate_cycle_CCW(robot, *p2, free, rcycle, graph);
            break;
        }
        default:
        {
            break;
        }
        }

        ++p1;
        ++p2;
    }
}


void move_robot(int robot, int where_id, GRAPH *graph, IDS_set &fvertices, IDS_set &fcycles)
{
    move_robot(robot, where_id, 0, graph, fvertices, fcycles);
}


int find_next_robot_CW(int robot, CYCLE *cycle, GRAPH *graph)
{
    IDS_list::const_iterator c;

    for (c = cycle->m_vertices.begin(); c != cycle->m_vertices.end(); ++c)
    {
        if (graph->m_vertices[*c].m_robot == robot)
        {
            break;
        }
    }

    assert(c != cycle->m_vertices.end());

    while (true)
    {
        ++c;

        if (c == cycle->m_vertices.end())
        {
            c = cycle->m_vertices.begin();
        }
        if (graph->m_vertices[*c].m_robot != 0 && graph->m_vertices[*c].m_robot != -1)
        {
            return graph->m_vertices[*c].m_robot;
        }
    }
    assert(false);
}


int find_first_final_CW(CYCLE *cycle, GRAPH *graph)
{
    IDS_list::const_iterator c;

    for (c = cycle->m_vertices.begin(); c != cycle->m_vertices.end(); ++c)
    {
        if (graph->m_vertices[*c].m_final != 0 && graph->m_vertices[*c].m_final != -1)
        {
            return graph->m_vertices[*c].m_final;
        }
    }

    assert(false);
}


int find_next_final_CW(int robot, CYCLE *cycle, GRAPH *graph)
{
    IDS_list::const_iterator c;

    for (c = cycle->m_vertices.begin(); c != cycle->m_vertices.end(); ++c)
    {
        if (graph->m_vertices[*c].m_final == robot)
        {
            break;
        }
    }

    assert(c != cycle->m_vertices.end());

    while (true)
    {
        ++c;

        if (c == cycle->m_vertices.end())
        {
            c = cycle->m_vertices.begin();
        }
        if (graph->m_vertices[*c].m_final != 0 && graph->m_vertices[*c].m_final != -1)
        {
            return graph->m_vertices[*c].m_final;
        }
    }
    assert(false);
}


int find_next_robot_CCW(int robot, CYCLE *cycle, GRAPH *graph)
{
    IDS_list::reverse_iterator c;

    for (c = cycle->m_vertices.rbegin(); c != cycle->m_vertices.rend(); ++c)
    {
        if (graph->m_vertices[*c].m_robot == robot)
        {
            break;
        }
    }

    assert(c != cycle->m_vertices.rend());

    while (true)
    {
        ++c;
        if (c == cycle->m_vertices.rend())
        {
            c = cycle->m_vertices.rbegin();
        }
        if (graph->m_vertices[*c].m_robot != 0 && graph->m_vertices[*c].m_robot != -1)
        {
            return graph->m_vertices[*c].m_robot;
        }
    }
    assert(false);
}


void exchange_last_cycle(int robot1, int robot2, GRAPH *graph, IDS_set &UNUSED(fvertices), IDS_set &UNUSED(fcycles))
{
    int robot3;

    CYCLE *cycle_0 = &graph->m_cycles[0];
    CYCLE *cycle_1 = &graph->m_cycles[1];

    robot3 = find_next_robot_CW(robot1, cycle_0, graph);

    while (graph->m_vertices[cycle_1->m_x_id].m_robot != robot1)
    {
        rotate_cycle_CW(-1, cycle_0, graph);
    }
    swap_robots(cycle_1->m_x_id, *cycle_1->m_loop.begin(), graph);

    while (graph->m_vertices[cycle_0->m_x_id].m_robot != robot2)
    {
        rotate_cycle_CW(0, cycle_0, graph);
    }
    swap_robots(cycle_1->m_x_id, *cycle_1->m_loop.begin(), graph);
    rotate_cycle_CW(-1, cycle_0, graph);
    swap_robots(cycle_1->m_x_id, *cycle_1->m_loop.begin(), graph);

    if (robot1 != robot3 && robot2 != robot3)
    {
        while (graph->m_vertices[cycle_0->m_y_id].m_robot != robot3)
        {
            rotate_cycle_CW(0, cycle_0, graph);
        }
    }

    swap_robots(cycle_1->m_x_id, *cycle_1->m_loop.begin(), graph);
}


bool solve_critical_last_free(GRAPH *graph, IDS_set &fcycles)
{
    IDS_set fvertices;

    CYCLE *cycle_0 = &graph->m_cycles[0];
    CYCLE *cycle_1 = &graph->m_cycles[1];

    VERTICES_map::const_iterator f = find_robot(0, graph);
    move_free(f->second.m_id, cycle_1->m_x_id, graph, fvertices, fcycles);
    swap_robots(cycle_1->m_x_id, *cycle_1->m_loop.begin(), graph);

    int back_robot = graph->m_vertices[cycle_1->m_x_id].m_robot;
    int final_robot = find_first_final_CW(cycle_0, graph);
    int r1 = find_next_robot_CW(final_robot, cycle_0, graph);
    int r2 = find_next_final_CW(final_robot, cycle_0, graph);

    while (r2 != final_robot)
    {
        if (r1 != r2)
        {
            exchange_last_cycle(r1, r2, graph, fvertices, fcycles);
        }
        r1 = find_next_robot_CW(r2, cycle_0, graph);
        r2 = find_next_final_CW(r2, cycle_0, graph);
    }
    while (graph->m_vertices[cycle_1->m_x_id].m_robot != back_robot)
    {
        rotate_cycle_CW(-1, cycle_0, graph);
    }
    swap_robots(cycle_1->m_x_id, *cycle_1->m_loop.begin(), graph);
    VERTICES_map::const_iterator zero = find_robot(0, graph);
    VERTICES_map::const_iterator zero_final = find_final(0, graph);
    move_free(zero->second.m_id, zero_final->second.m_id, graph, fvertices, fcycles);

    fvertices.insert(0);
    VERTICES_map::const_iterator nega = find_robot(-1, graph);
    VERTICES_map::const_iterator nega_final = find_final(-1, graph);
    move_free(nega->second.m_id, nega_final->second.m_id, graph, fvertices, fcycles);

    final_robot = find_first_final_CW(cycle_0, graph);
    VERTICES_map::const_iterator ff = find_final(final_robot, graph);
    while (graph->m_vertices[ff->second.m_id].m_robot != final_robot)
    {
        VERTICES_map::const_iterator nega_1 = find_robot(-1, graph);
        rotate_cycle_CCW(0, cycle_0, graph);
        VERTICES_map::const_iterator nega_2 = find_robot(-1, graph);
        swap_robots(nega_1->second.m_id, nega_2->second.m_id, graph);
    }

    return true;
}


bool solve_critical_free(GRAPH *graph)
{
    IDS_set zero_nega_fvertices,  zero_nega_fcycles;

    CYCLE *cycle_0 = &graph->m_cycles[0];

    IDS_list zero_path;
    IDS_list nega_path;

    VERTICES_map::const_iterator zero = find_final(0, graph);

    find_shortest_path(zero->second.m_id, *cycle_0->m_vertices.begin(), graph, zero_nega_fvertices, zero_nega_fcycles);
    int z = *cycle_0->m_vertices.begin();

    while (z >= 0)
    {
        zero_path.push_front(z);
        z = graph->m_vertices[z].m_prev_id;
    }

    IDS_list::iterator p1 = zero_path.begin();
    IDS_list::iterator p2 = p1;
    ++p2;

    while (p2 != zero_path.end())
    {
        swap_finals(*p1, *p2, graph);
        ++p1;
        ++p2;
    }

    zero_nega_fvertices.insert(*cycle_0->m_vertices.begin());

    VERTICES_map::const_iterator nega = find_final(-1, graph);

    find_shortest_path(nega->second.m_id, *cycle_0->m_vertices.rbegin(), graph, zero_nega_fvertices, zero_nega_fcycles);
    z = *cycle_0->m_vertices.rbegin();

    while (z >= 0)
    {
        nega_path.push_front(z);
        z = graph->m_vertices[z].m_prev_id;
    }

    p1 = nega_path.begin();
    p2 = p1;
    ++p2;

    while (p2 != nega_path.end())
    {
        swap_finals(*p1, *p2, graph);
        ++p1;
        ++p2;
    }

    int c;

    IDS_set fcycles;
    int free = 0;

    for (c = graph->m_cycles.size() - 1; c >= 1; --c)
    {
        INDEPENDENT = true;
        CURRENT_CYCLE = c;

        IDS_set dummy_fvertices;

        CYCLE *cycle = &graph->m_cycles[c];

        int n_free_id = find_nearest_free(cycle->m_x_id, graph, dummy_fvertices, fcycles, cycle->m_loop);
        int n_free = graph->m_vertices[n_free_id].m_robot;

        IDS_list::reverse_iterator l;

        bool forbidden_free = false;

        for (l = cycle->m_loop.rbegin(); l != cycle->m_loop.rend(); ++l)
        {
            if (graph->m_vertices[*l].m_goal == n_free)
            {
                forbidden_free = true;
                break;
            }
        }

        if (forbidden_free)
        {
            n_free = free;
        }

        VERTICES_map::const_iterator ff = find_robot(n_free, graph);
        move_free(ff->second.m_id, cycle->m_x_id, graph, dummy_fvertices, fcycles);

        bool finished = true;

        for (l = cycle->m_loop.rbegin(); l != cycle->m_loop.rend(); ++l)
        {
            if (graph->m_vertices[*l].m_robot != graph->m_vertices[*l].m_final)
            {
                finished = false;
            }
        }

        if (!finished)
        {
            for (l = cycle->m_loop.rbegin(); l != cycle->m_loop.rend(); ++l)
            {
                int robot = graph->m_vertices[*l].m_final;
                VERTICES_map::const_iterator r = find_robot(robot, graph);

                if (r->second.m_cyc_id < c)
                {
                    IDS_set fvertices;
                    fcycles.insert(c);

                    VERTICES_map::const_iterator r = find_robot(robot, graph);

                    int n1_free_id = find_nearest_free(r->second.m_id, graph, dummy_fvertices, fcycles, cycle->m_loop);
                    int n1_free = graph->m_vertices[n1_free_id].m_robot;

                    move_robot(robot, cycle->m_x_id, n1_free, graph, fvertices, fcycles);

                    fvertices.clear();
                    fvertices.insert(cycle->m_x_id);

                    int n2_free_id = find_nearest_free(cycle->m_y_id, graph, fvertices, fcycles, cycle->m_loop);
                    int n2_free = graph->m_vertices[n2_free_id].m_robot;
                    VERTICES_map::const_iterator f = find_robot(n2_free, graph);

                    fvertices.clear();
                    fvertices.insert(cycle->m_x_id);
                    move_free(f->second.m_id, cycle->m_y_id, graph, fvertices, fcycles);
                    rotate_cycle_CW(n2_free, cycle, graph);
                }
                else
                {
                    IDS_set fvertices;

                    fcycles.insert(c);

                    int n3_free_id = find_nearest_free(cycle->m_x_id, graph, fvertices, fcycles, cycle->m_loop);
                    int n3_free = graph->m_vertices[n3_free_id].m_robot;
                    VERTICES_map::const_iterator f = find_robot(n3_free, graph);
                    move_free(f->second.m_id, cycle->m_x_id, graph, fvertices, fcycles);

                    int CWs = 0;

                    while (graph->m_vertices[cycle->m_y_id].m_robot != robot)
                    {
                        rotate_cycle_CW(n3_free, cycle, graph);
                        ++CWs;
                    }

                    fvertices.clear();

                    int out_id = find_nearest_outsider(cycle->m_y_id, cycle, graph, fvertices, fcycles);

                    VERTICES_map::const_iterator r35 = find_robot(robot, graph);

                    int n35_free_id = find_nearest_free(/*r35->second.m_id*/out_id, graph, fvertices, fcycles, cycle->m_loop);
                    int n35_free = graph->m_vertices[n35_free_id].m_robot;

                    move_robot(robot, out_id, n35_free, graph, fvertices, fcycles);

                    fcycles.insert(c);
                    fvertices.insert(out_id);

                    int n4_free_id = find_nearest_free(cycle->m_x_id, graph, fvertices, fcycles, cycle->m_loop);
                    int n4_free = graph->m_vertices[n4_free_id].m_robot;

                    f = find_robot(n4_free, graph);
                    move_free(f->second.m_id, cycle->m_x_id, graph, fvertices, fcycles);

                    int i;

                    for (i = 0; i < CWs; ++i)
                    {
                        rotate_cycle_CCW(n4_free, cycle, graph);
                    }

                    fvertices.clear();

                    VERTICES_map::const_iterator r = find_robot(robot, graph);

                    int n45_free_id = find_nearest_free(cycle->m_x_id/*r->second.m_id*/, graph, fvertices, fcycles, cycle->m_loop);
                    int n45_free = graph->m_vertices[n45_free_id].m_robot;

                    move_robot(robot, cycle->m_x_id, n45_free, graph, fvertices, fcycles);

                    fvertices.clear();
                    fvertices.insert(cycle->m_x_id);

                    int n5_free_id = find_nearest_free(cycle->m_y_id, graph, fvertices, fcycles, cycle->m_loop);
                    int n5_free = graph->m_vertices[n5_free_id].m_robot;

                    f = find_robot(n5_free, graph);
                    move_free(f->second.m_id, cycle->m_y_id, graph, fvertices, fcycles);

                    rotate_cycle_CW(n5_free, cycle, graph);
                }
            }
        }

        if (INDEPENDENT)
        {
            printf("Cycle: %d is independent !!!\n", CURRENT_CYCLE);
        }
        free = (free == 0) ? -1 : 0;
        fcycles.insert(c);
    }

    if (!solve_critical_last_free(graph, fcycles))
    {
        return false;
    }

    IDS_list::reverse_iterator rp1 = nega_path.rbegin();
    IDS_list::reverse_iterator rp2 = rp1;
    ++rp2;

    while (rp2 != nega_path.rend())
    {
        swap_robots(*rp1, *rp2, graph);
        ++rp1;
        ++rp2;
    }

    rp1 = zero_path.rbegin();
    rp2 = rp1;
    ++rp2;

    while (rp2 != zero_path.rend())
    {
        swap_robots(*rp1, *rp2, graph);
        ++rp1;
        ++rp2;
    }

    return true;
}


bool solve_critical(GRAPH *UNUSED(graph))
{
    return true;
}


void move_robot(int robot, int where, GRAPH *graph)
{
    IDS_set vdummy, cdummy;

    move_robot(robot, where, graph, vdummy, cdummy);
}


void generate_bipartite(int c, int m, int l, int f, GRAPH *graph)
{
    int id = 0;

    int min = c;

    graph->m_cycles.push_back(CYCLE());
    CYCLE *cycle = &graph->m_cycles.back();

    int i, j;

    for (i = 0; i < c; ++i)
    {
        if (id < f)
        {
            graph->add_vertex(id, 0, -id, -id);
        }
        else
        {
            graph->add_vertex(id, 0, id, id);
        }
        cycle->m_x_id = cycle->m_y_id = 0;
        cycle->m_vertices.push_back(id);
        cycle->m_loop.push_back(id);
        ++id;
    }
    for (i = 1; i < c; ++i)
    {
        graph->add_edge(id - i - 1, id - i, 0);
    }
    graph->add_edge(id - c, id - 1, 0);



    for (j = 0; j < m; ++j)
    {
        graph->m_cycles.push_back(CYCLE());
        cycle = &graph->m_cycles.back();

        int x, y;
        x = random() % id;
        y = random() % id;

        if (x == y)
        {
            if (x > 0)
            {
                --y;
            }
            else
            {
                ++y;
            }
        }

        cycle->m_x_id = x;
        cycle->m_y_id = y;

        if (j == 0)
        {
            if (cycle->m_x_id > 0)
            {
                graph->m_cycles.begin()->m_x_id = cycle->m_x_id - 1;
            }
            else
            {
                graph->m_cycles.begin()->m_x_id = id - 1;
            }
            if (cycle->m_x_id < id - 1)
            {
                graph->m_cycles.begin()->m_y_id = cycle->m_x_id + 1;
            }
            else
            {
                graph->m_cycles.begin()->m_y_id = 0;
            }
        }

        find_shortest_path(x, y, graph);

        int z = y;

        while (z >= 0)
        {
            cycle->m_vertices.push_back(z);
            z = graph->m_vertices[z].m_prev_id;
        }

        c = l/2;

        if (c < 1)
        {
            c = 1;
        }

        for (i = 0; i < c; ++i)
        {
            if (id < f)
            {
                graph->add_vertex(id, j + 1, -id, -id);
            }
            else
            {
                graph->add_vertex(id, j + 1, id, id);
            }
            cycle->m_vertices.push_back(id);
            cycle->m_loop.push_back(id);
            ++id;
        }

        graph->add_edge(x, id - c, j + 1);
        graph->add_edge(y, id - 1, j + 1);

        for (i = 1; i < c; ++i)
        {
            graph->add_edge(id - i - 1, id - i, j + 1);
        }
    }

    int x, y;

//	x = (random() % (id - 1)) + 1;
//	graph->m_vertices[x].m_robot = graph->m_vertices[x].m_final = graph->m_vertices[x].m_goal = -1;

    int k;

    for (k = 0; k < id * id; ++k)
    {
        x = random() % id;
        y = random() % id;

        if (x != y)
        {
            int temp;

            temp = graph->m_vertices[x].m_robot;
            graph->m_vertices[x].m_robot = graph->m_vertices[y].m_robot;
            graph->m_vertices[y].m_robot = temp;
        }
    }

    for (k = 0; k < id * id; ++k)
    {
        x = random() % id;
        y = random() % id;

        if (x != y)
        {
            int temp;

            temp = graph->m_vertices[x].m_goal;
            graph->m_vertices[x].m_goal = graph->m_vertices[y].m_goal;
            graph->m_vertices[y].m_goal = temp;

            temp = graph->m_vertices[x].m_final;
            graph->m_vertices[x].m_final = graph->m_vertices[y].m_final;
            graph->m_vertices[y].m_final = temp;
        }
    }
}


void export_graph_2_PDDL(const char *problem, const char *filename, GRAPH *graph)
{
    FILE *f;

    f = fopen(filename, "w");
    if (f == NULL)
    {
        printf("Error: cannot open file %s\n", filename);
        exit(-1);
    }

    fprintf(f, "(define (problem %s)\n", problem);
    fprintf(f, " (:domain multirobot)\n");
    fprintf(f, " (:objects\n");

    VERTICES_map::const_iterator v;

    for (v = graph->m_vertices.begin(); v != graph->m_vertices.end(); ++v)
    {
        if (v->second.m_robot > 0)
        {
            fprintf(f, "  v%d r%d\n", v->second.m_id, v->second.m_robot);
        }
        else
        {
            fprintf(f, "  v%d\n", v->second.m_id);
        }
    }

    fprintf(f, " )\n");
    fprintf(f, " (:init\n");

    for (v = graph->m_vertices.begin(); v != graph->m_vertices.end(); ++v)
    {
        if (v->second.m_robot > 0)
        {
            fprintf(f, "  (at r%d v%d)\n", v->second.m_robot, v->second.m_id);
        }
        else
        {
            fprintf(f, "  (free v%d)\n", v->second.m_id);
        }
    }

    EDGES_set::const_iterator e;

    for (e = graph->m_edges.begin(); e != graph->m_edges.end(); ++e)
    {
        fprintf(f, "  (adjacent v%d v%d)\n", e->m_U_id, e->m_V_id);
    }

    fprintf(f, " )\n");
    fprintf(f, " (:goal\n");
    fprintf(f, "  (and\n");

    for (v = graph->m_vertices.begin(); v != graph->m_vertices.end(); ++v)
    {
        if (v->second.m_final > 0)
        {
            fprintf(f, "  (at r%d v%d)\n", v->second.m_final, v->second.m_id);
        }
        else
        {
            fprintf(f, "  (free v%d)\n", v->second.m_id);
        }
    }

    fprintf(f, "  )\n");
    fprintf(f, " )\n");
    fprintf(f, ")\n");

    fclose(f);
}

void export_graph_rec (GRAPH *graph)
{
    FILE *f ;

    f = fopen(GraphRec.c_str(), "w");

    if (f == NULL)
    {
        printf("Error: cannot open file %s\n", GraphRec.c_str());
        exit(-1);
    }

    fprintf(f, "id:1\nV=\n");

    // Writing nodes :

        VERTICES_map::const_iterator v;

      for (v = graph->m_vertices.begin(); v != graph->m_vertices.end(); ++v){
            if (v->second.m_robot > 0)
                    fprintf(f,"(%d:0)[%d:0:0]\n", v->second.m_id , v->second.m_robot);
            else  // no agent
                    fprintf(f, "(%d:0)[-1:0:0]\n",v->second.m_id );
        }

    // Writing edges :
      EDGES_set::const_iterator e;
      fprintf(f,"E=\n");
      for (e = graph->m_edges.begin(); e != graph->m_edges.end(); ++e)
          fprintf(f, "{%d,%d} (0)\n", e->m_U_id, e->m_V_id);
      // Writing modes :
      fprintf(f, "MOD =\nM:TRANSITIVE\nSolution\n");


    fclose(f);
}


void generate_bipartite(int m, int l, GRAPH *graph)
{
    int c = random() % l;
    if (c < 3)
    {
        c = 3;
    }
    generate_bipartite(c, m, l, 2, graph);
}

//struct BiData { int moves; double exec_time; int nodes; int edges; };

void generate_bipartite(int c, int m, int l, int f,int& nodes_number,std::vector<std::pair<int,int> >& Edges,std::vector<Agent>& agents,DataRow& dataRow) {
    GRAPH g;

    generate_bipartite(c,m,l, f, &g);

    // Nodes
    nodes_number= g.m_vertices.size();

    // Edges
    for (EDGES_set::const_iterator edge = g.m_edges.begin(); edge != g.m_edges.end(); ++edge)
    {

        Edges.push_back( std::make_pair(edge->m_U_id, edge->m_V_id));
    }

    // Agents
    std::vector<std::pair <int,int> >R_IV;
    std::vector<std::pair <int,int> >R_GV;
    for (VERTICES_map::const_iterator v = g.m_vertices.begin(); v != g.m_vertices.end(); ++v)
    {
        if (v->second.m_robot > 0)
        {
            R_IV.push_back(std::make_pair(v->second.m_robot, v->second.m_id));
        }
    }
    for (VERTICES_map::const_iterator v = g.m_vertices.begin(); v != g.m_vertices.end(); ++v)
    {
        if (v->second.m_final > 0)
        {
            R_GV.push_back(std::make_pair(v->second.m_final, v->second.m_id));
        }
    }

    for (int i=0; i<R_IV.size(); i++){
        int cntr=0;
        int start=-1,goal=-1;
        int id = R_IV[i].first;
        start = R_IV[i].second;
        for (int j=0; j<R_GV.size(); j++)
            if (id == R_GV[j].first) {
                goal = R_GV[j].second;
                agents.push_back(Agent(cntr++,start,goal));
            }
    }

    dataRow.agentsNumber=g.m_vertices.size() - f;
    dataRow.nodesNumber =g.m_vertices.size();
    dataRow.EdgesNumber =g.m_edges.size();
    if (dataRow.graphName =="") dataRow.graphName= "BiconnectedGraph";
    dataRow.freesNumber =f;
    dataRow.completenessRate = 100;
    dataRow.handelsNumber = m;
    dataRow.maxHandleLength = l/2;
    dataRow.sizeOfC0 = c;
}

void readBibox (int C0,int handlesNum, int maxHalndleLength, int free,string& graphRec,string& DataFile,DataRow& dataRow) /*(std::vector<Bicycle>& cycles,std::vector <BiEdge>& BiEdges)*/
{


    /*
    for (int i=0; i<cycles.size(); i++) {
        std::cout<<"C ("<<cycles[i].id<<") \n";
        std::cout<<"x = "<<cycles[i].x<<", y = "<<cycles[i].y<<"\n";
        for (int j=0; j<cycles[i].nodes.size(); j++)
            std::cout<<"node id="<<cycles[i].nodes[j].id<<", cycle_id="<<cycles[i].nodes[j].cycles_id<<", start="<<cycles[i].nodes[j].Agent_start<<" , goal="<<cycles[i].nodes[j].Agent_goal<<"\n";

    }
    // graph vertixes

    for (int i=0; i<cycles.size(); i++) {
        g.m_cycles.push_back(CYCLE());
        CYCLE *cycle_ = &g.m_cycles.back();
        cycle_->m_x_id = cycles[i].x;
        std::cout<<"cycle_->m_x_id ="<<cycles[i].x<<"\n";
        cycle_->m_y_id = cycles[i].y;
        std::cout<<"cycle_->m_y_id ="<<cycles[i].y<<"\n";
        if ( cycles[i].id == 0 ) { // Special case (original cycle)
            for (int j=0; j<cycles[i].nodes.size(); j++)
            {
                g.add_vertex(cycles[i].nodes[j].id,cycles[i].nodes[j].cycles_id,cycles[i].nodes[j].Agent_start,cycles[i].nodes[j].Agent_goal);
                std::cout<<"g.add_vertex ("<<cycles[i].nodes[j].id <<","<<cycles[i].nodes[j].cycles_id<<","<<cycles[i].nodes[j].Agent_start<<","<<cycles[i].nodes[j].Agent_goal<<")\n";
                cycle_->m_vertices.push_back(cycles[i].nodes[j].id);
                std::cout<<"cycle_->m_vertices.push_back("<<cycles[i].nodes[j].id<<")\n";
                cycle_->m_loop.push_back(cycles[i].nodes[j].id);
                std::cout<<"cycle_->m_loop.push_back("<<cycles[i].nodes[j].id<<")\n";

            }
        }
        else {
            for (int j=0; j<cycles[i].nodes.size(); j++)
                if (cycles[i].nodes[j].cycles_id ==  cycles[i].id) {// node implies to this cycle
                    std::cout<<"g.add_vertex ("<<cycles[i].nodes[j].id<<","<<cycles[i].nodes[j].cycles_id<<","<<cycles[i].nodes[j].Agent_start<<","<<cycles[i].nodes[j].Agent_goal<<")\n";
                    g.add_vertex(cycles[i].nodes[j].id,cycles[i].nodes[j].cycles_id,cycles[i].nodes[j].Agent_start,cycles[i].nodes[j].Agent_goal);
                    cycle_->m_vertices.push_back(cycles[i].nodes[j].id);
                    std::cout<<"cycle_->m_vertices.push_back("<<cycles[i].nodes[j].id<<")\n";
                    cycle_->m_loop.push_back(cycles[i].nodes[j].id);
                    std::cout<<"cycle_->m_loop.push_back("<<cycles[i].nodes[j].id<<")\n";
                    }
                }
            for (int j=0; j<cycles[i].nodes.size(); j++)
                if (cycles[i].nodes[j].cycles_id ==  cycles[i].id && cycles[i].nodes[j].id !=1) { // node isnt implies to this cycle
                    cycle_->m_vertices.push_back(cycles[i].nodes[j].id);
                    std::cout<<"cycle_->m_vertices.push_back("<<cycles[i].nodes[j].id<<")\n";
                }
            if (cycles[i].id == 3) {
                cycle_->m_vertices.push_back(1);
                std::cout<<"cycle_->m_vertices.push_back("<<1<<")\n";
            }
    }


   // Edges vertixes
    for (int i=0; i<BiEdges.size(); i++){
        g.add_edge(BiEdges[i].from,BiEdges[i].to, BiEdges[i].cycle);
        std::cout<<"g.add_edge("<<BiEdges[i].from<<","<<BiEdges[i].to<<","<<BiEdges[i].cycle<<")\n";
        std::cout<<"<"<<BiEdges[i].from<<","<<BiEdges[i].to<<">\n";
    }
    for (int n=0; n<BiNodes.size(); n++){
        QMessageBox msgBox1;
        QString st;
        st.clear();
        st.append( "Node id=: " );
        st.append(QString::number(BiNodes[n].id));
        st.append( "Cycle id=" );
        st.append(QString::number(BiNodes[n].cycles_id));
        st.append( "StartAgent id= " );
        st.append(QString::number(BiNodes[n].Agent_start));
        st.append( "GoalAgent id=" );
        st.append(QString::number(BiNodes[n].Agent_goal));
        msgBox1.setText(st);
        msgBox1.exec();
    }*/


    printf("Mulirobot %s\n", VERSION);
    printf("(c) 2008 %s\n\n", AUTHOR);
    GRAPH g;

    GraphRec = graphRec;
    std::cout<<"GraphRec="<<graphRec<<"\n";
    std::cout<<"DataFile="<<DataFile<<"\n";
    clock_t tStart = clock();
    generate_bipartite(C0,handlesNum,maxHalndleLength, free, &g);
    export_graph_rec (&g);
    solve_critical_free(&g);

    FILE *f ;
    f = fopen(GraphRec.c_str(), "ab");
    fprintf(f, "Length:%d\n", MOVE_COUNT);
    fclose(f);
    double exexc_time = (double)(clock() - tStart)/CLOCKS_PER_SEC;

    dataRow.agentsNumber=g.m_vertices.size() - free;
    dataRow.nodesNumber =g.m_vertices.size();
    dataRow.EdgesNumber =g.m_edges.size();
    if (dataRow.graphName =="") dataRow.graphName= "BiconnectedGraph";
    dataRow.solverName = "Bibox";
    dataRow.freesNumber =free;
    dataRow.completenessRate = 100;
    dataRow.condenseLength = MOVE_COUNT;
    dataRow.pathLength = MOVE_COUNT;
    dataRow.handelsNumber = handlesNum;
    dataRow.maxHandleLength = maxHalndleLength;
    dataRow.sizeOfC0 = C0;
    dataRow.executionTime = exexc_time;

    writeStatisitcs (DataFile,dataRow);

    std::cout << "SOLUTION FOUND" << std::endl;
    std::cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n\n";
    std::cout<<"Completed Agents = "<<dataRow.completenessRate<<"%"<<std::endl;
    printf("Execution Time = %.2fs\n",dataRow.executionTime  );
    std::cout<<"Path Length = "<<(dataRow.pathLength)<<std::endl;

    // Reporting

    QString str;
    str.clear();
    str.append( "Solver: " );
    str.append(dataRow.solverName.c_str());
    str.append( "\nNodes=" );
    str.append(IntToString(dataRow.nodesNumber).c_str());
    str.append( " | Edges=" );
    str.append(IntToString(dataRow.EdgesNumber).c_str());
    str.append( " | Agents=" );
    str.append(IntToString(dataRow.agentsNumber).c_str());
    str.append("\nCompleted Agents = ");
    str.append(IntToString(dataRow.completenessRate).c_str());
    str.append("%\n");
    str.append("Execution Time = ");
    str.append(IntToString(dataRow.executionTime ).c_str());
    str.append(" ms\n");
    str.append("Path Length = " );
    str.append(IntToString(dataRow.pathLength).c_str());
    str.append(" moves\n");
    str.append("Condense Length = ");
    str.append(IntToString(dataRow.condenseLength).c_str());
    str.append(" moves\n" );
    QMessageBox msgBox;
    msgBox.setText("Result Summary");
    msgBox.setInformativeText(str);
    //msgBox.exec();

    // Exporting PDDL file for other planners testing
    char name[1024];
    char filename[1024];
    sprintf(name, "/home/ubuntu/Alhamdulellah/Demos/PAS_BiconnectedGraph/FixedFreeVariableSize/multirobot_f_%d_c_%d_m_%d_l_%d",dataRow.freesNumber, dataRow.sizeOfC0, dataRow.handelsNumber, dataRow.maxHandleLength);
    sprintf(filename, "%s.pddl", name);
    export_graph_2_PDDL(name, filename, &g);
    }
