#include<iostream>
using namespace std;
class People
{
public:
    People();
    ~People();
    int get_age(void)
    {
        return age;
    }
    string  get_name(void)
    {
        return name;
    }
    void set_age(int ag)
    {
        age = ag;
    }
    void set_name(string  nam)
    {
        name = nam;
    }
    People(const People& p1)
    {
        cout << "�������캯��" << endl;
        this->name = p1.name;
        this->age = p1.age;
    }
    //������������������˵��������
    People operator+(const People& b)
    {
        People people;
        people.age = this->age + b.age;
        people.name = this->name + b.name;
        return people;
    }
    friend void printage(People people);
protected:
    int age;
    string  name;
};

People::People(void)
{
    cout << "People is being created" << endl;
}

People::~People(void)
{
    cout << "People is being deleted" << endl;
}
void printage(People people)
{
    cout << "age of people is " << people.age << endl;
}
class PIG:public People
{
public:
    int get_b(void)
    {
        return weight*age;
    }
    //PIG();
    //~PIG();
    void set_weight(int b);
    int get_weight(void);
private:
   int weight;
};
/*
PIG::PIG(void)
{
    cout "pig is being created" << endl;
}
PIG::~PIG(void)
{
    cout "pig is being deleted" << endl;
}
*/
int PIG::get_weight(void)
{
    return weight;
}

void PIG::set_weight(int b)
{
    weight = b;
}

int main()
{
    People people1;
    People people2;
    People people3;
    PIG pig1;
    int age_sum = 0;
    string name_sum = "null";

    cout << people1.get_age() << endl;
    cout << people2.get_age() << endl;
    cout << people3.get_age() << endl;
    people1.set_age(26);
    people1.set_name("yufan");

    people2.set_age(25);
    people2.set_name("weicaiyan");
    pig1.set_name("xiaozhuzhu");
    pig1.set_age(5);
    pig1.set_weight(100);
    people3 = people1 + people2;
    age_sum = people3.get_age();
    name_sum = people3.get_name();
    cout << people1.get_name() << "  " << people1.get_age() << endl;
    cout << people2.get_name() << "  " << people2.get_age() << endl;
    cout << "�����˵��������ǣ� " << age_sum << endl;
    cout << "�����������������ǣ� " << name_sum << endl;
    cout << "���������  " << pig1.get_name() << "  " << pig1.get_b() << "  "<< pig1.get_weight() << endl;
    printage(people1);
    return 0;
}
