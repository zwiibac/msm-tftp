#pragma  once

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

#include <gmock/gmock.h>

class TimerMock 
{
public:
    using Duration = std::chrono::milliseconds;
    using SystemErrorCode = boost::system::error_code;

    MOCK_METHOD(void, expires_from_now, (const Duration&));
    MOCK_METHOD(void ,async_wait, (std::function<void(const SystemErrorCode&)>));
    MOCK_METHOD(void, cancel, ()); // should return size_t

    void DelegateToFake() 
    {
        ON_CALL(*this, expires_from_now).WillByDefault([](const Duration&) {});
        ON_CALL(*this, async_wait).WillByDefault([](std::function<void(const SystemErrorCode&)>){});
        ON_CALL(*this, cancel).WillByDefault([](){});
    }
};