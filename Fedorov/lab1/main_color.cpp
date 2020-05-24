#include <cmath>
#include <ctime>
#include <cstring>
#include <stdlib.h>
#include <iostream>     // std::cout
#include <algorithm>    // std::find
#include <vector>       // std::vector

using namespace std;

// Default, Red, Creen, Yellow, Blue, Pyrp, Bluel
const std::vector<std::string> Colors { "\x1b[0m", "\x1b[1;31m", "\x1b[1;32m", "\x1b[1;33m",
                                       "\x1b[1;34m", "\x1b[1;35m", "\x1b[1;36m"};

class Map;

class Piece{             //класс обрезка
friend Map;      
private:
	int cellX;           //координаты вверхнего                      
    int cellY;           //левого угла
    int width;	
    int index;
    int colorIndex;
    int vectorIndex;
    static int usedColor;
    static std::vector<int> colors;
    void setIndex();
public:
	int getX();
	int getY();
	int getWidth();
    Piece(int x, int y,  int width);
    virtual ~Piece();
    Piece* copy();
};
std::vector<int> Piece::colors;
int Piece::usedColor = 1;

void Piece::setIndex()
{
    bool find_ = false;
	if (width == 1) 
	    index = 1;
	else if (width > 9) 
	    index = 9;
	else 
	    index = width; 
	
    auto it = find(Piece::colors.begin(), Piece::colors.end(), index);
    if (it != Piece::colors.end())
        find_ = true;
	Piece::colors.push_back(index);
	if (find_){
	    colorIndex = Piece::usedColor;
	    if (Piece::usedColor >= 6) 
	        Piece::usedColor = 1;
	    else
	        Piece::usedColor++;
	}
	else
	    colorIndex = 0;
	vectorIndex = Piece::colors.size()-1;
	
}

Piece::~Piece(){
    if (Piece::colors.size() > 0 && vectorIndex < Piece::colors.size())
        Piece::colors.erase(Piece::colors.begin()+vectorIndex);
}


Piece::Piece(int x, int y, int width)
: cellX(x), cellY(y), width(width), colorIndex(0), vectorIndex(0)
{
	this->setIndex();
}

Piece* Piece::copy()
{
	return new Piece(cellX, cellY, width);
}

int Piece::getX()
{
	return cellX;
}

int Piece::getY()
{
	return cellY;
}

int Piece::getWidth()
{
	return width;
}

bool PRINT = true;



class Map{
private:
    int** matrix;                //матрица заполненности столешницы          
    int** matrix_color;
    int nullCell;                //счетчик "разряженности" матрицы          
    int countPiece;          
    //Piece** buffer;              // буфер обрезков  
    std::vector<Piece*> buffer;
    void fillNull();             //заполнение матрицы нулями.
    int width;                   //размер столешницы
    int resultOut;       		 // результат: список обрезков            
public:
    std::vector<Piece*> resultArray;
    Map(int width);
    virtual ~Map();
    bool addPiece(Piece*);    	 	 //добавление в буфер
    Piece* pullPiece();       		 //удаление из буфера
    bool findEmptyCell(int&, int&);  //поиск свободной клетки
    Piece* fillMax(int, int);        //"жадное" размещение обрезка
    int findMinPartition();          //поиск минимального разбиения
    void optimiseEven();             //оптимизация 
    void optimiseTrip();
    void optimisePrime();
    void optimiseMultipFive();
    friend std::ostream& operator<<(std::ostream &stream, const Map &obj);
    void show();
};



void Map::show()                       //промежуточный вывод
{
	for (int i = 0; i < width; i++)
	{
		cout << "\n";
		for (int j = 0; j < width; j++)
		{
			cout << Colors[matrix_color[i][j]] << matrix[i][j] << " "; 
		}
    }	
    cout << Colors[0] << "\n\n";
}


void Map::fillNull()                  //заполнение нулями матрицы
{
    for (int x = 0; x < this->width; x++)
    {
        for (int y = 0; y < this->width; y++)
        {
            matrix[x][y] = 0;
            matrix_color[x][y] = 0;
        }
    }
    countPiece = 0;
    nullCell = width*width;
}


Map::Map (int width)                //конструктор
{
    this->width = width;
    matrix = new int* [width];
    matrix_color = new int*[width];
    for (int i = 0; i < width; i++)
    {
        matrix[i] = new int[width];
        matrix_color[i] = new int[width];
    }
    fillNull();                     //обнуление
    nullCell = width*width;
   // buffer = new Piece*[nullCell];   //буферный список решения
    buffer.resize(nullCell, nullptr);
    resultArray.resize(nullCell, nullptr);
    countPiece = 0;
}


