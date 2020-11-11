/**
*  @file GraphicsDriver.h
*  @date 10/11/2020
*  @author MetalMario971
*/
#pragma once
#ifndef __GRAPHICSDRIVER_1605093757691494556638447457_H__
#define __GRAPHICSDRIVER_1605093757691494556638447457_H__

#include "../core/CoreHeader.h"

namespace VG {
/**
*  @class GraphicsDriver
*  @brief
*/
class GraphicsDriver : public VirtualMemoryShared<GraphicsDriver> {
public:
    GraphicsDriver();
    virtual ~GraphicsDriver() override;
    void checkErrors(){}
    void updateLoop(){}
    void cleanup(){}
};

}//ns Game



#endif
