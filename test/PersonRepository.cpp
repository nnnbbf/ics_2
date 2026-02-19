#include "../util/log.hpp"
#include "../repository/PersonRepository.hpp"
#include "../model/PersonModel.hpp"
#include <iostream>
#include <jsoncpp/json/json.h>  
int main()
{
    Json::Value testJson;
    testJson["id"] = "1";
    testJson["name"] = "John Doe";
    testJson["card_number"] = "1234567890";
    testJson["balance"] = "100.50";
    testJson["status"] = "active";
    testJson["department"] = "Engineering";
    testJson["work_center"] = "WC001";
    testJson["create_at"] = "2024-01-01 10:00";
    LOG(LogLevel::Info) << std::stoi(testJson["id"].asString());
    PersonModel person = PersonModel::fromJson(testJson);
    LOG(LogLevel::Info) << "Person ID: " << person.getId();
    LOG(LogLevel::Info) << "Person Name: " << person.getName();
    LOG(LogLevel::Info) << "Person Card Number: " << person.getCardNumber();
    LOG(LogLevel::Info) << "Person Balance: " << person.getBalance();
    LOG(LogLevel::Info) << "Person Status: " << person.getStatus();
    LOG(LogLevel::Info) << "Person Department: " << person.getDepartment();
    LOG(LogLevel::Info) << "Person Work Center: " << person.getWorkCenter();
    LOG(LogLevel::Info) << "Person Created At: " << person.getCreateAt();   
    return 0;
}