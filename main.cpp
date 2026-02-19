// #include "api/PersonHandler.hpp"
// #include "api/UserHandler.hpp"
// #include "api/MaterialHandler.hpp"
// #include "api/WarehousesHandler.hpp"
// #include "api/ComplexQueryHandler.hpp"
// #include "api/InventoryHandler.hpp"
// #include "api/InboundHandler.hpp"
// #include "api/OutboundHandler.hpp"
// #include "src/api/PersonController.hpp"
// #include "database/Database.hpp"
// #include "util/httplib.h"
// #include "util/log.hpp"
// #include <iostream>
// #include <fstream>
// #include <filesystem>

// int main()
// {
//     连接数据库
//     Database &db = Database::getInstance();
//     if (!db.connect("121.41.28.21", "mgh", "114455", "ics_warehouse"))
//     {
//         LOG(LogLevel::Error) << "Failed to connect to database";
//         return 1;
//     }
//     LOG(LogLevel::Info) << "Connected to database successfully";

//     PersonHandler personHandler;
//     MaterialHandler materialHandler;
//     UserHandler userHandler;
//     ComplexQueryHandler complexQueryHandler;
//     WarehousesHandler warehousesHandler;
//     InventoryHandler inventoryHandler;
//     InboundHandler inboundHandler;
//     OutboundHandler outboundHandler;

//     httplib::Server svr;

//     ================ 注册所有路由 ================

//     人员相关
//     svr.Get("/api/persons", [&](auto &req, auto &res)
//             { personHandler.handleGetPersons(req, res); });

//     svr.Post("/api/persons", [&](auto &req, auto &res)
//              { personHandler.handleCreatePerson(req, res); });

//     svr.Post("/api/persons/:id/recharge", [&](auto &req, auto &res)
//              { personHandler.handleRecharge(req, res); });
//     svr.Post("/api/persons/card", [&](auto &req, auto &res)
//              { personHandler.handleGetPersonByCardNumber(req, res); });

//     svr.Delete("/api/persons/:id", [&](auto &req, auto &res)
//                { personHandler.handleDeletePerson(req, res); });
//     svr.Put("/api/persons/:id", [&](auto &req, auto &res)
//             { personHandler.handleUpdatePerson(req, res); });

//     材料相关
//     svr.Get("/api/materials", [&](auto &req, auto &res)
//             { materialHandler.handleGetMaterial(req, res); });

//     svr.Post("/api/materials", [&](auto &req, auto &res)
//              { materialHandler.handleCreateMaterial(req, res); });
//     svr.Delete("/api/materials/:id", [&](auto &req, auto &res)
//                { materialHandler.handleDeleteMaterial(req, res); });
//     svr.Put("/api/materials/:id", [&](auto &req, auto &res)
//             { materialHandler.handleUpdateMaterial(req, res); });

//     用户相关
//     svr.Post("/api/login", [&](auto &req, auto &res)
//              { userHandler.handleAuthenticateUser(req, res); });
//     svr.Post("/api/users", [&](auto &req, auto &res)
//              { userHandler.handleCreateUser(req, res); });
//     svr.Get("/api/users", [&](auto &req, auto &res)
//             { userHandler.handleGetAllUsers(req, res); });
//     svr.Get("/api/users/:id", [&](auto &req, auto &res)
//             { userHandler.handleGetUserById(req, res); });
//     svr.Put("/api/users/:id", [&](auto &req, auto &res)
//             { userHandler.handleUpdateUser(req, res); });
//     svr.Delete("/api/users/:id", [&](auto &req, auto &res)
//                { userHandler.handleDeleteUser(req, res); });
//     svr.Post("/api/users/change-password/:id", [&](auto &req, auto &res)
//              { userHandler.handleChangePassword(req, res); });
//     svr.Post("/api/users/reset-password/:id", [&](auto &req, auto &res)
//              { userHandler.handleResetPassword(req, res); });
//     svr.Post("/api/version", [&](auto &req, auto &res)
//              { userHandler.handleCheckVersion(req, res); });
//     svr.Post("/api/users/change-username", [&](auto &req, auto &res)
//              { userHandler.handleChangeUsername(req, res); });

//     仓库相关
//     svr.Get("/api/warehouses", [&](auto &req, auto &res)
//             { warehousesHandler.handleGetAllWarehouses(req, res); });
//     svr.Post("/api/warehouses", [&](auto &req, auto &res)
//              { warehousesHandler.handleCreateWarehouse(req, res); });
//     svr.Put("/api/warehouses/:id", [&](auto &req, auto &res)
//             { warehousesHandler.handleUpdateWarehouse(req, res); });
//     svr.Delete("/api/warehouses/:id", [&](auto &req, auto &res)
//                { warehousesHandler.handleDeleteWarehouse(req, res); });

//     充值记录
//     svr.Post("/api/recharge-records", [&](auto &req, auto &res)
//              { complexQueryHandler.handleGetrecharger(req, res); });

//     库存数据
//     svr.Post("/api/inventory", [&](auto &req, auto &res)
//              { inventoryHandler.handleGetInventory(req, res); });

//     入库相关
//     svr.Post("/api/inbound", [&](auto &req, auto &res)
//              { inboundHandler.handleCreateInbound(req, res); });
//     svr.Post("/api/inbound-records", [&](auto &req, auto &res)
//              { inboundHandler.handleGetInbound(req, res); });

//     出库相关
//     svr.Post("/api/outbound", [&](auto &req, auto &res)
//              { outboundHandler.handleCreateOutbound(req, res); });
//     svr.Post("/api/outbound-records", [&](auto &req, auto &res)
//              { outboundHandler.handleGetOutbound(req, res); });

//     ================ 启动服务器 ================
//     LOG(LogLevel::Info) << "Starting server on port 8082...";
//     svr.listen("0.0.0.0", 8083);
//     return 0;
// }

#include "src/api/PersonController.hpp"
#include "database/Database.hpp"
#include "shared/util/httplib.h"
#include "shared/util/log.hpp"

int main()
{
        // 连接数据库
        Database &db = Database::getInstance();
        if (!db.connect("121.41.28.21", "mgh", "114455", "ics_warehouse"))
        {
                LOG(LogLevel::Error) << "Failed to connect to database";
                return 1;
        }
        LOG(LogLevel::Info) << "Connected to database successfully";

        httplib::Server svr;

        PersonController personController;
        svr.Get("/api/persons", [&](auto &req, auto &res)
                { personController.handleGetPersons(req, res); });

        svr.Post("/api/persons", [&](auto &req, auto &res)
                 { personController.handleCreatePerson(req, res); });

        svr.Post("/api/persons/:id/recharge", [&](auto &req, auto &res)
                 { personController.HandleRecharge(req, res); });
        svr.Post("/api/persons/card", [&](auto &req, auto &res)
                 { personController.handleGetPersonByCardNumber(req, res); });

        svr.Delete("/api/persons/:id", [&](auto &req, auto &res)
                   { personController.handleDeletePerson(req, res); });
        svr.Put("/api/persons/:id", [&](auto &req, auto &res)
                { personController.handleUpdatePerson(req, res); });
        // ================ 启动服务器 ================
        LOG(LogLevel::Info) << "Starting server on port 8082...";
        svr.listen("0.0.0.0", 8083);
        return 0;
}