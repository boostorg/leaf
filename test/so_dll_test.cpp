// Copyright 2018-2024 Emil Dotchevski and Reverge Studios, Inc.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_LEAF_SO_DLL_TEST_BUILDING_EXE

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/diagnostics.hpp>
#   include <boost/leaf/result.hpp>
#endif

#include "so_dll_lib1.hpp"
#include "so_dll_lib2.hpp"

#if BOOST_LEAF_CFG_STD_STRING
#   include <sstream>
#   include <iostream>
#endif

#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

int test_result(leaf::result<void> (*f)(), bool print)
{
    int r = leaf::try_handle_all(
        [f]() -> leaf::result<int>
        {
            BOOST_LEAF_CHECK(f());
            return 0;
        },
        [&]( my_info<1> x1, my_info<2> x2, leaf::diagnostic_details const & info, leaf::diagnostic_details const & vinfo )
        {
            if( x1.value != 1 )
                return 1;
            if( x2.value != 2 )
                return 2;
            if( BOOST_LEAF_CFG_DIAGNOSTICS && print )
            {
#if BOOST_LEAF_CFG_STD_STRING
                std::ostringstream ss; ss << vinfo;
                std::string s = ss.str();
                std::cout << "Handler matched, diagnostics:\n" << s << std::endl;
                if( BOOST_LEAF_CFG_DIAGNOSTICS && BOOST_LEAF_CFG_CAPTURE )
                    if( s.find("Test my_info<3>::value = 3") == std::string::npos )
                        return 3;
#endif
            }
            return 0;
        },
        [&](leaf::diagnostic_details const & vinfo)
        {
#if BOOST_LEAF_CFG_STD_STRING
            if( print )
                std::cout << "Test is failing (catch-all), diagnostics:\n" << vinfo << std::endl;
#endif
            return 4;
        } );
    return r;
}

#ifndef BOOST_LEAF_NO_EXCEPTIONS
int test_exception(void (*f)(), bool print)
{
    int r = leaf::try_catch(
        [f]
        {
            f();
            return 0;
        },
        [&]( my_info<1> x1, my_info<2> x2, leaf::diagnostic_details const & info, leaf::diagnostic_details const & vinfo )
        {
            if( x1.value != 1 )
                return 1;
            if( x2.value != 2 )
                return 2;
            if( BOOST_LEAF_CFG_DIAGNOSTICS && print )
            {
#if BOOST_LEAF_CFG_STD_STRING
                std::ostringstream ss; ss << vinfo;
                std::string s = ss.str();
                std::cout << s << std::endl;
                if( BOOST_LEAF_CFG_DIAGNOSTICS && BOOST_LEAF_CFG_CAPTURE )
                    if( s.find("Test my_info<3>::value = 3") == std::string::npos )
                        return 3;
#endif
            }
            return 0;
        },
        [&](leaf::diagnostic_details const & vinfo)
        {
#if BOOST_LEAF_CFG_STD_STRING
            if( print )
                std::cout << "Test is failing\n" << vinfo;
#endif
            return 4;
        } );
    return r;
}

int test_catch(void (*f)())
{
    try
    {
        f();
        return 1;
    }
    catch( leaf::error_id const & )
    {
        return 0;
    }
    catch(...)
    {
        return 2;
    }
}
#endif // #ifndef BOOST_LEAF_NO_EXCEPTIONS

void test_single_thread()
{
    BOOST_TEST_EQ(test_result(hidden_result2, true), 0);
    BOOST_TEST_EQ(test_result(hidden_result1, true), 0);

#ifndef BOOST_LEAF_NO_EXCEPTIONS
    BOOST_TEST_EQ(test_exception(hidden_throw1, true), 0);
    BOOST_TEST_EQ(test_catch(hidden_throw1), 0);
    BOOST_TEST_EQ(test_exception(hidden_throw2, true), 0);
    BOOST_TEST_EQ(test_catch(hidden_throw2), 0);
#endif // #ifndef BOOST_LEAF_NO_EXCEPTIONS
}

void test_multithreaded()
{
    constexpr int N = 100;
    {
        std::mutex mtx;
        std::condition_variable cv;
        bool ready = false;
        auto test_function = [&]
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]{ return ready; });
            lock.unlock();
            int result1 = test_result(hidden_result1, false);
            int result2 = test_result(hidden_result2, false);
            return result1 + result2;
        };
        std::vector<std::future<int>> futures;
        for (int i = 0; i < N; ++i)
            futures.push_back(std::async(std::launch::async, test_function));
        {
            std::lock_guard<std::mutex> lock(mtx);
            ready = true;
        }
        cv.notify_all();
        for (auto start = std::chrono::steady_clock::now(); std::chrono::steady_clock::now() - start < std::chrono::seconds(1); )
            if ((std::rand() % 2) == 0 || futures.empty())
                futures.push_back(std::async(std::launch::async, test_function));
            else
            {
                BOOST_TEST_EQ(futures.back().get(), 0);
                futures.pop_back();
            }
        for (auto & f : futures)
            BOOST_TEST_EQ(f.get(), 0);
    }

#ifndef BOOST_LEAF_NO_EXCEPTIONS
    {
        std::mutex mtx;
        std::condition_variable cv;
        bool ready = false;
        auto test_function = [&]
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]{ return ready; });
            lock.unlock();
            int result1 = test_exception(hidden_throw1, false);
            int result2 = test_catch(hidden_throw1);
            int result3 = test_exception(hidden_throw2, false);
            int result4 = test_catch(hidden_throw2);
            return result1 + result2 + result3 + result4;
        };
        std::vector<std::future<int>> futures;
        for (int i = 0; i < N; ++i)
            futures.push_back(std::async(std::launch::async, test_function));
        {
            std::lock_guard<std::mutex> lock(mtx);
            ready = true;
        }
        cv.notify_all();
        for (auto start = std::chrono::steady_clock::now(); std::chrono::steady_clock::now() - start < std::chrono::seconds(1); )
            if ((std::rand() % 2) == 0 || futures.empty())
                futures.push_back(std::async(std::launch::async, test_function));
            else
            {
                BOOST_TEST_EQ(futures.back().get(), 0);
                futures.pop_back();
            }
        for (auto & f : futures)
            BOOST_TEST_EQ(f.get(), 0);
    }
#endif // #ifndef BOOST_LEAF_NO_EXCEPTIONS
}

int main()
{
    std::srand(std::hash<unsigned>{}(static_cast<unsigned>(std::time(nullptr))));

    test_single_thread();
    test_multithreaded();

    return boost::report_errors();
}
