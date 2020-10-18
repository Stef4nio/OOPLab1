#include <utility>
#include <vector>
#include <map>
#include <algorithm>
#include <set>


#ifndef GRAPH_H
#define GRAPH_H

using namespace std;

struct Vertex
{
    int number;

    bool operator<(const Vertex& r)const
    {
        return number<r.number;
    }

    bool operator==(const Vertex& r)const
    {
        return number == r.number;
    }

    bool operator!=(const Vertex& r)const
    {
        return number != r.number;
    }
};


struct Segment
{
     vector<Vertex> SegmentVector;

     explicit Segment(const vector<Vertex>& elements)
     {
         SegmentVector = elements;
     }

    bool operator==(const Segment& r)const
    {
        vector<Vertex> sortedLeftOperand = SegmentVector;
        vector<Vertex> sortedRightOperand = r.SegmentVector;
        sort(sortedLeftOperand.begin(), sortedLeftOperand.end());
        sort(sortedRightOperand.begin(),sortedRightOperand.end());
        return sortedLeftOperand==sortedRightOperand;
    }

    bool operator!=(const Segment& r)const
    {
        return !((*this)==r);
    }

    Vertex& operator[](int x)
    {
         return SegmentVector[x];
    }
};

template<typename T,typename V>
class AbstractGraph
{
public:
    virtual bool CheckPlanarity() = 0;
    virtual bool CheckConnectivity() = 0;

    virtual void AddVertex(T vertexData) = 0;
    virtual void AddEdge(V edgeData, int firstVertexIndex, int secondVertexIndex) = 0;
    virtual void RemoveVertex(int vertexID) = 0;
    virtual void RemoveEdgeBetween(int firstVertexIndex, int secondVertexIndex) = 0;
    virtual int GetDistance(int firstVertexID, int secondVertexID) = 0;

protected:
    map<Vertex,T> VertexData;
    map<set<int>,V> EdgeData;
};

template<typename T,typename V>
class ListGraph: public AbstractGraph<T,V>
{
public:
    explicit ListGraph(const map<Vertex,vector<Vertex>>& adjList)
    {
        AdjList = adjList;
    }

    ListGraph()
    {
        AdjList = map<Vertex,vector<Vertex>>();
    }

    bool CheckPlanarity() override
    {
        if(!CheckConnectivity())
        {
            return false;
        }
        ListGraph<T,V> cycle = GetCycle();
        Segment currSegment = GetSegment(cycle);
        while (currSegment[0].number!=-1)
        {
            bool isPlaceForSegmentFound = false;
            for(auto plane:planes)
            {
                bool isStartInPlane = false,isEndInPlane = false;
                for(auto vertex:plane)
                {
                    if(vertex==currSegment[0])
                    {
                        isStartInPlane = true;
                    }
                    else if(vertex == currSegment.SegmentVector.back())
                    {
                        isEndInPlane = true;
                    }
                }
                if(isStartInPlane&&isEndInPlane)
                {
                    //Вставити сегмент в цикл
                    int segmentSize = currSegment.SegmentVector.size();
                    cycle.AdjList[currSegment[0]].push_back(currSegment[1]);
                    cycle.AdjList[currSegment[segmentSize-1]].push_back(currSegment[segmentSize-2]);
                    for(int i = 1;i<segmentSize-1;i++)
                    {
                        cycle.AdjList[currSegment[i]].push_back(currSegment[i-1]);
                        cycle.AdjList[currSegment[i]].push_back(currSegment[i+1]);
                    }
                    //Розділити відповідну грань на 2
                    vector<Vertex> firstPlane;
                    vector<Vertex> secondPlane;

                    auto segmentStart = currSegment.SegmentVector.begin();
                    auto segmentEnd = currSegment.SegmentVector.end()-1;

                    auto segmentStartInPlane = find(plane.begin(),plane.end()
                            ,currSegment.SegmentVector.front());
                    auto segmentEndInPlane = find(plane.begin(),plane.end()
                            ,currSegment.SegmentVector.back());

                    if(segmentStartInPlane>segmentEndInPlane)
                    {
                        reverse(segmentStart,segmentEnd+1);
                        auto temp = segmentStartInPlane;
                        segmentStartInPlane = segmentEndInPlane;
                        segmentEndInPlane = temp;
                    }

                    firstPlane.insert(firstPlane.begin(),segmentStartInPlane,segmentEndInPlane+1);
                    reverse(segmentStart+1,segmentEnd);
                    firstPlane.insert(firstPlane.end(),segmentStart+1,segmentEnd);
                    reverse(segmentStart+1,segmentEnd);

                    secondPlane.insert(secondPlane.begin(),segmentStart,segmentEnd+1);
                    secondPlane.insert(secondPlane.begin(),plane.begin(),segmentStartInPlane);
                    secondPlane.insert(secondPlane.end(),segmentEndInPlane+1,plane.end());

                    planes.erase(find(planes.begin(),planes.end(),plane));
                    planes.insert(planes.begin(),firstPlane);
                    planes.insert(planes.begin(),secondPlane);
                    isPlaceForSegmentFound = true;
                    break;
                }
            }
            if(!isPlaceForSegmentFound)
            {
                return false;
            }
            currSegment = GetSegment(cycle);
        }
        return true;
    }

