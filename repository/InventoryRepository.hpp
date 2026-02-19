// repository/InventoryRepository.hpp
#pragma once
#include "../database/Database.hpp"
#include "../model/InventoryDetailModel.hpp"
#include <vector>

class InventoryRepository
{
private:
    Database &db;

public:
    InventoryRepository(Database &database) : db(database) {}

    /**************************************************************************
     * 基本CRUD操作
     **************************************************************************/

    // 根据ID查找库存记录
    InventoryDetailModel findById(int id)
    {
        InventoryDetailModel inventory;
        if (id <= 0)
            return inventory; // 验证ID有效性

        std::string sql = "SELECT * FROM inventory WHERE id = " + std::to_string(id);
        Json::Value result = db.query(sql);
        if (result.size() > 0)
        {
            inventory = InventoryDetailModel::fromJson(result[0]);
        }
        return inventory;
    }

    // 查找所有库存记录
    std::vector<InventoryDetailModel> findAll()
    {
        std::vector<InventoryDetailModel> inventories;
        std::string sql = "SELECT * FROM inventory ORDER BY updated_at DESC";
        Json::Value result = db.query(sql);

        for (const auto &item : result)
        {
            try
            {
                auto inventory = InventoryDetailModel::fromJson(item);
                inventories.push_back(inventory);
            }
            catch (const std::exception &e)
            {
                LOG(LogLevel::Error) << "Error parsing inventory record: " << e.what();
            }
        }
        return inventories;
    }

    // 新增库存记录
    // 新增库存记录
    int save(InventoryDetailModel &inventory)
    {
        // 计算总价
        inventory.calculateTotalPrice();

        std::string sql = "INSERT INTO inventory ("
                          "material_id, material_name, material_category, "
                          "material_sub_category, material_unit, material_specification, "
                          "warehouse_id, warehouse_name, "
                          "system_quantity, actual_quantity, shelf_name, "
                          "placement_layer, unit_price, total_price"
                          ") VALUES (" +
                          std::to_string(inventory.getMaterialId()) + ", '" +
                          db.escape(inventory.getMaterialName()) + "', '" +
                          db.escape(inventory.getMaterialCategory()) + "', '" +
                          db.escape(inventory.getMaterialSubCategory()) + "', '" +
                          db.escape(inventory.getMaterialUnit()) + "', '" +
                          db.escape(inventory.getMaterialSpecification()) + "', " +
                          std::to_string(inventory.getWarehouseId()) + ", '" +
                          db.escape(inventory.getWarehouseName()) + "', " +
                          std::to_string(inventory.getSystemQuantity()) + ", " +
                          std::to_string(inventory.getActualQuantity()) + ", '" +
                          db.escape(inventory.getShelfName()) + "', '" +
                          db.escape(inventory.getPlacementLayer()) + "', " +
                          std::to_string(inventory.getUnitPrice()) + ", " +
                          std::to_string(inventory.getTotalPrice()) + ")";

        LOG(LogLevel::Debug) << "Executing SQL: " << sql;

        if (db.execute(sql))
        {
            int newId = db.getLastInsertId();
            inventory.setId(newId);
            return newId;
        }
        return -1;
    }

    // 更新库存记录
    // 更新库存记录
    bool update(const InventoryDetailModel &inventory)
    {
        if (inventory.getId() <= 0)
        {
            LOG(LogLevel::Error) << "Invalid inventory ID for update: " << inventory.getId();
            return false;
        }

        std::string sql = "UPDATE inventory SET " +
                          std::string("material_id = ") + std::to_string(inventory.getMaterialId()) +
                          ", material_name = '" + db.escape(inventory.getMaterialName()) +
                          "', material_category = '" + db.escape(inventory.getMaterialCategory()) +
                          "', material_sub_category = '" + db.escape(inventory.getMaterialSubCategory()) +
                          "', material_unit = '" + db.escape(inventory.getMaterialUnit()) +
                          "', material_specification = '" + db.escape(inventory.getMaterialSpecification()) +
                          "', warehouse_id = " + std::to_string(inventory.getWarehouseId()) +
                          ", warehouse_name = '" + db.escape(inventory.getWarehouseName()) +
                          "', system_quantity = " + std::to_string(inventory.getSystemQuantity()) +
                          ", actual_quantity = " + std::to_string(inventory.getActualQuantity()) +
                          ", shelf_name = '" + db.escape(inventory.getShelfName()) +
                          "', placement_layer = '" + db.escape(inventory.getPlacementLayer()) + "', " + // 修改：加单引号
                          "unit_price = " + std::to_string(inventory.getUnitPrice()) +
                          ", total_price = " + std::to_string(inventory.getTotalPrice()) +
                          " WHERE id = " + std::to_string(inventory.getId());

        LOG(LogLevel::Debug) << sql;
        return db.execute(sql);
    }

