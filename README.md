# Grpc

Folder structure：                                                                                         
Grpc                                                                                                                                      
|—— build                                                                                                                                 
|—— |—— CMakeLists.txt -----------------Compile Grpc project                                                                               
|—— Lib                                                                                                                                   
|—— |—— CMakeLists.txt -----------------Compile Lib target                                                                                 
|—— |—— include -------------------------Expose the header files of Lib                                                                   
|—— |—— Semaphore                                                                                                               
|—— |—— |—— CMakeLists.txt -----------Compile Semaphore as a sub target of Lib                                                             
|—— |—— Thread                                                                                                               
|—— |—— |—— CMakeLists.txt -----------Compile Semaphore as a sub target of Lib                                                                                                                                                                                                       |—— Grpc                                                                                                                                   
|—— |—— CMakeLists.txt -----------------Compile grpc target of Grpc project                                                               
|—— src                                                                                                                                   
|—— |—— main.cpp                                                                                                                         
|—— Proto                                                                                                                                 
|—— |—— gnmi.proto                                                                                                                         
|—— doc
 
