# Running Stratum with NIKSS (P4 eBPF target)

### Building and running `stratum_nikss` for development

You can build and run the Stratum binary directly using the following
Bazel command:

```bash
# build Docker image with build deps (only once)
# TODO: this step will be removed once stratum_nikss is merged with upstream
docker build \
  -t stratumproject/build:build \
  -f Dockerfile.build .

setup_dev_env.sh  # You're inside the Docker container now
bazel build //stratum/hal/bin/nikss:stratum_nikss

# run stratum_nikss
bazel-bin/stratum/hal/bin/nikss/stratum_nikss
```
