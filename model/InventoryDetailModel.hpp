// inventorydetailmodel.h
#ifndef INVENTORYDETAILMODEL_H
#define INVENTORYDETAILMODEL_H

#include <string>
#include <jsoncpp/json/json.h>

class InventoryDetailModel
{
private:
    int id;                             // 库存记录ID
    int material_id;                    // 物料ID
    std::string material_name;          // 物料名称
    std::string material_category;      // 物料分类
    std::string material_sub_category;  // 物料二级分类
    std::string material_unit;          // 物料单位
    std::string material_specification; // 物料规格

    int warehouse_id;               // 仓库ID
    std::string warehouse_name;     // 仓库名称

    int system_quantity;    // 系统数量
    int actual_quantity;    // 实际数量
    std::string shelf_name; // 货架名称
    std::string placement_layer;    // 放置层数
    double unit_price;      // 单价
    double total_price;     // 总价（普通列，由后端计算存储）

    std::string created_at; // 创建时间
    std::string updated_at; // 更新时间

public:
    InventoryDetailModel() : id(0), material_id(0), warehouse_id(0),
                             system_quantity(0), actual_quantity(0),
                             unit_price(0.0),total_price(0.0) {}

    static InventoryDetailModel fromJson(const Json::Value &json)
    {
        InventoryDetailModel model;

        // 所有数字字段都需要从字符串转换
        if (json.isMember("id") && !json["id"].asString().empty())
            model.id = std::stoi(json["id"].asString());

        if (json.isMember("material_id") && !json["material_id"].asString().empty())
            model.material_id = std::stoi(json["material_id"].asString());

        if (json.isMember("warehouse_id") && !json["warehouse_id"].asString().empty())
            model.warehouse_id = std::stoi(json["warehouse_id"].asString());

        // 字符串字段直接赋值
        if (json.isMember("material_name"))
            model.material_name = json["material_name"].asString();

        if (json.isMember("warehouse_name"))
            model.warehouse_name = json["warehouse_name"].asString();

        if (json.isMember("material_category"))
            model.material_category = json["material_category"].asString();

        if (json.isMember("material_sub_category"))
            model.material_sub_category = json["material_sub_category"].asString();

        if (json.isMember("material_unit"))
            model.material_unit = json["material_unit"].asString();

        if (json.isMember("material_specification"))
            model.material_specification = json["material_specification"].asString();

        if (json.isMember("shelf_name"))
            model.shelf_name = json["shelf_name"].asString();

        // 数字字段从字符串转换
        if (json.isMember("placement_layer"))
            model.placement_layer = json["placement_layer"].asString();

        if (json.isMember("system_quantity") && !json["system_quantity"].asString().empty())
            model.system_quantity = std::stoi(json["system_quantity"].asString());

        if (json.isMember("actual_quantity") && !json["actual_quantity"].asString().empty())
            model.actual_quantity = std::stoi(json["actual_quantity"].asString());

        if (json.isMember("unit_price") && !json["unit_price"].asString().empty())
            model.unit_price = std::stod(json["unit_price"].asString());

        // 从JSON解析total_price字段
        if (json.isMember("total_price") && !json["total_price"].asString().empty())
            model.total_price = std::stod(json["total_price"].asString());
        else
            model.calculateTotalPrice();  // 如果没有提供，就自动计算


        if (json.isMember("created_at"))
            model.created_at = json["created_at"].asString();

        if (json.isMember("updated_at"))
            model.updated_at = json["updated_at"].asString();

        return model;
    }

    Json::Value toJson() const
    {
        Json::Value json;

        json["id"] = std::to_string(id);
        json["material_id"] = std::to_string(material_id);
        json["material_name"] = material_name;
        json["warehouse_id"] = std::to_string(warehouse_id);
        json["warehouse_name"] = warehouse_name;
        json["material_category"] = material_category;
        json["shelf_name"] = shelf_name;
        json["placement_layer"] = placement_layer;
        json["material_unit"] = material_unit;
        json["material_sub_category"] = material_sub_category;
        json["material_specification"] = material_specification;
        json["system_quantity"] = std::to_string(system_quantity);
        json["actual_quantity"] = std::to_string(actual_quantity);
        json["unit_price"] = std::to_string(unit_price);
        json["total_price"] = std::to_string(total_price);  // 存储后端计算的总价
        json["created_at"] = created_at;
        json["updated_at"] = updated_at;

        return json;
    }

