//
//  Header.h
//  removeSpace
//
//  Created by Abdel M. Sako on 9/6/23.
//

#ifndef Header_h
#define Header_h
#include <iostream>
#include <cstdlib>


#endif /* Header_h */
using namespace std;
void Main(void) {
    string str("Hello World");
    
    char *ptr = str.data();
    
    size_t s = strlen(ptr);
    
    ptr = str.data() + 6;
    
    s = strlen(ptr);
    
    str.append(" of Crazy");
    
    s = strlen(ptr);
    
    ptr = str.data() + 6;
    
    s = strlen(ptr);
}
