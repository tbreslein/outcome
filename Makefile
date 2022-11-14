build_folder = ./build/
bin_folder = ./bin/
jobs := $(or $(jobs),$(j))
buildargs := $(or $(buildargs),$(b))
configargs := $(or $(configargs),$(c))
testargs := $(or $(testargs),$(t))

.PHONY: build
build:
	cmake --build build -j${jobs} ${buildargs}

.PHONY: config-example
config-examples: clean
	cmake . --preset examples ${configargs}

.PHONY: config-test
config-test: clean
	cmake . --preset test ${configargs}

.PHONY: config-all
config-all: clean
	cmake . --preset all ${configargs}

.PHONY: test
test:
	./build/test/tests -a

.PHONY: clean
clean:
	rm -rf $(build_folder)
