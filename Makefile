dependency:
	cd build && cmake .. --graphviz=graph.dot && dot -Tpng graph.dot -o graphImage.png

prepare:
ifeq ($(OS),Windows_NT)
	if exist build rmdir /s /q build
else
	rm -rf build
endif
	mkdir build
	
conan_d:
	rm -rf build
	mkdir build
	cd build && conan install .. -s build_type=Debug -s compiler.cppstd=17 --output-folder=. --build missing

conan_r:
	rm -rf build
	mkdir build
	cd build && conan install .. -s build_type=Release -s compiler.cppstd=17 --output-folder=. --build missing