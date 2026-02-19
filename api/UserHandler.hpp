#pragma once
#include <fstream>
#include "../service/UserService.hpp"
#include "../util/httplib.h"
#include "../util/jsonutil.hpp"

class UserHandler
{
private:
    UserService service;

public:
    void handleGetAllUsers(const httplib::Request &req, httplib::Response &res)
    {
        auto users = service.getAllUsers();
        Json::Value jsonArray = Json::Value(Json::arrayValue);
        for (const auto &user : users)
        {
            Json::Value json = user.toJson();
            jsonArray.append(json);
        }
        res.set_content(jsonArray.toStyledString(), "application/json");
    }
    void handleGetUserById(const httplib::Request &req, httplib::Response &res)
    {
        int id = std::stoi(req.path_params.at("id"));
        UserModel user = service.getUserById(id);
        Json::Value json = user.toJson();
        res.set_content(json.toStyledString(), "application/json");
    }
    void handleCreateUser(const httplib::Request &req, httplib::Response &res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << "CreateUser request JSON: " << json.toStyledString();
        UserModel user = UserModel::fromJson(json);
        UserModel newUser = service.createUser(user);
        if (newUser.getId() < 0)
        {
            res.status = 409; // 用户已经存在
            Json::Value errorJson;
            errorJson["error"] = "Username already exists or failed to create user.";
            res.set_content(errorJson.toStyledString(), "application/json");
            return;
        }
        Json::Value responseJson = newUser.toJson();
        res.set_content(responseJson.toStyledString(), "application/json");
    }
    void handleUpdateUser(const httplib::Request &req, httplib::Response &res)
    {
        int id = std::stoi(req.path_params.at("id"));
        Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << json.toStyledString();
        UserModel user = UserModel::fromJson(json);
        LOG(LogLevel::Debug) << user.getRole();
        bool success = service.updateUser(user);
        if(success) LOG(LogLevel::Error) << "Updata User Failure";
        Json::Value responseJson;
        responseJson["success"] = success;
        res.set_content(responseJson.toStyledString(), "application/json");
    }
    void handleDeleteUser(const httplib::Request &req, httplib::Response &res)
    {
        int id = std::stoi(req.path_params.at("id"));
        bool success = service.deleteUser(id);
        Json::Value responseJson;
        responseJson["success"] = success;
        res.set_content(responseJson.toStyledString(), "application/json");
    }

    void handleAuthenticateUser(const httplib::Request &req, httplib::Response &res)
    {
        Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << "AuthenticateUser request JSON: " << json.toStyledString();
        std::string username = json["username"].asString();
        std::string password = json["password"].asString();
        UserModel user = service.authenticate(username, password);
        if (user.getId() < 0)
        {
            res.status = 401; // 未授权
            Json::Value errorJson;
            errorJson["error"] = "Authentication failed.";
            res.set_content(errorJson.toStyledString(), "application/json");
            return;
        }
        Json::Value responseJson = user.toJson();
        LOG(LogLevel::Debug) << "Authenticated user JSON: " << responseJson.toStyledString();
        res.set_content(responseJson.toStyledString(), "application/json");
    }
    // 重置密码
    void handleResetPassword(const httplib::Request &req, httplib::Response &res)
    {
        int id = std::stoi(req.path_params.at("id"));
        bool success = service.resetPassword(id);
        Json::Value responseJson;   
        responseJson["success"] = success;
        if (!success)
        {
            res.status = 400; // 请求错误
            responseJson["error"] = "Failed to reset password.";
        }
        res.set_content(responseJson.toStyledString(), "application/json");
    }
    // 修改密码
    void handleChangePassword(const httplib::Request &req, httplib::Response &res)
    {
        int id = std::stoi(req.path_params.at("id"));
        Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << json.toStyledString();
        std::string oldPassword = json["oldPassword"].asString();
        std::string newPassword = json["newPassword"].asString();
        bool success = service.changePassword(id, oldPassword, newPassword);
        Json::Value responseJson;
        responseJson["success"] = success;
        if (!success)
        {
            res.status = 400; // 请求错误
            responseJson["error"] = "Failed to change password. Check old password or new password complexity.";
        }
        res.set_content(responseJson.toStyledString(), "application/json");
    }
    // 修改用户名
    void handleChangeUsername(const httplib::Request &req, httplib::Response &res)
    {
        int id = std::stoi(req.get_param_value("id"));
        Json::Value json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << json.toStyledString();
        std::string newUsername = json["newUsername"].asString();
        bool success = service.changeUsername(id, newUsername);
        Json::Value responseJson;
        responseJson["success"] = success;
        if (!success)
        {
            res.status = 400; // 请求错误
            responseJson["error"] = "Failed to change username. It might already exist.";
        }
        res.set_content(responseJson.toStyledString(), "application/json");
    }
    void handleGetCurrentUser(const httplib::Request &req, httplib::Response &res)
    {
        UserModel user = service.getCurrentUser();
        Json::Value json = user.toJson();
        res.set_content(json.toStyledString(), "application/json");
    }

    // 检查版本更新
    void handleCheckVersion(const httplib::Request &req, httplib::Response &res)
    {
        // 解析请求
        Json::Value request_json = JsonUtil::parseJson(req.body);
        LOG(LogLevel::Debug) << "CheckVersion request JSON: " << request_json.toStyledString();
        std::string client_version = "1.0.0";
        if (request_json.isMember("current_version"))
        {
            client_version = request_json["current_version"].asString();
        }
        
        // 读取版本配置文件
        std::ifstream file("config/version.json");
        if (!file.is_open())
        {
            res.status = 500;
            Json::Value error_json;
            error_json["error"] = "版本配置文件不存在";
            res.set_content(error_json.toStyledString(), "application/json");
            return;
        }
        
        Json::Value config_json = JsonUtil::parseJson(std::string((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>()));
        std::string server_version = config_json["version"].asString();
        
        // 构建响应
        Json::Value response_json;
        
        if (client_version != server_version)
        {
            // 有新版本
            response_json["has_update"] = true;
            response_json["current_version"] = server_version;
            response_json["download_url"] = config_json["download_url"];
            response_json["changelog"] = config_json["changelog"];
            response_json["force_update"] = config_json["force_update"];
        }
        else
        {
            // 已是最新版本
            response_json["has_update"] = false;
            response_json["message"] = "已是最新版本";
        }
        
        res.set_content(response_json.toStyledString(), "application/json");
    }
};