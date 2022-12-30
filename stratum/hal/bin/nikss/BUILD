load("//bazel:rules.bzl", "HOST_ARCHES", "stratum_cc_binary", "stratum_license_tar")
load("@rules_pkg//:pkg.bzl", "pkg_deb", "pkg_tar")

licenses(["notice"])  # Apache v2

package(
    default_visibility = ["//visibility:public"],
)

stratum_cc_binary(
    name = "stratum_nikss",
    srcs = [
        "main.cc",
    ],
    arches = HOST_ARCHES,
    deps = [
        "//stratum/glue:init_google",
        "//stratum/glue:logging",
        "//stratum/hal/lib/nikss:nikss_chassis_manager",
        "//stratum/hal/lib/nikss:nikss_wrapper",
        "//stratum/hal/lib/nikss:nikss_node",
        "//stratum/hal/lib/nikss:nikss_switch",
        "//stratum/hal/lib/common:hal",
        "//stratum/hal/lib/phal:phal_sim",
        "//stratum/lib/security:auth_policy_checker",
        "//stratum/lib/security:credentials_manager",
        "@com_github_google_glog//:glog",
    ],
)

