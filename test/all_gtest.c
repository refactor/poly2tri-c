#ifndef __APPLE__
#define _GNU_SOURCE
#endif
#include "../src/mylog.h"
#include "greatest.h"

#include "simple_gtest.h"
#include "i18_gtest.h"
#include "comb_gtest.h"
#include "hand_gtest.h"

GREATEST_MAIN_DEFS();

int main(int argc, char* argv[]) {
    INFO("vidx_t.sz: %zu, coord_t.sz: %zu", sizeof(vidx_t), sizeof(coord_t));

    GREATEST_MAIN_BEGIN();

    RUN_SUITE(simple_suite);
    RUN_SUITE(the_i18_suite);
    RUN_SUITE(the_comb_suite);
    RUN_SUITE(the_hand_suite);

    GREATEST_MAIN_END();
}
