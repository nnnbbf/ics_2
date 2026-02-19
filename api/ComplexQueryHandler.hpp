#include "../service/RechargerRecordsService.hpp"
#include "../util/httplib.h"
#include "../util/jsonutil.hpp"

class ComplexQueryHandler
{
    private:
    RechargerRecordsService rechargeService;

    public:
    void handleGetrecharger(const httplib::Request& req, httplib::Response& res)
    {

        auto materials = rechargeService.getAllRecords();
        LOG(LogLevel::Debug) << materials.size();
        Json::Value jsonArray = Json::Value(Json::arrayValue);
        for (const auto& material : materials) {
        
            Json::Value json = material.toJson();
            jsonArray.append(json);
        }
        res.set_content(jsonArray.toStyledString(), "application/json");
        
    }

};