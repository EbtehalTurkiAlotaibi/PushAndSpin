#include "DataStructures.h"
#
std::string IntToString (  float number  )
{
std::ostringstream oss;
oss<< number;
return oss.str();
}
void PushAndSpinTest (string& graphName, string& instanceName,string& graphRec,string& DataFile,string& instanceType,DataRow& dataRow)  {

        // Create Graph :
        Instance config;

        config.Parsing(instanceName.c_str());
        // Read in the problem instance

        // Construct the graph for the instance
        Graph graph;
        graph.readGraphML(graphName.c_str());

        // Create PushAndSpin planner
        std::vector<Agent> agents = config.getAgents();

        bool PAR=true;
        PushAndSpin pas(agents, &graph, PAR);

        if (!PAR) {//Push and Swap
            PushAndSwapTest (graphName, instanceName,graphRec,DataFile,instanceType,dataRow);
            return;
        }

        //Prepare GraphRec records
        CreateGraphRecRecords (graph,config,graphRec) ;
        std::vector<Agent> BU = pas.getAgents();
        // Solve the problem
        clock_t tStart =clock();
        if (pas.solve())
        {
            //Prepare GraphRec records
            int path_len=0,condense_len=0;
            PushAndSpin::Plan plan = pas.getPlan();
            PushAndSpin::Condense simulatnous_indexes = pas.getCondense();
            int reversed_moves = pas.getReversedmoves();
            writePlan(plan,graphRec,path_len,condense_len,true,simulatnous_indexes);
            agents = pas.getAgents();
            float sol_per = ComputeCompletenessRate (BU,agents);
            float completed_agents = sol_per*100;
            double exexc_time = (double)(clock() - tStart)/CLOCKS_PER_SEC;
            float reverseRate = 100*(float (reversed_moves) / float (path_len));

            dataRow.agentsNumber=agents.size();
            dataRow.nodesNumber = graph.numNodes();
            dataRow.EdgesNumber = graph.numEdges();
            if (dataRow.graphName =="") dataRow.graphName= instanceType;
            dataRow.solverName = "Push And Spin";
            dataRow.freesNumber =dataRow.nodesNumber-dataRow.agentsNumber;
            dataRow.completenessRate = completed_agents;
            dataRow.condenseLength = condense_len;
            dataRow.executionTime = exexc_time;
            dataRow.pathLength = path_len;
            dataRow.solverName = "Push And Spin";
            dataRow.reverseRate = reverseRate;


            std::cout << "SOLUTION FOUND" << std::endl;
            std::cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n\n";
            std::cout<<"\t\tSolution Completeness = ";
            std::cout<<completed_agents<<std::endl<<std::endl<<std::endl;
            std::cout<<"++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n\n";
            std::cout << "======== Unsmoothed plan ========" << std::endl;
            std::cout<<"********* Plan Statistics *********" << std::endl;
            std::cout<<"Completed Agents = "<<completed_agents<<"%"<<std::endl;
            printf("Execution Time (Net) = %.2fs\n",exexc_time );
            std::cout<<"Path Length = "<<(condense_len)<<std::endl;
            // ==========================//
            string solver ="Push And Spin";
            writeStatisitcs (DataFile,dataRow);

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
           /*QMessageBox msgBox;
           msgBox.setText("Result Summary");
           msgBox.setInformativeText(str);
           msgBox.exec();
           */

        }
        else
        {
            std::cout << "NO SOLUTION FOUND" << std::endl;
        }

}
