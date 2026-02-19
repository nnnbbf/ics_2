#pragma
#include "../application/material/MaterialAppService.hpp"
#include "../infrastructure/persistence/MaterialRepository.hpp"
#include "../infrastructure/uow/DatabaseUnitOfWork.hpp"
#include "../../shared/util/httplib.h"
#include "../../shared/util/jsonutil.hpp"
#include "../../shared/util/log.hpp"

class MaterialController
{
private:
    Database &db_;
    Infrastructure::Persistence::MaterialRepository materialRepo_;
    DatabaseUnitOfWork uow_;
    Application::MaterialUseCases::MaterialAppService materialApp_;

public:
    MaterialController()
        : db_(Database::getInstance()),
          materialRepo_(db_),
          uow_(db_),
          materialApp_(materialRepo_)
    {
    }
};