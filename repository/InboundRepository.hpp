// inboundrepository.h
#pragma once
#include "../database/Database.hpp"
#include "../model/InboundModel.hpp"
#include <vector>
#include <string>

class InboundRepository
{
private:
    Database &db;

public:
    InboundRepository(Database &database) : db(database) {}

    /**************************************************************************
     * 基本的CRUD操作
     **************************************************************************/
    int save(InboundModel &record)
    {
        // 计算批次总价
        record.calculateTotalPrice();

        std::string sql = R"(
                INSERT INTO inbound_records (
                    material_id, material_name, material_category,
                    material_sub_category, material_unit, material_specification,
                    warehouse_id, warehouse_name,  -- 删除了 warehouse_location
                    quantity, unit_price, total_price,
                    purchaser, supplier, shelf_name,
                    placement_layer, registrant, remarks
                ) VALUES ()" +
                          std::to_string(record.getMaterialId()) + ", " + "'" + db.escape(record.getMaterialName()) + "', " + "'" + db.escape(record.getMaterialCategory()) + "', " + "'" + db.escape(record.getMaterialSubCategory()) + "', " + "'" + db.escape(record.getMaterialUnit()) + "', " + "'" + db.escape(record.getMaterialSpecification()) + "', " + std::to_string(record.getWarehouseId()) + ", " + "'" + db.escape(record.getWarehouseName()) + "', " + std::to_string(record.getQuantity()) + ", " + std::to_string(record.getUnitPrice()) + ", " + std::to_string(record.getTotalPrice()) + ", " + "'" + db.escape(record.getPurchaser()) + "', " + "'" + db.escape(record.getSupplier()) + "', " + "'" + db.escape(record.getShelfName()) + "', " + "'" + db.escape(record.getPlacementLayer()) + "', " + "'" + db.escape(record.getRegistrant()) + "', " + "'" + db.escape(record.getRemarks()) + "'" + ")";

        LOG(LogLevel::Debug) << sql;

        if (db.execute(sql))
        {
            LOG(LogLevel::Debug) << "Insert successful.";
            int newId = db.getLastInsertId();
            record.setId(newId);
            return newId;
        }

        LOG(LogLevel::Error) << "Insert failed.";
        return -1;
    }

    bool update(const InboundModel &record)
    {
        if (record.getId() <= 0)
        {
            LOG(LogLevel::Error) << "Invalid inbound ID for update: " << record.getId();
            return false;
        }

        // 注意：删除了 warehouse_location 字段
        std::string sql = R"(
        UPDATE inbound_records SET
            material_id = )" +
                          std::to_string(record.getMaterialId()) + ", " +
                          "material_name = '" + db.escape(record.getMaterialName()) + "', " +
                          "material_category = '" + db.escape(record.getMaterialCategory()) + "', " +
                          "material_sub_category = '" + db.escape(record.getMaterialSubCategory()) + "', " +
                          "material_unit = '" + db.escape(record.getMaterialUnit()) + "', " +
                          "material_specification = '" + db.escape(record.getMaterialSpecification()) + "', " +
                          "warehouse_id = " + std::to_string(record.getWarehouseId()) + ", " +
                          "warehouse_name = '" + db.escape(record.getWarehouseName()) + "', " +
                          "quantity = " + std::to_string(record.getQuantity()) + ", " +
                          "unit_price = " + std::to_string(record.getUnitPrice()) + ", " +
                          "total_price = " + std::to_string(record.getTotalPrice()) + ", " +
                          "purchaser = '" + db.escape(record.getPurchaser()) + "', " +
                          "supplier = '" + db.escape(record.getSupplier()) + "', " +
                          "shelf_name = '" + db.escape(record.getShelfName()) + "', " +
                          "placement_layer = '" + db.escape(record.getPlacementLayer()) + "', " +
                          "registrant = '" + db.escape(record.getRegistrant()) + "', " +
                          "remarks = '" + db.escape(record.getRemarks()) + "' " +
                          "WHERE id = " + std::to_string(record.getId());

