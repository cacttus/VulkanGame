﻿#include "../base/Img32.h"
#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/EngineConfig.h"
#include "../base/Hash.h"
#include "../base/Allocator.h"
#include "../base/ApplicationPackage.h"
#include "../base/BinaryFile.h"
#include "../base/GLContext.h"
#include "../gfx/UiControls.h"
#include "../gfx/RenderUtils.h"
#include "../gfx/MegaTex.h"
#include "../base/GraphicsWindow.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../ext/stb_truetype.h"

#include <algorithm>

namespace BR2 {
#pragma region MtTex
void MtTex::setImg(std::shared_ptr<Img32> img) {
  _pImg = img;
  //We don't save the img data on the CPU, so just store what we need
  _iHeight = img->getHeight();
  _iWidth = img->getWidth();
  _fSizeRatio = (float)_iWidth / (float)_iHeight;
}
void MtTex::freeTmp() {
  _pImg = nullptr;
  _pMtNode = nullptr;
}
#pragma endregion
#pragma region MtTexPatch
MtTexPatch::MtTexPatch(std::shared_ptr<MegaTex> mt, std::string imgName, Hash32 nameHash) {
  _strName = imgName;
  _iNameHash = nameHash;
  _pMegaTex = mt;
}
MtTexPatch::~MtTexPatch() {
}
void MtTexPatch::addTexImage(std::string img, int32_t iPatch) {
  std::shared_ptr<MtTex> mt = std::make_shared<MtTex>(img, iPatch);
  _pMegaTex->getContext()->chkErrDbg();
  _vecTexs.push_back(mt);
}
void MtTexPatch::loadData() {
  if (_vecTexs.size() == 0) {
    BRLogError("Image patch was invalid for " + getName() + ".");
  }
  else if (_vecTexs.size() == 1) {
    if (_vecTexs[0]->img() == nullptr) {
      //If image isn't null, then it was already provided and should be loaded.
      if (!FileSystem::fileExists(getName())) {
        BRLogError("Failed to load, image file '" + getName() + "' didn't exist");
        Gu::debugBreak();
      }
      else {
        std::shared_ptr<Img32> img = Gu::loadImage(getName());
        _vecTexs[0]->setImg(img);
      }
    }
  }
  else {
    std::vector<std::shared_ptr<Img32>> imgs = parseImagePatch(getName());
    if (imgs.size() != _vecTexs.size()) {
      BRLogError("Tex Count Mismatch, or texture not found for '" + getName() + "'.");
      Gu::debugBreak();
    }
    else {
      for (size_t i = 0; i < imgs.size(); ++i) {
        _vecTexs[i]->setImg(imgs[i]);
      }
    }
  }
}
std::vector<std::shared_ptr<Img32>> MtTexPatch::parseImagePatch(std::string file) {
  std::vector<std::shared_ptr<Img32>> ret;

  if (!FileSystem::fileExists(file)) {
    BRLogError("Failed to load, image file '" + file + "' didn't exist");
    return ret;
  }

  std::shared_ptr<Img32> master = Gu::loadImage(file);

  //So we have to flip it because we load it into OpenGL space but we're in screen space.
  if (master == nullptr) {
    BRLogError("Error parsing 9-tile. Invalid or missing master image file '" + file + "'");
    return ret;
  }

  if (Img32::parseImagePatch(master, ret) == false) {
    BRLogError("Error parsing image patch for file " + file);
  }

  bool b = false;
  if (b) {
    //save images (and master
    Gu::saveImage("./data/cache/saved_9P_master.png", master);
    for (int n = 0; n < ret.size(); ++n) {
      std::shared_ptr<Texture2D> tex = std::make_shared<Texture2D>(getName(), TextureFormat::Image4ub, ret[n], _pMegaTex->getContext(), TexFilter::e::Nearest);
      _pMegaTex->getContext()->getRenderUtils()->saveTexture(std::move(Stz "./data/cache/saved_9P_" + n + ".png"), tex->getGlId(), GL_TEXTURE_2D);
    }
  }

  return ret;
}
#pragma endregion
#pragma region MtFont
MtFont::MtFont(std::shared_ptr<MegaTex> mt, std::string name, Hash32 nameHash) : MtTexPatch(mt, name, nameHash) {
}
MtFont::~MtFont() {
  //shouldn't get called
  int nn = 0;
  nn++;
}
void MtFont::loadData() {
  createFont();
}
void MtFont::createFont() {
  _iBakedCharSizePixels = Gu::getConfig()->getBakedCharSize();
  _atlasWidth = Gu::getConfig()->getFontBitmapSize();
  _atlasHeight = Gu::getConfig()->getFontBitmapSize();

  BRLogInfo("Creating font '" + getName() + "'. size=" + _atlasWidth + "x" + _atlasHeight + ".  Baked Char Size =" + _iBakedCharSizePixels);

  _pFontBuffer = std::make_unique<BinaryFile>("<none>");
  if (Gu::getPackage()->getFile(getName(), _pFontBuffer.get()) == false) {
    BRLogError("Failed to get font file '" + getName() + "'");
    Gu::debugBreak();
    return;
  }

  //Chinese Test
  if (StringUtil::contains(getName(), "simhei")) {
    //https://stackoverflow.com/questions/1366068/whats-the-complete-range-for-chinese-characters-in-unicode
    //Han Ideographs: 4E00 - 9FFF   Common
    _firstChar = 0x4E00;
    _charCount = 0x62FF - 0x4E00;  //0x9FFF is the whole range, that's a lot
    //Compute size for a 20x20 pixel han character
    _iBakedCharSizePixels = 20;
    float ch_w = ceilf(sqrtf((float)_charCount));
    _atlasWidth = _atlasHeight = (uint32_t)((ch_w) * (_iBakedCharSizePixels + _oversampleX));

    //Test "huan"
    //_firstChar = 0x6B61;// 喜..喜欢 0x559C, 0x6B61.. correct.. seems to work..Note: 欢 prints, 歡.. the traditioanl character
    //_charCount = 1;
    Gu::debugBreak();
  }

  //Get soem font metrics
  int ascent, descent, lineGap;
  stbtt_InitFont(&_fontInfo, (const unsigned char*)_pFontBuffer->getData().ptr(), 0);
  _fScaleForPixelHeight = stbtt_ScaleForPixelHeight(&_fontInfo, (float)_iBakedCharSizePixels);
  stbtt_GetFontVMetrics(&_fontInfo, &ascent, &descent, &lineGap);
  _fAscent = (float)ascent * _fScaleForPixelHeight;
  _fDescent = (float)descent * _fScaleForPixelHeight;
  _fLineGap = (float)lineGap * _fScaleForPixelHeight;

  if (false) {
    std::string x = "Hello ";
    for (int c : x) {
      int adv, be;
      stbtt_GetGlyphHMetrics(&_fontInfo, c, &adv, &be);
      float fa = (float)adv * _fScaleForPixelHeight;
      float fb = (float)be * _fScaleForPixelHeight;
      int nn = 0;
      nn++;
    }
  }

  //pack the image into a bitmap **nice version**
  std::unique_ptr<uint8_t[]> atlasData = std::make_unique<uint8_t[]>(_atlasWidth * _atlasHeight);
  _charInfo = std::make_unique<stbtt_packedchar[]>(_charCount);

  stbtt_pack_context context;
  if (!stbtt_PackBegin(&context, atlasData.get(), _atlasWidth, _atlasHeight, 0, 1, nullptr)) {
    BRLogError("Failed to initialize font");
    Gu::debugBreak();
    return;
  }

  stbtt_PackSetOversampling(&context, _oversampleX, _oversampleY);
  if (!stbtt_PackFontRange(&context, (unsigned char*)_pFontBuffer->getData().ptr(), 0, (float)_iBakedCharSizePixels, _firstChar, _charCount, _charInfo.get())) {
    BRLogError("Failed to pack font");
    Gu::debugBreak();
    return;
  }

  stbtt_PackEnd(&context);

  //Set the megatex image.
  std::shared_ptr<Img32> img = createFontImage(atlasData);
  if (false) {
    std::string imgName = Stz "./data/cache/dbg_font_" + FileSystem::getFileNameFromPath(getName()) + ".png";
    BRLogInfo("Saving " + imgName + "...");
    Gu::saveImage(imgName, img);
  }
  std::shared_ptr<MtTex> mt = std::make_shared<MtTex>(getName(), 0);
  mt->setImg(img);
  getTexs().push_back(mt);

  _bInitialized = true;
}
std::shared_ptr<Img32> MtFont::createFontImage(std::unique_ptr<uint8_t[]>& pData) {
  //Copied from fontspec
  auto imgData = std::make_unique<uint8_t[]>(_atlasWidth * _atlasHeight * 4);
  if (_charInfo == nullptr) {
    Gu::debugBreak();
  }

  for (int32_t iPix = 0; iPix < _atlasWidth * _atlasHeight * 4; iPix += 4) {
    uint8_t dat = pData[iPix / 4];
    imgData[iPix + 0] = 255;  //r
    imgData[iPix + 1] = 255;  //g
    imgData[iPix + 2] = 255;  //b
    imgData[iPix + 3] = dat;  //a
  }

  std::shared_ptr<Img32> img = std::make_shared<Img32>();
  img->init(_atlasWidth, _atlasHeight, imgData.get());

  //Stb creates the image upside-down for OpenGL, h owever in our GUi
  //we create it right-side up, then store it upside down, so we need to flip it rightise up first.
  //this also helps us see it when we cache it
  img->flipV();

  return img;
}
void MtFont::getCharQuad(int32_t cCode, int32_t cCodePrev, FontSize fontSize, float& outWidth, float& outHeight, Box2f& texs,
                         float& padTop, float& padRight, float& padBot, float& padLeft) {
  stbtt_aligned_quad stbQuad;
  Box2f worldQuad;
  if (_bInitialized == false) {
    BRLogError("Font was not initialized.");
    Gu::debugBreak();
    return;
  }
  if (_charInfo == nullptr) {
    return;
  }
  if (cCode < _firstChar || cCode >= (_firstChar + _charCount)) {
    //char is an invalid character such as a newline.
    //Process it as a space
    cCode = ' ';
  }

  float curX = 0, curY = 0;  //Dummies
  stbtt_GetPackedQuad(_charInfo.get(), _atlasWidth, _atlasHeight, cCode - _firstChar, &curX, &curY, &stbQuad, 0);
  if (getTexs().size() == 0) {
    //You didn't save the image
    BRLogError("Failure to save font image somewhere.");
    Gu::debugBreak();
    return;
  }

  //**TExs
  //Scale hte returned texcoodrs from [0,1] to the width of the baked texture
  float tw = getTexs()[0]->uv1().x - getTexs()[0]->uv0().x;  //top left, origin
  float th = getTexs()[0]->uv0().y - getTexs()[0]->uv1().y;  //This is flipped; We are in OpenGL tcoords, however our origin is at the top left

  //Scale
  float dv = stbQuad.t1 - stbQuad.t0;
  float du = stbQuad.s1 - stbQuad.s0;
  vec2 uv0, uv1;
  uv0.u() = getTexs()[0]->uv0().x + stbQuad.s0 * tw;
  uv0.v() = getTexs()[0]->uv1().y + stbQuad.t0 * th;  //Bottom-left = uv1
  uv1.u() = getTexs()[0]->uv0().x + stbQuad.s1 * tw;
  uv1.v() = getTexs()[0]->uv1().y + stbQuad.t1 * th;

  //Don't flip Y - we will do that in the regenmesh
  texs.construct(uv0, uv1);
  //**End TExs

  //Debug - Save texture
  if (false) {
    //TODO: this is just some debug, but we can fix this in the future.
    //RenderUtils::saveTexture("./data/cache/saved_TEST.png", Gu::getActiveWindow()->getGui()->getTex()->getGlId(), GL_TEXTURE_2D);
  }

  //**Pos
  //Transform quad by scale.  This is new - transorm the local quad only.  Not the whole text line.
  float fScale = fontSizeToFontScale(fontSize);
  outWidth = (stbQuad.x1 - stbQuad.x0) * fScale;
  outHeight = (stbQuad.y1 - stbQuad.y0) * fScale;

  //Position character horizontally
  //Compute the padding between characters
  int advWidth, bearing;
  float fAdvWidth, fBearing;
  stbtt_GetCodepointHMetrics(&_fontInfo, cCode, &advWidth, &bearing);
  fAdvWidth = (float)advWidth * _fScaleForPixelHeight;
  fBearing = (float)bearing * _fScaleForPixelHeight;
  fAdvWidth *= fScale;
  fBearing *= fScale;

  //Kerning, adds to padding not really necessary but, I assume it makes fonts look better rarely
  float fKern = 0.0f;
  if (cCodePrev >= 0) {
    //untested
    int kern = stbtt_GetCodepointKernAdvance(&_fontInfo, cCode, cCodePrev);
    fKern = (float)kern * _fScaleForPixelHeight;
    fKern *= fScale;
  }
  advWidth += fKern;

  //Compute the glyph padding values, and spaceing
  //for some reason space has a negative x0
  padLeft = fBearing;               // leftSideBearing is the offset from the current horizontal position to the left edge of the character
  padRight = fAdvWidth - outWidth;  // advanceWidth is the offset from the current horizontal position to the next horizontal position

  //Position character vertically
  //The ascent + descent of the character is wherever the quad is above, or below zero (zero is the baseline, we pass it in with curY)
  //_fAscent adn _fDescent are the scaled MAXIMUM ascent + descent of the font.  So the math here is correct
  padBot = (fabsf(_fDescent) - fabsf(stbQuad.y1));  // usually negative
  padTop = (fabsf(_fAscent) - fabsf(stbQuad.y0));   //
  padBot *= fScale;
  padTop *= fScale;
}
float MtFont::fontSizeToFontScale(float fs) {
  //Incorrect but I'm just,, o
  //Dividing by _ascent gives us a larger font than the actual extent.
  return fs / (float)_iBakedCharSizePixels;
}
//void MtFont::scaleStbQuad(const stbtt_aligned_quad* const stbQuad, Box2f* __out_ worldQuad, const vec2& basePos, float fScale) {
//    //From FontSpec
//    *worldQuad = Box2f(vec2(stbQuad->x0, stbQuad->y0), vec2(stbQuad->x1, stbQuad->y1));
//
//    //Apply scale
//    worldQuad->_p0 = basePos + (worldQuad->_p0 - basePos) * fScale;
//    worldQuad->_p1 = basePos + (worldQuad->_p1 - basePos) * fScale;
//}
#pragma endregion
#pragma region MegaTex
MegaTex::MegaTex(const string_t& name, std::shared_ptr<GLContext> ctx, bool bCache) : Texture2D(name, TextureFormat::Image4ub, ctx) {
  _bCache = bCache;
}
MegaTex::~MegaTex() {
}
std::shared_ptr<MtFont> MegaTex::getFont(std::string fn) {
  std::string low = fn;

  Hash32 h = STRHASH(low);
  auto f = _mapTexs.find(h);
  if (f == _mapTexs.end()) {
    _eState = MegaTexCompileState::Dirty;
    std::shared_ptr<MtFont> mtf = std::make_shared<MtFont>(getThis<MegaTex>(), fn, h);
    _mapTexs.insert(std::make_pair(h, mtf));
    _eState = MegaTexCompileState::Dirty;

    f = _mapTexs.find(h);
  }

  std::shared_ptr<MtFont> ft = std::dynamic_pointer_cast<MtFont>(f->second);
  return ft;
}
std::shared_ptr<MtTexPatch> MegaTex::getTex(std::shared_ptr<Img32> tx) {
  static uint64_t genId = 0;
  std::string genName = Stz "|gen-" + genId++;
  std::shared_ptr<MtTexPatch> p = getTex(genName, 1, true);
  if (p != nullptr && p->getTexs().size() > 0) {
    p->getTexs()[0]->setImg(tx);
  }
  else {
    BRLogError("Failed to add texture 23458242");
    Gu::debugBreak();
  }

  return p;
}
/**
*  @fn getTex
*  @brief Returns the given texture image by name, separated into patches. Not case sensitive.
*  @param nPatches - number of patches to expect - this is more of a debug thing to prevent invalid patches
*  @param bPreloaded - if we already loaded the image (skips validation and texture coords)
*  @param bLoadNow - Load the image immediately in this function (skips validation of texture coords)
*/
std::shared_ptr<MtTexPatch> MegaTex::getTex(std::string img, int32_t nPatches, bool bPreloaded, bool bLoadNow) {
  AssertOrThrow2(nPatches > 0);
  std::string imgNameLow = img;

  if (bPreloaded == false) {
    if (!FileSystem::fileExists(imgNameLow)) {
      BRLogError("Image file " + imgNameLow + " did not exist, compiling MegaTex.");
      return nullptr;
    }
  }

  std::shared_ptr<MtTexPatch> ret;
  Hash32 hImg = STRHASH(imgNameLow);
  std::map<Hash32, std::shared_ptr<MtTexPatch>>::iterator it;
  it = _mapTexs.find(hImg);
  if (it == _mapTexs.end()) {
    ret = std::make_shared<MtTexPatch>(getThis<MegaTex>(), imgNameLow, hImg);
    for (int i = 0; i < nPatches; ++i) {
      ret->addTexImage(imgNameLow, i);  //we could do "preloaded' as a bool, but it's probably nto necessary
    }
    _mapTexs.insert(std::make_pair(hImg, ret));
    _eState = MegaTexCompileState::Dirty;

    if (bLoadNow) {
      ret->loadData();
    }

    it = _mapTexs.find(hImg);
  }
  else {
    ret = it->second;
  }

  //**MUST return nPatches number of textures, never return a different number
  if (ret == nullptr) {
    BRLogError("Could not find MegaTex Texture " + imgNameLow);
    Gu::debugBreak();
  }
  else if (ret->getTexs().size() != (size_t)nPatches) {
    BRLogError("Failed to return an appropriate number of texture patches.");
    Gu::debugBreak();
  }
  return ret;
}
void MegaTex::loadImages() {
  BRLogInfo("Mega Tex: Loading " + _mapTexs.size() + " images.");

  for (auto p : _mapTexs) {
    std::shared_ptr<MtTexPatch> mtt = p.second;
    mtt->loadData();
  }

  //_bImagesLoaded = true;
}
std::shared_ptr<Img32> MegaTex::compile() {
  //Images should be loaded here with loadImages()
  //This is required because we use images sizes when constructing the gui
  _eState = MegaTexCompileState::Compiling;

  //Flatten Patches into individual images
  BRLogDebug("Mega Tex: Flattening " + _mapTexs.size() + " images.");
  std::vector<std::shared_ptr<MtTex>> vecTexs;
  for (auto texPair : _mapTexs) {
    for (auto tex : texPair.second->getTexs()) {
      vecTexs.push_back(tex);
    }
  }

  //Sort by wh - speeds up + saves room
  BRLogDebug("MegaTex - Mega Tex: Sorting " + vecTexs.size() + ".");
  struct {
    bool operator()(std::shared_ptr<MtTex> a, std::shared_ptr<MtTex> b) const {
      float f1 = a->getWidth() * a->getHeight();
      float f2 = b->getWidth() * b->getHeight();
      return f1 > f2;
    }
  } customLess;
  std::sort(vecTexs.begin(), vecTexs.end(), customLess);

  //Tex size
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, (GLint*)&_iMaxTexSize);
  getContext()->chkErrDbg();

