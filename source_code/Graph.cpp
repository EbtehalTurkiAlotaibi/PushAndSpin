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

#include "Graph.h"
#include "Logger.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/astar_search.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/property_map/dynamic_property_map.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim_all.hpp>

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <limits>




static const unsigned int NO_VERTEX = std::numeric_limits<unsigned int>::max();

enum vertex_location_t { vertex_location };
enum vertex_counter_t { vertex_counter};
enum vertex_type_t { vertex_type};

namespace boost
{
    BOOST_INSTALL_PROPERTY(vertex, location);
    BOOST_INSTALL_PROPERTY(vertex, counter);
    BOOST_INSTALL_PROPERTY(vertex, type);
}

struct Point
{
    Point() {}
    Point(double _x, double _y, double _z) : x(_x), y(_y), z(_x) {}
    double x,y,z;

    friend std::ostream& operator << (std::ostream &o, const Point& p);
    friend std::istream& operator >> (std::istream &i, const Point& p);
};

std::ostream& operator << (std::ostream& o, const Point& p)
{
    o << p.x << " " << p.y << " " << p.z;
    return o;
}

std::istream& operator >> (std::istream& i, Point& p)
{
    std::string str;
    std::getline(i, str);
    boost::trim_all(str);

    std::vector<std::string> strs;
    boost::split(strs, str, boost::is_space());

    assert(strs.size() == 3);
    p.x = atof(strs[0].c_str());
    p.y = atof(strs[1].c_str());
    p.z = atof(strs[2].c_str());
    return i;
}

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                              boost::property<boost::vertex_index_t, unsigned int,
                              boost::property<vertex_location_t, Point,
                              boost::property<vertex_counter_t, int,
                              boost::property<vertex_type_t, std::string > > > >,
                              boost::property<boost::edge_weight_t, double> > GraphType;


typedef boost::graph_traits<GraphType>::vertex_descriptor  Vertex;
typedef boost::graph_traits<GraphType>::edge_descriptor    Edge;
typedef boost::graph_traits<GraphType>::vertex_iterator    VIterator;
typedef boost::graph_traits<GraphType>::edge_iterator      EIterator;
typedef boost::graph_traits<GraphType>::adjacency_iterator AdjIterator;

#define graph_ reinterpret_cast<GraphType*>(graphRaw_)



void Graph::print (){

  std::pair<VIterator, VIterator> vp;
  boost::property_map<GraphType, boost::vertex_index_t>::type vertices_indx = get(boost::vertex_index, *graph_);
  boost::property_map<GraphType, vertex_type_t >::type vertices_name = get(vertex_type, (*graph_));
  boost::property_map<GraphType, vertex_counter_t >::type vertices_counter = get(vertex_counter, (*graph_));
  std::cout << "vertices(g) = \n";
  std::pair<VIterator, VIterator> vp2;
  for (vp2 = vertices(*graph_); vp2.first != vp2.second; ++vp2.first)
    std::cout << "Index = " << vertices_indx [*vp2.first]
              << "\tType = " << vertices_name[*vp2.first]
              <<  "\tCounter = " << vertices_counter[*vp.first]
              << std::endl;
}

void Graph::set_counter (int index,int dis_time){
    boost::property_map<GraphType, vertex_counter_t >::type name = get(vertex_counter, (*graph_));
    boost::put(name,index,dis_time);
}

int Graph::get_counter(int index){
    boost::property_map<GraphType, vertex_counter_t >::type name = get(vertex_counter, (*graph_));
    return name[index];
}

void Graph::set_type (int index,std::string type){
    boost::property_map<GraphType, vertex_type_t >::type name = get(vertex_type, (*graph_));
    boost::put(name,index,type);
}

std::string Graph::get_type(int index){
    boost::property_map<GraphType, vertex_type_t >::type name =  get(vertex_type, *graph_);
    return name[index];
}




