#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <time.h>
#include <math.h>
#include <memory>
#include <cstring>
#include <filesystem>
#include <sys/unistd.h>

#ifndef PROJECT_VERSION
#define PROJECT_VERSION "PROJECT_VERSION Undefined"
#endif

typedef unsigned int t_Date;

typedef enum { ext_none, ext_h, ext_cc, ext_hpp, ext_cpp, ext_java } t_ext;

std::string outp;

struct SWITCHES {
  static std::string company_name;
};
std::string SWITCHES::company_name = "";
std::string getHeader(std::string fileName,
                      std::string strAddExt = "");  // - CPP header
std::string getJava(std::string fileName);          // - Java file
std::string getSource(std::string fileName,
                      std::string strAddExt = "");  // CPP Source
t_ext getExt(std::string filename);
std::string getFilePart(std::string filename);
void output(std::string filename, std::string data, std::string outdir,
            std::string strAddExt = "");
std::string uppercase(std::string& in);
std::string lowercase(std::string in);
// std::string strdate();
void parseFiles(std::vector<std::string>& args, std::string);
void printHeaderInfo(const std::string& cwd);
void parseUserInput(std::vector<std::string>& args);
std::string replaceAll(const std::string& str, char charToRemove,
                       char charToAdd) {
  std::string::size_type x = 0;
  std::string ret = str;
  std::string ch;
  ch += charToAdd;
  while ((x = ret.find(charToRemove, x)) != std::string::npos) {
    ret.replace(x, 1, ch);
    x += ch.length();
  }
  return ret;
}
std::string formatPath(std::string p) { return replaceAll(p, '\\', '/'); }
std::string getFileNameFromPath(const std::string& name) {
  // Returns the TLD for the path. Either filename, or directory.
  // Does not include the / in the directory.
  // ex ~/files/dir would return dir
  // - If formatPath is true then we'll convert the path to a / path.
  std::string fn = "";
  fn = std::filesystem::path(name).filename();
  return fn;
}
std::string getDirectoryNameFromPath(const std::string& pathName) {
  // Returns the path without the filename OR top level directory.
  // See C# GetDirectoryName()
  std::string ret = "";

  std::string formattedPath;
  std::string tmpPath = pathName;
  std::string fn;

  formattedPath = formatPath(pathName);
  fn = getFileNameFromPath(formattedPath);
  size_t x = formattedPath.rfind('/');
  if (x != std::string::npos) {
    ret = formattedPath.substr(0, x);
  } else {
    ret = formattedPath;
  }
  return ret;
}

