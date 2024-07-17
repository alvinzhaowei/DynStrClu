cmake_minimum_required(VERSION 3.25)
project(DynamicStrClu_ours)

set(CMAKE_CXX_STANDARD 17)

add_executable(DynamicStrClu_ours main.cpp graph/Jaccard.cpp graph/Vertex.cpp graph/Graph.cpp dt/DTBucket.cpp dt/DTInstance.cpp dt/DTManager.cpp MyLib/ParaReader.cpp MyLib/MyTimer.cpp)
