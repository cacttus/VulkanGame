/**
*  @file XmlFile.h
*  @date August 4, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __XMLFILE_15649469792082348406_H__
#define __XMLFILE_15649469792082348406_H__

#include "../base/TextConfigFile.h"

namespace VG {
class PackageConfiguration;
/**
*  @class XmlFile
*  @brief Brief interface that handles loading the XML file into a pugixml document.
*/
class XmlFile : public TextConfigFile {
public:
  XmlFile();
  XmlFile(const string_t& loc);
  virtual ~XmlFile() override;

  static std::shared_ptr<PackageConfiguration> getXMLConfiguration(const string_t& filepath);

protected:
  virtual void parse(char* buf, int64_t size) override;
  virtual void pkp(std::vector<string_t>& tokens) override = 0;
};

}//ns BR2



#endif
