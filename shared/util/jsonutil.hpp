#pragma once
#include <jsoncpp/json/json.h>
#include <string>  
#include <sstream>

class JsonUtil {
public:
    // 将 JSON 对象转换为字符串
    static std::string jsonToString(const Json::Value& json) {
        Json::StreamWriterBuilder writer;
        return Json::writeString(writer, json);
    }
    // 将字符串解析为 JSON 对象
    static Json::Value stringToJson(const std::string& str) {
        Json::CharReaderBuilder reader;
        Json::Value json;
        std::string errs;
        std::istringstream s(str);
        if (!Json::parseFromStream(reader, s, &json, &errs)) {
            // 抛异常
            throw std::runtime_error("Failed to parse JSON: " + errs);
        }
        return json;
    }
    // 构建成功响应
    static Json::Value buildSuccessResponse(const Json::Value& data) {
        Json::Value response;
        response["status"] = "success";
        response["data"] = data;
        return response;
    }
    // 构建错误响应
    static Json::Value buildErrorResponse(const std::string& errorMessage) {
        Json::Value response;
        response["status"] = "error";
        response["message"] = errorMessage; 
        return response;
    }
    static Json::Value parseJson(const std::string& str) {
        return stringToJson(str);
    }   
};