// Euclidean distance heuristic for A* search
// Returns square of distance between two vertices
template <class G, class CostType, class LocMap>
class DistanceHeuristic : public boost::astar_heuristic<G, CostType>
{
public:
    typedef typename boost::graph_traits<G>::vertex_descriptor Vertex;

    // Constructor.  Accepts a LocMap and the id of the goal of the search
    DistanceHeuristic (LocMap l, Vertex goal) : location_(l), goal_(goal) {}

    // Comparison operator.  Returns the estimated cost of traveling
    // between the goal and the vertex u
    CostType operator () (Vertex u)
    {
        CostType dx = location_[goal_].x - location_[u].x;
        CostType dy = location_[goal_].y - location_[u].y;
        CostType dz = location_[goal_].z - location_[u].z;
        return dx*dx + dy*dy + dz*dz;
    }

protected:

    LocMap location_;
    Vertex goal_;
};

// Empty structure.  Used to throw an exception indicating A* search termination.
struct foundAStarGoal {};

// A* search visitor.  Used during the A* search to check for the goal condition.
template <class Vertex>
class AStarGoalVisitor : public boost::default_astar_visitor
{
public:
    // Constructor.  Accepts the goal for use in termination condition.
    AStarGoalVisitor(Vertex goal) : goal_(goal) {}

    // Callback used during A* search when the vertex u is expanded.  If the
    template <class Graph>
    void examine_vertex(Vertex u, Graph& g)
    {
        if(u == goal_)
            throw foundAStarGoal ();
    }
private:
    Vertex goal_;
};


// Structure used to indicate the breadth first search has terminated.
template <class SearchData>
struct foundBrFSGoal
{
    /// Constructor.  Takes the goal node and related search data.
    foundBrFSGoal(unsigned int _g, const SearchData &d) : goal(_g), data(d) {}

    /// The goal node expanded
    unsigned int goal;
    // Search data computed during the brfs.  Useful for extracting the path from the start to the goal node.
    SearchData data;
};

// Breadth-first search visitor class.  Parameters: Graph (boost adjacency_list
// structure), SearchData (map used to store parent information), Goals (list
// of potential goal nodes).
template <class Graph, class SearchData, class Targets>
class brfsSearchVisitor : public boost::default_bfs_visitor
{
public:
    // Type describing a vertex in the graph
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    // Type describing an edge in the graph
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;

    // Constructor.  Accepts a list of target nodes for the search.
    // If negative_targets, goals are states NOT in targets.
    brfsSearchVisitor(const Targets& _targets, bool _negative_targets) : boost::default_bfs_visitor(), targets(_targets), negative_targets(_negative_targets) {}

    // Initialization callback from breadth-first search.  Sets parent information to NO_VERTEX (invalid)
    void initialize_vertex(Vertex v, const Graph &g)
    {
        search_data[v] = NO_VERTEX;
    }

    // Edge expansion callback from breadth-first search.  Updates parent information in search_data.
    void tree_edge(Edge e, const Graph &g)
    {
        // Target vertices point to source in search_data
        search_data[boost::target (e, g)] = boost::source(e, g);
    }

    // Vertex expansion callback from breadth-first search.
    void discover_vertex(Vertex v, const Graph& g)
    {
        bool found = targets.find(v) != targets.end();

        // Expanded a goal vertex
        if ((found && !negative_targets) || (!found && negative_targets))
        {
            // Throw the goal structure.  Populate it with this vertex, and the search_data computed to find this vertex.
            foundBrFSGoal<SearchData> the_goal(get(boost::vertex_index, g, v), search_data);
            throw the_goal;
        }
    }

protected:
    // Map containing parent information.  Used to extract shortest path.
    SearchData search_data;
    // Set containing candidate goal nodes
    Targets targets;
    bool negative_targets;
};

Graph::Graph(void)
{
    graphRaw_ = new GraphType();
}

Graph::~Graph(void)
{
    delete graph_;
    graphRaw_ = NULL;
}

void Graph::clear(void)
{
    graph_->clear();
}

