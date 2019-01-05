#include <iostream>
#include <string>
#include <vector>
#include <yara.h>
#include "stdio.h"
#include "util.hpp"

int main() {
    yr_initialize();

    YR_RULES* rules;
    YR_RULE* rule;

    std::vector<std::string> matches;
    char* buf = "foo bar";
    char* rules_str = "rule true_rule { condition: true }";

    yr_initialize();
    int result = compile_rule(rules_str, &rules);
    if (result != ERROR_SUCCESS)
    {
        fprintf(stderr, "failed to compile rule: %s\n", compile_error);
        perror("compile_rule");
        exit(EXIT_FAILURE);
    }
    yr_rules_scan_mem(rules, (uint8_t *) buf, strlen(buf), 0, count_matches, &matches, 0);
    std::cout << "Total matches: " << matches.size() << std::endl << "Matched rules:" << std::endl;
    for (auto const& match : matches) {
        std::cout << match << std::endl;
    }
    yr_finalize();
    return 0;
}
