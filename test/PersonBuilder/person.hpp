#include<iostream>
#include<string>

class Person {
private:
    std::string name;//姓名
    std::string phoneNumber;//电话号码
    int age;//年龄
    std::string address;//地址
public:
    Person(const std::string& name) : name(name) {}
    std::string getName() const { return name; }
    void setPhoneNumber(const std::string& phone) { phoneNumber = phone; }
    void setAge(int a) { age = a; }
    void setAddress(const std::string& addr) { address = addr; }

    void displayInfo() const {
        std::cout << "Name: " << name << std::endl;
        std::cout << "Phone Number: " << phoneNumber << std::endl;
        std::cout << "Age: " << age << std::endl;
        std::cout << "Address: " << address << std::endl;
    }
};