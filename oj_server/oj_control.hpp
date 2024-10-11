#pragma once

#include <iostream>
#include <string>

#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include "oj_model.hpp"

namespace ns_control
{
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_model;

    class Control
    {
    private:
        Model __model;
        
    public:
        Control() {}
        ~Control() {}
    public:

    };
} // namespace ns_control