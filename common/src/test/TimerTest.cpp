#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <chrono>
#include <iostream>
#include <utility>
#include <functional>
#include <type_traits>
#include <asio.hpp>

namespace {

class TimerTest : public ::testing::Test {
protected:
    asio::io_context _ioContext;
};

namespace checker {

template <typename ArgT = int>
class CallChecker {
public:
    MOCK_CONST_METHOD0(call, void());
    MOCK_CONST_METHOD1_T(callArg, void(const ArgT&));
};

} // namespace checker


TEST_F(TimerTest, DISABLED_WaitsXSecondsSynchronously) {
    auto waitSeconds = 1;
    auto start = std::chrono::steady_clock::now();

    //call
    asio::steady_timer timer(_ioContext, asio::chrono::seconds(waitSeconds));
    timer.wait();

    auto stop = std::chrono::steady_clock::now();
    auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    EXPECT_GE(elapsedSeconds.count(), waitSeconds);
    std::cout << "Elapsed: " << elapsedSeconds.count()
            << "\nExpected min: " << waitSeconds
            << std::endl;
}

TEST_F(TimerTest, DISABLED_WaitsXMillisecondsSynchronously) {
    using namespace std::chrono_literals;
    std::chrono::milliseconds waitMilliseconds = 1s;
    auto start = std::chrono::steady_clock::now();

    //call
    asio::steady_timer timer(_ioContext, waitMilliseconds);
    timer.wait();

    auto stop = std::chrono::steady_clock::now();
    auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    EXPECT_GE(elapsedMilliseconds, waitMilliseconds); //TODO customize GTest value printing
    std::cout << "Elapsed: " << elapsedMilliseconds.count()
            << "\nExpected min: " << waitMilliseconds.count()
            << std::endl;
}

TEST_F(TimerTest, DISABLED_WaitsXNanosecondsSynchronously) {
    using namespace std::chrono_literals;
    std::chrono::nanoseconds waitNanoseconds = 1s;
    auto start = std::chrono::steady_clock::now();

    //call
    asio::steady_timer timer(_ioContext, waitNanoseconds);
    timer.wait();

    auto stop = std::chrono::steady_clock::now();
    auto elapsedNanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
    EXPECT_GE(elapsedNanoseconds, waitNanoseconds);
    std::cout << "Elapsed: " << elapsedNanoseconds.count()
            << "\nExpected min: " << waitNanoseconds.count()
            << std::endl;
}

TEST_F(TimerTest, DISABLED_WaitsAsynchronously) {
    checker::CallChecker callChecker;
    EXPECT_CALL(callChecker, call());

    using namespace std::chrono_literals;
    auto waitTime = 1s;
    auto start = std::chrono::steady_clock::now();

    // The asio library provides a guarantee that callback handlers will only be called from threads that are currently calling io_context::run().
    auto callback = [start, waitTime, &callChecker](const asio::error_code&) {
        auto stop = std::chrono::steady_clock::now();
        auto elapsed = stop - start;    // duration_cast<decltype(start)>
        EXPECT_GE(elapsed, waitTime);
        callChecker.call();
    };

    asio::steady_timer timer(_ioContext, waitTime);
    timer.async_wait(std::move(callback));
    _ioContext.run(); // the call will not return until the timer has expired and the callback has completed

    auto eventLoopStop = std::chrono::steady_clock::now();
    auto eventLoopElapsed = eventLoopStop - start;
    EXPECT_GE(eventLoopElapsed, waitTime);
}

TEST_F(TimerTest, DISABLED_WaitsAsynchronouslyRepeatedly) {
    using namespace std::chrono_literals;
    constexpr auto WAIT_NUM = 5; //TODO try const (for lambda without capture)
    constexpr auto WAIT_TIME = 1s;
    auto waitCounter = 0;

    checker::CallChecker<std::remove_const_t<decltype(WAIT_NUM)>> callChecker;
    {
        testing::InSequence dummy;
        for (auto i = 0; i < WAIT_NUM; ++i)
            EXPECT_CALL(callChecker, callArg(i));   // Pointee? (ref type)
    }

    const auto start = std::chrono::steady_clock::now();
    asio::steady_timer timer(_ioContext, WAIT_TIME);

    /*// an aggregate
    class Callback {
    public:
        using TimePoint = std::chrono::steady_clock::time_point;
        //Callback(asio::steady_timer &timer, int &waitCounter, TimePoint start, checker::CallChecker<int> &callChecker) : ...
        asio::steady_timer &timer;
        int &waitCounter;
        TimePoint start;
        checker::CallChecker<int> &callChecker;

        void operator()(const asio::error_code&) const {
            auto stop = std::chrono::steady_clock::now();
            auto elapsed = stop - start;
            EXPECT_GE(elapsed, (waitCounter + 1) * WAIT_TIME);
            callChecker.callArg(waitCounter);
            ++waitCounter;

            if (waitCounter < WAIT_NUM) {
                timer.expires_at(timer.expiry() + WAIT_TIME);
                //timer.async_wait(Callback(*this));    // copy -> const ref?
                //timer.async_wait(std::move(*this));   // moved-from(?) , std::move(*this) ??
                timer.async_wait(*this);                // ref(?)
            }
        }
    } callback{timer, waitCounter, start, callChecker}; // aggregate initialization

    timer.async_wait(std::move(callback));*/

    std::function<void(const asio::error_code&)> callback = [&timer, &waitCounter, start, &callChecker, &callback](const asio::error_code&) {
        auto stop = std::chrono::steady_clock::now();
        auto elapsed = stop - start;
        EXPECT_GE(elapsed, (waitCounter + 1) * WAIT_TIME);
        callChecker.callArg(waitCounter);
        ++waitCounter;

        if (waitCounter < WAIT_NUM) {
            timer.expires_at(timer.expiry() + WAIT_TIME);
            timer.async_wait(callback);
        }
    };
    timer.async_wait(callback);
    //std::move(callback)  // std::function's target moved-from -> std::bad_function_call exception on next std::function call

    _ioContext.run();

    auto eventLoopStop = std::chrono::steady_clock::now();
    auto eventLoopElapsed = eventLoopStop - start;
    EXPECT_GE(eventLoopElapsed, WAIT_NUM * WAIT_TIME);

    EXPECT_EQ(waitCounter, WAIT_NUM);

    /*auto doTest = [&timer, &waitCounter, start, &callChecker](){};
    auto callback = [&timer, &waitCounter, start, &callChecker, &doTest](const asio::error_code&) {
        auto stop = std::chrono::steady_clock::now();
        auto elapsed = stop - start;
        EXPECT_GE(elapsed, WAIT_TIME);
        callChecker.callArg(waitCounter);

        if (waitCounter < WAIT_NUM) {
            ++waitCounter;
            timer.expires_at(timer.expiry() + WAIT_TIME);
            //
        }
    };
    timer.async_wait(std::move(callback));
    //timer.async_wait([&callback](const asio::error_code &err){ callback(err, callback); }); // std::function*/
}

} // namespace