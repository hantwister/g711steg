CXX := g++
CXXFLAGS := -g

COMMONC = main.cpp common/*.cpp common/g72x/*.c
COMMONH = common/*.hpp common/g72x/*.h common/g72x/spandsp/*.h common/g72x/spandsp/private/*.h
COMMON = $(COMMONC) $(COMMONH)

main-aoki: $(COMMON) aoki/*
	echo '#include "aoki/AokiStegAlgorithm.hpp"' > .class.hpp
	$(CXX) $(CXXFLAGS) -DCLASS=AokiStegAlgorithm $(COMMONC) aoki/*.cpp -lm -o main-aoki
	rm .class.hpp

main-ito: $(COMMON) ito/*
	echo '#include "ito/ItoStegAlgorithm.hpp"' > .class.hpp
	$(CXX) $(CXXFLAGS) -DCLASS=ItoStegAlgorithm $(COMMONC) ito/*.cpp -lm -o main-ito
	rm .class.hpp

main-lsb: $(COMMON) lsb/*
	echo '#include "lsb/LSBStegAlgorithm.hpp"' > .class.hpp
	$(CXX) $(CXXFLAGS) -DCLASS=LSBStegAlgorithm $(COMMONC) -lm -o main-lsb
	rm .class.hpp

main-miao: $(COMMON) miao/*
	echo '#include "miao/MiaoStegAlgorithm.hpp"' > .class.hpp
	$(CXX) $(CXXFLAGS) -DCLASS=MiaoStegAlgorithm $(COMMONC) miao/*.cpp -lm -o main-miao
	rm .class.hpp

miao-search: $(COMMON) miao/search/*
	$(CXX) $(CXXFLAGS) -std=gnu++0x miao/search/*.cpp -lm -o miao-search

main-neal: $(COMMON) neal/* ito/*
	echo '#include "neal/NealStegAlgorithm.hpp"' > .class.hpp
	$(CXX) $(CXXFLAGS) -DCLASS=NealStegAlgorithm $(COMMONC) ito/*.cpp -lm -o main-neal
	rm .class.hpp
