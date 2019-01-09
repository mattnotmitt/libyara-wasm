//
// Created by matt on 07/01/19.
//

#ifndef LIBYARA_WASM_YARACC_HPP
#define LIBYARA_WASM_YARACC_HPP
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

    const std::vector<matched_rule> &getMatchedRules() const {
        return matched_rules;
    }

    void setMatchedRules(const std::vector<matched_rule> &matched_rules) {
        YaraCC::matched_rules = matched_rules;
    }

    const std::vector<compile_error> &getCompileErrors() const {
        return compile_errors;
    }

    void setCompileErrors(const std::vector<compile_error> &compile_errors) {
        YaraCC::compile_errors = compile_errors;
    }

    std::vector<matched_rule> matched_rules = std::vector<matched_rule>();
    std::vector<compile_error> compile_errors = std::vector<compile_error>();
};


#endif //LIBYARA_WASM_YARACC_HPP
