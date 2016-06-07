#ifndef ELEMENTARYCYCLESSEARCH_H
#define ELEMENTARYCYCLESSEARCH_H

#include <vector>
#include <algorithm>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <set>
#include <hash_set>
#include <stddef.h>
#include <cstddef>
#define nullptr NULL
//*****************************************************************************************************************************//
//												class SCCResult																   //
//*****************************************************************************************************************************//


                    /// <summary>
                    /// Calculates a adjacency-list for a given array of an adjacency-matrix.
                    /// </summary>
                    /// <param name="adjacencyMatrix"> array with the adjacency-matrix that represents
                    /// the graph </param>
                    /// <returns> int[][]-array of the adjacency-list of given nodes. The first
                    /// dimension in the array represents the same node as in the given
                    /// adjacency, the second dimension represents the indicies of those nodes,
                    /// that are direct successornodes of the node. </returns>
                class SCCResult
                {
                private:
                    std::set<int> nodeIDsOfSCC;
                    std::vector<std::vector<int> > adjList;
                    int lowestNodeId;

                public:
                    SCCResult(std::vector<std::vector<int> > &adjList, int lowestNodeId)
                    {
                        this->adjList = adjList;
                        this->lowestNodeId = lowestNodeId;
                        this->nodeIDsOfSCC = std::set<int>();
                        if (this->adjList.size() > 0)
                        {
                            for (unsigned int i = this->lowestNodeId; i < this->adjList.size(); i++)
                            {
                                if (this->adjList[i].size() > 0)
                                {
                                    this->nodeIDsOfSCC.insert(int(i));
                                }
                            }
                        }
                    }

                    std::vector<std::vector<int> > getAdjList()
                    {
                        return adjList;
                    }

                    virtual int getLowestNodeId()
                    {
                        return lowestNodeId;
                    }
                };


 std::vector<std::vector<int> > adjList;
 int lowestNodeId=-1;

