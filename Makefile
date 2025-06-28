TARGET = mytop

build:
	mkdir -p build
	cd build && cmake .. && make

run: build
	./build/$(TARGET)

clean:
	rm -rf build
