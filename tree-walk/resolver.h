//
// Created by hlx on 2023/9/25.
//

#ifndef CPPLOX_RESOLVER_H
#define CPPLOX_RESOLVER_H

#include "stmt.h"
#include "expr.h"

namespace cpplox {

    class Resolver : public expr::Visitor, public stmt::Visitor {

    };

}

#endif //CPPLOX_RESOLVER_H
