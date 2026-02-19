// inventoryservice.h
#pragma once
#include "../repository/InventoryRepository.hpp"
#include "../model/InventoryDetailModel.hpp"
#include <vector>
#include <string>

class InventoryService
{
private:
    InventoryRepository repository;

public:
    InventoryService() : repository(Database::getInstance()) {}

    /**************************************************************************
     * 查询操作
     **************************************************************************/

    // 获取所有库存
    std::vector<InventoryDetailModel> getAllInventory()
    {
        return repository.findAll();
    }

    // 根据ID获取库存
    InventoryDetailModel getInventoryById(int id)
    {
        return repository.findById(id);
    }

    // 根据物料和仓库获取库存
    InventoryDetailModel getInventoryByMaterialAndWarehouse(int material_id, int warehouse_id)
    {
        return repository.findByMaterialAndWarehouse(material_id, warehouse_id);
    }

    // 根据物料名称搜索库存
    std::vector<InventoryDetailModel> getInventoryByMaterialName(const std::string &material_name)
    {
        return repository.findByMaterialName(material_name);
    }

    // 根据仓库名称获取库存
    std::vector<InventoryDetailModel> getInventoryByWarehouseName(const std::string &warehouse_name)
    {
        return repository.findByWarehouseName(warehouse_name);
    }

    // 根据物料分类获取库存
    std::vector<InventoryDetailModel> getInventoryByMaterialCategory(const std::string &material_category)
    {
        return repository.findByMaterialCategory(material_category);
    }

    // 组合条件查询库存
    std::vector<InventoryDetailModel> getInventoryByConditions(
        const std::string &warehouse_name = "",
        const std::string &material_category = "",
        const std::string &material_name_keyword = "",
        const std::string &start_date = "",
        const std::string &end_date = "")
    {
        return repository.findByConditions(warehouse_name, material_category, material_name_keyword, start_date, end_date);
    }

    // 搜索库存（全字段搜索）
    std::vector<InventoryDetailModel> searchInventory(const std::string &keyword)
    {
        if (keyword.empty())
        {
            return std::vector<InventoryDetailModel>();
        }
        return repository.search(keyword);
    }

    /**************************************************************************
     * 业务操作
     **************************************************************************/

    // 创建库存记录
    InventoryDetailModel createInventory(const InventoryDetailModel &inventory)
    {
        InventoryDetailModel newInventory = inventory;
        
        // 验证必需字段
        if (newInventory.getMaterialId() <= 0 || newInventory.getWarehouseId() <= 0)
        {
            LOG(LogLevel::Error) << "Invalid material_id or warehouse_id for inventory creation";
            return InventoryDetailModel();
        }

        // 检查是否已存在相同物料和仓库的库存记录
        if (repository.existsByMaterialAndWarehouse(newInventory.getMaterialId(), newInventory.getWarehouseId()))
        {
            LOG(LogLevel::Error) << "Inventory already exists for material_id: " 
                                << newInventory.getMaterialId() 
                                << " and warehouse_id: " << newInventory.getWarehouseId();
            return InventoryDetailModel();
        }

        // 计算总价
        newInventory.calculateTotalPrice();

        // 保存库存记录
        if (repository.save(newInventory) == -1)
        {
            LOG(LogLevel::Error) << "Failed to save new inventory for material: " 
                                << newInventory.getMaterialName();
            return InventoryDetailModel();
        }

        return newInventory;
    }

    // 更新库存记录
    bool updateInventory(const InventoryDetailModel &inventory)
    {
        if (inventory.getId() <= 0)
        {
            LOG(LogLevel::Error) << "Invalid inventory ID for update: " << inventory.getId();
            return false;
        }

        // 计算总价
        InventoryDetailModel updatedInventory = inventory;
        updatedInventory.calculateTotalPrice();

        return repository.update(updatedInventory);
    }

    // 删除库存记录
    bool deleteInventory(int id)
    {
        return repository.remove(id);
    }

    /**************************************************************************
     * 库存调整操作
     **************************************************************************/

    // 入库操作（增加库存数量）
    InventoryDetailModel inboundInventory(int inventory_id, int quantity, double unit_price = -1.0)
    {
        InventoryDetailModel inventory = repository.findById(inventory_id);
        if (!inventory.isValid())
        {
            LOG(LogLevel::Error) << "Inventory not found for inbound: " << inventory_id;
            return InventoryDetailModel();
        }

        if (quantity <= 0)
        {
            LOG(LogLevel::Error) << "Invalid quantity for inbound: " << quantity;
            return InventoryDetailModel();
        }

        // 如果有新单价，使用加权平均计算新单价
        if (unit_price >= 0)
        {
            double currentTotal = inventory.getTotalPrice();
            double inboundTotal = quantity * unit_price;
            int totalQuantity = inventory.getSystemQuantity() + quantity;
            double newAvgPrice = (currentTotal + inboundTotal) / totalQuantity;
            
            inventory.setUnitPrice(newAvgPrice);
        }

        // 更新库存数量
        if (!repository.increaseQuantity(inventory_id, quantity))
        {
            LOG(LogLevel::Error) << "Failed to increase quantity for inventory: " << inventory_id;
            return InventoryDetailModel();
        }

        // 重新计算总价并更新
        inventory.setSystemQuantity(inventory.getSystemQuantity() + quantity);
        inventory.setActualQuantity(inventory.getActualQuantity() + quantity);
        inventory.calculateTotalPrice();
        
        if (!repository.update(inventory))
        {
            LOG(LogLevel::Error) << "Failed to update inventory after inbound: " << inventory_id;
            return InventoryDetailModel();
        }

        return inventory;
    }

