#include <iostream>
#include <emscripten/bind.h>
#include "util.hpp"
#include "YaraCC.hpp"

YaraCC run(const std::string &buf_str, const std::string &rules_str) {
    // Init variables
    YaraCC resp;
    YR_RULES* rules;
    // Convert C++ strings to terrible C pointer arrays
    auto buf = (unsigned char*) buf_str.c_str();
    char* rules_chr = const_cast<char*>(rules_str.c_str());

    yr_initialize(); // Init YARA
    int result = compile_rule(rules_chr, &rules, &resp); // Compile inputted rules
    if (result != ERROR_SUCCESS) // If failure, return to JS
    {
        return resp;
    }
    // Scan data in buf with rules
    yr_rules_scan_mem(rules, buf, buf_str.size(), 0, get_matched_rules, &resp, 0);
    // Stop YARA
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
            .property("matchedRules", &YaraCC::getMatchedRules, &YaraCC::setMatchedRules);
    emscripten::register_vector<std::string>("vectorString");

    emscripten::register_vector<YaraCC::meta>("vectorMeta");
    emscripten::register_vector<YaraCC::compile_error>("vectorCompileError");
    emscripten::register_vector<YaraCC::resolved_match>("vectorResolvedMatch");
    emscripten::register_vector<YaraCC::matched_rule>("vectorMatchedRule");

    emscripten::value_object<YaraCC::meta>("meta")
            .field("identifier", &YaraCC::meta::identifier)
            .field("data", &YaraCC::meta::data);
    emscripten::value_object<YaraCC::matched_rule>("matchedRule")
            .field("ruleName", &YaraCC::matched_rule::rule_name)
            .field("resolvedMatches", &YaraCC::matched_rule::resolved_matches)
            .field("metadata", &YaraCC::matched_rule::metadata);
    emscripten::value_object<YaraCC::resolved_match>("resolvedMatch")
            .field("location", &get_resolved_match_location, &set_resolved_match_location)
            .field("matchLength", &YaraCC::resolved_match::match_length)
            .field("data", &YaraCC::resolved_match::data)
            .field("stringIdentifier", &YaraCC::resolved_match::string_identifier)
            .field("dataLength", &YaraCC::resolved_match::data_length);

    emscripten::value_object<YaraCC::compile_error>("compileError")
            .field("message", &YaraCC::compile_error::message)
            .field("lineNumber", &YaraCC::compile_error::line_number)
            .field("warning", &YaraCC::compile_error::warning);

    emscripten::function("run", &run);
}