int main(int argc, char** argv) {
  // SWITCHES::company_name="Fulmination Interactive";
  std::vector<std::string> args;

  if (argc == 0) {
    return 0;
  }
  std::string dirname = getDirectoryNameFromPath(argv[0]);
  chdir(dirname.c_str());

  for (int i = 1; i < argc; ++i) {
    args.push_back(std::string(argv[i]));
  }

  if (argc == 1) {
    // - print title and get some input if user didn't add any command lien args
    printHeaderInfo(dirname);

    parseUserInput(args);
  }
  parseFiles(args, dirname);
}
void printHeaderInfo(const std::string& cwd) {
  std::cout << "|**************************************************************"
               "***************"
            << std::endl;
  std::cout << "|  C++/Java Class Generator v" << PROJECT_VERSION << "  "
            << std::endl;
  std::cout << "|  Created by github/MetalMario971" << std::endl;
  std::cout << "|      Type filenames separated by spaces. " << std::endl;
  std::cout
      << "|      For classes (h/cpp) type a class name without any extension."
      << std::endl;
  std::cout << "|      For Specific files use .h, .cpp, .hpp, .cxx, .hxx or "
               ".java extension."
            << std::endl;
  std::cout << "|   Options:       " << std::endl;
  std::cout
      << "|      Type \\c [company name] to include a company name.       "
      << std::endl
      << "|    cwd: " << cwd << std::endl
      << std::endl;
  std::cout << " > ";
}
void parseUserInput(std::vector<std::string>& args) {
  char buf[3000];
  std::memset(buf, 0, 3000);
  std::cin.getline(buf, 3000);
  int n = 0;
  int i = 0;
  char* c = buf;
  while (*c) {
    if (buf[i] == ' ') {
      args.push_back(std::string(buf + n, i - n));
      n = i + 1;
    }
    c++;
    i++;
  }
  args.push_back(std::string(buf + n, i - n));
}
void parseFiles(std::vector<std::string>& args, std::string outdir) {
  try {
    for (size_t i = 0; i < args.size(); ++i) {
      if (args[i] == lowercase("\\c")) {
        SWITCHES::company_name = args[i + 1];
      }
      t_ext e = getExt(args[i]);
      if (e == ext_h || e == ext_hpp) {
        outp = getHeader(args[i]);
        output(args[i], outp, outdir);
      } else if (e == ext_cpp || e == ext_cc) {
        outp = getSource(args[i]);
        output(args[i], outp, outdir);
      } else if (e == ext_java) {
        outp = getJava(args[i]);
        output(args[i], outp, outdir);
      } else if (e == ext_none) {
        outp = getHeader(args[i], "h");
        output(args[i], outp, outdir, "h");
        outp = getSource(args[i], "cpp");
        output(args[i], outp, outdir, "cpp");
      }
    }
  } catch (std::bad_exception* e) {
    std::cout << e->what();
    std::cin.get();
  }
}
std::string lowercase(std::string in) {
  std::string out;
  for (size_t i = 0; i < in.length(); ++i) {
    out += tolower(in[i]);
  }
  return out;
}
t_ext getExt(std::string filename) {
  size_t off = filename.rfind(".");

  if (off == std::string::npos) return ext_none;

  off += 1;

  if (off >= filename.size())
    throw new std::runtime_error(
        "Error: Incorrect file name, no extension given or incorrect "
        "extension.");

  if (!filename.substr(off, filename.size() - off).compare("h"))
    return ext_h;
  else if (!filename.substr(off, filename.size() - off).compare("hpp"))
    return ext_hpp;
  else if (!filename.substr(off, filename.size() - off).compare("cc"))
    return ext_cc;
  else if (!filename.substr(off, filename.size() - off).compare("cpp"))
    return ext_cpp;
  else if (!filename.substr(off, filename.size() - off).compare("java"))
    return ext_java;

  throw new std::runtime_error("Error: incorrect extension: " + std::to_string(off));
  return ext_cpp;
}
std::string uppercase(std::string& in) {
  for (size_t n = 0; n < in.size(); ++n) in[n] = toupper(in[n]);
  return in;
}
std::string getFilePart(std::string filename) {
  size_t off = filename.find(".");

  if (off == std::string::npos) return filename;

  if (off >= filename.size())
    throw new std::runtime_error("Incorrect file name.");

  return filename.substr(0, off);
}
int strToInt(const std::string& s) { return atoi(s.c_str()); }
t_Date strToDate(const std::string& szDate) {
  if (szDate.length() < 10) return 0;
  std::string copy(szDate);
  int m, d, y1, y2;
  char buf[3];
  buf[2] = '\0';
  buf[0] = szDate[0];
  buf[1] = szDate[1];
  m = strToInt(std::string((char*)buf));
  buf[0] = szDate[3];
  buf[1] = szDate[4];
  d = strToInt(std::string((char*)buf));
  buf[0] = szDate[6];
  buf[1] = szDate[7];
  y1 = strToInt(std::string((char*)buf));
  buf[0] = szDate[8];
  buf[1] = szDate[9];
  y2 = strToInt(std::string((char*)buf));
  t_Date dt = (t_Date)((m << (24)) | (d << (16)) | (y1 << (8)) | (y2));

  return dt;
}
std::string getDate() {
  char cptime[50];
  time_t now;
  now = time((time_t*)NULL);
  // time(&now); // shouldn't be needed right?
  struct tm* l_time = localtime(&now);
  sprintf(cptime, "%.2d/%.2d/%.4d", l_time->tm_mon, l_time->tm_mday,
          l_time->tm_year + 1900);
  std::string strTime = std::string(cptime);
  return strTime;
}
std::string getTime() {
  char cptime[50];
  time_t now;
  now = time((time_t*)NULL);
  // time(&now); // shouldn't be needed right?
  struct tm* l_time = localtime(&now);
  sprintf(cptime, "%.2d:%.2d:%.2d", l_time->tm_hour, l_time->tm_min,
          l_time->tm_sec);
  std::string strTime = std::string(cptime);
  return strTime;
}
std::string intToStr(const int i) { return std::to_string(i); }

