#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cmath>

std::ostream &operator<<(std::ostream& stream, std::vector<int> vect){
    stream << vect[0];
    for (int i = 1; i < vect.size(); i++){
        stream << "," << vect[i];
    }
}

struct Splice{
   std::string text;
   std::string sample;
   char separator = '#';
   int length()
   {
       return text.length()+sample.length()+1;
   }
   
   char operator[](const int index)
   {
       if (index < sample.length())
            return sample[index];
       else if (index == sample.length())
            return separator;
       else if (index > sample.length())
            return text[index-sample.length()-1];
   }
};


void prefixFunction(Splice& splice, std::vector<int>& prefix){
  prefix[0] = 0; //всегда 0
  
  for (int i = 1; i < splice.length(); i++) 
    {
       // ищем, какой префикс-суффикс можно расширить
        int k = prefix[i-1]; // длина предыдущего префикса-суффикса, возможно нулевая
        char a = splice[i];
        char b = splice[k];
        while (k > 0 && splice[i] != splice[k]) // этот нельзя расширить,
            k = prefix[k-1];   // берем длину меньшего префикса-суффикса

        if (splice[i] == splice[k]) 
            k++;  // расширяем найденный (возможно пустой) префикс-суффикс
        prefix[i] = k;
     }
    
}


bool findEntry(std::string& text, std::string& sample, std::vector<int>& prefix, int flow_count = 1){
    int begin, end;
    bool find = false;
    //vector<vector<int>> for flow
    std::vector<int> result;
    
    for (int j = 0; j < flow_count; j++){
        
        begin = std::ceil(text.length()/flow_count)*j + sample.length() + 1;
        end = std::ceil(text.length()/flow_count)*(j+1) + sample.length() + 1;
        if (end > sample.length() + text.length() + 1)
            end = sample.length() + text.length() + 1;
            
        for (int i = begin; i < end; i++){
            //print now text part po &texta i
            if (prefix[i] == sample.length()){
                //cout << new entry
                find = true;
                result.push_back(i - 2*sample.length());
            }
        }    
    }
    
    if (find){
        std::cout << result;
    }
    else{
        std::cout << "-1";
    }
    
   return find;
    
}


int main()
{
    Splice splice;
    std::cin >> splice.sample;
    std::cin >> splice.text;
    std::vector<int> prefix(splice.length());
    prefixFunction(splice, prefix);
    findEntry(splice.text, splice.sample, prefix);

    return 0;
}
