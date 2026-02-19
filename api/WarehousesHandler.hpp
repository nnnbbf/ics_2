// warehouseshandler.h
#pragma once
#include "../service/WarehousesService.hpp"
#include "../util/httplib.h"
#include "../util/jsonutil.hpp"

class WarehousesHandler
{
private:
    WarehouseService service;

public:
    void handleGetAllWarehouses(const httplib::Request &req, httplib::Response &res)
    {
        std::string keyword = "";
        if (req.has_param("keyword")) {
            keyword = req.get_param_value("keyword");
        }
        
        auto warehouses = service.getAllWarehouses(keyword);
        Json::Value jsonArray = Json::Value(Json::arrayValue);
        for (const auto &warehouse : warehouses)
        {
            Json::Value json = warehouse.toJson();
            jsonArray.append(json);
        }
        res.set_content(jsonArray.toStyledString(), "application/json");
    }

    void handleGetWarehouseById(const httplib::Request &req, httplib::Response &res)
    {
        int id = std::stoi(req.path_params.at("id"));
        WarehousesModel warehouse = service.getWarehouseById(id);
        Json::Value json = warehouse.toJson();
        res.set_content(json.toStyledString(), "application/json");
    }

    void handleGetWarehouseByName(const httplib::Request &req, httplib::Response &res)
    {
        std::string name = req.path_params.at("name");
        WarehousesModel warehouse = service.getWarehouseByName(name);
        Json::Value json = warehouse.toJson();
        res.set_content(json.toStyledString(), "application/json");
    }

    void handleCreateWarehouse(const httplib::Request &req, httplib::Response &res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        WarehousesModel warehouse = WarehousesModel::fromJson(json);
        WarehousesModel newWarehouse = service.createWarehouse(warehouse);
        
        if (newWarehouse.getId() <= 0)
        {
            res.status = 400;
            Json::Value errorJson;
            errorJson["error"] = "Failed to create warehouse or warehouse name already exists";
            res.set_content(errorJson.toStyledString(), "application/json");
            return;
        }
        
        Json::Value responseJson = newWarehouse.toJson();
        res.set_content(responseJson.toStyledString(), "application/json");
    }

    void handleUpdateWarehouse(const httplib::Request &req, httplib::Response &res)
    {
        int id = std::stoi(req.path_params.at("id"));
        Json::Value json = JsonUtil::parseJson(req.body);
        WarehousesModel warehouse = WarehousesModel::fromJson(json);
        warehouse.setId(id);
        
        bool success = service.updateWarehouse(warehouse);
        Json::Value responseJson;
        responseJson["success"] = success;
        if (!success) {
            responseJson["error"] = "Failed to update warehouse";
        }
        res.set_content(responseJson.toStyledString(), "application/json");
    }

    void handleDeleteWarehouse(const httplib::Request &req, httplib::Response &res)
    {
        int id = std::stoi(req.path_params.at("id"));
        bool success = service.deleteWarehouse(id);
        Json::Value responseJson;
        responseJson["success"] = success;
        if (!success) {
            responseJson["error"] = "Failed to delete warehouse";
        }
        res.set_content(responseJson.toStyledString(), "application/json");
    }

    void handleDeleteWarehouseByName(const httplib::Request &req, httplib::Response &res)
    {
        std::string name = req.path_params.at("name");
        bool success = service.deleteWarehouseByName(name);
        Json::Value responseJson;
        responseJson["success"] = success;
        if (!success) {
            responseJson["error"] = "Failed to delete warehouse";
        }
        res.set_content(responseJson.toStyledString(), "application/json");
    }
};