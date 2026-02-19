// infrastructure/uow/IUnitOfWork.hpp
#pragma once
#include <functional>

class IUnitOfWork {
public:
    virtual ~IUnitOfWork() = default;

    virtual bool WithTransaction(const std::function<bool()>& action) = 0;
};
