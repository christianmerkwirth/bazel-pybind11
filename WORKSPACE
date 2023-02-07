workspace(name = "org_mizux_bazelpybind11")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")


http_archive(
    name = "bazel_skylib",
    sha256 = "f7be3474d42aae265405a592bb7da8e171919d74c16f082a5457840f06054728",  # Last updated 2022-05-18
    urls = ["https://github.com/bazelbuild/bazel-skylib/releases/download/1.2.1/bazel-skylib-1.2.1.tar.gz"],
)


http_archive(
    name = "com_google_absl",
    sha256 = "44634eae586a7158dceedda7d8fd5cec6d1ebae08c83399f75dd9ce76324de40",  # Last updated 2022-05-18
    strip_prefix = "abseil-cpp-3e04aade4e7a53aebbbed1a1268117f1f522bfb0",
    urls = ["https://github.com/abseil/abseil-cpp/archive/3e04aade4e7a53aebbbed1a1268117f1f522bfb0.zip"],
)    

git_repository(
    name = "com_google_googletest",
    tag = "release-1.12.1",
    remote = "https://github.com/google/googletest.git",
)

# Python
git_repository(
    name = "rules_python",
    tag = "0.16.2",
    remote = "https://github.com/bazelbuild/rules_python.git",
)

git_repository(
    name = "pybind11_bazel",
    commit = "faf56fb3df11287f26dbc66fdedf60a2fc2c6631",
    patches = ["//patches:pybind11_bazel.patch"],
    patch_args = ["-p1"],
    remote = "https://github.com/pybind/pybind11_bazel.git",
)

new_git_repository(
    name = "pybind11",
    build_file = "@pybind11_bazel//:pybind11.BUILD",
    tag = "v2.10.3",
    remote = "https://github.com/pybind/pybind11.git",
)


# -------- Load and call dependencies of underlying libraries --------

load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")

bazel_skylib_workspace()

load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(name = "local_config_python", python_version = "3")
