#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

struct Node;
using Trie = std::vector<Node>;                        //хранение бора
using ALphabet = std::map<char, int>;                  //алфавит, по условию состоит из {A,C,G,T,N}
using Patterns = std::vector<std::string>;             //вектор образцов
using Output = std::vector<std::pair<int,int>>;        //вывод позиция - образец

const int Alphabet_length = 5;
ALphabet trie_alphabet = { {'A', 0}, {'C', 1}, {'G', 2}, {'T', 3}, {'N', 4} };

int getIndex(char symb){
    return trie_alphabet[symb];
}

bool Compare(std::pair<int, int> a, std::pair<int, int> b) 
{                                                           //компаратор для вывода результата
    if (a.first == b.first)
        return a.second < b.second;
    else
        return a.first < b.first;
}


// структура вершины
struct Node{
  int neighbours[Alphabet_length];      // соседние вершины
  int movePath[Alphabet_length];        // массив переходов
  int parrent;                          // вершина-предок
  int patternNumber;                    // номер строки-образца
  int suffLink;                         // суффиксная ссылка  
  int upSuffLink;                       // "сжатая" суффиксная ссылка
  char charToParrent;                   // символ ведущий к предку
  bool terminal;                        // является ли терминальной
};                                      // (совпадает со строкой)    



Node makeNode(int parrent, char transfer){                    // создание новой вершины
    Node newNode;
    memset(newNode.neighbours, 255, sizeof(newNode.neighbours));
    memset(newNode.movePath, 255, sizeof(newNode.neighbours));
    newNode.suffLink = newNode.upSuffLink = -1;               // изначально нет ссылки
    newNode.parrent = parrent;                                  
    newNode.charToParrent = transfer; 
    newNode.terminal = false;
    newNode.patternNumber = -1;
    return newNode;
}



void initTrie(Trie& trie){
    // создаем бор, изначально только корень
    trie.push_back(makeNode(0, '#'));     
}


void addString(std::string& str, Trie& trie, int count_patterns){
    int index = 0;
    std::cout << "Add pattern: " << str << std::endl;
    for (int i = 0; i < str.length(); i++){
        int curr = getIndex(str[i]);
        if (trie[index].neighbours[curr] == -1){        // если нет ребра по символу
            trie.push_back(makeNode(index, str[i]));
            trie[index].neighbours[curr] = trie.size() - 1;
            std::cout << "\tcreate node for transition: " << index << "--" << str[i] << "->" << trie.size()-1 << std::endl;
        }   
         index = trie[index].neighbours[curr];
    }
    trie[index].terminal = true;
    trie[index].patternNumber = count_patterns;
}


bool findString(std::string& str, Trie& trie){       //функция поиска строки в боре
    int index = 0;
    for (int i = 0; i < str.length(); i++){
        int curr = getIndex(str[i]);
        if (trie[index].neighbours[curr] == -1)
            return false;
        index = trie[index].neighbours[curr];    
    }
    return true;
}


int getLink(int vert, int index, Trie& trie);

// Функция для вычисления суффиксной ссылки
int getSuffLink(int vert, Trie& trie){
    if (trie[vert].suffLink == -1)                     // еще не искали
       if (vert == 0 || trie[vert].parrent == 0)       // корень или родитель корень
           trie[vert].suffLink = 0;                    // для корня ссылка в корень
       else
           trie[vert].suffLink = getLink(getSuffLink(trie[vert].parrent, trie), getIndex(trie[vert].charToParrent), trie);
    return trie[vert].suffLink;       
}

// Функция для вычисления перехода
int getLink(int vert, int index, Trie& trie){
   if (trie[vert].movePath[index] == -1)                                // если переход по данному 
      if (trie[vert].neighbours[index] != -1)                           // символу ещё не вычислен
         trie[vert].movePath[index] = trie[vert].neighbours[index];
      else if (vert == 0)                  
          trie[vert].movePath[index] = 0;
      else
          trie[vert].movePath[index] = getLink(getSuffLink(vert, trie), index, trie);
   return trie[vert].movePath[index];   
}


