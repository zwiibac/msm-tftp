#include <boost/filesystem/operations.hpp>
#include <tuple>
#include <fstream>

#include <boost/filesystem.hpp>

#include <gtest/gtest-param-test.h>
#include <gtest/gtest.h>

#include <zwiibac/tftp/data/file_path.h>

namespace {

using namespace zwiibac::tftp;

using boost::filesystem::create_directory;
using boost::filesystem::remove_all;
using boost::filesystem::path;
using boost::filesystem::temp_directory_path;

const path temp_path = temp_directory_path()/"zwiibac_tftp_filepath_test_4ee2cb38-1371-40e0-bd4a-5127067f0afe";

class FilePathFixture : public ::testing::TestWithParam<std::tuple<std::string, std::string, bool, std::string>>
{
public:
    enum Param 
    {
        RootPath = 0,
        FileName = 1,
        HasValue = 2,
        Result = 3
    };

    static void SetUpTestSuite() 
    {
        

        remove_all(temp_path);
        create_directory(temp_path);
        create_directory(temp_path /"unit");
        std::ofstream file1(temp_path /"test123.txt", std::ofstream::trunc);
        file1.close();
        std::ofstream file2(temp_path /"unit"/"test123.txt", std::ofstream::trunc);
        file2.close();
    }

protected:
    std::string root_path;
    std::string file_name;
    bool expect_value;
    std::string expected_file_path;

    virtual void SetUp() override 
    {
        root_path = Get<Param::RootPath>();
        file_name = Get<Param::FileName>();
        expect_value = Get<Param::HasValue>();
        expected_file_path = Get<Param::Result>();
    }

    template<size_t Index>
    typename std::tuple_element<Index, ParamType>::type Get() { return std::get<Index>(GetParam()); }    
};

TEST_P(FilePathFixture, Basic) 
{
    // arrange
    FilePath sut;
    sut.SetServerRootPath(root_path);
    // act
    auto file_path = sut.GetReadFilePath(file_name);
    // assert
    EXPECT_EQ(file_path.has_value(), expect_value);

    if (expect_value) 
    {
        ASSERT_TRUE(file_path.has_value());
        EXPECT_STREQ(file_path.value().data(), expected_file_path.data());
        EXPECT_EQ(file_path.value(), expected_file_path);
    }
}

TEST_P(FilePathFixture, Basic2) 
{
    // arrange
    FilePath sut;
    sut.SetServerRootPath(root_path);
    // act
    auto file_path = sut.GetWriteFilePath(file_name);
    // assert
    EXPECT_EQ(file_path.has_value(), expect_value);

    if (expect_value) 
    {
        ASSERT_TRUE(file_path.has_value());
        EXPECT_STREQ(file_path.value().data(), expected_file_path.data());
        EXPECT_EQ(file_path.value(), expected_file_path);
    }
}

INSTANTIATE_TEST_SUITE_P(FilePathTest, FilePathFixture, ::testing::Values(
    std::make_tuple((temp_path /"unit").string(), "/unit/test123.txt", false, ""),
    std::make_tuple((temp_path /"unit").string(), "test123.txt", true, (temp_path /"unit"/"test123.txt").string()),
    std::make_tuple((temp_path /"unit").string(), "../test123.txt", false, ""),
    std::make_tuple((temp_path /"unit").string(), "../unit/test123.txt", true, (temp_path /"unit"/"test123.txt").string()),
    std::make_tuple((temp_path).string(), "unit/../test123.txt", true, (temp_path /"test123.txt").string()),
    std::make_tuple((temp_path).string(), "test123.txt", true, (temp_path /"test123.txt").string()),
    std::make_tuple((temp_path).string(), "/test123.txt", false, "")
));

} // end anonymous namespace