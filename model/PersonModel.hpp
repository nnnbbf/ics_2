// model/PersonModel.hpp
#pragma once
#include <jsoncpp/json/json.h>
#include <string>

class PersonModel
{
private:
    int id;
    std::string name;       // 姓名
    std::string cardNumber; // 卡号
    double balance;         // 余额
    std::string status;     // 状态
    std::string department; // 部门
    std::string workCenter; // 工作中心
    std::string created_at; // 创建时间
public:
    PersonModel() : id(-1), balance(0.0), status("正常") {}
    PersonModel(int newId, const std::string &newName, const std::string &newCardNumber,
                double newBalance, const std::string &newStatus, const std::string &newDepartment,
                const std::string &newWorkCenter, const std::string &newCreateAt)
        : id(newId), name(newName), cardNumber(newCardNumber),
          balance(newBalance), status(newStatus), department(newDepartment),
          workCenter(newWorkCenter), created_at(newCreateAt)
    {
    }
    // Getter 方法
    int getId() const { return id; }
    const std::string &getName() const { return name; }
    const std::string &getCardNumber() const { return cardNumber; }
    double getBalance() const { return balance; }
    const std::string &getStatus() const { return status; }
    const std::string &getDepartment() const { return department; }
    const std::string &getWorkCenter() const { return workCenter; }
    const std::string &getCreateAt() const { return created_at; }

    // Setter 方法
    void setId(int newId) { id = newId; }
    void setName(const std::string &newName) { name = newName; }
    void setCardNumber(const std::string &newCardNumber) { cardNumber = newCardNumber; }
    void setBalance(double newBalance) { balance = newBalance; }
    void setStatus(const std::string &newStatus) { status = newStatus; }
    void setDepartment(const std::string &newDepartment) { department = newDepartment; }
    void setWorkCenter(const std::string &newWorkCenter) { workCenter = newWorkCenter; }
    void setCreatedAt(const std::string &newCreateAt) { created_at = newCreateAt; }

    // JSON转换
    Json::Value toJson() const
    {
        Json::Value json;
        json["id"] = std::to_string(id);
        json["name"] = name;
        json["card_number"] = cardNumber;
        json["balance"] = std::to_string(balance);
        json["status"] = status;
        json["department"] = department;
        json["work_center"] = workCenter;
        json["created_at"] = created_at;
        return json;
    }

    static PersonModel fromJson(const Json::Value &json)
    {
        PersonModel person;
        if (json.isMember("id"))
            person.id = std::stoi(json["id"].asString());
        if (json.isMember("name"))
            person.name = json["name"].asString();
        if (json.isMember("card_number"))
            person.cardNumber = json["card_number"].asString();
        if (json.isMember("balance"))
            person.balance = std::stod(json["balance"].asString());
        if (json.isMember("status"))
            person.status = json["status"].asString();
        if (json.isMember("department"))
            person.department = json["department"].asString();
        if (json.isMember("work_center"))
            person.workCenter = json["work_center"].asString();
        if (json.isMember("created_at"))
            person.setCreatedAt(json["created_at"].asString());
        return person;
    }

    // 业务方法
    bool canDeduct(double amount) const
    {
        return balance >= amount;
    }

    void deduct(double amount)
    {
        if (canDeduct(amount))
        {
            balance -= amount;
        }
    }

    void topup(double amount)
    {
        balance += amount;
    }

    // 便捷方法：设置所有字段
    void setAll(int newId, const std::string &newName, const std::string &newCardNumber,
                double newBalance, const std::string &newStatus, const std::string &newDepartment,
                const std::string &newWorkCenter, const std::string &newCreateAt)
    {
        id = newId;
        name = newName;
        cardNumber = newCardNumber;
        balance = newBalance;
        status = newStatus;
        department = newDepartment;
        workCenter = newWorkCenter;
        created_at = newCreateAt;
    }

    // 重置方法
    void reset()
    {
        id = -1;
        name.clear();
        cardNumber.clear();
        balance = 0.0;
        status = "正常";
        department.clear();
        workCenter.clear();
        created_at.clear();
    }

    // 检查是否有效（有合法ID）
    bool isValid() const
    {
        return id > 0;
    }
};