#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <ostream>



bool PRINT_INTEMEDIA = false;

const std::string UNDERLINE = "\x1b[1;4;36m";
const std::string CYAN = "\x1b[1;36m";
const std::string RED = "\x1b[1;31m";
const std::string GREEN = "\x1b[1;32m";
const std::string YELLOW = "\x1b[1;33m";
const std::string NORMAL = "\x1b[0m";
const std::string LINE = "--------------------------------------------";


//перегруженный оператор вывода в поток
//для полученного пути
std::ostream &operator<<(std::ostream& stream, std::vector<char>& vect)
{
    for (const auto &elem: vect){
        stream << elem;
        if (elem != vect.back())
            stream << "->";
    }
    return stream;
}

void fixed(){
    std::cin.clear();
    while (std::cin.get() != '\n');
}


struct Edge{                      //структура "ребра"
  char startVertex;               //начало и конец ребра
  char endVertex;
  double distant;                 //вес ребра
  Edge(char,char, double);
};


Edge::Edge(char start, char end, double distant_)
: startVertex(start), endVertex(end), distant(distant_) {}


using EdgeList = std::vector <Edge>;      //вектор смежных ребер


struct Adjacencylist{                     //структура "Список инциндентности"
    std::map<char, EdgeList> adjList;
    int countEdge;                        //число ребер в списке
    Adjacencylist();
    int size();
    void addEdge(char, char, double); 	  //метод добавления ребра в список
};



Adjacencylist::Adjacencylist() : countEdge(0) {}


int Adjacencylist::size(){
    return adjList.size();
}


void Adjacencylist::addEdge(char start, char end, double weight_)
{
    Edge newEdge(start, end, weight_);
    EdgeList newList;
    if (adjList.find(start) == adjList.end())
        adjList[start] = newList;
    adjList[start].push_back(newEdge);
    countEdge++;
}



class Dialog{
 private:
    Adjacencylist graph;
    std::map<char,char> resultPath;
    void reconstructPath(char,char);
    void toLower(std::string &str);
    void clear();
 public:
    void runDialog();
    void printInter(std::vector<char>& openSet, std::vector<char>& finishedSet);
    int heuristic(char, char);
    char min_F(std::vector<char>& openSet, std::map<char, double>& function);
    char min_FD(std::vector<char>& openSet, std::map<char, double>& function);
    bool Dijkstra(char start, char goal);
};



void Dialog::printInter(std::vector<char>& openSet, std::vector<char>& finishedSet)
{
    std::cout << "\n" << YELLOW << "Открытый список:" << CYAN;
    for (int i = 0; i < openSet.size(); i++)
        std::cout << " " << openSet[i]; 
    std::cout << "\n" << YELLOW << "Обработанные вершины:" << CYAN;
    for (int i = 0; i <  finishedSet.size(); i++)
        std::cout << " " << finishedSet[i];
    std::cout << NORMAL << "\n";    
}



void Dialog::toLower(std::string &str){
    int i = 0;
    while (str[i]){
      str[i] = tolower(str[i++]);
    }
}

void Dialog::clear(){
  for(auto& item : graph.adjList)
   {
     item.second.clear();
     item.second.shrink_to_fit();
   }
   graph.adjList.clear();
}



void Dialog::runDialog(){                                      //общая функция, поддерживающая диалог
    std::string str;												   // с пользователем
    char start, goal;
    char startVertex, endVertex;
    double distant;
 do{
    std::cout << "Выводить подробно? [y/n]: ";
    std::cin >> str;
    while (str!="y" && str!="n"){
          std::cout << "\nОшибка, введите [y/n] :";
          std::cin >> str; toLower(str);
      } 
    if (str == "y") PRINT_INTEMEDIA = true;
    std::cout << GREEN << "Введите начальную и конечную вершины: " << NORMAL;
    while (!(std::cin >> start >> goal)){
        std::cout << RED << "Ошибка ввода. Попробуйте снова: " <<NORMAL;
    }

    std::cout << GREEN << "Введите ребра (конец ввода-"<< UNDERLINE << "qqq "<< NORMAL << GREEN <<"):" << NORMAL<<std::endl;
    while(std::cin >> startVertex >> endVertex >> distant){
        graph.addEdge(startVertex,endVertex, distant);
    }

    bool check = Dijkstra(start, goal);
    if (!check){
        std::cout << RED << "\nОшибка! Путь не найден!" << NORMAL << std::endl;
        return;
    }

    reconstructPath(start, goal);

    fixed();
    std::cout << "\n\n" << "Вы хотите продолжить \033[1;34m[y/n]\033[0m ?: ";
    std::cin >> str;
    toLower(str);

    while (str!="y" && str!="n"){
          std::cout << "\nОшибка, введите [y/n] :";
          std::cin >> str; toLower(str);
      }
    PRINT_INTEMEDIA = false;
    clear();
 }while(str != "n");
}



