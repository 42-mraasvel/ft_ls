ifdef RELEASE
BUILD_DIR = target
CMAKE_FLAGS = -DCMAKE_BUILD_TYPE=Release
else
	ifdef TEST_BUILD
		BUILD_DIR = build_test
		CMAKE_FLAGS = -DCMAKE_BUILD_TYPE=Debug -DTestBuild=1
	else
		BUILD_DIR = debug
		CMAKE_FLAGS = -DCMAKE_BUILD_TYPE=Debug
	endif
endif

NAME = $(BUILD_DIR)/src/ft_ls
TEST_NAME = $(BUILD_DIR)/test/test

run: build
	$(NAME)

build: $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

$(BUILD_DIR):
	cmake -B $(BUILD_DIR) $(CMAKE_FLAGS) -S .

test:
	$(MAKE) run_test TEST_BUILD=1

run_test: build
	$(TEST_NAME)

system_test:
	$(MAKE) run_system_test TEST_BUILD=1

run_system_test: build
	cd system_test && FT_LS_PATH="$(NAME)" bash test_ls.sh

release:
	$(MAKE) RELEASE=1

.PHONY: run build release test system_test
