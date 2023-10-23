//  (C) Copyright Gennadiy Rozental 2001-2008.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at 
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  File        : $RCSfile$
//
//  Version     : $Revision: 49312 $
//
//  Description : included (vs. linked) version of Unit Test Framework
// ***************************************************************************

#ifndef BOOST_INCLUDED_UNIT_TEST_FRAMEWORK_HPP_071894GER
#define BOOST_INCLUDED_UNIT_TEST_FRAMEWORK_HPP_071894GER

#include "../../test/impl/compiler_log_formatter.ipp"
#include "../../test/impl/debug.ipp"
#include "../../test/impl/framework.ipp"
#include "../../test/impl/exception_safety.ipp"
#include "../../test/impl/execution_monitor.ipp"
#include "../../test/impl/interaction_based.ipp"
#include "../../test/impl/logged_expectations.ipp"
#include "../../test/impl/plain_report_formatter.ipp"
#include "../../test/impl/progress_monitor.ipp"
#include "../../test/impl/results_collector.ipp"
#include "../../test/impl/results_reporter.ipp"
#include "../../test/impl/test_tools.ipp"
#include "../../test/impl/unit_test_log.ipp"
#include "../../test/impl/unit_test_main.ipp"
#include "../../test/impl/unit_test_monitor.ipp"
#include "../../test/impl/unit_test_parameters.ipp"
#include "../../test/impl/unit_test_suite.ipp"
#include "../../test/impl/xml_log_formatter.ipp"
#include "../../test/impl/xml_report_formatter.ipp"

#define BOOST_TEST_INCLUDED
#include "../../test/unit_test.hpp"

#endif // BOOST_INCLUDED_UNIT_TEST_FRAMEWORK_HPP_071894GER
