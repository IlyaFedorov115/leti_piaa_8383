#include <iostream>
#include <map>
#include <deque>
#include <queue>
#include <stack>
#include <climits>
#include <algorithm>
#include <fstream>

using Graph = std::map<char, std::map<char, int>>;          //граф хранится как список смежности
std::string LINE = "-----------------------------------------\n";

// поиск улучшающего пути
std::pair<int, bool> findPath(Graph & residGraph, char start, char goal, std::map<char, char>& currPath){
  // контейнер посещенных вершин 
  std::map<char, bool> visitVertex;
  char current;
  currPath.clear();
  
  // сначала выбирается вершина в алфавитном порядке
  // поэтому смежные вершины кладутся в очередь с приоритетом
  auto compare = [&current] (char a, char b) {
    return a > b; 
    //return residGraph[current][a] < residGraph[current][b];
  };
  std::priority_queue <char, std::vector<char>, decltype(compare)> pathVertex(compare);
  
  // сначала только стартовая
  pathVertex.push(start);
  visitVertex[start] = true;
  
  //пока не просмотрели все вершины
  while (!pathVertex.empty()){
    current = pathVertex.top();
    pathVertex.pop();
    visitVertex[current] = true;
    if (current == goal) break;
    
    //просматриваем соседей и кладем в очередь
    for (auto& neighbour: residGraph[current]){
      // пропускаются посещенные вершины и пути с нулевой остаточной
      // стоимостью, т.к. такой путь нельзя улучшить
	  if (!visitVertex[neighbour.first] && neighbour.second > 0){
	      pathVertex.push(neighbour.first);
          currPath[neighbour.first] = current;
          visitVertex[neighbour.first] = true;
	   }
	}
  }

  // минимальной стоимости присвоить макс. значение.
  int minRes = std::numeric_limits<int>::max();

  // если нашли путь, то найдем минимальное значение на нем
  if (visitVertex[goal]){
      current = goal;
      char parr;
      while(current != start){
          parr = currPath[current];
          if (minRes > residGraph[parr][current])
              minRes = residGraph[parr][current];
          current = currPath[current];
      }
  }
  
  std::pair<int, bool> res(minRes, visitVertex[goal]);
  
  return res;
    
}



//алгоритм Форда-Фалкерсона
int algoFF(Graph& graph, char start, char goal)
{                                    // "остаточный граф", в начале равен исходному
    Graph residGraph = graph;        // остаточные емкости = пропусным способностям 
    std::map<char, char> currPath;   // текущий путь сын-родитель
    std::pair<int, bool> findRes;    // результат поиска улучшающего пути 
    std::string strPath = "";        
    char current, parrent;
    long maxFlow = 0;                // максимальный поток
    bool cycle = true;
    findRes = findPath(residGraph, start, goal, currPath);
  
    //пока выходит найти улучшающий путь в "остаточном" графе
    
    while (findRes.second){
       strPath = "";
       strPath += goal;
       current = goal;
       
       // по найденному пути обновляем остаточные мощности в 
       // "остаточном" графе
       while (current != start){
           parrent = currPath[current];                    //вычитаем поток пути из ребер
           residGraph[current][parrent] += findRes.first;  //вдоль пути и + вдоль обратных 
           residGraph[parrent][current] -= findRes.first;  
           std::string add;
           add = parrent; add += " ---> ";
           strPath.insert(0, add);
           current = parrent;
       }
    
     // обновляем максимальный поток
      maxFlow += findRes.first;  
      
     
      if (true){
          std::cout << LINE;
          std::cout << "Найденный путь: " << strPath << "\nМинимальный вес: " << findRes.first << "\tтекущий поток: " << maxFlow << std::endl;
      }
      findRes = findPath(residGraph, start, goal, currPath);
    
    }
    
    int resFlow;
    
    std::cout << LINE << "Максимальный поток сети = " << maxFlow << std::endl;
    std::cout << "Фактические величины потоков через ребра: " << std::endl; 
     
     
    // проходим по всем ребрам исходного и остаточного графа и 
    // выводим фактические потоки через них равные = пропускной способности - остаточные емкости
    // т.к. в остаточном графе емкости ребер - на сколько не улучшили в итоге
    for (auto& vert: graph){                                                 
      for (auto adjEdge: graph[vert.first]) {                                
        if (adjEdge.second < residGraph[vert.first][adjEdge.first]){      
            resFlow = 0;
        }
        else{
            resFlow = adjEdge.second - residGraph[vert.first][adjEdge.first];
        }
                                                                                        
        std::cout << vert.first << " " << adjEdge.first << " " << resFlow << std::endl;
       
      }
    } 
    
    return maxFlow;
}




// считывание графа
void readGraph(Graph& graph, std::istream& stream = std::cin){
    int count, capacity;
    char start, goal;
    char parrent, current;
    stream >> count >> start >> goal;
    for (int i = 0; i < count; i++) {                                         //Считывание вершин графа
       stream >> parrent >> current >> capacity;
       graph[parrent][current] = capacity;
    }    
    
    algoFF(graph, start, goal);
}


int main()
{
    Graph graph;
    bool file = true;
    std::string fileName = "test1.txt";
    std::ifstream fin;
    fin.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    
    if (file) {
      try
      {
        fin.open(fileName);
        readGraph(graph, fin);
        fin.close();
      }
      catch(const std::ifstream::failure& exep)
      {
          std::cout << exep.what() << std::endl;
          //std::cout << exep.code()  << std::endl;
          readGraph(graph);
      }
    }
    else{
       readGraph(graph);
    }
    
    return 0;
}
