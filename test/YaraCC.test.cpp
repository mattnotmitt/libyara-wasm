//
// Created by mitt on 18/09/22.
//
#include <catch2/catch_test_macros.hpp>
#include "YaraCC.h"

TEST_CASE("Console output", "")
{
    SECTION("Simple Console Example")
    {
        auto ycc = run("CyberChef YARA", "import \"console\" \n"
                       "rule a \n"
                       "{\n"
                       "  strings: \n"
                       "    $s=\" \" \n"
                       "  condition: \n"
                       "    $s and console.log(\"log rule a\")\n"
                       "} \n"
                       "rule b \n"
                       "{\n"
                       "  strings: \n"
                       "    $r=\" \" \n"
                       "  condition: \n"
                       "    $r and console.hex(\"log rule b: int8(0)=\", int8(0))\n"
                       "}");
        REQUIRE(ycc.console_logs[0] == "log rule a");
        REQUIRE(ycc.console_logs[1] == "log rule b: int8(0)=0x43");
        REQUIRE(ycc.compile_errors[0].line_number == 5);
        REQUIRE(ycc.compile_errors[0].message == "string \"$s\" may slow down scanning");
        REQUIRE(ycc.compile_errors[1].line_number == 12);
        REQUIRE(ycc.compile_errors[1].message == "string \"$r\" may slow down scanning");
        REQUIRE(ycc.matched_rules[0].rule_name == "a");
        REQUIRE(ycc.matched_rules[0].resolved_matches[0].string_identifier == "$s");
        REQUIRE(ycc.matched_rules[1].resolved_matches[0].string_identifier == "$r");    }
}