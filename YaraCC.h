//
// Created by matt on 07/01/19.
//

#pragma once
#include <string>
#include <vector>
#include <map>

class YaraCC {
public:
    YaraCC() = default;
    struct resolved_match {
        long long location;
        long match_length;
        std::string data;
        std::string string_identifier;
        long data_length;
    };
    struct meta {
        std::string identifier;
        std::string data;
    };
    struct matched_rule {
        std::string rule_name;
        std::vector<resolved_match> resolved_matches;
        std::vector<meta> metadata;
    };
    struct compile_error {
        std::string message;
        int line_number;
        bool warning;
    };

    std::vector<std::string> console_logs = std::vector<std::string>();
    std::vector<matched_rule> matched_rules = std::vector<matched_rule>();
    std::vector<compile_error> compile_errors = std::vector<compile_error>();
};

YaraCC run(const std::string &buf_str, const std::string &rules_str);
