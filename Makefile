all: proxy_manager.exe proxy.exe

proxy_manager.exe: proxy_manager.cpp 
	g++ -o proxy_manager.exe proxy_manager.cpp -O3 -static -mwindows -lurlmon 

proxy.exe: proxy.cpp
	g++ -o proxy.exe         proxy.cpp         -O3 -static -mwindows -lwbemuuid -lole32
