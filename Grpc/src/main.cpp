/*
    Date: 2018/01/23
    Author: shpdqyfan
    profile: grpc
*/

#include <iostream>
#include <memory>
#include <unistd.h>

#include "GrpcSessionMgr.h"

int main()
{
    std::cout<<"Hello, Grpc"<<std::endl;

    std::unique_ptr<GrpcSessionMgr> myGrpcSessionMgrPtr(new GrpcSessionMgr("10.13.13.108", 50051));

    myGrpcSessionMgrPtr->initialize();
    myGrpcSessionMgrPtr->activate();

    while(1)
    {
        sleep(1);
    }

    return 0;
}

