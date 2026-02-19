#pragma once
#include "../service/OutboundService.hpp"
#include "../util/httplib.h"
#include "../util/jsonutil.hpp"

class OutboundHandler
{
private:
    OutboundService service;

    std::vector<OutboundModel> queryOutbound(const Json::Value &json)
    {
        std::string warehouse_name = "";
        std::string material_name_keyword = "";
        std::string person_name = "";
        std::string person_card_number = "";
        std::string person_department = "";
        std::string operator_name = "";
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
        if (json.isMember("person_name"))
        {
            person_name = json["person_name"].asString();
        }
        if (json.isMember("person_card_number"))
        {
            person_card_number = json["person_card_number"].asString();
        }
        if (json.isMember("person_department"))
        {
            person_department = json["person_department"].asString();
        }
        if (json.isMember("operator_name"))
        {
            operator_name = json["operator_name"].asString();
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
                              !person_name.empty() ||
                              !person_card_number.empty() ||
                              !person_department.empty() ||
                              !operator_name.empty() ||
                              !start_date.empty() ||
                              !end_date.empty();

        if (!has_conditions)
        {
            return service.getAllOutbound();
        }
        else
        {
            // 使用条件查询
            return service.getOutboundByConditions(
                warehouse_name,
                material_name_keyword,
                person_name,
                person_card_number,
                person_department,
                operator_name,
                start_date,
                end_date);
        }
    }

public:
    // 单次出库处理
    void handleCreateOutbound(const httplib::Request &req, httplib::Response &res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << "Received JSON for CreateOutbound: " << json.toStyledString();

        // 创建出库记录模型
        OutboundModel record = OutboundModel::fromJson(json);
        LOG(LogLevel::Debug) << "Handling create outbound for material: "
                             << record.getMaterialName()
                             << ", person: " << record.getPersonName();

        // 执行业务逻辑
        int outboundId = service.createOutbound(record);

        if (outboundId <= 0)
        {
            res.status = 500; // 服务器错误
            Json::Value errorJson;
            errorJson["error"] = "出库失败";
            res.set_content(errorJson.toStyledString(), "application/json");
            return;
        }

        Json::Value responseJson;
        responseJson["id"] = outboundId;
        responseJson["success"] = true;
        LOG(LogLevel::Debug) << "Response JSON for CreateOutbound: " << responseJson.toStyledString();
        res.set_content(responseJson.toStyledString(), "application/json");
    }

    // 组合查询出库记录
    void handleGetOutbound(const httplib::Request &req, httplib::Response &res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        auto outbounds = queryOutbound(json);
        Json::Value jsonArray = Json::Value(Json::arrayValue);

        for (const auto &outbound : outbounds)
        {
            Json::Value json = outbound.toJson();
            LOG(LogLevel::Debug) << "Outbound Record JSON: " << json.toStyledString();
            jsonArray.append(json);
        }
        res.set_content(jsonArray.toStyledString(), "application/json");
    }
};