void Graph::getNeighbors(unsigned int vtx, std::vector<unsigned int>& nbrs) const
{
    nbrs.clear();
    std::pair<AdjIterator, AdjIterator> iterators = boost::adjacent_vertices(boost::vertex(vtx, *graph_), *graph_);
    boost::property_map<GraphType, boost::vertex_index_t>::type vertices = get(boost::vertex_index, *graph_);
    for (AdjIterator iter = iterators.first; iter != iterators.second; ++iter)
        nbrs.push_back(vertices[*iter]);
}




unsigned int Graph::getNumNeighbors(unsigned int vtx) const
{
    return boost::degree(boost::vertex(vtx, *graph_), *graph_);
}

void Graph::readGraphML(const char* filename)
{
    clear();

    std::ifstream fin;
    fin.open(filename);
    if (!fin)
    {
        std::cout<<"Could not open"<<filename;
        return;
    }

    boost::dynamic_properties dp;
    dp.property("Coordinate", get(vertex_location, *graph_));
    dp.property("Weight", get(boost::edge_weight, *graph_));

    try
    {
        boost::read_graphml(fin, *(graph_), dp);
    }
    catch (boost::parse_error err)
    {
        std::cout << err.what() << std::endl;
    }
    fin.close();
}

void Graph::writeGraphML(const char* filename)
{
    std::ofstream fout;
    fout.open(filename);
    assert(filename);

    boost::dynamic_properties dp;
    dp.property("Coordinate", get(vertex_location, *graph_));
    dp.property("Weight", get(boost::edge_weight, *graph_));
    try
    {
        boost::write_graphml(fout, *(graph_), dp, true);
    }
    catch (boost::parse_error err)
    {
        std::cout << err.what() << std::endl;
    }
    fout.close();
}

