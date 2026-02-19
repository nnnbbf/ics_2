#pragma once

#include "../database/Database.hpp"
#include "../model/OutboundModel.hpp"
#include <vector>
#include <string>

class OutboundRepository
{
private:
    Database &db;

public:
    OutboundRepository(Database &database) : db(database) {}

    /**************************************************************************
     * 基本的CRUD操作
     **************************************************************************/
    int save(OutboundModel &record)
    {
        // 计算批次总价
        record.calculateTotalPrice();

        std::string sql = R"(
                        INSERT INTO outbound_records (
                            person_id, person_name, person_card_number, person_department, person_balance_at_outbound,
                            material_id, material_name, material_category, material_sub_category, material_unit, material_specification,
                            warehouse_id, warehouse_name, shelf_name, placement_layer,
                            unit_price_at_outbound, quantity, total_price,
                            stock_before_outbound, stock_after_outbound,
                            operator_name
                        ) VALUES ()" +
                          std::to_string(record.getPersonId()) + ", " +
                          "'" + db.escape(record.getPersonName()) + "', " +
                          "'" + db.escape(record.getPersonCardNumber()) + "', " +
                          "'" + db.escape(record.getPersonDepartment()) + "', " +
                          std::to_string(record.getPersonBalanceAtOutbound()) + ", " +
                          std::to_string(record.getMaterialId()) + ", " +
                          "'" + db.escape(record.getMaterialName()) + "', " +
                          "'" + db.escape(record.getMaterialCategory()) + "', " +
                          "'" + db.escape(record.getMaterialSubCategory()) + "', " +
                          "'" + db.escape(record.getMaterialUnit()) + "', " +
                          "'" + db.escape(record.getMaterialSpecification()) + "', " +
                          std::to_string(record.getWarehouseId()) + ", " +
                          "'" + db.escape(record.getWarehouseName()) + "', " +
                          "'" + db.escape(record.getShelfName()) + "', " +
                          "'" + db.escape(record.getPlacementLayer()) + "', " +
                          std::to_string(record.getUnitPriceAtOutbound()) + ", " +
                          std::to_string(record.getQuantity()) + ", " +
                          std::to_string(record.getTotalPrice()) + ", " +
                          std::to_string(record.getStockBeforeOutbound()) + ", " +
                          std::to_string(record.getStockAfterOutbound()) + ", " +
                          "'" + db.escape(record.getOperatorName()) + "'" +
                          ")";

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

    bool update(const OutboundModel &record)
    {
        if (record.getId() <= 0)
        {
            LOG(LogLevel::Error) << "Invalid outbound ID for update: " << record.getId();
            return false;
        }

        std::string sql = R"(
                        UPDATE outbound_records SET
                            person_id = )" +
                          std::to_string(record.getPersonId()) + ", " +
                          "person_name = '" + db.escape(record.getPersonName()) + "', " +
                          "person_card_number = '" + db.escape(record.getPersonCardNumber()) + "', " +
                          "person_department = '" + db.escape(record.getPersonDepartment()) + "', " +
                          "person_balance_at_outbound = " + std::to_string(record.getPersonBalanceAtOutbound()) + ", " +
                          "material_id = " + std::to_string(record.getMaterialId()) + ", " +
                          "material_name = '" + db.escape(record.getMaterialName()) + "', " +
                          "material_category = '" + db.escape(record.getMaterialCategory()) + "', " +
                          "material_sub_category = '" + db.escape(record.getMaterialSubCategory()) + "', " +
                          "material_unit = '" + db.escape(record.getMaterialUnit()) + "', " +
                          "material_specification = '" + db.escape(record.getMaterialSpecification()) + "', " +
                          "warehouse_id = " + std::to_string(record.getWarehouseId()) + ", " +
                          "warehouse_name = '" + db.escape(record.getWarehouseName()) + "', " +
                          "shelf_name = '" + db.escape(record.getShelfName()) + "', " +
                          "placement_layer = '" + db.escape(record.getPlacementLayer()) + "', " +
                          "unit_price_at_outbound = " + std::to_string(record.getUnitPriceAtOutbound()) + ", " +
                          "quantity = " + std::to_string(record.getQuantity()) + ", " +
                          "total_price = " + std::to_string(record.getTotalPrice()) + ", " +
                          "stock_before_outbound = " + std::to_string(record.getStockBeforeOutbound()) + ", " +
                          "stock_after_outbound = " + std::to_string(record.getStockAfterOutbound()) + ", " +
                          "operator_name = '" + db.escape(record.getOperatorName()) + "' " +
                          "WHERE id = " + std::to_string(record.getId());

        LOG(LogLevel::Debug) << "Updating outbound record: " << sql;
        return db.execute(sql);
    }

    bool remove(int id)
    {
        if (id <= 0)
        {
            LOG(LogLevel::Error) << "Invalid outbound ID for deletion: " << id;
            return false;
        }

        std::string sql = "DELETE FROM outbound_records WHERE id = " + std::to_string(id);
        LOG(LogLevel::Debug) << "Deleting outbound record: " << sql;
        return db.execute(sql);
    }