  int32_t iImageSize = _iStartWH;
  int32_t nFailures = 0;

  //Expand rect and grow by 128 if we fail,  this is a pretty quick procedure, so we
  //don't have to worry about sizes.
  BRLogDebug("MegaTex - Making space for " + vecTexs.size() + " texs.");
  while (iImageSize <= _iMaxTexSize) {
    //Root
    _pRoot = std::make_shared<MtNode>();
    _pRoot->_b2Rect._p0.construct(0, 0);
    _pRoot->_b2Rect._p1.construct(iImageSize, iImageSize);

    //Do your thing
    bool bSuccess = true;

    for (std::shared_ptr<MtTex> tex : vecTexs) {
      if (tex->img() != nullptr) {
        std::shared_ptr<MtNode> found = _pRoot->plop(tex);
        if (found != nullptr) {
          tex->setNode(found);
        }
        else {
          //Failure, try larger size
          iImageSize = iImageSize + _iGrowWH;
          nFailures++;
          bSuccess = false;
          break;
        }
      }
      else {
        //Error
        //loadImages() wasn't called
        Gu::debugBreak();
      }
    }

    if (bSuccess == true) {
      break;
    }
  }

  if (iImageSize > _iMaxTexSize) {
    //Failure
    BRLogError("MegaTex - Failed to compose mega texture, too many textures and not enough texture space.");
  }
  else {
    BRLogDebug("MegaTex - Successful. Tex size=" + iImageSize + ".. Creating Bitmap..");

    //Compose Master Image
    _pMaster = std::make_shared<Img32>();
    int32_t datSiz = iImageSize * iImageSize * 4;
    uint8_t* pData = new uint8_t[datSiz];
    memset(pData, 0, datSiz);
    _pMaster->init(iImageSize, iImageSize, pData);
    delete[] pData;

    float imgW = (float)iImageSize;
    float imgH = (float)iImageSize;

    BRLogDebug("MegaTex - Copying Sub-Images..");
    for (std::shared_ptr<MtTex> tex : vecTexs) {
      _pMaster->copySubImageFrom(tex->node()->_b2Rect._p0, ivec2(0, 0), ivec2(tex->getWidth(), tex->getHeight()), tex->img());
      getContext()->chkErrDbg();

      //New Tex coords
      tex->uv0().x = (float)tex->node()->_b2Rect._p0.x / imgW;
      tex->uv0().y = (float)tex->node()->_b2Rect._p1.y / imgH;
      tex->uv1().x = (float)tex->node()->_b2Rect._p1.x / imgW;
      tex->uv1().y = (float)tex->node()->_b2Rect._p0.y / imgH;  //*Note the Y flop - OpenGL

      //Free the image and node, we don't need it
      tex->freeTmp();
    }
    if (_bCache) {
      std::string imgName = "./data/cache/ui_master.png";
      BRLogDebug("MegaTex - Caching - Saving '" + imgName + "' to disk...");
      Gu::saveImage(imgName, _pMaster);
    }
    else {
      BRLogDebug("MegaTex caching is disabled for this texture '" + this->getName() + "'.");
    }
  }

