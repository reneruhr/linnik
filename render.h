#pragma once

#include "render/window.h"
#include <functional>
namespace Render 
{
auto DrawTriangle() -> std::function<void(int, float)> ;

}