int Dialog::heuristic(char current, char goal){              //эвристическая функция, по условию - близость
    return abs((int)current - (int)goal);                    //символов по таблице ASCII
}



char Dialog::min_F(std::vector<char>& openSet, std::map<char, double>& function){   //возвращает вершину с наименьшим
    int index = 0; double min = function[openSet[0]]; int i = 0;		  //значением F
    for (const auto &vertex: openSet){
        if (function[vertex] < min) index = i;
        i++;
    }
   return openSet[index];
}


char Dialog::min_FD(std::vector<char>& openSet, std::map<char, double>& function){   //возвращает вершину с наименьшим
    int index = 0; double min = function[openSet[0]]; int i = 0;		  //значением F
    char chmin = openSet[0];
    for (const auto &vertex: openSet){
        if (function[vertex] < min) index = i;
        else if (function[vertex] == min) {
            if (vertex > chmin) index = i;
        }
         i++;
    }
   return openSet[index];
}



bool Dialog::Dijkstra(char start, char goal){
    std::vector<char> openSet;                               // множество вершин, которые требуется рассмотреть
    std::vector<char> finishedSet;                           // множество рассмотренных вершин
    openSet.push_back(start);                                // изначально здесь присутствует только начальная вершина start
    std::map<char, double> fromStart;                        // стоимость пути от начальной вершины до текущей
    fromStart[start] = 0;
    char current; int tentative; char neighbour; int step = 0;
    int resDistant = 0;
    while(openSet.size() != 0){
        current = min_FD(openSet, fromStart);                              // вершина из openset имеющая самую низкую оценку f(x)
        if (current == goal){                                            // достигли целевой вершины
             std::cout << YELLOW << "Длина пути: " << UNDERLINE << resDistant << NORMAL << std::endl;
             return true;              //reconstructPath
        }
        
        step++;
        
        if (PRINT_INTEMEDIA){
            std::cout << LINE;
            std::cout << "\nШаг "<< step;
            printInter(openSet, finishedSet);
        }
        
        auto it = remove(openSet.begin(), openSet.end(), current);
        openSet.erase(it);
        std::cout << GREEN << "Вершина "<< CYAN << current <<  GREEN << " просмотрена."<<NORMAL<<std::endl;
        finishedSet.push_back(current);
        for (auto currEdge = graph.adjList[current].begin(); currEdge != graph.adjList[current].end(); currEdge++)
        {                                                                         // Проверяем каждого соседа текущей вершины
               neighbour = currEdge->endVertex;
               auto st = find(finishedSet.begin(), finishedSet.end(), neighbour);
               if (st != finishedSet.end())                                       // пропускаем соседей из "закрытого" списка
                     continue;
               tentative = fromStart[current] + currEdge->distant;                // вычисляем текущее расстояние от начальной
               bool better;
               auto bt = find(openSet.begin(), openSet.end(), neighbour);
               if( bt == openSet.end()){                                          // если сосед не в "открытом" списке - добавляем
                  openSet.push_back(neighbour);
                  better = true;                                                  // вводим признак того, что нужно обновить свойства для соседей
               }
               else{															  // сосед был в открытом списке (известна f() )
                   if (tentative < fromStart[neighbour])
                       better = true;
                   else
                       better = false;                                            // путь до этой вершины дороже (есть
               }																  // дешевле - игнорируем)


               if (better == true)
               {
                  resultPath[neighbour] = current;                                // обновляем значения
                  fromStart[neighbour] = tentative;
                  resDistant += currEdge->distant;
               }
        }//for

    }
                            //путь не был найден
    return false;
}











void Dialog::reconstructPath(char start, char goal){         //функция реконструкции пути
    std::vector<char> result;
    char current = goal;                                     // Добавляем goal в результирующий путь
    result.push_back(current);							     // (идем в обратном порядке)
    while (current != start){								 // получаем вершину из которой пришли в текущую
        current = resultPath[current];
        result.push_back(current);
    }

    std::reverse(result.begin(), result.end());              // т.к. конечная->..->начальная
    std::cout << YELLOW << "Полученный путь: " << NORMAL;
    std::cout << result << NORMAL << std::endl;
}





int main()
{
    Dialog go;
    go.runDialog();

    return 0;
}



