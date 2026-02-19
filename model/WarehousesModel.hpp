#pragma once
#include <string>
#include <jsoncpp/json/json.h>

class WarehousesModel
{
private:
    int id;
    std::string name;
    std::string location;
    std::string created_at;

public:
    WarehousesModel() : id(0), name(""), location(""), created_at("") {}

    WarehousesModel(int newId, const std::string &newName,
                    const std::string &newLocation, const std::string &newCreatedAt)
        : id(newId), name(newName), location(newLocation), created_at(newCreatedAt) {}

    int getId() const { return id; }
    const std::string &getName() const { return name; }
    const std::string &getLocation() const { return location; }
    const std::string &getCreatedAt() const { return created_at; }

    void setId(int newId) { id = newId; }
    void setName(const std::string &newName) { name = newName; }
    void setLocation(const std::string &newLocation) { location = newLocation; }
    void setCreatedAt(const std::string &newCreatedAt) { created_at = newCreatedAt; }

    Json::Value toJson() const
    {
        Json::Value json;
        json["id"] = std::to_string(id);
        json["name"] = name;
        json["location"] = location;
        json["created_at"] = created_at;
        return json;
    }

    static WarehousesModel fromJson(const Json::Value &json)
    {
        WarehousesModel warehouse;
        if (json.isMember("id"))
            warehouse.id = std::stoi(json["id"].asString());
        if (json.isMember("name"))
            warehouse.name = json["name"].asString();
        if (json.isMember("location"))
            warehouse.location = json["location"].asString();
        if (json.isMember("created_at"))
            warehouse.created_at = json["created_at"].asString();
        return warehouse;
    }
};
