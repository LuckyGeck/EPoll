all: main.cpp polls.cpp storage/inmem.cpp
	g++ main.cpp polls.cpp storage/inmem.cpp \
		--std=c++11 -fPIC -O2 \
		-I. -Irapidjson/include\
		-lfastcgi-daemon2 \
		-shared -o /target/libEPoll.so
