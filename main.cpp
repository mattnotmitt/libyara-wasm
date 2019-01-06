#include <iostream>
#include <emscripten/bind.h>
#include "util.hpp"

std::map<std::string, std::vector<resolved_match>> run(const std::string &buf_str, const std::string &rules_str) {
    YR_RULES* rules;
    std::map<std::string, std::vector<resolved_match>> matched_rules;

    char* buf = const_cast<char*>(buf_str.c_str());
    char* rules_chr = const_cast<char*>(rules_str.c_str());

    yr_initialize();
    int result = compile_rule(rules_chr, &rules);
    if (result != ERROR_SUCCESS)
    {
        fprintf(stderr, "failed to compile rule: %s\n", compile_error);
        perror("compile_rule");
        exit(EXIT_FAILURE);
    }
    yr_rules_scan_mem(rules, (uint8_t *) buf, strlen(buf), 0, get_matched_rules, &matched_rules, 0);
    yr_finalize();
    std::cout << matched_rules["bar"][0].location << std::endl;
    return matched_rules;
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::value_object<resolved_match>("resolved_match")
            .field("location", &resolved_match::location)
            .field("match_length", &resolved_match::match_length)
            .field("data", &resolved_match::data);
    emscripten::register_vector<resolved_match>("vector<resolved_match>");
    emscripten::register_map<std::string, std::vector<resolved_match>>("map<string, vector<resolved_match>>");
    emscripten::function("run", &run);
}

