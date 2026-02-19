// api/personhandler.hpp
#pragma once
#include "../service/PersonService.hpp"
#include "../service/RechargerRecordsService.hpp"
#include "../util/httplib.h"
#include "../util/jsonutil.hpp"

class PersonHandler {
private:
    PersonService service;
    RechargerRecordsService rechargeService;
public:
    void handleGetPersons(const httplib::Request& req, httplib::Response& res)
    {
        std::string keyword = req.get_param_value("keyword");
        auto persons = service.getAllPersons(keyword);
        Json::Value jsonArray = Json::Value(Json::arrayValue);
        for (const auto& person : persons) {
            //LOG(LogLevel::Debug) << "Person ID: " << person.getId() << " Name: " << person.getName() << "create_at: " << person.getCreateAt();
            Json::Value json = person.toJson();
            //LOG(LogLevel::Debug) << "Person JSON: " << json.toStyledString();
            jsonArray.append(json);
        }
        res.set_content(jsonArray.toStyledString(), "application/json");
    }
    void handleGetPersonById(const httplib::Request& req, httplib::Response& res)
    {
        int id = std::stoi(req.get_param_value("id"));
        PersonModel person = service.getPersonById(id);
        Json::Value json = person.toJson();
        res.set_content(json.toStyledString(), "application/json");
    }

    void handleGetPersonByCardNumber(const httplib::Request& req, httplib::Response& res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        std::string cardNumber = json["card_number"].asString();
        LOG(LogLevel::Debug) << cardNumber;
        PersonModel person = service.getPersonByCardNumber(cardNumber);
        Json::Value newjson = person.toJson();
        LOG(LogLevel::Debug) << newjson.toStyledString();
        res.set_content(newjson.toStyledString(), "application/json");
    }

    void handleCreatePerson(const httplib::Request& req, httplib::Response& res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << "Received JSON for CreatePerson: " << json.toStyledString();
        PersonModel person = PersonModel::fromJson(json);
        LOG(LogLevel::Debug) << "Handling create for person with card number: " << person.getCardNumber();
        PersonModel newPerson = service.createPerson(person);
        LOG(LogLevel::Debug) << "Created person ID: " << newPerson.getId();
        if(newPerson.getId() == -1) {
            res.status = 409; // 冲突错误
            Json::Value errorJson;
            LOG(LogLevel::Error) << "Failed to create person. Card number may already exist.";
            errorJson["error"] = "Failed to create person. Card number may already exist.";
            res.set_content(errorJson.toStyledString(), "application/json");
            return;
        }
        Json::Value responseJson = newPerson.toJson();
        LOG(LogLevel::Debug) << "Response JSON for CreatePerson: " << responseJson.toStyledString();
        res.set_content(responseJson.toStyledString(), "application/json");
    }
    void handleUpdatePerson(const httplib::Request& req, httplib::Response& res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << "Received JSON for UpdatePerson: " << json.toStyledString();
        PersonModel person = PersonModel::fromJson(json);
        LOG(LogLevel::Debug) << "Handling update for person ID: " << person.getId();
        bool success = service.updatePerson(person);
        Json::Value responseJson;
        responseJson["success"] = success;
        res.set_content(responseJson.toStyledString(), "application/json");
    }
    void handleDeletePerson(const httplib::Request& req, httplib::Response& res)
    {
        int id = std::stoi(req.path_params.at("id"));
        LOG(LogLevel::Debug) << "Handling delete for person ID: " << id;
        bool success = service.deletePerson(id);
        Json::Value responseJson;
        responseJson["success"] = success;
        res.set_content(responseJson.toStyledString(), "application/json");
    }
    void handleRecharge(const httplib::Request& req, httplib::Response& res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << json.toStyledString();
        int personId = json["person_id"].asInt();
        double amount = json["amount"].asDouble();
        std::string operatorname = json["operator"].asString();
        bool success = rechargeService.recharge(personId, amount, operatorname);
        Json::Value responseJson;
        responseJson["success"] = success;
        LOG(LogLevel::Debug) << responseJson.toStyledString();
        res.set_content(responseJson.toStyledString(), "application/json");
    }
    void handleDeduct(const httplib::Request& req, httplib::Response& res)
    {
                Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << json.toStyledString();
        int personId = json["person_id"].asInt();
        double amount = json["amount"].asDouble();
        std::string operatorname = json["operator"].asString();
        //消费

        //res.set_content(responseJson.toStyledString(), "application/json");
    }
};