        LOG(LogLevel::Debug) << "Updating inbound record: " << sql;
        return db.execute(sql);
    }

    bool remove(int id)
    {
        if (id <= 0)
        {
            LOG(LogLevel::Error) << "Invalid inbound ID for deletion: " << id;
            return false;
        }

        std::string sql = "DELETE FROM inbound_records WHERE id = " + std::to_string(id);
        LOG(LogLevel::Debug) << "Deleting inbound record: " << sql;
        return db.execute(sql);
    }

    InboundModel findById(int id)
    {
        if (id <= 0)
        {
            return InboundModel();
        }

        std::string sql = R"(
            SELECT * FROM inbound_records 
            WHERE id = )" +
                          std::to_string(id) + " LIMIT 1";

        Json::Value result = db.query(sql);
        if (!result.empty())
        {
            return InboundModel::fromJson(result[0]);
        }
        return InboundModel();
    }

    std::vector<InboundModel> findAll()
    {
        std::vector<InboundModel> results;
        std::string sql = "SELECT * FROM inbound_records ORDER BY registration_date DESC";

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            results.push_back(InboundModel::fromJson(item));
        }
        return results;
    }

    /**************************************************************************
     * 条件查询入库记录
     **************************************************************************/
    std::vector<InboundModel> findByConditions(
        const std::string &warehouse_name = "",
        const std::string &material_name_keyword = "",
        const std::string &supplier = "",
        const std::string &purchaser = "",
        const std::string &registrant = "",
        const std::string &start_date = "",
        const std::string &end_date = "")
    {
        std::vector<InboundModel> results;

        std::string sql = R"(
            SELECT 
                id,
                material_id,
                material_name,
                material_category,
                material_sub_category,
                material_unit,
                material_specification,
                warehouse_id,
                warehouse_name,
                quantity,
                unit_price,
                total_price,
                purchaser,
                supplier,
                shelf_name,
                placement_layer,
                registrant,
                remarks,
                registration_date
            FROM inbound_records
            WHERE 1=1
        )";

        // 动态添加查询条件
        if (!warehouse_name.empty())
        {
            sql += " AND warehouse_name = '" + db.escape(warehouse_name) + "'";
        }

        if (!material_name_keyword.empty())
        {
            sql += " AND material_name LIKE '%" + db.escape(material_name_keyword) + "%'";
        }

        if (!supplier.empty())
        {
            sql += " AND supplier = '" + db.escape(supplier) + "'";
        }

        if (!purchaser.empty())
        {
            sql += " AND purchaser = '" + db.escape(purchaser) + "'";
        }

        if (!registrant.empty())
        {
            sql += " AND registrant = '" + db.escape(registrant) + "'"; // 修复这里
        }

        // 时间范围查询
        if (!start_date.empty())
        {
            sql += " AND DATE(registration_date) >= '" + db.escape(start_date) + "'";
        }

        if (!end_date.empty())
        {
            sql += " AND DATE(registration_date) <= '" + db.escape(end_date) + "'";
        }

        sql += " ORDER BY registration_date DESC";

        LOG(LogLevel::Debug) << "Querying inbound records: " << sql;

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            results.push_back(InboundModel::fromJson(item));
        }

        return results;
    }

    /**************************************************************************
     * 统计入库数据
     **************************************************************************/
    Json::Value getInboundStatistics(const std::string &start_date = "",
                                     const std::string &end_date = "")
    {
        std::string sql = R"(
            SELECT 
                COUNT(*) as total_count,
                SUM(quantity) as total_quantity,
                SUM(total_price) as total_amount,
                COUNT(DISTINCT material_id) as material_types,
                COUNT(DISTINCT supplier) as supplier_count
            FROM inbound_records
            WHERE 1=1
        )";

        if (!start_date.empty())
        {
            sql += " AND DATE(registration_date) >= '" + db.escape(start_date) + "'";
        }

        if (!end_date.empty())
        {
            sql += " AND DATE(registration_date) <= '" + db.escape(end_date) + "'";
        }

        Json::Value result = db.query(sql);
        if (!result.empty())
        {
            return result[0];
        }

        return Json::Value();
    }

    /**************************************************************************
     * 按月份统计入库金额
     **************************************************************************/
    Json::Value getMonthlyStatistics(int year)
    {
        std::string sql = R"(
            SELECT 
                DATE_FORMAT(registration_date, '%Y-%m') as month,
                COUNT(*) as record_count,
                SUM(quantity) as total_quantity,
                SUM(total_price) as total_amount
            FROM inbound_records
            WHERE YEAR(registration_date) = )" +
                          std::to_string(year) +
                          " GROUP BY month ORDER BY month";

        return db.query(sql);
    }

    /**************************************************************************
     * 获取供应商统计
     **************************************************************************/
    Json::Value getSupplierStatistics()
    {
        std::string sql = R"(
            SELECT 
                supplier,
                COUNT(*) as inbound_count,
                SUM(quantity) as total_quantity,
                SUM(total_price) as total_amount,
                COUNT(DISTINCT material_id) as material_types
            FROM inbound_records
            GROUP BY supplier
            ORDER BY total_amount DESC
        )";

        return db.query(sql);
    }

    /**************************************************************************
     * 检查入库记录是否存在
     **************************************************************************/
    bool exists(int id)
    {
        if (id <= 0)
            return false;

        std::string sql = "SELECT COUNT(*) as count FROM inbound_records WHERE id = " +
                          std::to_string(id);

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

    /**************************************************************************
     * 根据物料ID查找入库记录
     **************************************************************************/
    std::vector<InboundModel> findByMaterialId(int material_id)
    {
        std::vector<InboundModel> results;
        if (material_id <= 0)
            return results;

        std::string sql = "SELECT * FROM inbound_records WHERE material_id = " +
                          std::to_string(material_id) +
                          " ORDER BY registration_date DESC";

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            results.push_back(InboundModel::fromJson(item));
        }
        return results;
    }

    /**************************************************************************
     * 根据仓库ID查找入库记录
     **************************************************************************/
    std::vector<InboundModel> findByWarehouseId(int warehouse_id)
    {
        std::vector<InboundModel> results;
        if (warehouse_id <= 0)
            return results;

        std::string sql = "SELECT * FROM inbound_records WHERE warehouse_id = " +
                          std::to_string(warehouse_id) +
                          " ORDER BY registration_date DESC";

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            results.push_back(InboundModel::fromJson(item));
        }
        return results;
    }
};