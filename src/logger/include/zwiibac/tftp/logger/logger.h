#pragma once

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#define ZWIIB_LOG(level) BOOST_LOG_TRIVIAL(level)

#define ZWIIB_LOG_IF(level, condition) \
if (!(condition))   \
    ;               \
else                \
    ZWIIB_LOG(level)

#define ZWIIB_LOG_SET_LEVEL() \
boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info)
