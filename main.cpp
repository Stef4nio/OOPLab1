#include <iostream>
#include <sstream>
#include "Graph.h"
#include "Network.h"

int main()
{
    IpV4NetworkParticipant pc1("192.168.0.1/30");
    IpV4NetworkParticipant pc2("192.168.0.2/25");
    IpV6NetworkParticipant pc1v6("0d00:f000:0000:0000:0000:ff00:0042:8329/18");
    IpV6NetworkParticipant pc2v6("0d00:c000:0000:0000:0000:ff00:0042:8329/18");
    cout <<"Pc1 " << (pc1.Ping(pc2)?"can":"can't")<<" ping pc2"<<endl;
    cout <<"Pc1v6 " << (pc1v6.Ping(pc2v6)?"can":"can't")<<" ping pc2v6"<<endl;

    AbstractGraph<IpV4NetworkParticipant,IpV6NetworkParticipant>* aGraph;
    ListGraph<IpV4NetworkParticipant,IpV6NetworkParticipant> listGraph;
    MatrixGraph<IpV4NetworkParticipant,IpV6NetworkParticipant> matrixGraph;
    aGraph = &matrixGraph;
    //aGraph = &listGraph;
    aGraph->AddVertex(pc1);
    aGraph->AddVertex(pc2);
    aGraph->AddVertex(pc2);
    aGraph->AddVertex(pc1);
    aGraph->AddEdge(pc1v6, 2, 0);
    aGraph->AddEdge(pc2v6, 1, 3);
    aGraph->AddEdge(pc1v6, 2, 3);
    aGraph->AddEdge(pc2v6, 1, 2);
    aGraph->AddEdge(pc2v6, 3, 0);
    aGraph->AddEdge(pc1v6, 1, 0);
    aGraph->AddVertex(pc2);
    aGraph->AddEdge(pc1v6,4,0);
    aGraph->AddEdge(pc2v6,4,3);
    aGraph->AddEdge(pc2v6,4,2);
    aGraph->AddEdge(pc1v6,4,1);
    //Uncomment this part of code to make graph planar
    /*aGraph->RemoveEdgeBetween(4,1);
    aGraph->RemoveEdgeBetween(0,3);
    aGraph->RemoveVertex(2);*/
    //Uncomment this part of code to make graph not connective
    //aGraph->AddVertex(pc1);

    int sourceVertex = 0;
    int destinationVertex = 3;
    cout<<"Distance between vertex "<<sourceVertex<<" and "<<destinationVertex<<" is "<<
    aGraph->GetDistance(0,3)<<endl;

    bool isPlanar = aGraph->CheckPlanarity();
    bool isConnective = aGraph->CheckConnectivity();
    std::cout << "Your graph is: "<<(isPlanar?"planar":"not planar")<<" and "<<
    (isConnective?"connective":"not connective")<< std::endl;
    return 0;
}