    // 出库操作（减少库存数量）
    InventoryDetailModel outboundInventory(int inventory_id, int quantity)
    {
        InventoryDetailModel inventory = repository.findById(inventory_id);
        if (!inventory.isValid())
        {
            LOG(LogLevel::Error) << "Inventory not found for outbound: " << inventory_id;
            return InventoryDetailModel();
        }

        if (quantity <= 0)
        {
            LOG(LogLevel::Error) << "Invalid quantity for outbound: " << quantity;
            return InventoryDetailModel();
        }

        // 检查库存是否足够
        if (inventory.getSystemQuantity() < quantity)
        {
            LOG(LogLevel::Error) << "Insufficient inventory for outbound. Current: " 
                                << inventory.getSystemQuantity() << ", Required: " << quantity;
            return InventoryDetailModel();
        }

        // 更新库存数量
        if (!repository.decreaseQuantity(inventory_id, quantity))
        {
            LOG(LogLevel::Error) << "Failed to decrease quantity for inventory: " << inventory_id;
            return InventoryDetailModel();
        }

        // 重新计算总价并更新
        inventory.setSystemQuantity(inventory.getSystemQuantity() - quantity);
        inventory.setActualQuantity(inventory.getActualQuantity() - quantity);
        inventory.calculateTotalPrice();
        
        if (!repository.update(inventory))
        {
            LOG(LogLevel::Error) << "Failed to update inventory after outbound: " << inventory_id;
            return InventoryDetailModel();
        }

        return inventory;
    }

    // 更新库存数量
    bool updateInventoryQuantity(int inventory_id, int system_quantity, int actual_quantity = -1)
    {
        if (system_quantity < 0)
        {
            LOG(LogLevel::Error) << "Invalid system quantity: " << system_quantity;
            return false;
        }

        return repository.updateQuantity(inventory_id, system_quantity, actual_quantity);
    }

    // 更新库存单价
    bool updateInventoryUnitPrice(int inventory_id, double unit_price)
    {
        if (unit_price < 0)
        {
            LOG(LogLevel::Error) << "Invalid unit price: " << unit_price;
            return false;
        }

        return repository.updateUnitPrice(inventory_id, unit_price);
    }

    /**************************************************************************
     * 库存检查操作
     **************************************************************************/

    // 检查库存是否存在
    bool existsInventory(int material_id, int warehouse_id)
    {
        return repository.existsByMaterialAndWarehouse(material_id, warehouse_id);
    }

    // 检查库存是否足够
    bool checkInventorySufficient(int inventory_id, int required_quantity)
    {
        InventoryDetailModel inventory = repository.findById(inventory_id);
        if (!inventory.isValid())
        {
            return false;
        }
        return inventory.getSystemQuantity() >= required_quantity;
    }

    // 检查低库存
    bool isLowInventory(int inventory_id, int threshold = 10)
    {
        InventoryDetailModel inventory = repository.findById(inventory_id);
        if (!inventory.isValid())
        {
            return false;
        }
        return inventory.isLowStock(threshold);
    }

    // 检查是否有库存
    bool hasInventory(int inventory_id)
    {
        InventoryDetailModel inventory = repository.findById(inventory_id);
        if (!inventory.isValid())
        {
            return false;
        }
        return inventory.hasStock();
    }

    /**************************************************************************
     * 统计操作
     **************************************************************************/

    // 获取库存总价值
    double getTotalInventoryValue()
    {
        std::vector<InventoryDetailModel> inventories = repository.findAll();
        double totalValue = 0.0;
        
        for (const auto &inventory : inventories)
        {
            totalValue += inventory.getTotalPrice();
        }
        
        return totalValue;
    }

    // 获取仓库库存总价值
    double getWarehouseInventoryValue(const std::string &warehouse_name)
    {
        std::vector<InventoryDetailModel> inventories = repository.findByWarehouseName(warehouse_name);
        double totalValue = 0.0;
        
        for (const auto &inventory : inventories)
        {
            totalValue += inventory.getTotalPrice();
        }
        
        return totalValue;
    }

    // 获取分类库存总价值
    double getCategoryInventoryValue(const std::string &material_category)
    {
        std::vector<InventoryDetailModel> inventories = repository.findByMaterialCategory(material_category);
        double totalValue = 0.0;
        
        for (const auto &inventory : inventories)
        {
            totalValue += inventory.getTotalPrice();
        }
        
        return totalValue;
    }
};