    OutboundModel findById(int id)
    {
        if (id <= 0)
        {
            return OutboundModel();
        }

        std::string sql = R"(
            SELECT * FROM outbound_records 
            WHERE id = )" +
                          std::to_string(id) + " LIMIT 1";

        Json::Value result = db.query(sql);
        if (!result.empty())
        {
            return OutboundModel::fromJson(result[0]);
        }
        return OutboundModel();
    }

    std::vector<OutboundModel> findAll()
    {
        std::vector<OutboundModel> results;
        std::string sql = "SELECT * FROM outbound_records ORDER BY outbound_date DESC";

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            results.push_back(OutboundModel::fromJson(item));
        }
        return results;
    }

    /**************************************************************************
     * 条件查询出库记录
     **************************************************************************/
    std::vector<OutboundModel> findByConditions(
        const std::string &warehouse_name = "",
        const std::string &material_name_keyword = "",
        const std::string &person_name = "",
        const std::string &person_card_number = "",
        const std::string &person_department = "",
        const std::string &operator_name = "",
        const std::string &start_date = "",
        const std::string &end_date = "")
    {
        std::vector<OutboundModel> results;

        std::string sql = R"(
            SELECT 
                id,
                person_id,
                person_name,
                person_card_number,
                person_department,
                person_balance_at_outbound,
                material_id,
                material_name,
                material_category,
                material_sub_category,
                material_unit,
                material_specification,
                warehouse_id,
                warehouse_name,
                shelf_name,
                placement_layer,
                unit_price_at_outbound,
                quantity,
                total_price,
                stock_before_outbound,
                stock_after_outbound,
                operator_name,
                outbound_date
            FROM outbound_records
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

        if (!person_name.empty())
        {
            sql += " AND person_name = '" + db.escape(person_name) + "'";
        }

        if (!person_card_number.empty())
        {
            sql += " AND person_card_number = '" + db.escape(person_card_number) + "'";
        }

        if (!person_department.empty())
        {
            sql += " AND person_department = '" + db.escape(person_department) + "'";
        }

        if (!operator_name.empty())
        {
            sql += " AND operator_name = '" + db.escape(operator_name) + "'";
        }

        // 时间范围查询
        if (!start_date.empty())
        {
            sql += " AND DATE(outbound_date) >= '" + db.escape(start_date) + "'";
        }

        if (!end_date.empty())
        {
            sql += " AND DATE(outbound_date) <= '" + db.escape(end_date) + "'";
        }

        sql += " ORDER BY outbound_date DESC";

        LOG(LogLevel::Debug) << "Querying outbound records: " << sql;

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            results.push_back(OutboundModel::fromJson(item));
        }

        return results;
    }

    /**************************************************************************
     * 统计出库数据
     **************************************************************************/
    Json::Value getOutboundStatistics(const std::string &start_date = "",
                                      const std::string &end_date = "")
    {
        std::string sql = R"(
            SELECT 
                COUNT(*) as total_count,
                SUM(quantity) as total_quantity,
                SUM(total_price) as total_amount,
                COUNT(DISTINCT material_id) as material_types,
                COUNT(DISTINCT person_id) as person_count,
                COUNT(DISTINCT operator_name) as operator_count
            FROM outbound_records
            WHERE 1=1
        )";

        if (!start_date.empty())
        {
            sql += " AND DATE(outbound_date) >= '" + db.escape(start_date) + "'";
        }

        if (!end_date.empty())
        {
            sql += " AND DATE(outbound_date) <= '" + db.escape(end_date) + "'";
        }

        Json::Value result = db.query(sql);
        if (!result.empty())
        {
            return result[0];
        }

        return Json::Value();
    }

    /**************************************************************************
     * 按月份统计出库金额
     **************************************************************************/
    Json::Value getMonthlyStatistics(int year)
    {
        std::string sql = R"(
            SELECT 
                DATE_FORMAT(outbound_date, '%Y-%m') as month,
                COUNT(*) as record_count,
                SUM(quantity) as total_quantity,
                SUM(total_price) as total_amount,
                COUNT(DISTINCT material_id) as material_types
            FROM outbound_records
            WHERE YEAR(outbound_date) = )" +
                          std::to_string(year) +
                          " GROUP BY month ORDER BY month";

        return db.query(sql);
    }

    /**************************************************************************
     * 获取人员出库统计
     **************************************************************************/
    Json::Value getPersonStatistics()
    {
        std::string sql = R"(
            SELECT 
                person_name,
                person_card_number,
                person_department,
                COUNT(*) as outbound_count,
                SUM(quantity) as total_quantity,
                SUM(total_price) as total_amount,
                AVG(person_balance_at_outbound) as avg_balance
            FROM outbound_records
            GROUP BY person_id, person_name, person_card_number
            ORDER BY total_amount DESC
        )";

        return db.query(sql);
    }

    /**************************************************************************
     * 获取物料出库统计
     **************************************************************************/
    Json::Value getMaterialStatistics()
    {
        std::string sql = R"(
            SELECT 
                material_name,
                material_category,
                COUNT(*) as outbound_count,
                SUM(quantity) as total_quantity,
                SUM(total_price) as total_amount,
                AVG(unit_price_at_outbound) as avg_price
            FROM outbound_records
            GROUP BY material_id, material_name
            ORDER BY total_quantity DESC
        )";

        return db.query(sql);
    }

    /**************************************************************************
     * 检查出库记录是否存在
     **************************************************************************/
    bool exists(int id)
    {
        if (id <= 0)
            return false;

        std::string sql = "SELECT COUNT(*) as count FROM outbound_records WHERE id = " +
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
     * 根据物料ID查找出库记录
     **************************************************************************/
    std::vector<OutboundModel> findByMaterialId(int material_id)
    {
        std::vector<OutboundModel> results;
        if (material_id <= 0)
            return results;

        std::string sql = "SELECT * FROM outbound_records WHERE material_id = " +
                          std::to_string(material_id) +
                          " ORDER BY outbound_date DESC";

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            results.push_back(OutboundModel::fromJson(item));
        }
        return results;
    }

    /**************************************************************************
     * 根据仓库ID查找出库记录
     **************************************************************************/
    std::vector<OutboundModel> findByWarehouseId(int warehouse_id)
    {
        std::vector<OutboundModel> results;
        if (warehouse_id <= 0)
            return results;

        std::string sql = "SELECT * FROM outbound_records WHERE warehouse_id = " +
                          std::to_string(warehouse_id) +
                          " ORDER BY outbound_date DESC";

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            results.push_back(OutboundModel::fromJson(item));
        }
        return results;
    }

    /**************************************************************************
     * 根据人员ID查找出库记录
     **************************************************************************/
    std::vector<OutboundModel> findByPersonId(int person_id)
    {
        std::vector<OutboundModel> results;
        if (person_id <= 0)
            return results;

        std::string sql = "SELECT * FROM outbound_records WHERE person_id = " +
                          std::to_string(person_id) +
                          " ORDER BY outbound_date DESC";

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            results.push_back(OutboundModel::fromJson(item));
        }
        return results;
    }

    /**************************************************************************
     * 根据人员卡号查找出库记录
     **************************************************************************/
    std::vector<OutboundModel> findByPersonCardNumber(const std::string &card_number)
    {
        std::vector<OutboundModel> results;
        if (card_number.empty())
            return results;

        std::string sql = "SELECT * FROM outbound_records WHERE person_card_number = '" +
                          db.escape(card_number) +
                          "' ORDER BY outbound_date DESC";

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            results.push_back(OutboundModel::fromJson(item));
        }
        return results;
    }

    /**************************************************************************
     * 根据日期范围查找出库记录
     **************************************************************************/
    std::vector<OutboundModel> findByDateRange(const std::string &start_date, const std::string &end_date)
    {
        std::vector<OutboundModel> results;
        
        std::string sql = "SELECT * FROM outbound_records WHERE 1=1";
        
        if (!start_date.empty())
        {
            sql += " AND DATE(outbound_date) >= '" + db.escape(start_date) + "'";
        }
        
        if (!end_date.empty())
        {
            sql += " AND DATE(outbound_date) <= '" + db.escape(end_date) + "'";
        }
        
        sql += " ORDER BY outbound_date DESC";
        
        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            results.push_back(OutboundModel::fromJson(item));
        }
        return results;
    }

    /**************************************************************************
     * 搜索出库记录（全字段模糊搜索）
     **************************************************************************/
    std::vector<OutboundModel> search(const std::string &keyword)
    {
        std::vector<OutboundModel> results;
        if (keyword.empty())
            return results;

        std::string sql = "SELECT * FROM outbound_records WHERE " +
                          std::string("person_name LIKE '%") + db.escape(keyword) + "%' " +
                          "OR person_card_number LIKE '%" + db.escape(keyword) + "%' " +
                          "OR person_department LIKE '%" + db.escape(keyword) + "%' " +
                          "OR material_name LIKE '%" + db.escape(keyword) + "%' " +
                          "OR material_category LIKE '%" + db.escape(keyword) + "%' " +
                          "OR material_specification LIKE '%" + db.escape(keyword) + "%' " +
                          "OR warehouse_name LIKE '%" + db.escape(keyword) + "%' " +
                          "OR shelf_name LIKE '%" + db.escape(keyword) + "%' " +
                          "OR operator_name LIKE '%" + db.escape(keyword) + "%' " +
                          "OR remarks LIKE '%" + db.escape(keyword) + "%' " +
                          "ORDER BY outbound_date DESC";

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            results.push_back(OutboundModel::fromJson(item));
        }
        return results;
    }

    /**************************************************************************
     * 获取最新出库记录
     **************************************************************************/
    std::vector<OutboundModel> findRecent(int limit = 10)
    {
        std::vector<OutboundModel> results;
        if (limit <= 0)
            return results;

        std::string sql = "SELECT * FROM outbound_records ORDER BY outbound_date DESC LIMIT " +
                          std::to_string(limit);

        Json::Value result = db.query(sql);
        for (const auto &item : result)
        {
            results.push_back(OutboundModel::fromJson(item));
        }
        return results;
    }
};