    // 删除库存记录
    bool remove(int id)
    {
        if (id <= 0)
        {
            LOG(LogLevel::Error) << "Invalid inventory ID for deletion: " << id;
            return false;
        }
        std::string sql = "DELETE FROM inventory WHERE id = " + std::to_string(id);
        return db.execute(sql);
    }

    /**************************************************************************
     * 特定查询方法
     **************************************************************************/

    // 根据物料ID和仓库ID查找库存记录
    InventoryDetailModel findByMaterialAndWarehouse(int material_id, int warehouse_id)
    {
        InventoryDetailModel inventory;
        if (material_id <= 0 || warehouse_id <= 0)
            return inventory;

        std::string sql = "SELECT * FROM inventory WHERE material_id = " +
                          std::to_string(material_id) +
                          " AND warehouse_id = " + std::to_string(warehouse_id) +
                          " LIMIT 1";
        Json::Value result = db.query(sql);
        if (result.size() > 0)
        {
            inventory = InventoryDetailModel::fromJson(result[0]);
        }
        return inventory;
    }

    // 根据物料名称查找
    std::vector<InventoryDetailModel> findByMaterialName(const std::string &material_name)
    {
        std::vector<InventoryDetailModel> inventories;
        if (material_name.empty())
            return inventories;

        std::string sql = "SELECT * FROM inventory WHERE material_name LIKE '%" +
                          db.escape(material_name) + "%' ORDER BY material_name";
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            inventories.push_back(InventoryDetailModel::fromJson(item));
        }
        return inventories;
    }

    // 根据仓库名称查找
    std::vector<InventoryDetailModel> findByWarehouseName(const std::string &warehouse_name)
    {
        std::vector<InventoryDetailModel> inventories;
        if (warehouse_name.empty())
            return inventories;

        std::string sql = "SELECT * FROM inventory WHERE warehouse_name = '" +
                          db.escape(warehouse_name) + "' ORDER BY material_name";
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            inventories.push_back(InventoryDetailModel::fromJson(item));
        }
        return inventories;
    }

    // 根据物料分类查找
    std::vector<InventoryDetailModel> findByMaterialCategory(const std::string &material_category)
    {
        std::vector<InventoryDetailModel> inventories;
        if (material_category.empty())
            return inventories;

        std::string sql = "SELECT * FROM inventory WHERE material_category = '" +
                          db.escape(material_category) + "' ORDER BY material_name";
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            inventories.push_back(InventoryDetailModel::fromJson(item));
        }
        return inventories;
    }

    /**************************************************************************
     * 组合查询方法
     **************************************************************************/

    std::vector<InventoryDetailModel> findByConditions(
        const std::string &warehouse_name = "",
        const std::string &material_category = "",
        const std::string &material_name_keyword = "",
        const std::string &start_date = "",
        const std::string &end_date = "")
    {
        std::vector<InventoryDetailModel> inventories;

        std::string sql = "SELECT * FROM inventory WHERE 1=1";

        if (!warehouse_name.empty())
        {
            sql += " AND warehouse_name = '" + db.escape(warehouse_name) + "'";
        }

        if (!material_category.empty())
        {
            sql += " AND material_category = '" + db.escape(material_category) + "'";
        }

        if (!material_name_keyword.empty())
        {
            sql += " AND material_name LIKE '%" + db.escape(material_name_keyword) + "%'";
        }

        if (!start_date.empty())
        {
            sql += " AND DATE(created_at) >= '" + db.escape(start_date) + "'";
        }

        if (!end_date.empty())
        {
            sql += " AND DATE(created_at) <= '" + db.escape(end_date) + "'";
        }

        sql += " ORDER BY created_at DESC";

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            inventories.push_back(InventoryDetailModel::fromJson(item));
        }
        return inventories;
    }

    /**************************************************************************
     * 业务操作
     **************************************************************************/

    // 更新库存数量
    bool updateQuantity(int id, int system_quantity, int actual_quantity = -1)
    {
        if (id <= 0 || system_quantity < 0)
            return false;

        std::string sql = "UPDATE inventory SET system_quantity = " +
                          std::to_string(system_quantity);

        if (actual_quantity >= 0)
        {
            sql += ", actual_quantity = " + std::to_string(actual_quantity);
        }

        sql += " WHERE id = " + std::to_string(id);
        return db.execute(sql);
    }

    // 更新单价和重新计算总价
    bool updateUnitPrice(int id, double unit_price)
    {
        if (id <= 0 || unit_price < 0)
            return false;

        std::string sql = "UPDATE inventory SET unit_price = " +
                          std::to_string(unit_price) +
                          " WHERE id = " + std::to_string(id);
        return db.execute(sql);
    }
    // 更新总价-用于出库
    bool updateTotalPrice(int id, double total_price)
    {
        if (id <= 0)
            return false;

        std::string sql = "UPDATE inventory SET total_price =  total_price - " +
                          std::to_string(total_price) +
                          " WHERE id = " + std::to_string(id);
        return db.execute(sql);
    }
    // 增加库存数量
    bool increaseQuantity(int id, int quantity)
    {
        if (id <= 0 || quantity <= 0)
            return false;

        std::string sql = "UPDATE inventory SET "
                          "system_quantity = system_quantity + " +
                          std::to_string(quantity) +
                          ", actual_quantity = actual_quantity + " + std::to_string(quantity) +
                          " WHERE id = " + std::to_string(id);
        return db.execute(sql);
    }

    // 减少库存数量
    bool decreaseQuantity(int id, int quantity)
    {
        if (id <= 0 || quantity <= 0)
            return false;

        // 先检查库存是否足够
        InventoryDetailModel inventory = findById(id);
        if (!inventory.isValid() || inventory.getSystemQuantity() < quantity)
            return false;

        std::string sql = "UPDATE inventory SET "
                          "system_quantity = system_quantity - " +
                          std::to_string(quantity) +
                          ", actual_quantity = actual_quantity - " + std::to_string(quantity) +
                          " WHERE id = " + std::to_string(id);
        return db.execute(sql);
    }

    // 检查物料在仓库中是否存在
    bool existsByMaterialAndWarehouse(int material_id, int warehouse_id)
    {
        if (material_id <= 0 || warehouse_id <= 0)
            return false;

        std::string sql = "SELECT COUNT(*) AS count FROM inventory WHERE material_id = " +
                          std::to_string(material_id) +
                          " AND warehouse_id = " + std::to_string(warehouse_id);

        Json::Value result = db.query(sql);
        if (result.size() > 0 && result[0].isMember("count"))
        {
            Json::Value countValue = result[0]["count"];
            int count = 0;

            if (countValue.isInt())
            {
                count = countValue.asInt();
            }
            else if (countValue.isString())
            {
                try
                {
                    count = std::stoi(countValue.asString());
                }
                catch (...)
                {
                    count = 0;
                }
            }
            else if (countValue.isNumeric())
            {
                count = countValue.asInt();
            }

            return count > 0;
        }
        return false;
    }

    // 搜索功能（全字段搜索）
    std::vector<InventoryDetailModel> search(const std::string &keyword)
    {
        std::vector<InventoryDetailModel> inventories;
        if (keyword.empty())
            return inventories;

        std::string sql = "SELECT * FROM inventory WHERE " +
                          std::string("material_name LIKE '%") + db.escape(keyword) + "%' " +
                          "OR material_category LIKE '%" + db.escape(keyword) + "%' " +
                          "OR material_sub_category LIKE '%" + db.escape(keyword) + "%' " +
                          "OR material_specification LIKE '%" + db.escape(keyword) + "%' " +
                          "OR warehouse_name LIKE '%" + db.escape(keyword) + "%' " +
                          "OR warehouse_location LIKE '%" + db.escape(keyword) + "%' " +
                          "OR shelf_name LIKE '%" + db.escape(keyword) + "%' " +
                          "OR remarks LIKE '%" + db.escape(keyword) + "%' " +
                          "ORDER BY updated_at DESC";

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            inventories.push_back(InventoryDetailModel::fromJson(item));
        }
        return inventories;
    }
};