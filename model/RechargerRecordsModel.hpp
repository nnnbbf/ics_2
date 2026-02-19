#pragma once
#include <jsoncpp/json/json.h>
#include <string>
#include "../util/jsonutil.hpp"
#include "../util/log.hpp"
class RechargerRecordsModel
{
private:
    int id;
    int person_id;
    std::string person_name;

    double amount;
    std::string recharge_date;
    std::string operator_name;

public:
    RechargerRecordsModel() : id(-1), person_id(-1), amount(0.0) {}
    RechargerRecordsModel(int newPersonId, const std::string &newPersonName,
                          double newAmount, const std::string &newRechargeTime,
                          const std::string &newOperator)
        : person_id(newPersonId), person_name(newPersonName),
          amount(newAmount), recharge_date(newRechargeTime), operator_name(newOperator)
    {
    }
    // Getter 方法
    int getId() const { return id; }
    int getPersonId() const { return person_id; }
    const std::string &getPersonName() const { return person_name; }
    double getAmount() const { return amount; }
    const std::string &getRechargeTime() const { return recharge_date; }
    const std::string &getOperator() const { return operator_name; }
    // Setter 方法
    void setId(int newId) { id = newId; }
    void setPersonId(int newPersonId) { person_id = newPersonId; }
    void setPersonName(const std::string &newPersonName) { person_name = newPersonName; }
    void setAmount(double newAmount) { amount = newAmount; }
    void setRechargeTime(const std::string &newRechargeTime) { recharge_date = newRechargeTime; }
    void setOperator(const std::string &newOperator) { operator_name= newOperator; }
    // JSON转换
    Json::Value toJson() const
    {
        Json::Value json;
        json["id"] = std::to_string(id);
        json["person_id"] = std::to_string(person_id);
        json["person_name"] = person_name;
        json["amount"] = std::to_string(amount);
        json["recharge_date"] = recharge_date;
        json["operator_name"] = operator_name;
        return json;
    }
    static RechargerRecordsModel fromJson(const Json::Value &json)
    {
        RechargerRecordsModel record;
        if (json.isMember("id"))
            record.id = std::stoi(json["id"].asString());
        if (json.isMember("person_id"))
            record.person_id = std::stoi(json["person_id"].asString());
        if (json.isMember("person_name"))
            record.person_name = json["person_name"].asString();
        if (json.isMember("amount"))
            record.amount = std::stod(json["amount"].asString());
        if (json.isMember("operator_name"))
            record.operator_name= json["operator_name"].asString();
        if (json.isMember("recharge_date"))
            record.recharge_date = json["recharge_date"].asString();
        return record;
    }
};