// domain/person/Person.hpp
#pragma once

#include <string>
#include <stdexcept>

#include "RechargeRecord.hpp"
#include <jsoncpp/json/json.h>

namespace Domain::PersonModule
{

    class Person
    {
    public:
        using IdType = int;

    private:
        IdType id_;
        std::string name_;
        std::string cardNumber_;
        double balance_;
        std::string status_; // "正常" / "禁用"
        std::string department_;
        std::string workCenter_;
        std::string created_at_;

    public:
        Person()
            : id_(-1),
              balance_(0.0),
              status_("正常") {}

        // 工厂构造（用于创建新人员）
        static Person CreateNew(
            const std::string &name,
            const std::string &cardNumber,
            const std::string &department,
            const std::string &workCenter)
        {
            if (name.empty())
            {
                throw std::invalid_argument("姓名不能为空");
            }
            if (cardNumber.empty())
            {
                throw std::invalid_argument("卡号不能为空");
            }

            Person p;
            p.name_ = name;
            p.cardNumber_ = cardNumber;
            p.department_ = department;
            p.workCenter_ = workCenter;
            p.balance_ = 0.0;
            p.status_ = "正常";
            return p;
        }
        Json::Value toJson() const
        {
            Json::Value json;
            json["id"] = std::to_string(id_);
            json["name"] = name_;
            json["card_number"] = cardNumber_;
            json["balance"] = std::to_string(balance_);
            json["status"] = status_;
            json["department"] = department_;
            json["work_center"] = workCenter_;
            json["created_at"] = created_at_;
            return json;
        }

        static Person fromJson(const Json::Value &json)
        {
            Person person;
            if (json.isMember("id"))
                person.id_ = std::stoi(json["id"].asString());
            if (json.isMember("name"))
                person.name_ = json["name"].asString();
            if (json.isMember("card_number"))
                person.cardNumber_ = json["card_number"].asString();
            if (json.isMember("balance"))
                person.balance_ = std::stod(json["balance"].asString());
            if (json.isMember("status"))
                person.status_ = json["status"].asString();
            if (json.isMember("department"))
                person.department_ = json["department"].asString();
            if (json.isMember("work_center"))
                person.workCenter_ = json["work_center"].asString();
            if (json.isMember("created_at"))
                person.created_at_ = json["created_at"].asString();
            return person;
        }
        // === 基础 getter ===
        IdType Id() const { return id_; }
        const std::string &Name() const { return name_; }
        const std::string &CardNumber() const { return cardNumber_; }
        double Balance() const { return balance_; }
        const std::string &Status() const { return status_; }
        const std::string &Department() const { return department_; }
        const std::string &WorkCenter() const { return workCenter_; }
        bool IsActive() const { return status_ == "正常"; }

        bool IsValid() const { return id_ > 0; }

        void SetId(IdType id) { id_ = id; }

        // === 状态修改 ===
        void Activate()
        {
            status_ = "正常";
        }

        void Disable()
        {
            status_ = "停用";
        }

        // === 领域行为：充值 ===
        RechargeRecord Recharge(double amount,
                                const std::string &operatorName)
        {
            if (!IsActive())
            {
                throw std::runtime_error("人员状态非正常，禁止充值");
            }
            if (amount <= 0.0)
            {
                throw std::invalid_argument("充值金额必须大于 0");
            }
            if (operatorName.empty())
            {
                throw std::invalid_argument("操作人不能为空");
            }

            balance_ += amount;

            return RechargeRecord::CreatePositive(
                id_,
                name_,
                amount,
                operatorName);
        }
    };

}
