#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <climits>

std::ostream &operator<<(std::ostream& stream, std::vector<int> vect){
    stream << vect[0];
    for (int i = 1; i < vect.size(); i++){
        stream << "," << vect[i];
    }
}


void prefixFunction(std::string& sample, std::vector<int>& prefix){
    prefix[0] = 0;
    for (int i = 1; i < sample.length(); i++){
        int k = prefix[i-1];
        while (k > 0 && sample[i]!=sample[k])   // не равны
            k = prefix[k-1];                    // берем ранее рассчитанное значение
        
        if (sample[k] == sample[i])
            k = k + 1;
            
        prefix[i] = k;    
        
    }
    
}



bool findEntry(std::string& text, std::string& sample, std::vector<int>& prefix){
    int k = 0;   //индекс сравниваемого символа в sample
    if (text.length() != sample.length()){
        std::cout << "-1";
        return false;
    }

    text = text + text;
    
    for (int i = 0; i < text.length(); i++){
        while (k > 0 && sample[k] != text[i])
            k = prefix[k-1];
            
        if (sample[k] == text[i])
            k = k + 1;
            
        if (k == sample.length()){
            std::cout << i - sample.length() + 1;
            return true;
        }    
        
    }
    
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