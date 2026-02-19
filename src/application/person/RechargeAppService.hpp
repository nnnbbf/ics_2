#pragma once
#include "../../domain/person/IPersonRepository.hpp"
#include "../../domain/person/IRechargeRecordRepository.hpp"
#include "../../infrastructure/uow/IUnitOfWork.hpp"
#include "../../../shared/util/log.hpp"
namespace Application::PersonUseCases
{

    using namespace Domain::PersonModule;

    class RechargeAppService
    {
    private:
        IPersonRepository &personRepo_;
        IRechargeRecordRepository &recordRepo_;
        IUnitOfWork &uow_;

    public:
        RechargeAppService(IPersonRepository &personRepo,
                           IRechargeRecordRepository &recordRepo,
                           IUnitOfWork &uow)
            : personRepo_(personRepo),
              recordRepo_(recordRepo),
              uow_(uow)
        {
        }

        bool Recharge(Person::IdType personId,
                      double amount,
                      const std::string &operatorName)
        {
            return uow_.WithTransaction([&]() -> bool
                                        {
                Person person = personRepo_.FindById(personId);
                if (!person.IsValid()) {
                    return false;
                }

                try {
                    // 1. 让领域对象完成业务逻辑，返回记录
                    RechargeRecord record = person.Recharge(amount, operatorName);

                    // 2. 持久化
                    personRepo_.Update(person);
                    recordRepo_.Add(record);
                } catch (const std::exception& e) {
                    // 任何异常都认为失败，由 UoW 回滚
                    LOG(LogLevel::Error) << "Recharge failed: " << e.what();
                    return false;
                }

                return true; });
        }
    };

} // namespace Application::PersonUseCases
