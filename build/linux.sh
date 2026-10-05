#!/usr/bin/env bash
# Builds RuneSchema's Linux library (main.so, for the dedicated server) in the UE4SS builder image.
# UE4SS is built from a local checkout of the Linux fork inside the same CMake project.
#   build/linux.sh                          build main.so
#   build/linux.sh src/Utility/Config.cpp   compile single files (paths relative to source/raw)
#   build/linux.sh UE4SS                    build any other target
# Env: UE4SS_SRC (default ../ue4ss-loonix), RUNESCHEMA_LINUX_BUILD (default
# ue4ss-linux-build/ubuntu/rs-build), UE4SS_BUILDER_IMAGE.
set -euo pipefail
repo=$(cd "$(dirname "$0")/.." && pwd)
src=$(cd "${UE4SS_SRC:-$repo/../ue4ss-loonix}" && pwd)
out=${RUNESCHEMA_LINUX_BUILD:-$repo/ue4ss-linux-build/ubuntu/rs-build}
image=${UE4SS_BUILDER_IMAGE:-localhost/ue4ss-builder:ubuntu24.04}
# main.so and libUE4SS.so each link libstdc++ statically: build both from this commit's source.
# Later commits that touch only CI or Markdown are accepted.
pin=7c26b8c
git -C "$src" diff --quiet "$pin" -- . ':!.github' ':!*.md' || { echo "$src: UE4SS source differs from $pin" >&2; exit 1; }

targets=()
for arg in "$@"; do
  case $arg in
    *.cpp) targets+=("CMakeFiles/RuneSchema.dir/${arg#source/raw/}.o") ;;
    *) targets+=("$arg") ;;
  esac
done
[ ${#targets[@]} -gt 0 ] || targets=(RuneSchema)

mkdir -p "$out"
podman run --rm -v "$repo:/rs:ro" -v "$src:/src:ro" -v "$out:/out" -e CARGO_HOME=/out/cargo \
  "$image" bash -euo pipefail -c '
    [ -f /out/build.ninja ] || cmake -S /rs/source/raw -B /out -G Ninja \
      -DCMAKE_BUILD_TYPE=Game__Shipping__Linux -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
      -DCMAKE_INSTALL_LIBDIR=lib -DFETCHCONTENT_SOURCE_DIR_UE4SS=/src -DRUNESCHEMA_ENABLE_IPO=OFF \
      -DBUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    cmake --build /out --target "$@" -- -k 0' build "${targets[@]}"
find "$out" \( -name main.so -o -name libUE4SS.so \) -print | sort
