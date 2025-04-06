all: proxy_manager.exe proxy.exe upgrade.exe

proxy_manager.exe: proxy_manager.cpp
	g++ -o proxy_manager.exe proxy_manager.cpp -O3 -static -mwindows -lurlmon 

proxy.exe: proxy.cpp getProxy.txt.hpp
	g++ -o proxy.exe         proxy.cpp         -O3 -static -mwindows -lwbemuuid -lole32

upgrade.exe: upgrade.cpp getProxy.txt.hpp
	g++ -o upgrade.exe 		 upgrade.cpp 	   -O3 -static -mwindows -lurlmon