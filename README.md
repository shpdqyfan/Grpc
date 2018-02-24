# Grpc

Folder structure：                                                                                         
Grpc                                                                                                                                      
|—— build                                                                                                                                 
|—— |—— CMakeLists.txt -----------------Compile Grpc project                                                                               
|—— Lib                                                                                                                                   
|—— |—— CMakeLists.txt -----------------Compile Lib target                                                                                 
|—— |—— include -------------------------Expose the header files of Lib                                                                   
|—— |—— Semaphore                                                                                                               
|—— |—— |—— CMakeLists.txt -----------Compile Semaphore target within Lib                                                                 
|—— |—— Thread                                                                                                               
|—— |—— |—— CMakeLists.txt -----------Compile Thread target within Lib                                                                     
|—— Grpc                                                                                                                                   
|—— |—— CMakeLists.txt -----------------Compile grpc target of Grpc project                                                               
|—— src                                                                                                                                   
|—— |—— main.cpp                                                                                                                         
|—— Proto                                                                                                                                 
|—— |—— gnmi.proto                                                                                                                         
|—— doc