    void AddVertex(T vertexData)
    {
        Vertex newVertex = CreateNewVertex();
        AdjList.insert(pair<Vertex,vector<Vertex>>(newVertex,vector<Vertex>(0)));
        AbstractGraph<T,V>::VertexData.insert(pair<Vertex,T>(newVertex,vertexData));
    }
    void RemoveVertex(int vertexID)
    {
        AdjList.erase(Vertex{vertexID});
        AbstractGraph<T,V>::VertexData.erase(Vertex{vertexID});
        for(auto pair:AdjList)
        {
            for(size_t i = 0;i<pair.second.size();i++)
            {
                Vertex currVertex = pair.second[i];
                if(currVertex.number == vertexID)
                {
                    pair.second.erase(pair.second.begin()+i);
                }
            }
        }
    }
    void AddEdge(V edgeData, int firstVertexIndex, int secondVertexIndex)
    {
        if(AdjList.count(Vertex{firstVertexIndex})==0||AdjList.count(Vertex{secondVertexIndex}) == 0)
        {
            return;
        }
        AbstractGraph<T,V>::EdgeData.insert(pair<set<int>,V>({firstVertexIndex,secondVertexIndex}, edgeData));
        AdjList[Vertex{firstVertexIndex}].push_back(Vertex{secondVertexIndex});
        AdjList[Vertex{secondVertexIndex}].push_back(Vertex{firstVertexIndex});
    }
    void RemoveEdgeBetween(int firstVertexIndex, int secondVertexIndex)
    {
        AbstractGraph<T,V>::EdgeData.erase(set<int>{firstVertexIndex,secondVertexIndex});
        for(size_t i = 0;i<AdjList[Vertex{firstVertexIndex}].size();i++)
        {
            Vertex currVertex = AdjList[Vertex{firstVertexIndex}][i];
            if(currVertex.number == secondVertexIndex)
            {
                AdjList[Vertex{firstVertexIndex}].erase(AdjList[Vertex{firstVertexIndex}].begin()+i);
            }
        }
        for(size_t i = 0;i<AdjList[Vertex{secondVertexIndex}].size();i++)
        {
            Vertex currVertex = AdjList[Vertex{secondVertexIndex}][i];
            if(currVertex.number == firstVertexIndex)
            {
                AdjList[Vertex{secondVertexIndex}].erase(AdjList[Vertex{secondVertexIndex}].begin()+i);
            }
        }
    }
    int GetDistance(int firstVertexID, int secondVertexID)
    {
        vector<Vertex> usedVerticies;
        return FindDistance(Vertex{secondVertexID},Vertex{firstVertexID},usedVerticies);
    }
    bool CheckConnectivity()
    {
        vector<Vertex> usedVerticies(0);
        TraverseGraph(AdjList.begin()->first,usedVerticies);
        return usedVerticies.size()==AdjList.size();
    }



private:
    int currVertexID = 0;
    map<Vertex,vector<Vertex>> AdjList;
    vector<vector<Vertex>> planes;
    Vertex CreateNewVertex()
    {
        return Vertex{currVertexID++};
    }
    ListGraph FindCycle(vector<Vertex>& usedVerticies, const Vertex &currVertex, const Vertex &parentVertex)
    {
        usedVerticies.push_back(currVertex);

        for(auto vertex: AdjList[currVertex])
        {
            if(find(usedVerticies.begin(),usedVerticies.end(),vertex)==usedVerticies.end())
            {
                ListGraph<T,V> result = FindCycle(usedVerticies, vertex, currVertex);
                if(!result.AdjList.empty())
                {
                    return result;
                }
            }
            else if(vertex!=parentVertex)
            {
                vector<Vertex> cycleVector;
                cycleVector.insert(cycleVector.begin(), find(usedVerticies.begin(), usedVerticies.end(), vertex),
                                   usedVerticies.end());
                //Додати внутрішню грань циклу
                planes.push_back(cycleVector);
                //Додати зовнішню грань циклу
                planes.push_back(cycleVector);
                map<Vertex, vector<Vertex>> result;
                result[cycleVector[0]] = {cycleVector[1],cycleVector[cycleVector.size()-1]};
                result[cycleVector[cycleVector.size()-1]] = {cycleVector[0],cycleVector[cycleVector.size()-2]};
                for(int i = 1;i<cycleVector.size()-1;i++)
                {
                    result[cycleVector[i]] = {cycleVector[i-1],cycleVector[i+1]};
                }
                return ListGraph<T,V>(result);
            }
        }
        return ListGraph<T,V>();
    }
    Segment FindSegment(const ListGraph &cycle, const Vertex &currVertex, vector<Vertex> usedVerticies)
    {
        usedVerticies.push_back(currVertex);
        if(cycle.AdjList.count(currVertex)==0)
        {
            for(auto vertex:AdjList[currVertex])
            {
                if(find(usedVerticies.begin(),usedVerticies.end(),vertex)==usedVerticies.end())
                {
                    Segment result = FindSegment(cycle,vertex,usedVerticies);
                    return result;
                }
            }
        }
        else
        {
            return Segment(usedVerticies);
        }
    }
    ListGraph GetCycle()
    {
        vector<Vertex> usedVerticies;
        return FindCycle(usedVerticies,AdjList.begin()->first,Vertex{-1});
    }
    Segment GetSegment(const ListGraph &cycle)
    {
        using namespace std;

        for(auto const& pair:cycle.AdjList)
        {
            auto cycleVertexAdjBegin = cycle.AdjList.at(pair.first).begin();
            auto cycleVertexAdjEnd = cycle.AdjList.at(pair.first).end();
            for(auto graphVertex:AdjList[pair.first])
            {
                if(find(cycleVertexAdjBegin,cycleVertexAdjEnd,graphVertex)==cycleVertexAdjEnd)
                {
                    vector<Vertex> usedVerticies;
                    usedVerticies.push_back(pair.first);
                    Segment segment = FindSegment(cycle,graphVertex,usedVerticies);
                    return segment;
                    //Segment to graph algorythm
                    int segmentSize = segment.SegmentVector.size();
                    map<Vertex, vector<Vertex>> segmentMap;
                    segmentMap[segment[0]] = {segment[1]};
                    segmentMap[segment[segmentSize-1]] = {segment[segmentSize-2]};
                    for(int i = 1;i<segmentSize-1;i++)
                    {
                        segmentMap[segment[i]] = {segment[i-1],segment[i+1]};
                    }
                }
            }
        }
        return Segment(vector<Vertex>{Vertex{-1}});
    }
    int FindDistance(const Vertex& destination, const Vertex& currVertex, vector<Vertex>& usedVerticies, int depth=1)
    {
        int result = -1;
        bool isFirstIteration = true;
        usedVerticies.push_back(currVertex);
        for(auto vertex:AdjList[currVertex])
        {
            if(vertex == destination)
            {
                return depth;
            }
            else
            {

                if(find(usedVerticies.begin(),usedVerticies.end(),vertex) == usedVerticies.end())
                {
                    int temp = FindDistance(destination,vertex,usedVerticies,depth+1);
                    if((temp!=-1&&temp<result)||isFirstIteration)
                    {
                        result = temp;
                        isFirstIteration = false;
                    }
                }
            }
        }
        return result;
    }
    void TraverseGraph(const Vertex& currVertex, vector<Vertex>& usedVerticies)
    {
        usedVerticies.push_back(currVertex);
        for(auto vertex: AdjList[currVertex])
        {
            if(find(usedVerticies.begin(),usedVerticies.end(),vertex)==usedVerticies.end())
            {
                TraverseGraph(vertex,usedVerticies);
            }
        }
    }
};

