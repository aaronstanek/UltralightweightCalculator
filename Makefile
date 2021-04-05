ucalc : source/main.cpp source/*/*.cpp source/*/*.h
	g++ -std=c++11 -o ucalc -Os -s -fno-rtti -ffunction-sections source/main.cpp source/*/*.cpp