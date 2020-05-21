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
int Word_Len = 0;

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
  int neighbours[Alphabet_length]; // соседние вершины
  int movePath[Alphabet_length];   // массив переходов
  int parrent;                     // вершина-предок
  std::vector<int> patternNumber;  // номер строки-образца
  int suffLink;                    // суффиксная ссылка  
  int upSuffLink;                  // "сжатая" суффиксная ссылка
  char charToParrent;              // символ ведущий к предку
  bool terminal;                   // является ли терминальной
};                                 // (совпадает со строкой)    



Node makeNode(int parrent, char transfer){                  // создание новой вершины
    Node newNode;
    memset(newNode.neighbours, 255, sizeof(newNode.neighbours));
    memset(newNode.movePath, 255, sizeof(newNode.neighbours));
    newNode.suffLink = newNode.upSuffLink = -1;             // изначально нет ссылки
    newNode.parrent = parrent;                                  
    newNode.charToParrent = transfer; 
    newNode.terminal = false;
    //newNode.patternNumber = -1;
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
    trie[index].patternNumber.push_back(count_patterns);
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
void checkUpLink(int C[], int vert, int index, Trie& trie, std::string& text, Patterns& patterns, std::vector<int>& positions){
    int n = 0; int prev;
    for (int i = vert; i != 0; i = getUpSuffLink(i, trie)){
        if (trie[i].terminal){
            if (n) std::cout << "\t" << prev << "--UpLink-->" << i << std::endl;
            n++; prev = i;
            for (auto& in: trie[i].patternNumber){
                int tmp = index + 1 - patterns[in].size() - positions[in];
                if (tmp >= 0 && tmp <= text.length() - Word_Len){
                    C[tmp]++;
                    std::cout << "\tNode " << i << " is terminal, at pos " << index-patterns[in].length()+1 << " find: " << patterns[in] << std::endl;                                
                }
            }
        }
    }
}

//Функция для процессинга текста
//в качестве набора образцов - множество подстрок образца без джокера
void processText(int C[], std::string& text, Trie& trie, Patterns& patterns, Output& output, std::vector<int>& positions){
    int current = 0;
    for (int i = 0; i < text.length(); i++){
        current = getLink(current, getIndex(text[i]), trie);
        checkUpLink(C, current, i, trie, text, patterns, positions);
    }
    std::cout << "\nResult: " << std::endl;
    int num = 0;
    for (int i = 0; i < text.size(); i++){             
        if (C[i] == patterns.size()){
            std::cout << i+1 << std::endl;
            num++;
        }
    }
    if (num == 0) std::cout << "\nNothing" << std::endl;	
}

// Функция разбиение образца на максимальные подстроки без джокера
void splitString(std::string& str, Patterns& patterns, std::vector<int>& positions, char joker){
    int index = 0;
    int position = 0;
    int count = 0;
    for (int i = 0; i < str.length(); i = index){
        std::string buff = "";
        while (index < str.length() && str[index] == joker)   // пропускаем джокера
            index++;
        if (index == str.length()) return;    
        position = index;
        while(index < str.length() && str[index] != joker)    // новоя подстрока
            buff += str[index++]; 
         if(!buff.empty()){
            count++;
            positions.push_back(position);                    //запоминаем позиции подстрок
            patterns.push_back(buff);
        }
    }    
        
}


int main()
{
    int count_patterns;
    Trie trie;
    std::string text;
    std::string word;
    std::vector<int> positions;
    char joker;
    Output output;
    
    std::cin >> text;
    std::cin >> word;
    std::cin >> joker;
    
    auto it = trie_alphabet.find(joker);
    while (it != trie_alphabet.end()){
        std::cout << "\nError, joker can`t be ALphabet symbol: ";
        std::cin >> joker;
        it = trie_alphabet.find(joker);
    }
    
    Word_Len = word.length();
    Patterns patterns;
    initTrie(trie);
    splitString(word, patterns, positions, joker);
    int i = 0;
    std::cout << "\n------------Fill trie start--------------" << std::endl;
    for (auto& buff: patterns){
        addString(buff, trie, i++);
    }
    std::cout << "--------------Fill trie end----------------" << std::endl;    

    int C[text.size()] = {0};
    
    std::cout << "Count of nodes: " << trie.size() << std::endl;
    std::cout << "Parts of pattern without joker " << joker << std::endl;
    for (auto& buff: patterns){
        std::cout << "\t" << buff << std::endl;
    }
    std::cout << "Array of starts positions for parts" << std::endl;
    for (auto& buff: positions){
        std::cout << " " << buff;
    }    
    
    std::cout << "\n------------Proccesing Text star-----------" << std::endl;
    processText(C,text, trie, patterns, output, positions);
    std::cout << "------------Proccesing Text end------------" << std::endl;    
    return 0;
}
