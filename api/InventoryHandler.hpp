#pragma once
#include "../service/InventoryService.hpp"
#include "../util/httplib.h"
#include "../util/jsonutil.hpp"

class InventoryHandler
{
private:
    InventoryService service;
    std::vector<InventoryDetailModel> queryInventory(const Json::Value &json)
    {
        std::string warehouse_name = "";
        std::string material_category = "";
        std::string material_name_keyword = "";
        std::string start_date = "";
        std::string end_date = "";

        // 从JSON中提取参数
        if (json.isMember("warehouse_name"))
        {
            warehouse_name = json["warehouse_name"].asString();
        }
        if (json.isMember("material_category"))
        {
            material_category = json["material_category"].asString();
        }
        if (json.isMember("material_name_keyword"))
        {
            material_name_keyword = json["material_name_keyword"].asString();
        }
        if (json.isMember("start_date"))
        {
            start_date = json["start_date"].asString();
        }
        if (json.isMember("end_date"))
        {
            end_date = json["end_date"].asString();
        }

        // 判断是否有查询条件
        bool has_conditions = !warehouse_name.empty() ||
                              !material_category.empty() ||
                              !material_name_keyword.empty() ||
                              !start_date.empty() ||
                              !end_date.empty();
        
        if (!has_conditions)
        {
            // 没有查询条件，返回所有库存
            return service.getAllInventory();
        }
        else
        {
            // 有条件查询 
            return service.getInventoryByConditions(
                warehouse_name,
                material_category,
                material_name_keyword,
                start_date,
                end_date);
        }
    }

public:
    void handleGetInventory(const httplib::Request &req, httplib::Response &res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        auto inventorys = queryInventory(json);
        Json::Value jsonArray = Json::Value(Json::arrayValue);

        for (const auto &inventory : inventorys)
        {
            Json::Value json = inventory.toJson();
            jsonArray.append(json);
        }
        res.set_content(jsonArray.toStyledString(), "application/json");
    }
};
