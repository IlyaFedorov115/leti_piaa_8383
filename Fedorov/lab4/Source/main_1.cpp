#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>


bool INTER = true;

//перегруженны оператор вывода вектора в поток
std::ostream &operator<<(std::ostream& stream, std::vector<int>& vect){
    stream << vect[0];
    for (int i = 1; i < vect.size(); i++){
        stream << "," << vect[i];
    }
}

//структура для храния строки вида: S = P # T
struct Splice{ 
   std::string text;                   //текст 
   std::string sample;                //шаблон
   char separator = '#';              //разделитель         
   int length()
   {
       return text.length()+sample.length()+1;
   }
   
   char operator[](const int index)                 //оператор индексации
   {
       if (index < sample.length())                 
            return sample[index];
       else if (index == sample.length())
            return separator;
       else if (index > sample.length())
            return text[index-sample.length()-1];
   }
};



void printInterInfo(Splice& splice, std::vector<int> vect, int n){
    std::cout << "\nСостояние префикс-функции для строки: " << std::endl;
    for (int i = 0; i <= n; i++)
        std::cout << splice[i] << " ";
    std::cout << std::endl;
    for (int i = 0; i <= n; i++)
        std::cout << vect[i] << " "; 
    std::cout << std::endl;    
}


// функция для построения вектора префикс функции для 
// "склеенной" строки S = P # T

void prefixFunction(Splice& splice, std::vector<int>& prefix){
  prefix[0] = 0; //всегда 0
  std::cout << "Начало рассчета префикс-функции" << std::endl;
  int n = 1;
  
  for (int i = 1; i < splice.length(); i++) 
    {
       // поиск какой префикс-суффикс можно расширить
        int k = prefix[i-1]; // длина предыдущего префикса
        while (k > 0 && splice[i] != splice[k])   // этот нельзя расширить,
            k = prefix[k-1];                     // берем длину меньшего префикс

        if (splice[i] == splice[k]) 
            k++;                        // расширяем найденный префикс-суффикс
        prefix[i] = k;
        
    
        if (INTER)
            printInterInfo(splice, prefix, n++);
        
     }
    
}


//функция поиска числа и индексов вхождений шаблона
// в заданный текст

bool findEntry(std::string& text, std::string& sample, std::vector<int>& prefix, int flow_count = 1){
    int begin, end;
    bool find = false;
    std::vector<int> result;
    
    //раздел поиска по "потокам"
    for (int j = 0; j < flow_count; j++){
        begin = std::ceil(text.length()/flow_count)*j + sample.length() + 1;   //начальная позиция для текущего потока
        end = std::ceil(text.length()/flow_count)*(j+1) + sample.length() + 1;  //конечная позиция для текущего потока
        if (end > sample.length() + text.length() + 1)
            end = sample.length() + text.length() + 1;
        
        std::cout << "Поток " << j+1 << " (begin = " << begin << ") (end = " << end << ")" << std::endl;   
        for (int i = begin; i < end; i++){
            if (prefix[i] == sample.length()){          //если префикс-функция == длине шаблона
                std::cout << "Найдено вхождение с индекса: " << i - 2*sample.length() << std::endl;//то нашли
                find = true;
                result.push_back(i - 2*sample.length());
            }
        }    
    }
    
    if (find){
        std::cout << "Число вхождений: " << result.size() << std::endl;
        std::cout << "Индексы: ";  std::cout << result;
        std::cout << std::endl;
    }
    else{
        std::cout << "Вхождений нет: ";
        std::cout << "-1" << std::endl;
    }
    
   return find;
    
}

// в начале вводится шаблон, затем
// текст и число "потоков"
int main()
{
    Splice splice;
    int flow_count;
    std::cin >> splice.sample;
    std::cin >> splice.text;
    std::cin >> flow_count;
    if (flow_count == 0)
        flow_count = 1;
    
    if (splice.sample.length() > splice.text.length()){
        std::cout << "\nLength error" << std::endl;
        return 0;
    }
    while(flow_count < 0 || flow_count > splice.text.length()){
        std::cout << "\nFlow count error, write new: ";
        std::cin >> flow_count;
    }
    
    if (splice.length() > 60)
        INTER = false; 
    
    std::vector<int> prefix(splice.length());
    prefixFunction(splice, prefix);
    findEntry(splice.text, splice.sample, prefix, flow_count);
    return 0;
}
