#include <iostream>
#include <emscripten/bind.h>
#include "util.hpp"
#include "YaraCC.hpp"

YaraCC run(const std::string &buf_str, const std::string &rules_str) {
    YR_RULES* rules;
    YaraCC resp;
    auto matched_rules = resp.matched_rules;

    char* buf = const_cast<char*>(buf_str.c_str());
    char* rules_chr = const_cast<char*>(rules_str.c_str());

    yr_initialize();
    int result = compile_rule(rules_chr, &rules, &resp);
    if (result != ERROR_SUCCESS)
    {
        //std::cout << resp.compile_errors.size() << std::endl;
        return resp;
    }
    yr_rules_scan_mem(rules, (uint8_t *) buf, strlen(buf), 0, get_matched_rules, &resp, 0);
    yr_finalize();
    return resp;
}

double get_resolved_match_location(const YaraCC::resolved_match &match) {
    return (double)match.location;
}

void set_resolved_match_location(YaraCC::resolved_match &match, double value) {
    match.location = (long long)value;
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::class_<YaraCC>("YaraCC")
            .property("compileErrors", &YaraCC::getCompileErrors, &YaraCC::setCompileErrors)
            .property("warnings", &YaraCC::getWarnings, &YaraCC::setWarnings)
            .property("matchedRules", &YaraCC::getMatchedRules, &YaraCC::setMatchedRules);

    emscripten::register_vector<YaraCC::compile_error>("vector<compileError>");
    emscripten::register_vector<YaraCC::resolved_match>("vector<resolvedMatch>");
    emscripten::register_vector<std::string>("vector<string>");
    emscripten::register_map<std::string, std::vector<YaraCC::resolved_match>>("map<string, vector<resolvedMatch>>");

    emscripten::value_object<YaraCC::resolved_match>("resolvedMatch")
            .field("location", &get_resolved_match_location, &set_resolved_match_location)
            .field("matchLength", &YaraCC::resolved_match::match_length)
            .field("data", &YaraCC::resolved_match::data)
            .field("dataLength", &YaraCC::resolved_match::data_length);

    emscripten::value_object<YaraCC::compile_error>("compileError")
            .field("message", &YaraCC::compile_error::message)
            .field("lineNumber", &YaraCC::compile_error::line_number)
            .field("warning", &YaraCC::compile_error::warning);

    emscripten::function("run", &run);
}

