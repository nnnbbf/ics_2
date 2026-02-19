// warehousesrepository.h
#pragma once
#include "../database/Database.hpp"
#include "../model/WarehousesModel.hpp"
#include <vector>

class WarehousesRepository
{
private:
    Database &db;

public:
    WarehousesRepository(Database &database) : db(database) {}
    
    // 通过ID查询
    std::vector<WarehousesModel> findById(int id)
    {
        std::vector<WarehousesModel> warehouses;
        if (id <= 0)
            return warehouses;
        std::string sql = "SELECT * FROM warehouses WHERE id = " + std::to_string(id) + " LIMIT 1";
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            warehouses.push_back(WarehousesModel::fromJson(item));
        }
        return warehouses;
    }

    // 通过仓库名称查询
    std::vector<WarehousesModel> findByName(std::string name)
    {
        std::vector<WarehousesModel> warehouses;
        if (name.empty())
            return warehouses;
        std::string sql = "SELECT * FROM warehouses WHERE name = '" + db.escape(name) + "' LIMIT 1";
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            warehouses.push_back(WarehousesModel::fromJson(item));
        }
        return warehouses;
    }

    // 通过位置模糊查询
    std::vector<WarehousesModel> findByLocation(std::string location)
    {
        std::vector<WarehousesModel> warehouses;
        if (location.empty())
            return warehouses;

        std::string sql = "SELECT * FROM warehouses WHERE location LIKE '%" + 
                          db.escape(location) + "%' ORDER BY id";
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            warehouses.push_back(WarehousesModel::fromJson(item));
        }
        return warehouses;
    }

    // 查询所有仓库
    std::vector<WarehousesModel> findAll()
    {
        std::vector<WarehousesModel> warehouses;
        std::string sql = "SELECT * FROM warehouses ORDER BY id";
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            warehouses.push_back(WarehousesModel::fromJson(item));
        }
        return warehouses;
    }

    // 增加仓库
    int save(WarehousesModel &warehouse)
    {
        std::string sql = "INSERT INTO warehouses (name, location) VALUES ('" +
                          db.escape(warehouse.getName()) + "', '" +
                          db.escape(warehouse.getLocation()) + "')";
        if (db.execute(sql))
        {
            int newId = db.getLastInsertId();
            warehouse.setId(newId);
            return newId;
        }
        return -1;
    }

    // 更新仓库
    bool update(const WarehousesModel &warehouse)
    {
        if (warehouse.getId() <= 0)
        {
            return false;
        }

        std::string sql = "UPDATE warehouses SET name = '" + db.escape(warehouse.getName()) +
                          "', location = '" + db.escape(warehouse.getLocation()) +
                          "' WHERE id = " + std::to_string(warehouse.getId());
        return db.execute(sql);
    }

    // 删除仓库
    bool remove(int id)
    {
        if (id <= 0)
        {
            return false;
        }
        std::string sql = "DELETE FROM warehouses WHERE id = " + std::to_string(id);
        return db.execute(sql);
    }

    // 根据名称删除仓库
    bool removeByName(std::string name)
    {
        if (name.empty())
        {
            return false;
        }
        std::string sql = "DELETE FROM warehouses WHERE name = '" + db.escape(name) + "'";
        return db.execute(sql);
    }
};