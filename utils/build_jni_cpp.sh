#!/usr/bin/env bash

# Some portions of this file where inspired from:
#   https://medium.com/@Drew_Stokes/bash-argument-parsing-54f3b81a6a8f

# If the current script is running in MacOS, print a warning
if [[ "$OSTYPE" == "darwin"* ]]; then

    # Define the realpath function, as MacOs doesn't have it
    realpath() {
        OURPWD=$PWD
        cd "$(dirname "$1")"
        LINK=$(readlink "$(basename "$1")")
        while [ "$LINK" ]; do
            cd "$(dirname "$LINK")"
            LINK=$(readlink "$(basename "$1")")
        done
        REALPATH="$PWD/$(basename "$1")"
        cd "$OURPWD"
        echo "$REALPATH"
    }

fi

# Parse incoming arguments
PARAMS=""
while (( "$#" )); do
  case "$1" in
    -c|--clean)
        FLAG_CLEAN=0
        shift
        ;;
    -d|--debug)
        FLAG_DEBUG=0
        shift
        ;;
    -sa|--sanitize_address)
        FLAG_SANITIZE_ADDRESS=0
        shift
        ;;
    -st|--sanitize_thread)
        FLAG_SANITIZE_THREAD=0
        shift
        ;;
    -*|--*=) # unsupported flags
        echo "Error: Unsupported flag $1" >&2
        exit 1
        ;;
    *) # preserve positional arguments
        PARAMS="$PARAMS $1"
        shift
        ;;
  esac
done

# Don't allow sanitize flags to coexist
if [ ! -z "$FLAG_SANITIZE_ADDRESS" ] && [ ! -z "$FLAG_SANITIZE_THREAD" ]; then
    echo "Error: Cannot use both --sanitize_address and --sanitize_thread" >&2
    exit 1
fi

# Set positional arguments in their proper place
eval set -- "$PARAMS"

# Extract extra build arguments from the positional arguments
EXTRA_BUILD_ARGS=${@:1}

PROJECT_ROOT=$(realpath $(dirname `realpath $0`)/..)
SOURCE_PATH=$PROJECT_ROOT/simplejavable/cpp
BUILD_PATH=$PROJECT_ROOT/build_simplejavable

# If FLAG_SANITIZE_ADDRESS is set, build the library with the sanitize address argument
if [[ ! -z "$FLAG_SANITIZE_ADDRESS" ]]; then
    BUILD_SANITIZE_ADDRESS_ARG="-D${LIB_NAME^^}_SANITIZE=Address"
fi

# If FLAG_SANITIZE_THREAD is set, build the library with the sanitize thread argument
if [[ ! -z "$FLAG_SANITIZE_THREAD" ]]; then
    BUILD_SANITIZE_THREAD_ARG="-D${LIB_NAME^^}_SANITIZE=Thread"
fi

# If FLAG_CLEAN is set, clean the build directory
if [[ ! -z "$FLAG_CLEAN" ]]; then
    rm -rf $BUILD_PATH
fi

if [[ ! -z "$FLAG_DEBUG" ]]; then
    DEBUG_ARG="-DCMAKE_BUILD_TYPE=Debug"
fi

cmake $DEBUG_ARG -H$SOURCE_PATH -B $BUILD_PATH $BUILD_SANITIZE_ADDRESS_ARG $BUILD_SANITIZE_THREAD_ARG $EXTRA_BUILD_ARGS
cmake --build $BUILD_PATH
