#include <iostream>
using namespace std;
class parent
{
    public:
        parent(int a =0):a(a){};
        ~parent()
        {
            cout << "parent is being xigou" << endl;
        };
        virtual void show_classname() = 0;
    private:
        int a;
};
class child1 : public parent
{
    public:
        child1(int a= 0, int b = 0):parent(a),b(b){};
        ~child1()
        {
            cout << "child1 is being xigou" << endl;
        };
        void show_classname(){
            cout << "class name is child1" <<endl;
        }
    private:
        int b;
};
class child2 : public parent
{
    public:
        child2(int a=0,int c =0):parent(a),c(c){};
        ~child2()
        {
            cout << "child2 is being xigou" << endl;
        };
        void show_classname()
        {
          cout<< "class name is child2" <<endl;
        }
    private:
        int c;
};
int main()
{
  child1 b;
  child2 c;
  parent *a;
  a= &b;
  cout << "a ---> b"<<endl;
  a->show_classname();
  a= &c;
  cout << "a ---> c"<<endl;
  a->show_classname();
}
