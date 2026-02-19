#pragma once
#include "../application/person/PersonAppService.hpp"
#include "../application/person/RechargeAppService.hpp"
#include "../infrastructure/persistence/PersonRepository.hpp"
#include "../infrastructure/persistence/RechargeRecordRepository.hpp"
#include "../infrastructure/uow/DatabaseUnitOfWork.hpp"
#include "../../shared/util/httplib.h"
#include "../../shared/util/jsonutil.hpp"
#include "../../shared/util/log.hpp"

class PersonController
{
private:
    Database &db_;
    Infrastructure::Persistence::PersonRepository personRepo_;
    Infrastructure::Persistence::RechargeRecordRepository recordRepo_;
    DatabaseUnitOfWork uow_;
    Application::PersonUseCases::PersonAppService personApp_;
    Application::PersonUseCases::RechargeAppService rechargeApp_;

public:
    PersonController()
        : db_(Database::getInstance()), 
          personRepo_(db_), 
          recordRepo_(db_), 
          uow_(db_), 
          personApp_(personRepo_), 
          rechargeApp_(personRepo_, recordRepo_, uow_)
    {
    }

    // 根据关键字获取人员列表
    void handleGetPersons(const httplib::Request &req, httplib::Response &res)
    {
        std::string keyword = req.get_param_value("keyword");
        auto persons = personApp_.GetAll(keyword);
        Json::Value jsonArray = Json::Value(Json::arrayValue);
        for (const auto &person : persons)
        {
            Json::Value json = person.toJson();
            jsonArray.append(json);
        }
        res.set_content(jsonArray.toStyledString(), "application/json");
    }

    void handleGetPersonByCardNumber(const httplib::Request &req, httplib::Response &res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        std::string cardNumber = json["card_number"].asString();
        Domain::PersonModule::Person person = personApp_.GetByCardNumber(cardNumber);
        Json::Value newjson = person.toJson();
        res.set_content(newjson.toStyledString(), "application/json");
    }

    void handleCreatePerson(const httplib::Request &req, httplib::Response &res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        std::string name = json["name"].asString();        // 移除 &
        std::string cardNumber = json["card_number"].asString();  // 移除 &
        std::string department = json["department"].asString();   // 移除 &
        std::string workCenter = json["work_center"].asString();  // 移除 &
        Domain::PersonModule::Person newperson = personApp_.Create(name, cardNumber, department, workCenter);
        Json::Value responseJson = newperson.toJson();  // 修正变量名
        res.set_content(responseJson.toStyledString(), "application/json");
    }
    
    void handleUpdatePerson(const httplib::Request &req, httplib::Response &res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << "Received JSON for UpdatePerson: " << json.toStyledString();
        Domain::PersonModule::Person person = Domain::PersonModule::Person::fromJson(json);  // 修正
        LOG(LogLevel::Debug) << "Handling update for person ID: " << person.Id();  // Id() 不是 getId()
        bool success = personApp_.Update(person);
        Json::Value responseJson;
        responseJson["success"] = success;
        res.set_content(responseJson.toStyledString(), "application/json");
    }
    
    void handleDeletePerson(const httplib::Request &req, httplib::Response &res)
    {
        int id = std::stoi(req.path_params.at("id"));
        LOG(LogLevel::Debug) << "Handling delete for person ID: " << id;
        bool success = personApp_.Delete(id);
        Json::Value responseJson;
        responseJson["success"] = success;
        res.set_content(responseJson.toStyledString(), "application/json");
    }

    void HandleRecharge(const httplib::Request &req, httplib::Response &res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        int personId = json["person_id"].asInt();
        double amount = json["amount"].asDouble();
        std::string opName = json["operator"].asString();

        bool success = rechargeApp_.Recharge(personId, amount, opName);

        Json::Value resp;
        resp["success"] = success;
        res.set_content(resp.toStyledString(), "application/json");
    }
};