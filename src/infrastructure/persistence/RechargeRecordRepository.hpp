#pragma once
#include "../../../database/Database.hpp"
#include "../../domain/person/IRechargeRecordRepository.hpp"
#include <vector>

namespace Infrastructure::Persistence
{

    class RechargeRecordRepository : public Domain::PersonModule::IRechargeRecordRepository
    {
    private:
        Database &db_;

    public:
        explicit RechargeRecordRepository(Database &database) : db_(database) {}

        // 实现所有接口方法
        Domain::PersonModule::RechargeRecord FindById(Domain::PersonModule::RechargeRecord::IdType id) override
        {
            Domain::PersonModule::RechargeRecord record;
            if (id <= 0)
                return record;

            std::string sql = "SELECT * FROM recharge_records WHERE id = " + std::to_string(id);
            Json::Value result = db_.query(sql);
            if (result.size() > 0)
            {
                record = Domain::PersonModule::RechargeRecord::fromJson(result[0]);
            }
            return record;
        }

        void Add(Domain::PersonModule::RechargeRecord &record) override
        {
            std::string sql = "INSERT INTO recharge_records (person_id, person_name, amount, recharge_date, operator_name) VALUES (" +
                              std::to_string(record.PersonId()) + ", '" +
                              db_.escape(record.PersonName()) + "', " +
                              std::to_string(record.Amount()) + ", NOW(), '" +
                              db_.escape(record.OperatorName()) + "')";

            if (db_.execute(sql))
            {
                int newId = db_.getLastInsertId();
                record.SetId(newId);
            }
        }

        std::vector<Domain::PersonModule::RechargeRecord> FindByPersonId(int personId) override
        {
            std::vector<Domain::PersonModule::RechargeRecord> list;
            if (personId <= 0)
                return list;

            std::string sql = "SELECT * FROM recharge_records WHERE person_id = " +
                              std::to_string(personId) + " ORDER BY recharge_date DESC";
            Json::Value result = db_.query(sql);
            for (const auto &item : result)
            {
                list.push_back(Domain::PersonModule::RechargeRecord::fromJson(item));
            }
            return list;
        }

        std::vector<Domain::PersonModule::RechargeRecord> FindByDateRange(
            const std::string &startDate,
            const std::string &endDate) override
        {
            std::vector<Domain::PersonModule::RechargeRecord> list;

            std::string sql = "SELECT * FROM recharge_records WHERE recharge_date BETWEEN '" +
                              db_.escape(startDate) + "' AND '" + db_.escape(endDate) +
                              "' ORDER BY recharge_date DESC";
            Json::Value result = db_.query(sql);
            for (const auto &item : result)
            {
                list.push_back(Domain::PersonModule::RechargeRecord::fromJson(item));
            }
            return list;
        }

        double GetTotalAmountByPersonId(int personId) override
        {
            if (personId <= 0)
                return 0.0;

            std::string sql = "SELECT SUM(amount) as total FROM recharge_records WHERE person_id = " +
                              std::to_string(personId);
            Json::Value result = db_.query(sql);
            if (result.size() > 0 && result[0].isMember("total"))
            {
                return std::stod(result[0]["total"].asString());
            }
            return 0.0;
        }
    };

} // namespace Infrastructure::Persistence