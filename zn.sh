#!/bin/sh

CC=clang
TARGET=zn

SOURCES="
    src/main.c
    src/lexer.c
    src/token.c
"

FLAGS="
    -std=c23
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -Wformat=2
    -Wundef
    -Wswitch-enum
    -fstack-protector-strong
    -fPIE
    -Og
    -fsanitize=address,undefined
"

build()
{
    $CC \
        $FLAGS \
        $SOURCES \
        -pie \
        -o "$TARGET"
}

run()
{
    "./$TARGET" "$1"
}

clean()
{
    rm -f "./$TARGET" zn-tmp-*
}

help()
{
    printf '%s\n' \
        "usage: ./zn.sh [build|run|clean]" \
        "" \
        "  build       Build $TARGET" \
        "  run FILE    Run a .zn file" \
        "  clean       Remove build files"
}

case "$1" in
    build)
        build
        ;;

    run)
        if [ -z "$2" ]; then
            printf '%s\n' "error: missing source file" >&2
            exit 1
        fi

        run "$2"
        ;;

    clean)
        clean
        ;;

    *)
        help
        exit 1
        ;;

esac
