
# This Starlark rule imports the NIKSS shared libraries and headers since there is
# not yet any native support for Bazel in NIKSS. The NIKSS_INSTALL environment
# variable needs to be set, otherwise the Stratum rules which depend on NIKSS
# cannot be built.

def _impl(repository_ctx):
    if "NIKSS_INSTALL" not in repository_ctx.os.environ:
        repository_ctx.file("BUILD", "")
        return
    nikss_path = repository_ctx.os.environ["NIKSS_INSTALL"]
    repository_ctx.symlink(nikss_path, "nikss-bin")
    repository_ctx.symlink(Label("@//bazel:external/nikss.BUILD"), "BUILD")

nikss_configure = repository_rule(
    implementation = _impl,
    local = True,
    environ = ["NIKSS_INSTALL"],
)
