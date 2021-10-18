/**
*  @file Locale.h
*  @date 09/16/2021
*  @author MetalMario971
*/
#pragma once
#ifndef __LOCALE_163441168612082086814585174165_H__
#define __LOCALE_163441168612082086814585174165_H__


namespace BR2 {
/**
*  @class Locale
*  @brief Dummy class for "actual" locale. Sets to current locale. Sets our use to wstring wprintf wcout .. 
*/
class Locale : public VirtualMemoryShared<Locale> {
public:
    Locale();
    virtual ~Locale() override;
    void init(); 
};

}//ns br2



#endif
