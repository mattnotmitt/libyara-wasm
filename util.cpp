/*
Copyright (c) 2016. The YARA Authors. All Rights Reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <iostream>
#include "util.hpp"

char compile_error[1024];
int warnings;
struct resolved_match {
    long long location;
    long match_length;
    std::string data;
    long data_length;
};
static void callback_function(
        int error_level,
        const char *file_name,
        int line_number,
        const char *message,
        void *user_data) {
    if (error_level == YARA_ERROR_LEVEL_WARNING)
        (*((int *) user_data))++;

    snprintf(
            compile_error,
            sizeof(compile_error),
            "line %d: %s",
            line_number,
            message);
}


int compile_rule(
        char *string,
        YR_RULES **rules) {
    YR_COMPILER *compiler = nullptr;
    int result = ERROR_SUCCESS;

    compile_error[0] = '\0';
    warnings = 0;

    if (yr_compiler_create(&compiler) != ERROR_SUCCESS) {
        perror("yr_compiler_create");
        yr_compiler_destroy(compiler);
        return result;
    }

    yr_compiler_set_callback(compiler, callback_function, &warnings);

    if (yr_compiler_add_string(compiler, string, nullptr) != 0) {
        yr_compiler_destroy(compiler);
        return compiler->last_error;
    }

    result = yr_compiler_get_rules(compiler, rules);
    yr_compiler_destroy(compiler);
    return result;
}


int get_matched_rules(
        int message,
        void *message_data,
        void *user_data) {
    if (message == CALLBACK_MSG_RULE_MATCHING) {
        auto rule = (YR_RULE *) message_data;
        YR_STRING* string;
        auto matches = (std::map<std::string, std::vector<resolved_match>> *) user_data;
        std::vector<resolved_match> resolved_matches;
        //std::cout << rule->identifier << std::endl;
        yr_rule_strings_foreach(rule, string) {
            YR_MATCH* match;
            yr_string_matches_foreach(string, match) {
                resolved_match rmatch = {
                        match->base + match->offset,
                        match->match_length,
                        std::string((char *) match->data, match->data_length),
                        match->data_length
                };
                std::cout << typeid(rmatch).name() << std::endl;
                resolved_matches.push_back(rmatch);
            }
        }
        matches->emplace(rule->identifier, resolved_matches);
    }

    return CALLBACK_CONTINUE;
}