template<typename T,typename V>
class MatrixGraph: public AbstractGraph<T,V>
{
public:

    void AddVertex(T vertexData)
    {
        AdjMatrix.push_back(vector<bool>(AdjMatrix.size(),false));
        AbstractGraph<T,V>::VertexData[Vertex{AdjMatrix.size()-1}] = vertexData;
        for(auto& row:AdjMatrix)
        {
            row.push_back(false);
        }
    }
    void RemoveVertex(int vertexID)
    {
        AdjMatrix.erase(AdjMatrix.begin()+vertexID);
        AbstractGraph<T,V>::VertexData.erase(Vertex{vertexID});
        for(auto& row:AdjMatrix)
        {
            row.erase(row.begin()+vertexID);
        }
    }
    void AddEdge(V edgeData, int firstVertexIndex, int secondVertexIndex)
    {
        AbstractGraph<T,V>::EdgeData.insert(pair<set<int>,V>{{firstVertexIndex,secondVertexIndex},edgeData});
        AdjMatrix[firstVertexIndex][secondVertexIndex] = true;
        AdjMatrix[secondVertexIndex][firstVertexIndex] = true;
    }
    void RemoveEdgeBetween(int firstVertexIndex, int secondVertexIndex)
    {
        AbstractGraph<T,V>::EdgeData.erase({firstVertexIndex,secondVertexIndex});
        AdjMatrix[firstVertexIndex][secondVertexIndex] = false;
        AdjMatrix[secondVertexIndex][firstVertexIndex] = false;
    }

