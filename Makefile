cpp = g++
out = final
tout = test
libs = 
sources = ./conf.cpp ./keys.cpp
conf = ./conf.cpp
s_test = 
m ?= 

compile:
	$(cpp) $(sources) $(libs) -o $(out)
	clear
clear:
	rm $(out)
	clear

crun:
	$(cpp) $(sources) $(libs) -o $(out)
	./$(out)
	rm $(out)

conf:
	$(cpp) $(conf) $(libs) -o $(out)
	./$(out)
	rm $(out)

test:
	$(cpp) $(s_test) -o $(tout)
	./$(tout)
	rm $(tout)

save:
	git add .
	git commit -m "$(m)"
	clear

push:
	git push master

deps:
	sudo apt-get install g++ build-essential libx11-dev libglu1-mesa-dev freeglut3-dev libglew2.2 libglew-dev libglu1-mesa libgl1-mesa-glx libgl1-mesa-dev libglfw3-dev libglfw3
	git clone https://github.com/glfw/glfw.git
	cd glfw
	cmake -G "Unix Makefiles"
	make
	sudo make install
	cd ..
	rm -r glfw
	clear
