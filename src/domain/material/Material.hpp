// domain/material/Material.hpp
#pragma once
#include <string>
#include <jsoncpp/json/json.h>
#include <stdexcept>

namespace Domain::MaterialModule {
    
    class Material {
    public:
        using IdType = int;
        
    private:
        IdType id_;
        std::string name_;
        std::string category_;
        std::string subCategory_;
        std::string unit_;
        std::string specification_;
        std::string createdAt_;
        
    public:
        // 构造函数
        Material()
            : id_(-1) {}
        
        // 工厂方法 - 创建新材料
        static Material Create(
            const std::string& name,
            const std::string& category,
            const std::string& subCategory,
            const std::string& unit,
            const std::string& specification) {
            
            // 基础验证
            if (name.empty()) {
                throw std::invalid_argument("材料名称不能为空");
            }
            if (unit.empty()) {
                throw std::invalid_argument("计量单位不能为空");
            }
            
            Material material;
            material.id_ = -1;  // 表示未持久化
            material.name_ = name;
            material.category_ = category;
            material.subCategory_ = subCategory;
            material.unit_ = unit;
            material.specification_ = specification;
            
            return material;
        }
        
        // 从JSON创建（用于数据库查询）
        static Material FromJson(const Json::Value& json) {
            Material material;
            
            if (json.isMember("id")) {
                material.id_ = std::stoi(json["id"].asString());
            }
            
            if (json.isMember("name")) {
                material.name_ = json["name"].asString();
            }
            
            if (json.isMember("category")) {
                material.category_ = json["category"].asString();
            }
            
            if (json.isMember("sub_category")) {
                material.subCategory_ = json["sub_category"].asString();
            }
            
            if (json.isMember("unit")) {
                material.unit_ = json["unit"].asString();
            }
            
            if (json.isMember("specification")) {
                material.specification_ = json["specification"].asString();
            }
            
            if (json.isMember("created_at")) {
                material.createdAt_ = json["created_at"].asString();
            }
            
            return material;
        }
        
        // 转换为JSON
        Json::Value ToJson() const {
            Json::Value json;
            json["id"] = std::to_string(id_);
            json["name"] = name_;
            json["category"] = category_;
            json["sub_category"] = subCategory_;
            json["unit"] = unit_;
            json["specification"] = specification_;
            json["created_at"] = createdAt_;
            return json;
        }
        
        // Getter方法
        IdType Id() const { return id_; }
        const std::string& Name() const { return name_; }
        const std::string& Category() const { return category_; }
        const std::string& SubCategory() const { return subCategory_; }
        const std::string& Unit() const { return unit_; }
        const std::string& Specification() const { return specification_; }
        const std::string& CreatedAt() const { return createdAt_; }
        
        // 判断是否有效
        bool IsValid() const { return !name_.empty(); }
        
        // 业务方法：更新名称
        void ChangeName(const std::string& newName) {
            if (newName.empty()) {
                throw std::invalid_argument("材料名称不能为空");
            }
            name_ = newName;
        }
        
        // 业务方法：更新分类
        void ChangeCategory(const std::string& newCategory, const std::string& newSubCategory) {
            category_ = newCategory;
            subCategory_ = newSubCategory;
        }
        
        // 业务方法：更新规格
        void ChangeSpecification(const std::string& newSpec) {
            specification_ = newSpec;
        }
        
        // 业务方法：更新单位
        void ChangeUnit(const std::string& newUnit) {
            if (newUnit.empty()) {
                throw std::invalid_argument("计量单位不能为空");
            }
            unit_ = newUnit;
        }
        
        // 设置ID（由仓储层调用）
        void SetId(IdType id) { id_ = id; }
        
        // 设置创建时间（由仓储层调用）
        void SetCreatedAt(const std::string& createdAt) { createdAt_ = createdAt; }
    };
    
} // namespace Domain::MaterialModule