#include "imgui_utils.h"

// imgui doesnt do this so i added it
ImVec2 operator+(const ImVec2& a, const ImVec2& b)
{
    return ImVec2(a.x + b.x, a.y + b.y);
}

ImVec2 operator-(const ImVec2& a, const ImVec2& b)
{
    return ImVec2(a.x - b.x, a.y - b.y);
}

ImVec2 operator/(const ImVec2& a, const ImVec2& b)
{
    return ImVec2(a.x / b.x, a.y / b.y);
}

ImVec2 operator*(const ImVec2& a, const ImVec2& b)
{
    return ImVec2(a.x * b.x, a.y * b.y);
}
