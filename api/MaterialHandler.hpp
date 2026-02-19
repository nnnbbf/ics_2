#pragma once
#include "../service/MaterialService.hpp"
#include "../util/httplib.h"
#include "../util/jsonutil.hpp"


class MaterialHandler
{
    private:
        MaterialService service;
    public:
    void handleGetMaterial(const httplib::Request& req, httplib::Response& res)
    {
        auto materials = service.getAllMaterials();
        Json::Value jsonArray = Json::Value(Json::arrayValue);
        for (const auto& material : materials) {
        
            Json::Value json = material.toJson();
            //LOG(LogLevel::Debug) << "Response JSON for Get: " << json.toStyledString();
            jsonArray.append(json);
        }
        res.set_content(jsonArray.toStyledString(), "application/json");
    }
    void handleCreateMaterial(const httplib::Request& req, httplib::Response& res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << "Response JSON for Create: " << json.toStyledString();
        MaterialModel material = MaterialModel::fromJson(json);
        MaterialModel newPerson = service.createMaterial(material);
        if(newPerson.getId() == -1) {
            res.status = 409; // 冲突错误
            Json::Value errorJson;
            errorJson["error"] = "Failed to create person. Card number may already exist.";
            res.set_content(errorJson.toStyledString(), "application/json");
            return;
        }
        Json::Value responseJson = newPerson.toJson();
        LOG(LogLevel::Debug) << "Response JSON for Create: " << responseJson.toStyledString();
        res.set_content(responseJson.toStyledString(), "application/json");
    }

    void handleUpdateMaterial(const httplib::Request& req, httplib::Response& res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << "Received JSON for UpdatePerson: " << json.toStyledString();
        MaterialModel material = MaterialModel::fromJson(json);
        LOG(LogLevel::Debug) << "Handling update for person ID: " << material.getId();
        bool success = service.updateMaterial(material);
        Json::Value responseJson;
        responseJson["success"] = success;
        res.set_content(responseJson.toStyledString(), "application/json");
    }

    void handleDeleteMaterial(const httplib::Request& req, httplib::Response& res)
    {
        int id = std::stoi(req.path_params.at("id"));
        LOG(LogLevel::Debug) << "Handling delete for person ID: " << id;
        bool success = service.deleteMaterial(id);
        Json::Value responseJson;
        responseJson["success"] = success;
        res.set_content(responseJson.toStyledString(), "application/json");
    }
};