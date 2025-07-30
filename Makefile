.PHONY: configure-debug configure-release debug release run-debug run-release clean

TARGET = mytop
BUILD_DIR_DEBUG = build/debug
BUILD_DIR_RELEASE = build/release

configure-debug:
	mkdir -p $(BUILD_DIR_DEBUG)
	cd $(BUILD_DIR_DEBUG) && cmake -DCMAKE_BUILD_TYPE=Debug ../..

configure-release:
	mkdir -p $(BUILD_DIR_RELEASE)
	cd $(BUILD_DIR_RELEASE) && cmake -DCMAKE_BUILD_TYPE=Release ../..

build_debug: configure-debug
	cd $(BUILD_DIR_DEBUG) && make

build_release: configure-release
	cd $(BUILD_DIR_RELEASE) && make

debug: build_debug
	./$(BUILD_DIR_DEBUG)/$(TARGET)

run: build_release
	./$(BUILD_DIR_RELEASE)/$(TARGET)

clean:
	rm -rf build

