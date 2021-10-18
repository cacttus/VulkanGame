/**
*  @file Environment.h
*  @date 09/16/2021
*  @author MetalMario971
*/
#pragma once
#ifndef __ENVIRONMENT_16344127901200704596810232481_H__
#define __ENVIRONMENT_16344127901200704596810232481_H__


namespace BR2 {
/**
*  @class Environment
*  @brief
*/
class Environment : public VirtualMemoryShared<Environment> {
public:
    Environment();
    virtual ~Environment() override;
};

}//ns Game



#endif
