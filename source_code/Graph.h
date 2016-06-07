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

#ifndef PAS_GRAPH_HPP
#define PAS_GRAPH_HPP


#include <vector>
#include <set>
#include <map>
#include <string>

class Graph
    {
        public:
            Graph();
            virtual ~Graph();

            // Remove all vertices and edges
            virtual void clear();

            // Search algorithms:
            // A* from start to goal.
            virtual bool AStar(unsigned int start, unsigned int goal, std::vector<unsigned int>& path) const;
            // A* from start to goal without expanding any vertex in obstacles
            virtual bool AStar(unsigned int start, unsigned int goal, std::vector<unsigned int>& path, const std::vector<unsigned int>& obstacles);
            // Breadth-first search from start to any target vertex without expanding any vertex in obstacles
            // If negative_target is true, then the search will cease when any vertex NOT in target is expanded
            virtual bool BrFS (unsigned int start, const std::set<unsigned int>& targets, std::vector<unsigned int>& path, const std::vector<unsigned int>& obstacles, bool negative_target=false);
            // Breadth-first search from start to any target vertex
            // If negative_target is true, then the search will cease when any vertex NOT in target is expanded
            virtual bool BrFS(unsigned int start, const std::set<unsigned int>& targets, std::vector<unsigned int>& path, bool negative_target=false) const;

            // Compute the minimum spanning tree of the graph rooted at root.  Predecessor list contains MST
            virtual void getMinSpanningTree(unsigned int root, std::vector<unsigned int>& predecessorList) const;

            // Returns number of neighboring vertices for the given vertex
            virtual unsigned int getNumNeighbors(unsigned int vtx) const;
            // Returns the neighbors of the given vertex
            virtual void getNeighbors(unsigned int vtx, std::vector<unsigned int>& nbrs) const;

            // Return the number of edges in the graph
            virtual unsigned int numEdges() const;
            // Return the number of vertices in the graph
            virtual unsigned int numNodes() const;

            // Returns the weight of the edge between vertices v1 and v2.  Zero is returned for a non-existant edge
            virtual double edgeWeight(unsigned int v1, unsigned int v2) const;

            // Returns true if an edge exists between vertices v1 and v2
            virtual bool edgeExists(unsigned int v1, unsigned int v2) const;

            // Deserializes this graph from the graphml file given at filename
            virtual void readGraphML(const char* filename);
            // Serializes this graph to a graphml file at the given filename
            virtual void writeGraphML(const char* filename);

            // Return an admissible distance heuristic between the two nodes.
            virtual double distanceHeuristic(unsigned int v1, unsigned int v2) const;

            // Set and Get functions.
            void set_counter (int index,int dis_time);
            int get_counter(int index);
            void set_type (int index,std::string type);
            std::string get_type(int index);

            // Print vertices.
            void print ();

        protected:
            void blockVertices(const std::vector <unsigned int>& blocked_vertices,
                               std::map<std::pair<unsigned int, unsigned int>, double>& cleared_edges);
            void restoreVertices(std::map<std::pair<unsigned int, unsigned int>, double>& cleared_edges);


            // Obscured to prevent unnecessary inclusion of BGL throughout the rest of the code.
            void* graphRaw_;
    };


#endif
