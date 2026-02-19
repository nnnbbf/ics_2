#pragma once

#include <jsoncpp/json/json.h>
#include <string>
#include <iostream>
#include "../util/log.hpp"

class MaterialModel
{
private:
    int id;
    std::string name;          // 材料名称
    std::string category;      // 类别
    std::string sub_category;   // 子类别
    std::string unit;          // 单位
    std::string specification; // 规格
    std::string created_at; // 创建时间
public:
    MaterialModel() : id(-1)
    {
    }
    MaterialModel(int newId, const std::string &newName, const std::string &newCategory,
                  const std::string &newsub_category, const std::string &newUnit,
                  const std::string &newSpecification)
        : id(newId), name(newName), category(newCategory),
          sub_category(newsub_category), unit(newUnit), specification(newSpecification)
    {
    }
    int getId() const
    {
        return id;
    }
    void setId(int newId)
    {
        id = newId;
    }
    const std::string &getName() const
    {
        return name;
    }
    void setName(const std::string &newName)
    {
        name = newName;
    }
    const std::string &getCategory() const
    {
        return category;
    }
    void setCategory(const std::string &newCategory)
    {
        category = newCategory;
    }
    const std::string &getsub_Category() const
    {
        return sub_category;
    }
    void setsub_Category(const std::string &newsub_Category)
    {
        sub_category = newsub_Category;
    }
    const std::string &getUnit() const
    {
        return unit;
    }
    void setUnit(const std::string &newUnit)
    {
        unit = newUnit;
    }
    const std::string &getSpecification() const
    {
        return specification;
    }
    void setSpecification(const std::string &newSpecification)
    {
        specification = newSpecification;
    }
    Json::Value toJson() const
    {
        Json::Value json;
        json["id"] = std::to_string(id);
        json["name"] = name;
        json["category"] = category;
        json["sub_category"] = sub_category;
        json["unit"] = unit;
        json["specification"] = specification;
        json["created_at"] = created_at;
        return json;
    }
    static MaterialModel fromJson(const Json::Value &json)
    {
        MaterialModel material;
        if (json.isMember("id"))
            material.id = std::stoi(json["id"].asString());
        if (json.isMember("name"))
            material.name = json["name"].asString();
        if (json.isMember("category"))
            material.category = json["category"].asString();
        if (json.isMember("sub_category"))
            material.sub_category = json["sub_category"].asString();
        if (json.isMember("unit"))
            material.unit = json["unit"].asString();
        if (json.isMember("specification"))
            material.specification = json["specification"].asString();
        if (json.isMember("created_at"))
            material.created_at = json["created_at"].asString();
        return material;
    }
};
