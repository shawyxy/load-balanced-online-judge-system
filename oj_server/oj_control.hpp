#pragma once

#include <iostream>
#include <string>

#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include "oj_model.hpp"
#include "oj_view.hpp"

namespace ns_control
{
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_model;
    using namespace ns_view;

    class Control
    {
    private:
        Model _model;
        
    public:
        Control() {}
        ~Control() {}

    public:
    };
} // namespace ns_control