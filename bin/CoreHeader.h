/**
*  @file CoreHeader.h
*  @date 10/11/2020
*  @author MetalMario971
*/
#pragma once
#ifndef __COREHEADER_16050938396409681455028822908_H__
#define __COREHEADER_16050938396409681455028822908_H__


namespace BR2 {
/**
*  @class CoreHeader
*  @brief
*/
class CoreHeader : public VirtualMemoryShared<CoreHeader> {
public:
    CoreHeader();
    virtual ~CoreHeader() override;
};

}//ns Game



#endif
