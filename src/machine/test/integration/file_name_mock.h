#pragma once

#include <gmock/gmock-function-mocker.h>
#include <optional>
#include <string_view>

#include <gmock/gmock.h>

struct FileNameMock
{
    MOCK_METHOD(std::optional<std::string>, GetReadFilePath, (const std::string_view& file_name));
    MOCK_METHOD(void, Reset, ());

    void DelegateFileNameMockToFake() 
    {
        ON_CALL(*this, GetReadFilePath).WillByDefault([this](const std::string_view& file_name){
            return file_name.data();
        });
    }
};