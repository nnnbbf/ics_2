// domain/person/IRechargeRecordRepository.hpp
#pragma once
#include <vector>
#include "RechargeRecord.hpp"

namespace Domain::PersonModule
{

    class IRechargeRecordRepository
    {
    public:
        virtual ~IRechargeRecordRepository() = default;

        virtual RechargeRecord FindById(RechargeRecord::IdType id) = 0;
        virtual void Add(RechargeRecord &record) = 0;

        virtual std::vector<RechargeRecord> FindByPersonId(int personId) = 0;
        virtual std::vector<RechargeRecord> FindByDateRange(
            const std::string &startDate,
            const std::string &endDate) = 0;

        virtual double GetTotalAmountByPersonId(int personId) = 0;
    };

}
