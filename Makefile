build_folder = ./build/
buildargs := $(or $(buildargs),$(b))
configargs := $(or $(configargs),$(c))
testargs := $(or $(testargs),$(t))

.PHONY: build
build:
	cmake --build $(build_folder) ${buildargs}

.PHONY: config-test
config-test: clean
	cmake -S. -Bbuild ${configargs} -DOUTCOME_TEST=ON -DOUTCOME_USE_ERRORREPORT=ON -DOUTCOME_USE_MACROS=ON

.PHONY: test
test:
	./build/test/test -a ${testargs}

.PHONY: clean
clean:
	rm -rf $(build_folder)
