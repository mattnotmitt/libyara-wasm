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
        long data_length;
    };
    struct compile_error {
        std::string message;
        int line_number;
        bool warning;
    };
    const std::map<std::string, std::vector<resolved_match>> &getMatchedRules() const {
        return matched_rules;
    }

    void setMatchedRules(const std::map<std::string, std::vector<resolved_match>> &matched_rules) {
        YaraCC::matched_rules = matched_rules;
    }

    const std::vector<compile_error> &getCompileErrors() const {
        return compile_errors;
    }

    void setCompileErrors(const std::vector<compile_error> &compile_errors) {
        YaraCC::compile_errors = compile_errors;
    }

    int getWarnings() const {
        return warnings;
    }

    void setWarnings(int warnings) {
        YaraCC::warnings = warnings;
    }

    std::map<std::string, std::vector<resolved_match>> matched_rules = std::map<std::string, std::vector<resolved_match>>();
    std::vector<compile_error> compile_errors = std::vector<compile_error>();
    int warnings = 0;
};


#endif //LIBYARA_WASM_YARACC_HPP
