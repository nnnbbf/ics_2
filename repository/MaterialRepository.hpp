#pragma once
#include "../database/Database.hpp"
#include "../model/MaterialModel.hpp"
#include <vector>

class MaterialRepository
{
private:
    Database &db;

public:
    MaterialRepository(Database &database) : db(database) {}
    // 通过材料名称
    std::vector<MaterialModel> findByName(std::string materialname)
    {
        std::vector<MaterialModel> materials;
        if (materialname.empty())
            return materials;
        std::string sql = "SELECT * FROM materials WHERE card_number = '" + db.escape(materialname) + "' LIMIT 1";
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            materials.push_back(MaterialModel::fromJson(item));
        }
        return materials;
        return materials;
    }

    // 通过材料分类
    std::vector<MaterialModel> findByCategory(std::string materialcategory)
    {
        std::vector<MaterialModel> materials;
        if (materialcategory.empty())
            return materials;

        std::string sql = "SELECT * FROM materials WHERE name LIKE '%" + db.escape(materialcategory) + "%' ORDER BY id";
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            materials.push_back(MaterialModel::fromJson(item));
        }
        return materials;
    }
    std::vector<MaterialModel> findAll()
    {
        std::vector<MaterialModel> materials;
        std::string sql = "SELECT * FROM materials";
        //LOG(LogLevel::Debug) << sql;
        Json::Value result = db.query(sql);
        //LOG(LogLevel::Debug) << result.toStyledString();
        for (const auto &item : result)
        {
            materials.push_back(MaterialModel::fromJson(item));
        }
        return materials;
    }
    // 增加
    int save(MaterialModel &material)
    {
        std::string sql = "INSERT INTO materials (name, category, sub_category, unit, specification) VALUES ('" +
                          db.escape(material.getName()) + "', '" +
                          db.escape(material.getCategory()) + "', '" +
                          db.escape(material.getsub_Category()) + "', '" +
                          db.escape(material.getUnit()) + "', '" +
                          db.escape(material.getSpecification()) + "')";
        LOG(LogLevel::Debug) << sql;
        if (db.execute(sql))
        {
            LOG(LogLevel::Debug) << "Insert successful.";
            int newId = db.getLastInsertId();
            material.setId(newId);
            return newId;
        }
        // LOG(LogLevel::Error) << "Failed to execute SQL: " << sql;
        return -1;
    }
    // 更新
    bool update(const MaterialModel &material)
    {
        if (material.getId() <= 0)
        {
            LOG(LogLevel::Error) << "Invalid materials ID for update: " << material.getId();
            return false;
        }

        std::string sql = "UPDATE materials SET name = '" + db.escape(material.getName()) +
                          "', category = '" + db.escape(material.getCategory()) +
                          "', sub_category = '" + db.escape(material.getsub_Category()) +
                          "', unit = '" + db.escape(material.getUnit()) +
                          "', specification = '" + db.escape(material.getSpecification()) +
                          "' WHERE id = " + std::to_string(material.getId());
        return db.execute(sql);
    }
    // 删除
    bool remove(int id)
    {
        if (id <= 0)
        {
            LOG(LogLevel::Error) << "Invalid material ID for deletion: " << id;
            return false;
        }
        std::string sql = "DELETE FROM materials WHERE id = " + std::to_string(id);
        return db.execute(sql);
    }
};