double Graph::distanceHeuristic(unsigned int v1, unsigned int v2) const
{
    assert(numNodes() > v1 && numNodes() > v2);
    boost::property_map<GraphType, vertex_location_t>::type locs = get(vertex_location, *graph_);

    double dx = locs[v1].x - locs[v2].x;
    double dy = locs[v1].y - locs[v2].y;
    double dz = locs[v1].z - locs[v2].z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

unsigned int Graph::numEdges() const
{
    return boost::num_edges(*graph_);
}

unsigned int Graph::numNodes() const
{
    return boost::num_vertices(*graph_);
}

double Graph::edgeWeight(unsigned int v1, unsigned int v2) const
{
    boost::property_map<GraphType, boost::edge_weight_t>::type weights = get(boost::edge_weight, *graph_);

    Edge e;
    bool found;
    tie(e, found) = boost::edge(v1, v2, *graph_);

    if (found)
        return weights[e];

    std::cout<<"Edge %u => %u NOT FOUND"<< v1<< v2;
    return 0.0;
}

bool Graph::edgeExists(unsigned int v1, unsigned int v2) const
{
    Edge e;
    bool found;
    tie(e, found) = boost::edge(v1, v2, *graph_);

    return found;
}

void Graph::blockVertices(const std::vector <unsigned int>& blocked_vertices,
                        std::map<std::pair<unsigned int, unsigned int>, double>& cleared_edges)
{
    boost::property_map<GraphType, boost::edge_weight_t>::type edgeweights = get(boost::edge_weight, *graph_);

    for (size_t i = 0; i < blocked_vertices.size (); i++)
    {
        std::vector <unsigned int> nbrs;
        getNeighbors (blocked_vertices[i], nbrs);

        // Getting weights
        for (size_t j = 0; j < nbrs.size (); j++)
        {
            Edge e;
            bool found;
            tie(e, found) = boost::edge(blocked_vertices[i], nbrs[j], *graph_);

            cleared_edges[std::make_pair<unsigned int, unsigned int> (blocked_vertices[i], nbrs[j])] =
                edgeweights[e];
        }

        // Removing edges from blocked vertices in graph
        boost::clear_vertex (blocked_vertices[i], *graph_);
    }
}

void Graph::restoreVertices(std::map<std::pair<unsigned int, unsigned int>, double>& cleared_edges)
{
    std::map<std::pair<unsigned int, unsigned int>, double>::const_iterator iter = cleared_edges.begin ();

    for (; iter != cleared_edges.end (); ++iter)
    {
        boost::add_edge(boost::vertex(iter->first.first, *graph_), boost::vertex(iter->first.second, *graph_), iter->second, *graph_);
    }
}

bool Graph::AStar(unsigned int start, unsigned int goal, std::vector<unsigned int>& path) const
{
    std::vector<Vertex> p(numNodes());
    std::vector<double> d(numNodes());
    path.clear ();

    try
    {
        boost::astar_search (*graph_, boost::vertex(start, *graph_),
                             DistanceHeuristic <GraphType, double, boost::property_map<GraphType, vertex_location_t>::type>(get(vertex_location, *graph_), boost::vertex(goal, *graph_)),
                             boost::predecessor_map(&p[0]).distance_map(&d[0]).visitor(AStarGoalVisitor<Vertex>(boost::vertex(goal, *graph_))));
    }
    catch (foundAStarGoal fg)
    {
        boost::property_map<GraphType, boost::vertex_index_t>::type indices = get(boost::vertex_index, *graph_);
        for (Vertex v = boost::vertex(goal, *graph_); v != boost::vertex(start, *graph_); v = p[v])
            path.insert(path.begin(), indices[v]);
        path.insert(path.begin(), boost::vertex(start, *graph_));
        return true;
    }

    return false;
}

bool Graph::AStar(unsigned int start, unsigned int goal, std::vector<unsigned int>& path, const std::vector<unsigned int>& obstacles)
{
    std::map<std::pair<unsigned int, unsigned int>, double> blocked;
    blockVertices(obstacles, blocked);
    bool ret = AStar(start, goal, path);
    restoreVertices(blocked);

    return ret;
}

bool Graph::BrFS(unsigned int start, const std::set<unsigned int>& targets,
                  std::vector<unsigned int>& path, const std::vector<unsigned int>& obstacles, bool negative_target)
{
    std::map<std::pair<unsigned int, unsigned int>, double> blocked;
    blockVertices(obstacles, blocked);
    bool ret = BrFS(start, targets, path, negative_target);
    restoreVertices(blocked);

    return ret;
}

bool Graph::BrFS(unsigned int start, const std::set<unsigned int>& targets, std::vector<unsigned int>& path, bool negative_target) const
{
    typedef std::map<unsigned int, unsigned int> SearchData;
    path.clear();

    brfsSearchVisitor<GraphType, SearchData, std::set<unsigned int> >brfs_visitor(targets, negative_target);

    try
    {
        boost::breadth_first_search (*graph_, boost::vertex(start, *graph_), boost::visitor(brfs_visitor));
    }
    catch (foundBrFSGoal<SearchData> brfs_goal) // End of successful search.  If exception isn't thrown, no goal found
    {
        unsigned int goal = brfs_goal.goal;

        // Constructing shortest path.  Work backwards from goal, and make sure to add in start (not part of search)
        unsigned int index = goal;
        do
        {
            path.insert(path.begin (), index);
            index = brfs_goal.data[index];

            assert (index != brfs_goal.data[index]);

        } while (index != NO_VERTEX);

        return true;
    }
    return false;
}

void Graph::getMinSpanningTree(unsigned int root, std::vector<unsigned int>& predecessorList) const
{
    std::vector<Vertex> pred(numNodes());
    boost::prim_minimum_spanning_tree(*graph_, &pred[0], boost::weight_map(get(boost::edge_weight, *graph_)).
                                                         vertex_index_map(get(boost::vertex_index, *graph_)).
                                                         root_vertex(boost::vertex(root, *graph_)));

    boost::property_map<GraphType, boost::vertex_index_t>::type indices = get(boost::vertex_index, *graph_);

    predecessorList.clear();
    predecessorList.resize(numNodes());
    for (size_t i = 0; i < pred.size(); ++i)
    {
        predecessorList[i] = indices[pred[i]];
    }
}

