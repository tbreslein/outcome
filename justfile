BUILD_DIR := "./build"

build *args:
    cmake --build {{BUILD_DIR}} {{args}}

config-test *args: clean
    cmake -S. -B{{BUILD_DIR}} {{args}} -DOUTCOME_TEST=ON

test: build
    ctest --test-dir {{BUILD_DIR}}/test

clean:
    rm -rf {{BUILD_DIR}}