// Функция для вычисления сжатой суффиксной ссылки
int getUpSuffLink(int vert, Trie& trie){
   if (trie[vert].suffLink == -1){             // если сжатая суффиксная ссылка ещё не вычислена
      int tmp = getSuffLink(vert, trie);
      if (trie[tmp].terminal)                  // если ссылка на терминальную, то ок
          trie[vert].upSuffLink = tmp;
      else if (tmp == 0)                       // на корень = 0
          trie[vert].upSuffLink = 0;
      else
          trie[vert].upSuffLink = getUpSuffLink(tmp, trie);   //поиск ближайшей
   }
   return trie[vert].upSuffLink;    
}


//проверка сжатых суффиксных ссылок
void checkUpLink(int vert, int index, Trie& trie, Output& output, Patterns& pattern){
    int n = 0; int prev;
    for (int i = vert; i != 0; i = getUpSuffLink(i, trie)){
        if (trie[i].terminal){ // если является терминальной, то нашли соответствующий образец
            if (n) std::cout << "\t" << prev << "--UpLink-->" << i << std::endl;
            std::cout << "\tNode " << i << " is terminal, at pos " << index-pattern[trie[i].patternNumber].length()+1 << " find: " << pattern[trie[i].patternNumber] << std::endl;
            output.push_back(std::pair<int,int> ((index-pattern[trie[i].patternNumber].length()+1), trie[i].patternNumber+1));
            n++; prev = i;
        }
    }
}

//Функция для процессинга текста
void processText(std::string& text, Trie& trie, Patterns& pattern, Output& output){
    int current = 0;
    for (int i = 0; i < text.length(); i++){
        //std::cout << "Current: ";
        current = getLink(current, getIndex(text[i]), trie);
        checkUpLink(current, i+1, trie, output, pattern);
    }
}


int main()
{
    int count_patterns;
    Trie trie;
    std::string text;
    Output output;
    std::cin >> text;
    std::cin >> count_patterns;
    while (count_patterns <= 0 ){
        std::cout << "\nError, try again: ";
        std::cin >> count_patterns;
    }
    Patterns pattern(count_patterns);
    initTrie(trie);
    for (int i = 0; i < count_patterns; i++){
        std::cin >> pattern[i];
    }
    
    std::cout << "\n------------Fill trie start--------------" << std::endl;
    for (int i = 0; i < count_patterns; i++){
        addString(pattern[i], trie, i);
    }    
    std::cout << "--------------Fill trie end----------------" << std::endl;
    
    std::cout << "------------Proccesing Text star-----------" << std::endl;
    processText(text, trie, pattern, output);
    std::cout << "------------Proccesing Text end------------" << std::endl;
    std::sort(output.begin(), output.end(), Compare);
    for (auto &curr: output){
        std::cout << curr.first << " " << curr.second << std::endl;
    }
    
    std::cout << "---------------------------------------\n" << std::endl;
    std::cout << "Count of nodes (states): " << trie.size() << std::endl;
    int pos1, pos2;
    int count_inter = 1;
	if (pattern.size() == 1 || text.size() < 2) return 0;
    for (int i = 0; i <= output.size()-2; i++){
        for (int j = i+1; j <= output.size()-1; j++){
            pos1 = pattern[output[i].second-1].size() + output[i].first - 1;
            pos2 = output[j].first;
            if (pos1 >= pos2 && output[i].second!=output[j].second){
                std::cout << "---------------------------------------" << std::endl;
                std::cout << "Intersection " << count_inter++ << "(" << output[i].first << ", " << output[j].first << ")" << std::endl;
                std::cout << "\tpattern 1: " << pattern[output[i].second-1] << std::endl;
                std::cout << "\tpattern 2: " << pattern[output[j].second-1] << std::endl;
            }
        }
    }
    
    
    return 0;
}