    int GetDistance(int firstVertexID, int secondVertexID)
    {
        vector<int> usedVerticies;
        return FindDistance(firstVertexID,secondVertexID,usedVerticies);
    }
    bool CheckConnectivity()
    {
        vector<int> usedVerticies(0);
        TraverseGraph(0,usedVerticies);
        return usedVerticies.size() == AdjMatrix.size();
    }
    bool CheckPlanarity()
    {
        map<Vertex,vector<Vertex>> AdjList;
        for(int i = 0;i<AdjMatrix.size();i++)
        {
            Vertex currVertex{i};
            AdjList.insert(pair<Vertex,vector<Vertex>>(currVertex,{}));
            for(int j = 0;j<AdjMatrix[i].size();j++)
            {
                if(AdjMatrix[i][j])
                {
                    AdjList[currVertex].push_back(Vertex{j});
                }
            }
        }
        ListGraph<T,V> graph(AdjList);
        return graph.CheckPlanarity();
    }

private:
    vector<vector<bool>> AdjMatrix;
    void TraverseGraph(int currVertex, vector<int>& usedVerticies)
    {
        usedVerticies.push_back(currVertex);
        for(int i = 0;i<AdjMatrix[currVertex].size();i++)
        {
            if(AdjMatrix[currVertex][i]&&find(usedVerticies.begin(),usedVerticies.end(),i)==usedVerticies.end())
            {
                TraverseGraph(i,usedVerticies);
            }
        }
    }
    int FindDistance(int currVertex, int destination, vector<int>& usedVerticies, int depth = 1)
    {
        int result = -1;
        bool isFirstIteration = true;
        usedVerticies.push_back(currVertex);
        for(int i = 0;i<AdjMatrix[currVertex].size();i++)
        {
            if(i == destination&&AdjMatrix[currVertex][i])
            {
                return depth;
            }
            else
            {
                if(AdjMatrix[currVertex][i]&&find(usedVerticies.begin(),usedVerticies.end(),i) == usedVerticies.end())
                {
                    int temp = FindDistance(destination,i,usedVerticies,depth+1);
                    if((temp!=-1&&temp<result)||isFirstIteration)
                    {
                        result = temp;
                        isFirstIteration = false;
                    }
                }
            }
        }
        return result;
    }
};


#endif //GRAPH_H