//**************************************************************************************************************************//
//											StrongConnectedComponents class													//
//**************************************************************************************************************************//


                /// <summary>
                /// This is a helpclass for the search of all elementary cycles in a graph
                /// with the algorithm of Johnson. For this it searches for strong connected
                /// components, using the algorithm of Tarjan. The constructor gets an
                /// adjacency-list of a graph. Based on this graph, it gets a nodenumber s,
                /// for which it calculates the subgraph, containing all nodes
                /// {s, s + 1, ..., n}, where n is the highest nodenumber in the original
                /// graph (e.g. it builds a subgraph with all nodes with higher or same
                /// nodenumbers like the given node s). It returns the strong connected
                /// component of this subgraph which contains the lowest nodenumber of all
                /// nodes in the subgraph.<br><br>
                ///
                /// For a description of the algorithm for calculating the strong connected
                /// components see:<br>
                /// Robert Tarjan: Depth-first search and linear graph algorithms. In: SIAM
                /// Journal on Computing. Volume 1, Nr. 2 (1972), pp. 146-160.<br>
                /// For a description of the algorithm for searching all elementary cycles in
                /// a directed graph see:<br>
                /// Donald B. Johnson: Finding All the Elementary Circuits of a Directed Graph.
                /// SIAM Journal on Computing. Volumne 4, Nr. 1 (1975), pp. 77-84.<br><br>
                ///
                /// @author Frank Meyer, web_at_normalisiert_dot_de
                /// @version 1.1, 22.03.2009
                ///
                /// </summary>
                class StrongConnectedComponents
                {
                    /// <summary>
                    /// Adjacency-list of original graph </summary>
                public:
                    std::vector<std::vector<int> > adjListOriginal;

                    /// <summary>
                    /// Adjacency-list of currently viewed subgraph </summary>
                    std::vector<std::vector<int> > adjList;

                    /// <summary>
                    /// Helpattribute for finding scc's </summary>
                    std::vector<bool> visited;

                    /// <summary>
                    /// Helpattribute for finding scc's </summary>
                    std::vector<int> stack;

                    /// <summary>
                    /// Helpattribute for finding scc's </summary>
                    std::vector<int> lowlink;

                    /// <summary>
                    /// Helpattribute for finding scc's </summary>
                    std::vector<int> number;

                    /// <summary>
                    /// Helpattribute for finding scc's </summary>
                    int sccCounter;

                    /// <summary>
                    /// Helpattribute for finding scc's </summary>
                    std::vector<std::vector<int> > currentSCCs;

                    /// <summary>
                    /// Constructor.
                    /// </summary>
                    /// <param name="adjList"> adjacency-list of the graph </param>
                public:
                    StrongConnectedComponents(std::vector<std::vector<int> > &adjList) : adjListOriginal (adjList)
                    {
                        this->sccCounter=0;

                    }

                    /// <summary>
                    /// This method returns the adjacency-structure of the strong connected
                    /// component with the least vertex in a subgraph of the original graph
                    /// induced by the nodes {s, s + 1, ..., n}, where s is a given node. Note
                    /// that trivial strong connected components with just one node will not
                    /// be returned.
                    /// </summary>
                    /// <param name="node"> node s </param>
                    /// <returns> SCCResult with adjacency-structure of the strong
                    /// connected component; null, if no such component exists </returns>
                    SCCResult *getAdjacencyList(int node)
                    {
                        this->visited = std::vector<bool>(this->adjListOriginal.size());
                        this->lowlink = std::vector<int>(this->adjListOriginal.size());
                        this->number = std::vector<int>(this->adjListOriginal.size());
                        this->visited = std::vector<bool>(this->adjListOriginal.size());
                        this->stack = std::vector<int>();
                        this->currentSCCs = std::vector<std::vector<int> >();

                        this->makeAdjListSubgraph(node);

                        for (int i = node; i < this->adjListOriginal.size(); i++)
                        {
                            if (!this->visited[i])
                            {

                                this->getStrongConnectedComponents(i);

                                std::vector<int> nodes = this->getLowestIdComponent(); // FILL NODES WITH ZEROS!

                                if ( (nodes.size() > 0) && (std::find(nodes.begin(), nodes.end(), int(node)) == nodes.end()) && (std::find(nodes.begin(), nodes.end(), int(node+1)) == nodes.end()) )
                                {


                                    return this->getAdjacencyList(node + 1);
                                }
                                else
                                {
                                    std::vector<std::vector<int> > adjacencyList = this->getAdjList(nodes);
                                    if (adjacencyList.size() > 0)
                                    {
                                        for (int j = 0; j < this->adjListOriginal.size(); j++)
                                        {

                                            if (adjacencyList[j].size() > 0)  // ALWAYS ZERO !!!!!
                                            {
                                                SCCResult* ss = new SCCResult(adjacencyList, j);
                                                return ss;
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        return nullptr;
                    }

                    /// <summary>
                    /// Builds the adjacency-list for a subgraph containing just nodes
                    /// >= a given index.
                    /// </summary>
                    /// <param name="node"> Node with lowest index in the subgraph </param>
                    void makeAdjListSubgraph(int node)
                    {

                        this->adjList = std::vector<std::vector<int> >(this->adjListOriginal.size(), std::vector<int>(0,0));
                        for (int i = node; i < this->adjList.size(); i++)
                        {
                            std::vector<int> successors = std::vector<int>();
                            for (int j = 0; j < this->adjListOriginal[i].size(); j++)
                            {
                                if (this->adjListOriginal[i][j] >= node)
                                {
                                    successors.push_back(int(this->adjListOriginal[i][j]));

                                }
                            }
                            if (successors.size() > 0)
                            {
                                this->adjList[i] = std::vector<int>(successors.size());
                                for (int j = 0; j < successors.size(); j++)
                                {
                                    int succ = static_cast<int>(successors[j]);
                                    this->adjList[i][j] = succ;
                                }
                            }
                        }

                    }

                    /// <summary>
                    /// Calculates the strong connected component out of a set of scc's, that
                    /// contains the node with the lowest index.
                    /// </summary>
                    /// <returns> Vector::Integer of the scc containing the lowest nodenumber </returns>
                    std::vector<int> getLowestIdComponent()
                    {
                        int min = this->adjList.size();
                        std::vector<int> currScc;

                        for (int i = 0; i < this->currentSCCs.size(); i++)
                        {
                            std::vector<int> scc = std::vector<int> (this->currentSCCs[i]);
                            for (int j = 0; j < scc.size(); j++)
                            {
                                //std::cout<<scc[j]<<"\n";
                                int node = scc[j];
                                if (node < min)
                                {
                                    currScc = scc;
                                    min = node;
                                }
                            }
                        }

                        return currScc;
                    }

                    /// <returns> Vector[]::Integer representing the adjacency-structure of the
                    /// strong connected component with least vertex in the currently viewed
                    /// subgraph </returns>
                    std::vector<std::vector<int> > getAdjList(std::vector<int>& nodes)
                    {

                        std::vector<std::vector<int> > lowestIdAdjacencyList = std::vector<std::vector<int> >(this->adjList.size());


                        if (nodes.size() > 0)
                        {

                            for (int i = 0; i < lowestIdAdjacencyList.size(); i++)
                            {
                                lowestIdAdjacencyList[i] = std::vector<int>();
                            }

                            for (int i = 0; i < nodes.size(); i++)
                            {
                                int node = (static_cast<int>(nodes[i]));
                                for (int j = 0; j < this->adjList[node].size(); j++)
                                {
                                    int succ = this->adjList[node][j];
                                    if (std::find(nodes.begin(), nodes.end(), int(succ)) != nodes.end())
                                    {
                                        lowestIdAdjacencyList[node].push_back(int(succ));
                                    }
                                }
                            }
                        }

                        return lowestIdAdjacencyList;
                    }

                    void getStrongConnectedComponents(int root)
                    {
                        this->sccCounter++;
                        this->lowlink[root] = this->sccCounter;
                        this->number[root] = this->sccCounter;
                        this->visited[root] = true;
                        this->stack.push_back(int(root));

                        for (int i = 0; i < this->adjList[root].size(); i++)
                        {
                            int w = this->adjList[root][i];
                            if (!this->visited[w])
                            {
                                this->getStrongConnectedComponents(w);
                                this->lowlink[root] = std::min(lowlink[root], lowlink[w]);
                            }
                            else if (this->number[w] < this->number[root])
                            {
                                if (std::find(this->stack.begin(), this->stack.end(), int(w)) != this->stack.end())
                                {
                                    lowlink[root] = std::min(this->lowlink[root], this->number[w]);
                                }
                            }
                       }
                        if ((lowlink[root] == number[root]) && (stack.size() > 0))
                        {
                            int next = -1;
                            std::vector<int> scc = std::vector<int>();
                            do
                            {
                                next = (static_cast<int>(this->stack[stack.size() - 1]));
                                this->stack.pop_back();
                                scc.push_back(int(next));
                            } while (this->number[next] > this->number[root]);
                            // simple scc's with just one node will not be added
                            if (scc.size() > 1)
                            {
                                this->currentSCCs.push_back (scc);
                            }
                        }
                    }
                };


//********************************************************************************************************************************//
//													AdjacencyList Class															  //
//********************************************************************************************************************************//
                    std::vector<std::vector<int> > getAdjacencyList(std::vector<std::vector<bool> > &adjacencyMatrix)
                    {
                        std::vector<std::vector<int> > list(adjacencyMatrix.size());

                        for (int i = 0; i < adjacencyMatrix.size(); i++)
                        {
                            std::vector<int> v = std::vector<int>();
                            for (int j = 0; j < adjacencyMatrix[i].size(); j++)
                            {
                                if (adjacencyMatrix[i][j])
                                {
                                    v.push_back(int(j));
                                }
                            }

                            list[i] = std::vector<int>(v.size());
                            for (int j = 0; j < v.size(); j++)
                            {
                                int in_Renamed = static_cast<int>(v[j]);
                                list[i][j] = in_Renamed;
                            }
                        }

                        return list;
                    }

//********************************************************************************************************************************//

//********************************************************************************************************************************//

//----------------------------------------------------------------------------------------
//	Copyright \A9 2007 - 2015 Tangible Software Solutions Inc.
//	This class can be used by anyone provided that the copyright notice remains intact.
//
//	This class includes methods to convert multidimensional arrays to C++ vectors.
//----------------------------------------------------------------------------------------

    std::vector<std::vector<bool> >& ReturnRectangularBoolVector(int size1, int size2)
    {
        std::vector<std::vector<bool> > newVector(size1);
        for (int vector1 = 0; vector1 < size1; vector1++)
        {
            newVector[vector1] = std::vector<bool>(size2);
            for (int x = 0 ; x <size2 ; x++)
                newVector[vector1][x] = false;
        }

        return newVector;
    }


//*****************************************************************************************************************************//
//												class ElementaryCyclesSearch												   //
//*****************************************************************************************************************************//
                /// <summary>
                /// Searchs all elementary cycles in a given directed graph. The implementation
                /// is independent from the concrete objects that represent the graphnodes, it
                /// just needs an array of the objects representing the nodes the graph
                /// and an adjacency-matrix of type boolean, representing the edges of the
                /// graph. It then calculates based on the adjacency-matrix the elementary
                /// cycles and returns a list, which contains lists itself with the objects of the
                /// concrete graphnodes-implementation. Each of these lists represents an
                /// elementary cycle.<br><br>
                ///
                /// The implementation uses the algorithm of Donald B. Johnson for the search of
                /// the elementary cycles. For a description of the algorithm see:<br>
                /// Donald B. Johnson: Finding All the Elementary Circuits of a Directed Graph.
                /// SIAM Journal on Computing. Volumne 4, Nr. 1 (1975), pp. 77-84.<br><br>
                ///
                /// The algorithm of Johnson is based on the search for strong connected
                /// components in a graph. For a description of this part see:<br>
                /// Robert Tarjan: Depth-first search and linear graph algorithms. In: SIAM
                /// Journal on Computing. Volume 1, Nr. 2 (1972), pp. 146-160.<br>
                ///
                /// @author Frank Meyer, web_at_normalisiert_dot_de
                /// @version 1.2, 22.03.2009
                ///
                /// </summary>
                class ElementaryCyclesSearch
                {
                    /// <summary>
                    /// List of cycles </summary>
                private:
                    std::vector<std::vector<unsigned int> >cycles;

                    /// <summary>
                    /// Adjacency-list of graph </summary>
                    std::vector<std::vector<int> > adjList;

                    /// <summary>
                    /// Graphnodes </summary>
                    std::vector<int> graphNodes;

                    /// <summary>
                    /// Blocked nodes, used by the algorithm of Johnson </summary>
                    std::vector<bool> blocked;

                    /// <summary>
                    /// B-Lists, used by the algorithm of Johnson </summary>
                    std::vector<std::vector<int> > B;

                    /// <summary>
                    /// Stack for nodes, used by the algorithm of Johnson </summary>
                    std::vector<int> stack;

                    /// <summary>
                    /// Constructor.
                    /// </summary>
                    /// <param name="matrix"> adjacency-matrix of the graph </param>
                    /// <param name="graphNodes"> array of the graphnodes of the graph; this is used to
                    /// build sets of the elementary cycles containing the objects of the original
                    /// graph-representation </param>
                public:
                    ElementaryCyclesSearch(std::vector<std::vector<bool> > &matrix, std::vector<int> &graphNodes)
                    {
                         this->graphNodes = graphNodes;
                         this->adjList = getAdjacencyList(matrix);

                    }

                    /// <summary>
                    /// Returns List::List::Object with the Lists of nodes of all elementary
                    /// cycles in the graph.
                    /// </summary>
                    /// <returns> List::List::Object with the Lists of the elementary cycles. </returns>
                    std::vector<std::vector<unsigned int> >getElementaryCycles()
                    {
                         cycles = std::vector<std::vector<unsigned int> >();
                         blocked = std::vector<bool>( adjList.size());
                         B = std::vector<std::vector<int> >( adjList.size());
                         stack = std::vector<int>();
                        StrongConnectedComponents sccs (adjList);

                        int s = 0;

                        while (true)
                        {


                            SCCResult* sccResult = sccs.getAdjacencyList(s);

                            if (sccResult!= nullptr)
                            {


                                std::vector<std::vector<int> > scc (sccResult->getAdjList());
                                s = (sccResult->getLowestNodeId());

                                for (int j = 0; j < scc.size(); j++)
                                {

                                    if ((scc[j].size() > 0) && (scc[j].size() > 0))
                                    {
                                         blocked[j] = false;
                                         B[j] = std::vector<int>();
                                    }
                                }


                                 findCycles(s, s, scc);
                                s++;
                            }
                            else
                            {
                                break;
                            }
                        }

                        return  cycles;
                    }

                    /// <summary>
                    /// Calculates the cycles containing a given node in a strongly connected
                    /// component. The method calls itself recursivly.
                    /// </summary>
                    /// <param name="v"> </param>
                    /// <param name="s"> </param>
                    /// <param name="adjList"> adjacency-list with the subgraph of the strongly
                    /// connected component s is part of. </param>
                    /// <returns> true, if cycle found; false otherwise </returns>
                private:

                    bool isBlocked (std::vector<unsigned int> cycle) {
                        // 1- check if cycle is subset any registered cycle

                        for (int c=0; c<cycle.size(); c++) {
                            int ptr = cycle[c];
                        for (int i=0; i<this->cycles.size(); i++)
                            for (int j=0; j<this->cycles[i].size(); j++){
                                if (ptr == this->cycles[i][j]) { // if one node is inside the cycle [i][j], check others
                                    int cntr=0;
                                    for (int p=0; p<cycle.size(); p++) {
                                        if (std::find(this->cycles[i].begin(),this->cycles[i].end(),cycle[p])!=this->cycles[i].end())
                                            cntr++;
                                    }
                                    if (cntr == cycle.size()) { // cycle is subset of other cycle !
                                        this->cycles.erase(this->cycles.begin()+i);
                                        this->cycles.push_back(cycle);
                                        return true;
                                    }
                                }
                            }
                        }

                        // 2- check if cycle is superset of any registered cycle
                        for (int c=0; c<cycle.size(); c++) {

                        for (int i=0; i<this->cycles.size(); i++)
                            for (int j=0; j<this->cycles[i].size(); j++){
                                int ptr = this->cycles[i][j];
                                if (ptr == cycle[c]) {
                                    int cntr=0;
                                    for (int p=0; p<this->cycles[i].size(); p++) {
                                        if (std::find(cycle.begin(),cycle.end(),this->cycles[i][p])!=cycle.end())
                                            cntr++;
                                    }
                                    if (cntr == this->cycles[i].size()) { // cycle is superset of other cycle !
                                        return true;
                                    }
                                }

                            }
                        }

                        return false;
                    }
                    bool findCycles(int v, int s, std::vector<std::vector<int> > &adjList)
                    {

                        bool f = false;
                         stack.push_back(int(v));           // we want to procces v
                         blocked[v] = true;                 // v is blocked
                        for (int i = 0; i < adjList[v].size(); i++) // for all neighbours of v
                        {
                            int w = (static_cast<int>(adjList[v][i])); // w = neighbours of v

                                // found cycle C = all stack nodes
                                if ((w == s))
                                {
                                    std::vector<unsigned int> cycle = std::vector<unsigned int>();
                                    for (int j = 0; j <  stack.size(); j++)
                                    {
                                        int index = (static_cast<int>( stack[j]));
                                        cycle.push_back( graphNodes[index]);
                                    }

                                    // if this cycle is not contains other registered cycle or not a part of other cycle
                                    this->cycles.push_back(cycle);
                                    f = true;

                                }
                                else if (! blocked[w])
                                {
                                    if ( findCycles(w, s, adjList))
                                    {
                                        f = true;
                                    }
                                }

                        }

                        if (f)
                        {
                             unblock(v);
                        }
                        else
                        {
                            for (int i = 0; i < adjList[v].size(); i++)
                            {
                                int w = (static_cast<int>(adjList[v][i]));
                                if (std::find( B[w].begin(),  B[w].end(), int(v)) ==  B[w].end())
                                {
                                     B[w].push_back(int(v));
                                }
                            }
                        }

                        stack.erase (std::remove(stack.begin(),stack.end(),v),stack.end());
                        //stack.erase(stack.begin()+(v));
                        return f;
                    }



                    /// <summary>
                    /// Unblocks recursivly all blocked nodes, starting with a given node.
                    /// </summary>
                    /// <param name="node"> node to unblock </param>
                    void unblock(int node)
                    {
                         blocked[node] = false;
                        std::vector<int> Bnode =  B[node];
                        while (Bnode.size() > 0)
                        {
                            int w = static_cast<int>(Bnode[0]);
                            Bnode.erase(Bnode.begin()+0);
                            if ( blocked[w])
                            {
                                 unblock(w);
                            }
                        }
                    }
                };





#endif // ELEMENTARYCYCLESSEARCH_H