  //Now finally create
  BRLogDebug("MegaTex - Creating GPU Texture.");
  create(TextureFormat::Image4ub, _pMaster->getData()->ptr(), iImageSize, iImageSize, false, false, false);
  setFilter(TexFilter::e::Nearest);

  _eState = MegaTexCompileState::Compiled;

  return _pMaster;
}
std::shared_ptr<MtNode> MtNode::plop(std::shared_ptr<MtTex> tex) {
  //Copied from
  //http://blackpawn.com/texts/lightmaps/default.html
  if (_pChild[0] != nullptr && _pChild[1] != nullptr) {
    //leaf
    std::shared_ptr<MtNode> ret = _pChild[0]->plop(tex);
    if (ret != nullptr) {
      return ret;
    }
    return _pChild[1]->plop(tex);
  }
  else {
    if (_pTex != nullptr) {
      return nullptr;
    }
    int32_t bw = _b2Rect.width();
    int32_t bh = _b2Rect.height();

    if (tex->getHeight() > bh) {
      return nullptr;
    }
    if (tex->getWidth() > bw) {
      return nullptr;
    }
    if (tex->getWidth() == bw && tex->getHeight() == bh) {
      //prefect fit
      _pTex = tex;
      return getThis<MtNode>();
    }

    _pChild[0] = std::make_shared<MtNode>();
    _pChild[1] = std::make_shared<MtNode>();

    int dw = bw - tex->getWidth();
    int dh = bh - tex->getHeight();

    if (dw > dh) {
      _pChild[0]->_b2Rect.construct(
          _b2Rect.left(),
          _b2Rect.top(),
          _b2Rect.left() + tex->getWidth(),
          _b2Rect.bottom());
      _pChild[1]->_b2Rect.construct(
          _b2Rect.left() + tex->getWidth(),
          _b2Rect.top(),
          _b2Rect.right(),
          _b2Rect.bottom());
    }
    else {
      _pChild[0]->_b2Rect.construct(
          _b2Rect.left(),
          _b2Rect.top(),
          _b2Rect.right(),
          _b2Rect.top() + tex->getHeight());
      _pChild[1]->_b2Rect.construct(
          _b2Rect.left(),
          _b2Rect.top() + tex->getHeight(),
          _b2Rect.right(),
          _b2Rect.bottom());
    }
    return _pChild[0]->plop(tex);
  }
}
void MegaTex::update() {
  if (_eState == MegaTexCompileState::Dirty || _eState == MegaTexCompileState::NotCompiled) {
    loadImages();
    compile();
  }
}
bool MegaTex::bind(TextureChannel::e eChannel, std::shared_ptr<ShaderBase> pShader, bool bIgnoreIfNotFound) {
  update();
  if (_eState == MegaTexCompileState::NotCompiled) {
    BRLogWarnCycle("MegaTex was not compiled.  Make sure to update() MegaTex.");
  }
  else {
    if (_eState == MegaTexCompileState::Dirty) {
      BRLogWarnCycle("MegaTex was dirty before rendering.");
    }
    return Texture2D::bind(eChannel, pShader, bIgnoreIfNotFound);
  }

  return false;
}

#pragma endregion
}  // namespace BR2