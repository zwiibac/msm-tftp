#pragma once

#include <boost/noncopyable.hpp>

#include <gmock/gmock.h>

struct FileStreamMock : private boost::noncopyable 
{
    // istream& read (char* s, streamsize n);
    MOCK_METHOD(FileStreamMock&, read, (char* s, size_t n));
    MOCK_METHOD(size_t, gcount, ());
    MOCK_METHOD(void, open, (const char* filename));
    MOCK_METHOD(bool, bad, ());
    MOCK_METHOD(bool, fail, ());
    MOCK_METHOD(void, clear, ());
    MOCK_METHOD(void, close, ());
    MOCK_METHOD(bool, is_open, ());

    void DelegateToFake() 
    {
        ON_CALL(*this, read).WillByDefault([this](char* s, size_t n) {
            stream_.read(s, n);
            return std::ref(*this);
        });

        ON_CALL(*this, gcount).WillByDefault([this](){
            return stream_.gcount();
        });

        ON_CALL(*this, open).WillByDefault([this](const char*){
           stream_.clear();
           stream_.seekp(0);
        });

        ON_CALL(*this, bad).WillByDefault([this](){
            return false;
        });

        ON_CALL(*this, fail).WillByDefault([this](){
            return false;
        });

        ON_CALL(*this, clear).WillByDefault([this](){
            stream_.clear();
        });

        ON_CALL(*this, close).WillByDefault([this](){});
    }

    std::stringstream stream_;    
};