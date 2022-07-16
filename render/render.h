#pragma once

#include "imgui/imgui.h"
#include "window.h"
#include "buffer.h"
#include "camera.h"
#include <functional>
#include <span>

namespace Render 
{
using MVP = std::span<float,16>;
using Vec3= std::span<float,3>;

enum class Color
{
	Green,Black,Salmon,ForestGreen,LightSeaGreen,RoyalBlue,HotPink,Chocolate,Ivory, Silver,Honeydew,CornFlowerBlue,DarkTurqoise,MediumSpringGreen,Tomato,LemonChiffon,
	COUNT
};

auto MakeRGB(Color) -> Vec3;
auto NextColor() -> Color;

auto DrawTriangle() -> std::function<void(int, float)> ;
auto DrawPoints(const Buffer&) -> std::function<void(MVP , Color)> ;

}
