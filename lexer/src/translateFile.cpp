#include <iostream>
#include <cmath>

#include "translateFile2.cpp"


class Dog
{
private:
    int age, size;
public:
    Dog(int age, int size)
    {
        this->age = age;
        this->size = size;
    }
    void setAge(int x)
    {
        this->age = x;
    }

    int getAge()
    {
        return this->age;
    }
};

void doStuff()
{
    std::cout << "Buna!";
}

void retruning(std::string s, int y, float z)
{
    return;
}

int retrun3(int x, int y, float z)
{
    return x;
}



int main()
{


    int a = 4; 
    //Variable
    int se = 43;
    float b = 4.5;

    int var[] = {1, 2, 3};

    b = std::pow(a, b);

    std::string mama = "Ana Are Mere";

    char c = 'e';
    

    std::cin >> c;

    int l = retrun3(3, 4, 3.4);
    Dog objTest(1, 2);

    makeSound();
    // Remember This Doesnt Word!!!
    objTest.getAge();

    int n = 4;
    int i = 0;
    ++i;
    i++;
    a = a + b;

    for(int i = 0; i < retrun3(1, 23, 3); ++i)
    {
        std::cout << "BUNA";
    }

    const int i = 0;

    while (i < 3)
    {
        while (i < 3)
        {
            ++i;
            std::cout << "Buna";
        }
        
    }

    if(i > 3)
    {
        std::cout << "NU";
    }
    else if(i < 3)
    {
        std::cout << "DA";
    }
    else
    {
        std::cout << "DA";
    }
       
    for(const char c : mama)
    {
        std::cout << c;
    }
  

    std::cout << "Hello Word";

    std::cout<<std::endl;

}