std::string getCurrentDateCompact() { return getDate(); }
typedef std::vector<std::string> WordList;
typedef enum {
  WLT_NONE,
  WLT_NUMBERED,
  WLT_ALPHA_LOWERCASE,
  WLT_ALPHA_UPPERCASE,
  WLT_ROMAN_NUMERAL_LOWERCASE,
  WLT_ROMAN_NUMERAL_UPPERCASE
} t_word_list_type;
class OrdinalList {
 protected:
  std::vector<char> _list;
  int Base;  // - The base of the list
  virtual std::string getOrdinal(int pos) = 0;
};
class NumberOrdinalList : public OrdinalList {
  std::string getOrdinal(int pos) {
    std::string out;
    int div = pos % Base;
    while (div > 26) {
      std::string s = (char*)&_list[div %= Base];
      out = s + out;
    }
    return out;
  }
  NumberOrdinalList() {
    Base = 10;
    _list.push_back('0');
    _list.push_back('1');
    _list.push_back('2');
    _list.push_back('3');
    _list.push_back('4');
    _list.push_back('5');
    _list.push_back('6');
    _list.push_back('7');
    _list.push_back('8');
    _list.push_back('9');
  }
};
/*
class AlphaOrdinalList : public OrdinalList {
    static const _list[] = {'a','b','c','d','e','f','g','h','i','j','k',
        'l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};

    std::string getOrdinal(int pos){
        std::string out;
        int div = pos/Base;
        int mod = pos%Base;
        for( int i=0; i<div; i+=26 )
            out++_list[]

        out+=_list[mod];
            //log(3);
        return out;
    }

    AlphaOrdinalList(): Base(26){}
};*/
/**
 *    @class ListEnum
 *    @brief Returns enumeration indicators like 1,2, a,b, I,II etc..
 *
 */
class ListEnum {
  std::string getInteger(int ordinal) { return intToStr(ordinal); }
  std::string getAlphaLower(int ordinal) {
    // TODO:implement
    return "";
  }
  std::string getAlphaUpper(int ordinal) {
    // TODO:implement
    return "";
  }
  std::string getRomanNuimeralLower(int ordinal) {
    // TODO:implement
    return "";
  }
  std::string getRomanNumeralUpper(int ordinal) {
    // TODO:implement
    return "";
  }
};

std::string flatten_word_list(WordList& l) {
  std::string out;
  for (size_t i = 0; i < l.size(); ++i) {
    out += l[i];
  }
  return out;
}
/**
 *    @fn wrap_text()
 *
 *    @brief Nifty function, Wraps a block of text to a next line simulating a
 * word processor. Also does a bunch of utility things as well, such as line
 * ordinal numbering.
 *
 *    @param windows_escape - if we use \r\n or just \n (default false)
 *    @param tabs_to_spaces - If true we'll put spaces in place of tabs (default
 * true)
 *    @param tabspaces - the number of spaces to count for a tab (default 3)
 *    @param ignore_newlines - If true we'll ignore any user-newlines and just
 * put our own in, this has the effect of stripping out newlines. (default false
 *    @param newline_sentinel - What we're going to put right after the newline,
 * at the left side of the new block of text (default "")
 *    @param line_ordinals - The type of indicator for a list beginning at each
 * line. The list will put a 1)2) or I)II)... at the beginning of the line
 * depending on what's set. Note that the newline_sentinel comes BEFORE the line
 * ordinal
 */
