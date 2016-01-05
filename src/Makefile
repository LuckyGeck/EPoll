all: main.cpp polls.cpp time_util.cpp storage/inmem.cpp storage/mongo.cpp
	g++ main.cpp polls.cpp time_util.cpp storage/inmem.cpp storage/mongo.cpp \
		--std=c++11 -fPIC -O2 \
		-I. -Irapidjson/include \
		-lfastcgi-daemon2 \
		-lmongoclient -lboost_thread -lboost_system -lboost_regex -lssl -lcrypto -lboost_filesystem -lpthread \
		-shared -o /target/libEPoll.so
