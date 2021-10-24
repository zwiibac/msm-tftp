#pragma once

#include <gmock/gmock-function-mocker.h>
#include <optional>
#include <string_view>

#include <gmock/gmock.h>

struct FileNameMock
{
    MOCK_METHOD(std::optional<std::string>, GetReadFilePath, (const std::string_view& file_name));
    MOCK_METHOD(std::optional<size_t>, GetFileSize, (const std::string& file));
    MOCK_METHOD(void, Reset, ());

    size_t file_size_fake_ = 0;

    void DelegateFileNameMockToFake() 
    {
        ON_CALL(*this, GetReadFilePath).WillByDefault([this](const std::string_view& file_name){
            return file_name.data();
        });
        ON_CALL(*this, GetFileSize).WillByDefault([this](const std::string& file)
        {
            return file_size_fake_;
        });
    }
};