Map::~Map() 
{
    for (int i = 0; i < width; i++){
        delete(matrix[i]);
        delete(matrix_color[i]);
    }
    delete(matrix);
    delete(matrix_color);
    for (int i = 0; i < width*width; i++){
        if (buffer[i]) delete(buffer[i]);
        if (resultArray[i]) delete(resultArray[i]);
    }
}



bool Map::addPiece(Piece* piece) 
{
    if (nullCell == 0) {
    	return false;                        //матрицa полностью заполнена
    }

    for (int x = piece->cellX; x < piece->cellX + piece->width; x++) 
    {
        for (int y = piece->cellY; y < piece->cellY + piece->width; y++)
        {
            matrix[x][y] = piece->index;      //заполняем клетки
            matrix_color[x][y] = piece->colorIndex;
        }
    }
    int num = countPiece;
    if (buffer[countPiece] != nullptr)
    {
        delete(buffer[countPiece]);    
    }
    buffer[countPiece++] = piece;             //добавляем в список
    nullCell -= piece->width * piece->width;  //уменьшение счетчика "разряженности"
    return true;
}




Piece* Map::pullPiece() 
{
    if (countPiece == 0) {                  //буфер пустой
    	return NULL;
    }
    Piece* tmp = buffer[countPiece-1];
    for (int i = tmp->cellX; i < tmp->cellX + tmp->width; i++)
    {
        for (int j = tmp->cellY; j < tmp->cellY + tmp->width; j++)
        {
            matrix[i][j] = 0;               //обнуление клеток
            matrix_color[i][j] = 0;
        }
    }
    countPiece--;
    nullCell += tmp->width * tmp->width;   //увеличение счетчика "разряженности"
    return tmp;
}




bool Map::findEmptyCell(int& cellX, int& cellY) //двойной цикл поиска по матрицы 
{  
    for (int i = 0; i < this->width; i++)
    {
        for (int j = 0; j < this->width; j++)
        {
            if (matrix[i][j] == 0){
            	cellX = i;                    //присвоение значений найденных координат
            	cellY = j;
            	return true;
            }
        }
    }
  return false;                        //свободная клетка не найдена
}


Piece* Map::fillMax(int coordX, int coordY) 
{   
    int size = 1;
    bool check = true;
    while(check && (size < width)){
        size++;
	    check = (coordX + size - 1 < width) &&  (coordY + size - 1 < width);  //контроль выхода за границы
        if (check){            
            for (int i = 0; i < size; i++) 
            {  
                if ((matrix[coordX+size-1][coordY+i] != 0) || (matrix[coordX+i][coordY+size-1] != 0)) 
                {
                    check = false;                     //контроль что не накладывается на
                    break;                             //соседние обрезки (по периметру)
                }
            }            
       }    
    }
    size--;
    return new Piece(coordX, coordY, size);
}



int Map::findMinPartition(){
    int resultCount = nullCell;          
    if ((width % 2) == 0){
    	optimiseEven();
    }
    else if ((width % 3) == 0){
        optimiseTrip();
    }
    else if ((width % 5) == 0){
        optimiseMultipFive();
    }
    else{
        optimisePrime();
    }
    int tmp_x, tmp_y;
    while (nullCell) {
    	findEmptyCell(tmp_x, tmp_y);
        addPiece(fillMax(tmp_x, tmp_y));
    }
    resultCount = countPiece;

    for (int i = 0; i < countPiece; i++) {
        resultArray[i] = buffer[i]->copy();
    }

    if (width==2) {
        resultOut = resultCount;
        return resultCount;
    }
    int k_While;// = countPiece - 1;
    while (true) {
    	if(PRINT){
    	    cout << "\n\x1b[36mТекущая заполненность: \x1b[0m" << endl;
    	    show();
    	}
        k_While = countPiece - 1;
        while (buffer[k_While--]->width == 1)           //убираем единичные 
            pullPiece();
        k_While++;
        if (k_While < 3) break;
            /* Откат назад на размер меньше */
        Piece* poppedSquare = pullPiece();                 //ставит на единицу меньше предыдущего
        addPiece(new Piece(poppedSquare->cellX, poppedSquare->cellY, poppedSquare->width - 1));
        while (nullCell && (countPiece < resultCount)){
        	findEmptyCell(tmp_x, tmp_y);                     //заполняем снова, если текущее число
            addPiece(fillMax(tmp_x, tmp_y));                 //обрезков превысило предыдущее, нет смысла
        }                                                    //продолжать, прошлое лучше -> выходим  

        if (countPiece < resultCount) {
            resultCount = countPiece;
            for (int j = 0; j < countPiece; j++) {
                if (resultArray[j]) delete(resultArray[j]);
                resultArray[j] = buffer[j]->copy();           //сохраняем лучший на данный
            }                                                 //момент рзультат 
        }
    }
    resultOut = resultCount;
    return resultCount;
}


