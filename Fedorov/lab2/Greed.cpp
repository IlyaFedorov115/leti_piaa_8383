#include <iostream>
#include <vector>

/*код жадного алгоритма */






using namespace std;

const std::string UNDERLINE = "\x1b[1;4;36m";
const std::string CYAN = "\x1b[1;36m";
const std::string RED = "\x1b[1;31m";
const std::string GREEN = "\x1b[1;32m";
const std::string YELLOW = "\x1b[1;33m";
const std::string NORMAL = "\x1b[0m";



struct Edge{                      // ребро
  char startVertex;               // начальная и 
  char endVertex;		      // конечная вершины
  double weight;                  // вес
  bool notTouch = true;           // проходили ли через    
  Edge(char,char, double);        // это ребро
};


Edge::Edge(char start, char end, double weight_)
: startVertex(start), endVertex(end), weight(weight_) {}



void dialog();

class EdgeList{                  //"список ребер"
  friend void dialog();
  private:
    vector<Edge*> list;        
    int countEdge;              // количество ребер графа
  public:
    EdgeList();
    double findDist(char start, char end);
    void addEdge(Edge*);              // методы добавления
    void addEdge(char, char, double); //ребра
};


EdgeList::EdgeList() : countEdge(0) {}


void EdgeList::addEdge(Edge* edge){
    list.push_back(edge);
}

void EdgeList::addEdge(char start, char end, double weight_)
{
    Edge* tmp =  new Edge(start, end, weight_);
    list.push_back(tmp);
}


double EdgeList::findDist(char start, char end){
    for (int i = 0; i < list.size(); i++){
        if (list[i]->startVertex == start && list[i]->endVertex == end)
            return list[i]->weight;
    }
    return -1.0;
}

void dialog(){
    EdgeList workList;
    char globalStart;
    char globalEnd;
    char start, goal;
    double weight;

    std::cout << GREEN << "Введите начальную и конечную вершины: " << NORMAL;
    while (!(std::cin >> globalStart >> globalEnd)){
        std::cout << RED << "Ошибка ввода. Попробуйте снова: " <<NORMAL;
    }

    std::cout << GREEN << "Введите ребра (конец ввода-"<< UNDERLINE << "qqq "<< NORMAL << GREEN <<"):" << NORMAL<<std::endl;

    while(cin >> start >> goal >> weight){
        workList.addEdge(start,goal, weight);
        workList.countEdge++;
    }



  char Current = globalStart;
  double currentMin; int index;
  vector<char> stack;                    //стек просмотренных вершин для возврата из тупика
                                         //он же результирующий список вершин
  while (Current != globalEnd){          // главный цикл, пока не достигнем конечной вершины
      index = 0;
                                         // игнорируем несмежные ребра и просмотренные
      while(workList.list[index]->startVertex != Current || workList.list[index]->notTouch==false){
          index++;
          if (index == workList.countEdge) break;                  //тупик
      }

      if (Current == globalEnd)  continue;

      if (index >= workList.countEdge){            //т.к. тупик, "откатываем" назад
         if (stack.empty()){                  // если откатывать не куда, то пути нет
              std::cout << RED << "\nОшибка! Путь не найден!" << NORMAL << std::endl;
              return;
          }



          Current = stack.back();
          stack.pop_back();
          continue;
      }


      currentMin = workList.list[index]->weight;

      for (int i = index; i < workList.list.size(); i++)       //ищем смежные ребра
      {
          if (workList.list[i]->startVertex != Current || workList.list[i]->notTouch==false)
                continue;
          else 
           {
               if (workList.list[i]->weight < currentMin)       //"жадный выбор"
                    {
                       index = i;
                       currentMin = workList.list[i]->weight;
                    }
           }
      }

      stack.push_back(Current);
      Current = workList.list[index]->endVertex;
      workList.list[index]->notTouch = false;             //пометели как "просмотренную"
  }


   std::cout << YELLOW << "Полученный путь: " << NORMAL;

   double resDist = 0.0;
   for (int i = 0; i < stack.size()-1; i++){
       double dist = workList.findDist(stack[i],stack[i+1]);
       resDist += dist;
       cout << stack[i] << " --" << dist << "--> ";
   }
   cout << globalEnd; 
   cout << YELLOW << "\nДлина пути: " << UNDERLINE << resDist << NORMAL << endl;
   
}




int main()
{
    dialog();

    return 0;
} 
