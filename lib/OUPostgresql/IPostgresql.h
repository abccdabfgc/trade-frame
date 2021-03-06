/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

//#include <OUSQL/Database.h>
#include <OUSQL/Constants.h>

namespace ou {
namespace db {

//class IPostgresql: public IDatabase {
class IPostgresql {
public:
  IPostgresql(void);
  virtual ~IPostgresql(void);
protected:
private:
};

} // db
} // ou
