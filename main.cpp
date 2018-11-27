#include <iostream>
#include <yara.h>
#include "util.hpp"

void test_disabled_rules()
{
    YR_RULES* rules;
    YR_RULE* rule;

    int matches = 0;
    char* buf = "foo bar";
    char* rules_str = " \
    rule disabled_rule {condition: true} \
    rule false_rule {condition: true and disabled_rule} \
    rule true_rule {condition: true or disabled_rule}";


    yr_initialize();

    if (compile_rule(rules_str, &rules) != ERROR_SUCCESS)
    {
        perror("compile_rule");
        exit(EXIT_FAILURE);
    }

    // diable any rule containing disable in its identifier
    yr_rules_foreach(rules, rule)
    {
        if (strstr(rule->identifier, "disabled"))
            yr_rule_disable(rule);
    }

    yr_rules_scan_mem(
            rules, (uint8_t *) buf, strlen(buf), 0, count_matches, &matches, 0);

    yr_rules_destroy(rules);

    // matches should be exactly one.
    if (matches != 1)
    {
        fprintf(stderr, "test_disabled_rules failed\n");
        exit(EXIT_FAILURE);
    }

    yr_finalize();
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    test_disabled_rules();
    return 0;
}