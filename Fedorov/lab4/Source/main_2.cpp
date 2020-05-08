#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <climits>

bool INTER = true;

//перегруженны оператор вывода вектора в поток
std::ostream &operator<<(std::ostream& stream, std::vector<int> vect){
    stream << vect[0];
    for (int i = 1; i < vect.size(); i++){
        stream << "," << vect[i];
    }
}


void printInterInfo(std::string& splice, std::vector<int> vect, int n){
    std::cout << "\nСостояние префикс-функции для строки: " << std::endl;
    for (int i = 0; i <= n; i++)
        std::cout << splice[i] << " ";
    std::cout << std::endl;
    for (int i = 0; i <= n; i++)
        std::cout << vect[i] << " "; 
    std::cout << std::endl;    
}


void prefixFunction(std::string& sample, std::vector<int>& prefix){
    prefix[0] = 0;
    std::cout << "Начало рассчета префикс-функции" << std::endl;
    int n = 1;
    for (int i = 1; i < sample.length(); i++)
    {   // поиск какой префикс-суффикс можно расширить
        int k = prefix[i-1];                      // длина предыдущего префикса
        while (k > 0 && sample[i]!=sample[k])   // этот нельзя расширить,
            k = prefix[k-1];                    // берем ранее рассчитанное значение
        
        if (sample[k] == sample[i])
            k = k + 1;                          // расширяем найденный префикс-суффикс
            
        prefix[i] = k;    
       
        if (INTER)
            printInterInfo(sample, prefix, n++);
    }
    
}



//функция определения сдвига
bool findEntry(std::string& text, std::string& sample, std::vector<int>& prefix){
    int k = 0;   //индекс сравниваемого символа в sample
    if (text.length() != sample.length()){
        std::cout << "\nНе является циклическим сдвигом: ";
        std::cout << "-1";
        return false;
    }

    text = text + text;              //удваиваем строку
    
    for (int i = 0; i < text.length(); i++){
        while (k > 0 && sample[k] != text[i])
            k = prefix[k-1];
            
        if (sample[k] == text[i])
            k = k + 1;
            
        if (k == sample.length()){
            std::cout << "Является сдвигом, позиция: ";
            std::cout << i - sample.length() + 1;
            return true;
        }    
        
    }
    
    std::cout << "\nНе является циклическим сдвигом: ";
    std::cout << "-1";
    return false;
    
    
}


int main()
{
    std::string text;
    std::string sample;
    std::cin >> text;
    std::cin >> sample;
    std::vector<int> prefix(sample.length());
    prefixFunction(sample, prefix);
    findEntry(text, sample, prefix);
    return 0;
}