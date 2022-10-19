build_folder = ./build/
bin_folder = ./bin/
jobs := $(or $(jobs),$(j))
buildargs := $(or $(buildargs),$(b))
configargs := $(or $(configargs),$(c))
testargs := $(or $(testargs),$(t))

.PHONY: build
build:
	cmake --build build -j${jobs} ${buildargs}

.PHONY: %-gcc %-clang %-msvc
%-gcc %-clang %-msvc: clean
	cmake . --preset $@ ${configargs}

.PHONY: test
test:
	ctest --output-on-failure --test-dir build -j${jobs} ${testargs}

.PHONY: clean
clean:
	rm -rf $(build_folder) $(bin_folder)
