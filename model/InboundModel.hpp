// inboundmodel.h
#pragma once

#include <string>
#include <jsoncpp/json/json.h>

class InboundModel
{
private:
    int id;                             // 入库记录ID
    int material_id;                    // 物料ID
    std::string material_name;          // 物料名称
    std::string material_category;      // 物料分类
    std::string material_sub_category;  // 物料子分类
    std::string material_unit;          // 物料单位
    std::string material_specification; // 物料规格

    int warehouse_id;               // 仓库ID
    std::string warehouse_name;     // 仓库名称

    int quantity;       // 入库数量
    double unit_price;  // 单价
    double total_price; // 总价（计算字段）

    std::string purchaser;  // 采购人
    std::string supplier;   // 供货商
    std::string shelf_name; // 货架名称
    std::string placement_layer;    // 放置层数
    std::string registrant; // 登记人
    std::string remarks;    // 备注

    std::string registration_date; // 登记日期

public:
    InboundModel() : id(0), material_id(0), warehouse_id(0),
                     quantity(0), 
                     unit_price(0.0), total_price(0.0) {}

    /**************************************************************************
     * 静态工厂方法：从JSON创建对象
     **************************************************************************/
    static InboundModel fromJson(const Json::Value &json)
    {
        InboundModel model;

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
            model.material_sub_category = json["material_sub_category"].asString();  // 修复这里

        if (json.isMember("material_unit"))
            model.material_unit = json["material_unit"].asString();

        if (json.isMember("material_specification"))
            model.material_specification = json["material_specification"].asString();

        if (json.isMember("shelf_name"))
            model.shelf_name = json["shelf_name"].asString();

        // 数字字段从字符串转换
        if (json.isMember("quantity") && !json["quantity"].asString().empty())
            model.quantity = std::stoi(json["quantity"].asString());

        if (json.isMember("placement_layer"))
            model.placement_layer = json["placement_layer"].asString();

        if (json.isMember("unit_price") && !json["unit_price"].asString().empty())
            model.unit_price = std::stod(json["unit_price"].asString());

        // 字符串字段直接赋值
        if (json.isMember("purchaser"))
            model.purchaser = json["purchaser"].asString();

        if (json.isMember("supplier"))
            model.supplier = json["supplier"].asString();

        if (json.isMember("registrant"))
            model.registrant = json["registrant"].asString();

        if (json.isMember("remarks"))
            model.remarks = json["remarks"].asString();

        if (json.isMember("registration_date"))
            model.registration_date = json["registration_date"].asString();

        // 计算总价
        model.calculateTotalPrice();

        return model;
    }

    /**************************************************************************
     * 转换为JSON对象
     **************************************************************************/
    Json::Value toJson() const
    {
        Json::Value json;

        json["id"] = std::to_string(id);
        json["material_id"] = std::to_string(material_id);
        json["material_name"] = material_name;
        json["material_category"] = material_category;
        json["material_sub_category"] = material_sub_category;
        json["material_unit"] = material_unit;
        json["material_specification"] = material_specification;

        json["warehouse_id"] = std::to_string(warehouse_id);
        json["warehouse_name"] = warehouse_name;

        json["quantity"] = std::to_string(quantity);
        json["unit_price"] = std::to_string(unit_price);
        json["total_price"] = std::to_string(total_price);

        json["purchaser"] = purchaser;
        json["supplier"] = supplier;
        json["shelf_name"] = shelf_name;
        json["placement_layer"] = placement_layer;
        json["registrant"] = registrant;
        json["remarks"] = remarks;
        json["registration_date"] = registration_date;

        return json;
    }

    /**************************************************************************
     * Getter方法
     **************************************************************************/
    int getId() const { return id; }
    int getMaterialId() const { return material_id; }
    const std::string &getMaterialName() const { return material_name; }
    const std::string &getMaterialCategory() const { return material_category; }
    const std::string &getMaterialSubCategory() const { return material_sub_category; }
    const std::string &getMaterialUnit() const { return material_unit; }
    const std::string &getMaterialSpecification() const { return material_specification; }

    int getWarehouseId() const { return warehouse_id; }
    const std::string &getWarehouseName() const { return warehouse_name; }

    int getQuantity() const { return quantity; }
    double getUnitPrice() const { return unit_price; }
    double getTotalPrice() const { return total_price; }

    const std::string &getPurchaser() const { return purchaser; }
    const std::string &getSupplier() const { return supplier; }
    const std::string &getShelfName() const { return shelf_name; }
    const std::string getPlacementLayer() const { return placement_layer; }
    const std::string &getRegistrant() const { return registrant; }
    const std::string &getRemarks() const { return remarks; }
    const std::string &getRegistrationDate() const { return registration_date; }

    /**************************************************************************
     * Setter方法
     **************************************************************************/
    void setId(int newId) { id = newId; }
    void setMaterialId(int newMaterialId) { material_id = newMaterialId; }
    void setMaterialName(const std::string &newMaterialName) { material_name = newMaterialName; }
    void setMaterialCategory(const std::string &newMaterialCategory) { material_category = newMaterialCategory; }
    void setMaterialSubCategory(const std::string &newMaterialSubCategory) { material_sub_category = newMaterialSubCategory; }
    void setMaterialUnit(const std::string &newMaterialUnit) { material_unit = newMaterialUnit; }
    void setMaterialSpecification(const std::string &newMaterialSpecification) { material_specification = newMaterialSpecification; }

    void setWarehouseId(int newWarehouseId) { warehouse_id = newWarehouseId; }
    void setWarehouseName(const std::string &newWarehouseName) { warehouse_name = newWarehouseName; }

    void setQuantity(int newQuantity)
    {
        quantity = newQuantity;
        calculateTotalPrice();
    }
    void setUnitPrice(double newUnitPrice)
    {
        unit_price = newUnitPrice;
        calculateTotalPrice();
    }

    void setPurchaser(const std::string &newPurchaser) { purchaser = newPurchaser; }
    void setSupplier(const std::string &newSupplier) { supplier = newSupplier; }
    void setShelfName(const std::string &newShelfName) { shelf_name = newShelfName; }
    void setPlacementLayer(const std::string &newPlacementLayer) { placement_layer = newPlacementLayer; }
    void setRegistrant(const std::string &newRegistrant) { registrant = newRegistrant; }
    void setRemarks(const std::string &newRemarks) { remarks = newRemarks; }
    void setRegistrationDate(const std::string &newRegistrationDate) { registration_date = newRegistrationDate; }

    /**************************************************************************
     * 业务逻辑方法
     **************************************************************************/
    void calculateTotalPrice()
    {
        total_price = quantity * unit_price;
    }

    // 检查是否为高价值入库（单价超过阈值）
    bool isHighValue(double threshold = 1000.0) const
    {
        return unit_price > threshold;
    }

    // 检查是否为大批量入库
    bool isBulkInbound(int threshold = 100) const
    {
        return quantity > threshold;
    }

    // 验证模型有效性
    bool isValid() const
    {
        return material_id > 0 &&
               warehouse_id > 0 &&
               quantity > 0 &&
               !material_name.empty() &&
               !warehouse_name.empty();
    }

    // 获取入库摘要信息
    std::string getSummary() const
    {
        return material_name + " - " + std::to_string(quantity) +
               material_unit + " - 总价: ¥" + std::to_string(total_price);
    }

    // 获取批次总价（用于区分库存总价）
    double getBatchTotalPrice() const
    {
        return total_price;
    }

    // 设置批次总价
    void setBatchTotalPrice(double batchTotalPrice)
    {
        total_price = batchTotalPrice;
    }
};