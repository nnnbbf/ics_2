#pragma once
#include "../service/InboundService.hpp"
#include "../util/httplib.h"
#include "../util/jsonutil.hpp"

class InboundHandler
{
private:
    InboundService service;
    
    std::vector<InboundModel> queryInbound(const Json::Value &json)
    {
        std::string warehouse_name = "";
        std::string material_name_keyword = "";
        std::string supplier = "";
        std::string purchaser = "";
        std::string registrant = "";
        std::string start_date = "";
        std::string end_date = "";

        // 从JSON中提取参数
        if (json.isMember("warehouse_name"))
        {
            warehouse_name = json["warehouse_name"].asString();
        }
        if (json.isMember("material_name_keyword"))
        {
            material_name_keyword = json["material_name_keyword"].asString();
        }
        if (json.isMember("supplier"))
        {
            supplier = json["supplier"].asString();
        }
        if (json.isMember("purchaser"))
        {
            purchaser = json["purchaser"].asString();
        }
        if (json.isMember("registrant"))
        {
            registrant = json["registrant"].asString();
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
                              !material_name_keyword.empty() ||
                              !supplier.empty() ||
                              !purchaser.empty() ||
                              !registrant.empty() ||
                              !start_date.empty() ||
                              !end_date.empty();
        if(!has_conditions)
        {
            return service.getAllInbound();
        }
        else
        {
            // 使用条件查询
        return service.getInboundByConditions(
            warehouse_name,
            material_name_keyword,
            supplier,
            purchaser,
            registrant,
            start_date,
            end_date);
        }
    }

public:
    void handleCreateInbound(const httplib::Request &req, httplib::Response &res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << "Received JSON for AddInbound: " << json.toStyledString();

        // 创建入库记录模型
        InboundModel record = InboundModel::fromJson(json);
        LOG(LogLevel::Debug) << "Handling add inbound for material: " << record.getMaterialName();

        // 执行业务逻辑
        int inboundId = service.createInbound(record);

        if (inboundId <= 0)
        {
            res.status = 500; // 服务器错误
            Json::Value errorJson;
            LOG(LogLevel::Debug) << "sdadasdas";
            errorJson["error"] = "入库失败";
            res.set_content(errorJson.toStyledString(), "application/json");
            return;
        }

        Json::Value responseJson;
        responseJson["id"] = inboundId;
        responseJson["success"] = true;
        LOG(LogLevel::Debug) << "Response JSON for AddInbound: " << responseJson.toStyledString();
        res.set_content(responseJson.toStyledString(), "application/json");
    }
    void handleGetInbound(const httplib::Request &req, httplib::Response &res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        auto inbounds = queryInbound(json);
        Json::Value jsonArray = Json::Value(Json::arrayValue);

        for (const auto &inbound : inbounds)
        {
            Json::Value json = inbound.toJson();
            LOG(LogLevel::Debug) << "Inbound Record JSON: " << json.toStyledString();
            jsonArray.append(json);
        }
        res.set_content(jsonArray.toStyledString(), "application/json");
    }
};