#pragma once
#include "imgui.h"

// imgui doesnt do this so i added it
ImVec2 operator+(const ImVec2& a, const ImVec2& b);

ImVec2 operator-(const ImVec2& a, const ImVec2& b);

ImVec2 operator/(const ImVec2& a, const ImVec2& b);

ImVec2 operator*(const ImVec2& a, const ImVec2& b);