std::string wrap_text(std::string instr, int charsperline,
                      bool windows_escape = false, bool tabs_to_spaces = true,
                      int tabspaces = 3, bool ignore_newlines = false,
                      std::string newline_sentinel = "",
                      t_word_list_type line_ordinals = WLT_NONE) {
  WordList words;
  std::string word;
  std::string delim_w;
  std::string sentinel_w;
  size_t sentinel_len = newline_sentinel.length();
  char delim;
  int c = 0, w = 0;

  while (c < (int)instr.length()) {
    // get the word
    while ((instr[c] != ' ') && (instr[c] != '\n') && (instr[c] != '\r') &&
           (instr[c] != '\t') && (instr[c] != '\0')) {
      word += instr[c];
      c++;
      w++;
    }
    delim = instr[c];

    if (delim != '\0') {
      // windows newline has \r\n
      if ((delim == '\r') && (instr[c + 1] == '\n')) {
        if (!ignore_newlines) {
          if (windows_escape)
            delim_w = "\r\n";
          else
            delim_w = "\n";
        }
        if (sentinel_len) {
          sentinel_w = newline_sentinel;
          w += (int)sentinel_len;
        }
        c += 2;
        w += 2;
      }
      // unix newline
      else if (delim == '\n') {
        if (!ignore_newlines) {
          if (windows_escape)
            delim_w = "\r\n";
          else
            delim_w = "\n";
        }
        if (sentinel_len) {
          sentinel_w = newline_sentinel;
          w += (int)sentinel_len;
        }
        c++;
        w++;
      } else if (delim == '\t') {
        delim_w.clear();
        if (tabs_to_spaces) {
          for (int ss = 0; ss < tabspaces; ++ss) delim_w += ' ';
        } else
          delim_w = '\t';

        w += tabspaces;
        c++;
      } else if (delim == ' ') {
        delim_w = ' ';
        c++;
      } else
        throw new std::runtime_error(
            "Error, Found an incorrect character as newline!");
    }

    if ((w + (int)sentinel_len) > charsperline) {
      // - word goes on next line.
      if (windows_escape)
        words.push_back("\r\n");
      else
        words.push_back("\n");
      w = 0;

      if (newline_sentinel.length()) sentinel_w = newline_sentinel;

      words.push_back(sentinel_w);
      words.push_back(word);
      words.push_back(delim_w);
      sentinel_w.clear();
      word.clear();
      delim_w.clear();
    } else {
      words.push_back(sentinel_w);
      words.push_back(word);
      words.push_back(delim_w);
      word.clear();
      delim_w.clear();
      sentinel_w.clear();
    }
  }
  return flatten_word_list(words);
}
std::string getHeader(std::string fileName, std::string strAddExt) {
  std::string head;

  std::string strModifiedFileName = fileName;
  if (strAddExt.length() > 0) strModifiedFileName += "." + strAddExt;

  std::string strFilePart = getFilePart(strModifiedFileName);

  head += "/**\n";
  head += "*  @file " + strModifiedFileName + "\n";
  head += "*  @date " + getCurrentDateCompact() +
          "\n";  // dateGetMonthName(date) + " " + dateGetDayStr(date) + ", " +
                 // dateGetYearStr(date) + "\n";
  head += "*  @author MetalMario971\n";
  head += "*/\n";

  // srand((unsigned int));
  // int r;
  // std::string idstr;
  // for( int i=0; i<5; ++i )
  //    idstr += intToStr( (int)rand() );

  time_t i3264_time = time(NULL);
  std::string strFilePartCopy = strFilePart;

  head += "#pragma once\n";
  head += "#ifndef __" + uppercase(strFilePartCopy) + "_" +
          std::to_string(i3264_time) +
          std::to_string(std::hash<std::string>()(strModifiedFileName)) +
          "_H__\n";
  head += "#define __" + uppercase(strFilePartCopy) + "_" +
          std::to_string(i3264_time) +
          std::to_string(std::hash<std::string>()(strModifiedFileName)) +
          "_H__\n";
  head += "\n";
  head += "\n";
  head += "namespace BR2 {\n";
  head += "/**\n";
  head += "*  @class ";
  head += strFilePart;
  head += "\n";
  head += "*  @brief\n";
  head += "*/\n";
  head += "class ";
  head += strFilePart;
  head += " : public VirtualMemoryShared<" + strFilePart + "> {\n";
  head += "public:\n";
  head += "    ";
  head += strFilePart;
  head += "();\n";
  head += "    virtual ~";
  head += strFilePart;
  head += "() override;\n";
  head += "};\n";
  head += "\n";
  head += "}//ns br2\n";
  head += "\n";
  head += "\n";
  head += "\n";
  head += "#endif\n";

  return head;
}
std::string getJava(std::string fileName) {
  std::string head;

  head += "/**\n";
  head += "*  @file " + fileName + "\n";
  head += "*  @date " + getCurrentDateCompact() + "\n";
  head += "*  @author MetalMario971\n";
  /*
      std::string Copyright=
      "*\tTHIS SOURCE CODE IS NOT FREE SOFTWARE. THE CODE HEREIN WEATHER IN \
  PHYSICAL OR ELECTRONIC FORM MAY NOT BE COPIED, USED, \
  OR DISTRIBUTED EITHER IN SOURCE OR OBJECT CODE \
  FOR DISTRIBUTION, SALE, OR PERSONAL USE WITHOUT EXPLICIT WRITTEN OR VERBAL
  CONSENT BY "; Copyright+= uppercase(SWITCHES::company_name);

      head+=wrap_text(Copyright,60,false,true,4,false,"*\t");
      */
  head += "*/\n";
  head += "\n";
  head += "package bro.cliffs;\n";
  head += "//import\n";
  head += "\n";
  head += "/**\n";
  head += "*  @class ";
  head += getFilePart(fileName);
  head += "\n";
  head += "*  @brief\n";
  head += "*/\n";
  head += "public class ";
  head += getFilePart(fileName);
  head += " {\n";
  head += " \n";
  head += "}\n";
  head += "\n";
  head += "\n";
  head += "\n";
  head += "\n";

  return head;
}
std::string getSource(std::string fileName, std::string strAddExt) {
  std::string head;
  head += "#include \"../base/BaseHeader.h\"\n";
  head += "#include \"./" + getFilePart(fileName) + ".h\"\n";
  head += "\n";
  head += "\n";
  head += "namespace BR2 {\n";
  head += getFilePart(fileName);
  head += "::";
  head += getFilePart(fileName);
  head += "() {\n";
  head += "\n";
  head += "}\n";
  head += getFilePart(fileName);
  head += "::~";
  head += getFilePart(fileName);
  head += "() {\n";
  head += "\n";
  head += "}\n";
  head += "\n";
  head += "}//ns br2\n";
  return head;
}
std::string combinePath(std::string a, std::string b) {
  // Combining paths on Linux doesn't actually treat the RHS as a path or file
  // if there's no /
  std::string bfmt;
  if (b.length() && ((b[0] != '\\') && (b[0] != '/'))) {
    bfmt = std::string("/") + b;
  } else {
    bfmt = b;
  }
  std::filesystem::path pa(a);
  std::filesystem::path pc = pa.concat(bfmt);
  std::string st = pc;
  return st;
}
void output(std::string filename, std::string data, std::string outdir,
            std::string strAddExt) {
  std::fstream out;
  std::string s = combinePath(outdir, filename);

  if (strAddExt.length() > 0) {
    s += "." + strAddExt;
  }
  out.open(s.c_str(), std::ios::out | std::ios::trunc);
  if (!out.good()) {
    out.close();
    throw std::runtime_error("Failed to output.");
  }

  out.write(data.c_str(), (std::streamsize)data.size());

  out.close();

  std::cout << s << std::endl;
}
