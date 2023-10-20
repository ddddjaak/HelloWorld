#include <iostream>
#include <stdio.h>
//using namespace std;

class base
{
private:
    /* data */
public:
    base(/* args */);
    virtual void girls() {
        printf("这是一个基类\n");
    }
    void boys() {
        printf("这是基类且无虚函数的部分\n");
    }
    ~base();
};

base::base(/* args */)
{
    printf("类已经被创建\n");
}

base::~base()
{
    printf("析构 \n");
}

class base1: public base {
    public:
        base1();
        ~base1();
        virtual void girls() {
            printf("女孩被重写\n");// << "重写女孩" << endl;
        }
        void boys() {
            printf("这是派生类且无虚函数的部分\n");
        }
};

base1::base1()
{

}
base1::~base1()
{
    printf(" xig\n");
}

int main()
{
    base1 girl1;
    base1 girl2;
    base* b = new base;
    b = new base1;
    b->boys();
    girl2.boys();
    girl1.girls();
    printf("Hello World !!\n");
    std::cout << "c++ code runner" << std::endl;
    delete b;
    return 0;
}