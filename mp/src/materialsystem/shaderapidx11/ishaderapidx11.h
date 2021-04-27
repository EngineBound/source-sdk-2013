#ifndef ISHADERAPIDX11_H
#define ISHADERAPIDX11_H

#ifdef _WIN32
#pragma once
#endif

#include "ishaderapi.h"
#include "materialsystem/idebugtextureinfo.h"

abstract_class IShaderAPIDX11 : public IShaderAPI, public IDebugTextureInfo
{

};

#endif