    /**************************************************************************
     * Getter方法
     **************************************************************************/
    int getId() const { return id; }
    int getMaterialId() const { return material_id; }
    const std::string &getMaterialName() const { return material_name; }
    const std::string &getWarehouseName() const { return warehouse_name; }
    const std::string &getMaterialCategory() const { return material_category; }
    const std::string &getShelfName() const { return shelf_name; }
    const std::string getPlacementLayer() const { return placement_layer; }
    const std::string &getMaterialUnit() const { return material_unit; }
    const std::string &getMaterialSubCategory() const { return material_sub_category; }
    const std::string &getMaterialSpecification() const { return material_specification; }
    int getSystemQuantity() const { return system_quantity; }
    int getActualQuantity() const { return actual_quantity; }
    double getUnitPrice() const { return unit_price; }
    double getTotalPrice() const { return total_price; }
    int getWarehouseId() const { return warehouse_id; }
    const std::string &getCreatedAt() const { return created_at; }
    const std::string &getUpdatedAt() const { return updated_at; }

    /**************************************************************************
     * Setter方法
     **************************************************************************/
    void setId(int newId) { id = newId; }
    void setMaterialId(int newMaterialId) { material_id = newMaterialId; }
    void setMaterialName(const std::string &newMaterialName) { material_name = newMaterialName; }
    void setWarehouseName(const std::string &newWarehouseName) { warehouse_name = newWarehouseName; }
    void setMaterialCategory(const std::string &newMaterialCategory) { material_category = newMaterialCategory; }
    void setShelfName(const std::string &newShelfName) { shelf_name = newShelfName; }
    void setPlacementLayer(const std::string newPlacementLayer) { placement_layer = newPlacementLayer; }
    void setMaterialUnit(const std::string &newMaterialUnit) { material_unit = newMaterialUnit; }
    void setMaterialSubCategory(const std::string &newMaterialSubCategory) { material_sub_category = newMaterialSubCategory; }
    void setMaterialSpecification(const std::string &newMaterialSpecification) { material_specification = newMaterialSpecification; }
    
    void setSystemQuantity(int newSystemQuantity)
    {
        system_quantity = newSystemQuantity;
        // 注意：这里不自动计算total_price，因为需要业务逻辑控制
    }
    
    void setActualQuantity(int newActualQuantity) { actual_quantity = newActualQuantity; }
    
    void setUnitPrice(double newUnitPrice)
    {
        unit_price = newUnitPrice;
        // 注意：这里不自动计算total_price，因为需要业务逻辑控制
    }
    
    void setTotalPrice(double newTotalPrice) { total_price = newTotalPrice; }  // 新增的setter
    
    void setCreatedAt(const std::string &newCreatedAt) { created_at = newCreatedAt; }
    void setUpdatedAt(const std::string &newUpdatedAt) { updated_at = newUpdatedAt; }
    void setWarehouseId(int newWarehouseId) { warehouse_id = newWarehouseId; }

    /**************************************************************************
     * 业务逻辑方法
     **************************************************************************/
    // 获取数量差异
    int getQuantityDifference() const
    {
        return actual_quantity - system_quantity;
    }

    // 计算总价（现在由后端控制）
    void calculateTotalPrice()
    {
        total_price = system_quantity * unit_price;
    }

    // 检查库存是否过低
    bool isLowStock(int threshold = 10) const
    {
        return system_quantity < threshold;
    }

    // 检查是否有库存
    bool hasStock() const
    {
        return system_quantity > 0;
    }

    // 验证模型有效性
    bool isValid() const
    {
        return material_id > 0 && warehouse_id > 0 && !material_name.empty() && !warehouse_name.empty();
    }

    // 计算加权平均价格（用于入库时的价格更新）
    double calculateWeightedAverage(int inboundQuantity, double inboundUnitPrice) const
    {
        double currentTotal = total_price;
        double inboundTotal = inboundQuantity * inboundUnitPrice;
        int totalQuantity = system_quantity + inboundQuantity;
        
        return (currentTotal + inboundTotal) / totalQuantity;
    }

    // 获取库存总价值
    double getInventoryValue() const
    {
        return total_price;  // 直接返回存储的总价
    }

    // 更新入库后的库存
    void updateAfterInbound(int inboundQuantity, double inboundUnitPrice)
    {
        // 计算新的加权平均单价
        double newAvgPrice = calculateWeightedAverage(inboundQuantity, inboundUnitPrice);
        
        // 更新数量
        system_quantity += inboundQuantity;
        actual_quantity += inboundQuantity;
        
        // 更新单价和总价
        unit_price = newAvgPrice;
        total_price = system_quantity * unit_price;
    }

    // 更新出库后的库存
    bool updateAfterOutbound(int outboundQuantity)
    {
        if (system_quantity < outboundQuantity) {
            return false;  // 库存不足
        }
        
        system_quantity -= outboundQuantity;
        actual_quantity -= outboundQuantity;
        
        // 单价不变，重新计算总价
        total_price = system_quantity * unit_price;
        
        return true;
    }
};

#endif // INVENTORYDETAILMODEL_H