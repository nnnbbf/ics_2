// domain/person/RechargeRecord.hpp
#pragma once

#include <string>
#include <jsoncpp/json/json.h>
namespace Domain::PersonModule
{

    class RechargeRecord
    {
    public:
        using IdType = int;

    private:
        IdType id_;
        int person_id_;
        std::string person_name_;
        double amount_;
        std::string recharge_date_;
        std::string operator_name_;

    public:
        RechargeRecord()
            : id_(-1),
              person_id_(-1),
              amount_(0.0) {}

        static RechargeRecord CreatePositive(
            int personId,
            const std::string &personName,
            double amount,
            const std::string &operatorName)
        {
            RechargeRecord r;
            r.person_id_ = personId;
            r.person_name_ = personName;
            r.amount_ = amount;
            r.operator_name_ = operatorName;
            return r;
        }
        // JSON转换
        Json::Value toJson() const
        {
            Json::Value json;
            json["id"] = std::to_string(id_);
            json["person_id"] = std::to_string(person_id_);
            json["person_name"] = person_name_;
            json["amount"] = std::to_string(amount_);
            json["recharge_date"] = recharge_date_;
            json["operator_name"] = operator_name_;
            return json;
        }
        static RechargeRecord fromJson(const Json::Value &json)
        {
            RechargeRecord record;
            if (json.isMember("id"))
                record.id_ = std::stoi(json["id"].asString());
            if (json.isMember("person_id"))
                record.person_id_ = std::stoi(json["person_id"].asString());
            if (json.isMember("person_name"))
                record.person_name_ = json["person_name"].asString();
            if (json.isMember("amount"))
                record.amount_ = std::stod(json["amount"].asString());
            if (json.isMember("recharge_date"))
                record.recharge_date_ = json["recharge_date"].asString();
            if (json.isMember("operator_name"))
                record.operator_name_ = json["operator_name"].asString();
            return record;
        }
        // getter
        IdType Id() const { return id_; }
        int PersonId() const { return person_id_; }
        const std::string &PersonName() const { return person_name_; }
        double Amount() const { return amount_; }
        const std::string &OperatorName() const { return operator_name_; }

        void SetId(IdType id) { id_ = id; }

        bool IsPositive() const { return amount_ > 0; }
        bool IsReversal() const { return amount_ < 0; }
    };

} // namespace Domain::PersonModule
