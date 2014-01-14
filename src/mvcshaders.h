#ifndef __MVC_SHADERS_H__
#define __MVC_SHADERS_H__

#include "utils.h"

inline QString getAlphaFS() { return readFileContent(":/src/AlphaFS.glsl"); }
inline QString getAlphaVS() { return readFileContent(":/src/AlphaVS.glsl"); }
inline QString getSamplingFS() { return readFileContent(":/src/SamplingFS.glsl"); }
inline QString getSamplingVS() { return readFileContent(":/src/SamplingVS.glsl"); }
inline QString getSampling2DFilterVS() { return readFileContent(":/src/Sampling2DFilterVS.glsl"); }

#endif
