load(
    "//bazel:rules.bzl",
    "STRATUM_INTERNAL",
    "stratum_cc_library",
)

licenses(["notice"])  # Apache v2

package(
    default_visibility = STRATUM_INTERNAL,
)

stratum_cc_library(
    name = "nikss_switch",
    srcs = ["nikss_switch.cc"],
    hdrs = ["nikss_switch.h"],
    deps = [
        ":nikss_chassis_manager",
        ":nikss_node",
        "//stratum/glue:integral_types",
        "//stratum/glue:logging",
        "//stratum/glue/gtl:map_util",
        "//stratum/glue/status:status_macros",
        "//stratum/hal/lib/common:phal_interface",
        "//stratum/hal/lib/common:switch_interface",
        "//stratum/lib:constants",
        "//stratum/lib:macros",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_googleapis//google/rpc:status_cc_proto",
    ]
)

stratum_cc_library(
    name = "nikss_chassis_manager",
    srcs = ["nikss_chassis_manager.cc"],
    hdrs = ["nikss_chassis_manager.h"],
    deps = [
        "//stratum/glue:integral_types",
        "//stratum/glue:logging",
        "//stratum/glue/gtl:map_util",
        "//stratum/glue/status:status_macros",
        "//stratum/hal/lib/common:phal_interface",
        "//stratum/hal/lib/common:switch_interface",
        "//stratum/hal/lib/nikss:nikss_interface",
        "//stratum/lib:constants",
        "//stratum/lib:macros",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/synchronization",
        "@com_google_googleapis//google/rpc:status_cc_proto",
        "@local_nikss_bin//:nikss",
    ]
)

stratum_cc_library(
    name = "nikss_interface",
    hdrs = ["nikss_interface.h"],
    deps = [
        "//stratum/glue:integral_types",
        "//stratum/glue/status",
        "//stratum/glue/status:statusor",
        "//stratum/hal/lib/common:common_cc_proto",
        "//stratum/hal/lib/common:utils",
        "//stratum/lib/channel",
        "@com_google_absl//absl/base:core_headers",
    ],
)

stratum_cc_library(
    name = "nikss_wrapper",
    srcs = ["nikss_wrapper.cc"],
    hdrs = ["nikss_wrapper.h"],
    deps = [
        ":nikss_interface",
        "//stratum/glue:integral_types",
        "//stratum/glue:logging",
        "//stratum/glue/gtl:map_util",
        "//stratum/glue/gtl:stl_util",
        "//stratum/glue/status",
        "//stratum/glue/status:statusor",
        "//stratum/hal/lib/common:common_cc_proto",
        "//stratum/hal/lib/p4:utils",
        "//stratum/lib:constants",
        "//stratum/lib:utils",
        "//stratum/lib/channel",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/cleanup",
        "@com_google_absl//absl/container:flat_hash_map",
        "@local_nikss_bin//:nikss",
    ],
)

stratum_cc_library(
    name = "nikss_node",
    srcs = ["nikss_node.cc"],
    hdrs = ["nikss_node.h"],
    deps = [
        ":nikss_interface",
        ":nikss_chassis_manager",
        "//stratum/glue:integral_types",
        "//stratum/glue:logging",
        "//stratum/glue/status:status_macros",
        "//stratum/hal/lib/common:common_cc_proto",
        "//stratum/hal/lib/common:proto_oneof_writer_wrapper",
        "//stratum/hal/lib/common:writer_interface",
        "//stratum/lib:constants",
        "//stratum/lib:macros",
        "//stratum/lib:utils",
        "//stratum/public/proto:error_cc_proto",
        "@com_github_p4lang_p4runtime//:p4runtime_cc_grpc",
        "@com_google_googleapis//google/rpc:status_cc_proto",
    ],
)
