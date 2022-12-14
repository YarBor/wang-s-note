#include <iostream>
static int i = 666;
static int &f = i;
int &demo_static(int **addr)
{
    *addr = &i;
    printf("demo_static: i 的地址: %p, i=%d\n", &i, i);
    int a = 5;
    int &aa = a;
    return aa;
}
int main(int argc, char **argv)
{
    int a = 0, b = 0;
    int &aaa = a;
    int &bb = aaa;
    std::cout << bb << std::endl;
    int *aa = &a;

    f = demo_static(&aa);

    std::cout << f << std::endl;
}
