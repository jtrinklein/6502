#include <iostream>
#include <vector>
#include <string>
#include "cpu.h"
#include "mem.h"

using namespace std;

int main()
{
    vector<string> msg {"Hello", "C++", "World", "from", "VS Code", "and the C++ extension!"};
    auto mem = new Mem();
    auto cpu = new CPU(mem);
    (void)cpu;
    for (const string& word : msg)
    {
        cout << word << " ";
    }
    cout << endl;
}