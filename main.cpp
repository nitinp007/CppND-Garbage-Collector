#include "gc_pointer.h"
#include "LeakTester.h"

int main()
{
    Pointer<int> p = new int(19);
    p = new int(21);
    p = new int(28);

    Pointer<float> s = new float(21.33);
    Pointer<float> s2 = new float(25.78);
    s2 =  s;

    Pointer<int,10> q  = new int[10]{10,20,30,40,50,60,70,80,90,100};

    std::cout<<"the address of p is: "<<p<<" : "<<*p<<std::endl;

    return 0;

}
