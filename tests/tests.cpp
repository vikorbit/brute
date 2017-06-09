#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "md5.h"
#include "parse_command_line.h"
#include "worker_pool.h"
#include "words_bucket.h"


class Tester {
public:
    bool operator()( size_t minLength, size_t maxLength, std::string && hashToCheck, std::string && alphabet )
    {
        brute::DescriptorTask descriptor_task;
        descriptor_task.minLength = minLength;
        descriptor_task.maxLength = maxLength;
        descriptor_task.hashToCheck = hashToCheck;
        brute::parse_alphabet(alphabet, descriptor_task.alphabet);

        auto workerPool = std::make_shared<brute::WorkerPool>( std::thread::hardware_concurrency() );
        auto wordsBucket = std::make_shared<brute::WordsBucket>();
        wordsBucket->load( descriptor_task.minLength, descriptor_task.maxLength, descriptor_task.alphabet );

        std::function<bool(const std::string&)> predicate = [ hashToCheck ]( const std::string& str ) { return md5( str ) == hashToCheck; };

        workerPool->run( wordsBucket, predicate );

        _result = workerPool->getResult();
        return workerPool->isResult();
    }

    std::string _result;
};


TEST_CASE( "Brute-force attack against a known hash", "[hash_to_check]" ) {
    SECTION( "test for error input data" ) {
        Tester tester;
        REQUIRE_THROWS( tester(0, 0, "0cc175b9c0f1b6a831c399e269772661", "65-90:97-122") );
        REQUIRE_THROWS( tester(10, 1, "0cc175b9c0f1b6a831c399e269772661", "65-90:97-122") );
        REQUIRE_THROWS( tester(0, 3, "0cc175b9c0f1b6a831c399e269772661", "65-90:97-122") );
        REQUIRE_THROWS( tester(1, 3, "0cc175b9c0f1b6a831c399e269772661", "65-sd:97-122") );
        REQUIRE_THROWS( tester(1, 3, "0cc175b9c0f1b6a831c399e269772661", "a-90:97-122") );
        REQUIRE_THROWS( tester(1, 3, "0cc175b9c0f1b6a831c399e269772661", "65-90:g-122") );
        REQUIRE_THROWS( tester(1, 3, "0cc175b9c0f1b6a831c399e269772661", "65-90:97-d") );
        REQUIRE_THROWS( tester(1, 3, "0cc175b9c0f1b6a831c399e269772661", "65-90:97122") );
        REQUIRE_THROWS( tester(1, 3, "0cc175b9c0f1b6a831c399e269772661", "6590:97-122") );
        REQUIRE_THROWS( tester(1, 3, "0cc175b9c0f1b6a831c399e269772661", "65-9097-122") );
        REQUIRE_THROWS( tester(1, 3, "0cc175b9c0f1b6a831c399e269772661", "659097-122") );
        REQUIRE_THROWS( tester(1, 3, "0cc175b9c0f1b6a831c399e269772661", "659097122") );
    }

    SECTION( "test brute-force attack" ) {
        Tester tester;

        CHECK( tester(1, 3, "0cc175b9c0f1b6a831c399e269772661", "65-90:97-122") );
        REQUIRE( tester._result == "a" );

        CHECK( tester(1, 3, "7b774effe4a349c6dd82ad4f4f21d34c", "65-90:97-122") );
        REQUIRE( tester._result == "u" );

        CHECK( tester(1, 3, "fbade9e36a3f36d3d676c1b808451dd7", "65-90:97-122") );
        REQUIRE( tester._result == "z" );

        CHECK( tester(1, 3, "e1e1d3d40573127e9ee0480caf1283d6", "65-90:97-122") );
        REQUIRE( tester._result == "R" );

        CHECK( tester(1, 3, "eff7d5dba32b4da32d9a67a519434d3f", "65-90:97-122") );
        REQUIRE( tester._result == "df" );

        CHECK( tester(1, 3, "47e2e8c3fdb7739e740b95345a803cac", "65-90:97-122") );
        REQUIRE( tester._result == "em" );

        CHECK( tester(1, 3, "638f8da31ea4241c05e1d29cfbe98895", "65-90:97-122") );
        REQUIRE( tester._result == "eH" );

        CHECK( tester(1, 3, "38d7355701b6f3760ee49852904319c1", "65-90:97-122") );
        REQUIRE( tester._result == "dfg" );

        CHECK( tester(1, 3, "8e4d7ca3456e36a2cf725b8858633eac", "65-90:97-122") );
        REQUIRE( tester._result == "emy" );

        CHECK( tester(1, 3, "b58f7a3b389308b6645df1ff013e528f", "65-90:97-122") );
        REQUIRE( tester._result == "eHy" );

        CHECK( !tester(1, 2, "b58f7a3b389308b6645df1ff013e5283", "65-90:97-122") );
    }
}
