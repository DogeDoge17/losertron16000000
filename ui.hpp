#pragma once
#include <imgui.h>
#include <iostream>
#include <imgui.h>

#include "doki.hpp"

struct DokiExpression;


constexpr ImColor GREEN = ImColor(0, 128, 0, 255);
constexpr ImColor YELLOW = ImColor(255, 255, 0, 255);
constexpr ImColor YELLOWGREEN = ImColor(154, 205, 50, 255);

inline int selectedExpressionGroup;
inline char** expressionGroups = nullptr;
inline size_t tabCount;

void update_warnings(const DokiExpression* item);
void draw_ui();