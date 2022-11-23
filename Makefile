ifdef RELEASE
BUILD_DIR := target
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=Release
else
BUILD_DIR := debug
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=Debug
endif

NAME = $(BUILD_DIR)/examples/example
TEST_NAME = $(BUILD_DIR)/test/test

run: build
	$(NAME)

build: $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

$(BUILD_DIR):
	cmake -B $(BUILD_DIR) $(CMAKE_FLAGS) -S .

test: build
	$(TEST_NAME)

release:
	$(MAKE) RELEASE=1

.PHONY: run build release
