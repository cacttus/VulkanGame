/**
*  @file Singleton.h
*  @date 10/07/2020
*  @author MetalMario971
*/
#pragma once
#ifndef __SINGLETON_16047914072329720466646877649_H__
#define __SINGLETON_16047914072329720466646877649_H__


namespace BR2 {
/**
*  @class Singleton
*  @brief
*/
class Singleton : public VirtualMemoryShared<Singleton> {
public:
    Singleton();
    virtual ~Singleton() override;
};

}//ns Game



#endif