void Map::optimiseEven()
{
    addPiece(new Piece(width/2, width/2, width/2));
    addPiece(new Piece(width/2, 0, width / 2));
    addPiece(new Piece(0, width/2, width/2));
}


void Map::optimiseTrip()
{
    addPiece(new Piece(0, 0,width*2/3));
    addPiece(new Piece(width*2/3, 0, width/3));
    addPiece(new Piece(0, width*2/3, width/3));
}


void Map::optimiseMultipFive()
{
    addPiece(new Piece(0, 0, width*3/5));
    addPiece(new Piece(width*3/5, 0, width/5));
    addPiece(new Piece(0, width*3/5, width/5));	
}



void Map::optimisePrime()
{
    addPiece(new Piece(width/2, width/2, ceil(width/2.0)));
    addPiece(new Piece(ceil(width/2.0), 0, width/2));
    addPiece(new Piece(0, ceil(width / 2.0), width/2));
}


ostream& operator<<(ostream &stream, const Map &obj)
{ 
	for (int i = 0; i < obj.resultOut; i++)
	{
		stream << "\033[1;33m";
		stream << obj.resultArray[i]->getX()+1 << " ";
		stream << obj.resultArray[i]->getY()+1 << " ";
		stream << obj.resultArray[i]->getWidth();
		stream << endl;
    }
	stream << "\033[0m";
	return stream;
}


class Dialog{
  private:
  	int fieldSize;
  	Map* field;
  public:
  	Dialog();
  	virtual ~Dialog();
  	void dialog();
  	bool isDigit(const char* str);
  	void update();
  	void toLower(string &str);

};


Dialog::Dialog() : fieldSize(0), field(nullptr)
{

}

Dialog::~Dialog()
{
	if (field != nullptr){
		delete field;
	}
}

void Dialog::update()
{
	if (field != nullptr){
		delete field;
		field = nullptr;
	}
}


void Dialog::toLower(string &str){
    int i = 0;
    while (str[i]){
        str[i] = tolower(str[i]);
        i++;
    }
}


bool Dialog::isDigit(const char* str){
    if (((str[0] == '+') && strlen(str)>1) || isdigit(str[0])){
       for (int i = 1; i < strlen(str) ; i++){
            if (isdigit(str[i]) == 0) return false;
    }
    return true;
    }
    else 
        return false;
} 



void Dialog::dialog()
{
   string str;
   do{
      Map* field_;
   	  cout  << "Выводить промежуточные результаты? \033[1;34m[y/n]\033[0m ?: ";
      cin >> str;
      toLower(str);
      while (str!="y" && str!="n"){
          cout << "\nОшибка, введите [y/n] :";
          cin >> str; toLower(str);
      }
      if (str == "n")  PRINT = false;
      cout << "\033[1;32mВведите размер квадрата:\033[0m  ";
      cin >> str; 
      while (!isDigit(str.c_str()))
        {
          cout << "\033[1;31mОшибка ввода, попробуйте снова: \033[0m" ;
          cin >> str;
        }

      fieldSize = atoi(str.c_str());
      if (fieldSize == 1){
      	cout << "\n Размер 1 не предусмотрен.";
      	return;
      }
      if (fieldSize > 28) PRINT = false;
      field_ = new Map(fieldSize);
      size_t start_time = clock();
      int result = field_->findMinPartition();
      size_t end_time = clock();
      size_t time = end_time-start_time;
      cout << "\n\033[1;32m" << "Количество квадратов: \033[1;34m" << result << "\033[0m" << endl;
      cout << (*field_);
      cout << "\033[1;32mВремя работы: \033[1;34m" << ((float)time)/ CLOCKS_PER_SEC << "\033[1;32m sec\033[0m";
      cout << "\n\n" << "Вы хотите продолжить \033[1;34m[y/n]\033[0m ?: ";
      cin >> str;
      toLower(str);
      while (str!="y" && str!="n"){
          cout << "\nОшибка, введите [y/n] :";
          cin >> str; toLower(str);
      }
      cout << "\n";
      delete(field_);
      field_ = nullptr;
      PRINT = true;
    }while(str != "n");
}





int main(){
    Dialog work;
    work.dialog();
    return 0;
}
