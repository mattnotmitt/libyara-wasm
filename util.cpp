#include "util.hpp"
#include <iostream>


static void callback_function(
        int error_level,
        const char *file_name,
        int line_number,
        const char *message,
        void *user_data) {
    auto resp = (YaraCC *) user_data;
    //std::cout << line_number << std::endl;
    YaraCC::compile_error error = {message, line_number, error_level == YARA_ERROR_LEVEL_WARNING};
    resp->compile_errors.push_back(error);
}


int compile_rule(
        char *string,
        YR_RULES **rules,
        YaraCC* resp) {
    YR_COMPILER *compiler = nullptr;
    int result;


    if (yr_compiler_create(&compiler) != ERROR_SUCCESS) {
        YaraCC::compile_error error = {"Compiler creation failed.", 0, 0};
        resp->compile_errors.push_back(error);
        return ERROR_NOT_INDEXABLE;
    }
    //std::cout << "Compiler created!" << std::endl;
    yr_compiler_set_callback(compiler, callback_function, resp);
    //std::cout << "Compiler callback added!" << std::endl;
    if (yr_compiler_add_string(compiler, string, nullptr) != 0) {
        result = compiler->last_error;
        yr_compiler_destroy(compiler);
        return result;
    }
    //std::cout << "Rule compiled!" << std::endl;
    result = yr_compiler_get_rules(compiler, rules);
    //std::cout << result << std::endl;
    yr_compiler_destroy(compiler);
    return result;
}


int get_matched_rules(
        int message,
        void *message_data,
        void *user_data) {
    if (message == CALLBACK_MSG_RULE_MATCHING) {
        auto resp = (YaraCC *) user_data;
        auto rule = (YR_RULE *) message_data;
        YR_STRING *string;
        std::vector<YaraCC::resolved_match> resolved_matches;
        yr_rule_strings_foreach(rule, string) {
            YR_MATCH *match;
            yr_string_matches_foreach(string, match) {
                YaraCC::resolved_match rmatch = {
                        match->base + match->offset,
                        match->match_length,
                        std::string((char *) match->data, match->data_length),
                        string->identifier,
                        match->data_length
                };
                resolved_matches.push_back(rmatch);
            }
        }
        YR_META* meta;
        std::vector<YaraCC::meta> metadata;
        yr_rule_metas_foreach(rule, meta)
        {
            std::stringstream metaString;
            if (meta->type == META_TYPE_INTEGER)
            {
                metaString << meta->integer;
            }
            else if (meta->type == META_TYPE_BOOLEAN)
            {
                metaString << (meta->integer ? "true" : "false");
            }
            else
            {
                metaString << meta->string;
            }
            metadata.push_back((YaraCC::meta) {meta->identifier, metaString.str()});
        }
        resp->matched_rules.push_back({rule->identifier, resolved_matches, metadata});
    }

    return CALLBACK_CONTINUE;
}
