// infrastructure/uow/DatabaseUnitOfWork.hpp
#pragma once
#include "IUnitOfWork.hpp"
#include "../../../database/Database.hpp"
#include "../../../shared/util/log.hpp"

class DatabaseUnitOfWork : public IUnitOfWork {
private:
    Database& db_;

public:
    explicit DatabaseUnitOfWork(Database& db)
        : db_(db) {}

    bool WithTransaction(const std::function<bool()>& action) override
    {
        if (!db_.beginTransaction()) {
            LOG(LogLevel::Error) << "Begin transaction failed";
            return false;
        }

        try {
            bool ok = action();
            if (ok) {
                if (!db_.commit()) {
                    LOG(LogLevel::Error) << "Commit failed";
                    db_.rollback();
                    return false;
                }
                return true;
            } else {
                db_.rollback();
                return false;
            }
        } catch (const std::exception& e) {
            LOG(LogLevel::Error) << "Exception in transaction: " << e.what();
            db_.rollback();
            return false;
        }
    }
};
