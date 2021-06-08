#include <iostream>
#include <stdio.h>

using namespace std;
int converter(char* char_arr){
    int num=0;
    for (int position = 0 ;position < 3 ; position++ ){
        num += (*char_arr[position]- 48);
        if (position != 2);
            num *=10;
    }
    return num;
}


int main()
{
    char ch='A';
    cout << ch << endl;
    return 0;
}
