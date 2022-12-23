
load("@rules_cc//cc:defs.bzl", "cc_import", "cc_library")

package(
    default_visibility = ["//visibility:public"],
)

cc_library(
    name = "nikss_headers",
    hdrs = glob([
        "nikss-bin/include/**/*.h",
        "nikss-bin/include/**/*.c",
    ]),
    includes = ["nikss-bin/include"],
)

cc_import(
    name = "nikss",
    hdrs = [],  # see cc_library rule above
    shared_library = "nikss-bin/lib/libnikss.so",
)



