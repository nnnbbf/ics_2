#pragma once
#include <string>
#include <cstdlib>
#include <crypt.h>
#include <jsoncpp/json/json.h>
#include "../util/jsonutil.hpp"
#include "../util/log.hpp"

class UserModel
{ 
    public:
    UserModel(int id) : id(id),username(""), password(""), password_hash(""), salt(""), role("warehouse_admin"), created_at("") {}
    UserModel() : id(0), username(""), password(""), password_hash(""), salt(""), role("warehouse_admin"), created_at("") {}
    UserModel(int newId, const std::string &newUsername, const std::string &newPasswordHash,
              const std::string &newSalt, const std::string &newRole, const std::string &newCreatedAt)
        : id(newId), username(newUsername), password(""), password_hash(newPasswordHash), salt(newSalt),
          role(newRole), created_at(newCreatedAt)
    {}
    // Getter 方法
    int getId() const { return id; }
    const std::string &getUsername() const { return username; }
    const std::string &getPasswordHash() const { return password_hash; }
    const std::string &getSalt() const { return salt; }
    const std::string &getRole() const { return role; }
    const std::string &getCreatedAt() const { return created_at; }
    const std::string &getPassword() const { return password; }
    // Setter 方法
    void setId(int newId) { id = newId; }
    void setUsername(const std::string &newUsername) { username = newUsername; }
    void setPasswordHash(const std::string &newPasswordHash) { password_hash = newPasswordHash; }
    void setSalt(const std::string &newSalt) { salt = newSalt; }
    void setRole(const std::string &newRole) { role = newRole; }
    void setCreatedAt(const std::string &newCreatedAt) { created_at = newCreatedAt; }
    void setPassword(const std::string &newPassword) { password = newPassword; }
    // JSON转换
    Json::Value toJson() const
    {
        Json::Value json;
        json["id"] = std::to_string(id);
        json["username"] = username;
        json["password"] = password;
        json["password_hash"] = password_hash;
        json["salt"] = salt;
        json["role"] = role;
        json["created_at"] = created_at;
        return json;
    }   
    static UserModel fromJson(const Json::Value &json)
    {
        UserModel user;
        if (json.isMember("id"))
            user.id = std::stoi(json["id"].asString());
        if (json.isMember("username"))
            user.username = json["username"].asString();
        if (json.isMember("password"))
            user.password = json["password"].asString();
        if (json.isMember("password_hash"))
            user.password_hash = json["password_hash"].asString();
        if (json.isMember("salt"))
            user.salt = json["salt"].asString();
        if (json.isMember("role"))
            user.role = json["role"].asString();
        if (json.isMember("created_at"))
            user.created_at = json["created_at"].asString();
        return user;
    }
    //哈希加密 - 使用bcrypt算法
    static std::string hashPassword(const std::string &password, const std::string &salt = "")
    {
        // 如果没有提供盐值，则生成一个
        std::string finalSalt = salt;
        if (finalSalt.empty()) {
            // 生成bcrypt盐值 ($2a$10$ + 22个字符的随机字符串)
            const std::string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789./";
            finalSalt = "$2a$10$";
            for (int i = 0; i < 22; ++i) {
                finalSalt += chars[rand() % chars.size()];
            }
        }
        
        // 使用crypt_r进行bcrypt哈希
        struct crypt_data data;
        data.initialized = 0;
        char* result = crypt_r(password.c_str(), finalSalt.c_str(), &data);
        
        if (result) {
            return std::string(result);
        } else {
            // 哈希失败时返回空字符串
            return "";
        }
    }
   private:
    int id;
    std::string username;
    std::string password;
    std::string password_hash;
    std::string salt;
    std::string role;
    std::string created_at;
    
};  
