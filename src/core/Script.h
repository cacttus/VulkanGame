/**
*  @file Script.h
*  @date 20200304
*  @author MetalMario971
*/
#pragma once
#ifndef __SCRIPT_15833810871154937548_H__
#define __SCRIPT_15833810871154937548_H__

#include "../core/Component.h"

namespace VG {
/**
*  @class Script
*  @brief Base script component. This class by itself is the interface for compiled (c++) scripts.
*/
class Script : public Component {
public:
  Script();
  virtual ~Script() override;

  virtual void onDrawUi() {}
  virtual void onIdle(t_timeval ms) {}
  virtual void onScreenChanged(uint32_t uiWidth, uint32_t uiHeight, bool bFullscreen) {}
  virtual void onServerPacketReceived(std::shared_ptr<Packet> pack) {}
};

}//ns Game



#endif
