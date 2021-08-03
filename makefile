all : bc cc

bc : _graph.h _graph.cpp mainbc.cpp btw.cu general.cu general.cuh 
	nvcc -g -G -arch=sm_50  -Wno-deprecated-gpu-targets  general.cu btw.cu mainbc.cpp _graph.cpp -o bc
	
cc :  _graph.h _graph.cpp maincc.cpp cc.cu 
	nvcc -g -G -arch=sm_50  -Wno-deprecated-gpu-targets  cc.cu maincc.cpp _graph.cpp -o cc
