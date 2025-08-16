#pragma once
#include <imgui.h>
#include <iostream>
#include "doki.hpp"

struct DokiExpression;

const ImColor GREEN = ImColor(0, 128, 0, 255);
const ImColor YELLOW = ImColor(255, 255, 0, 255);
const ImColor YELLOWGREEN = ImColor(154, 205, 50, 255);

inline int selectedExpressionGroup;
inline char** expressionGroups = nullptr;
inline size_t tabCount;

inline double zoom = 1.0;

void update_warnings(const DokiExpression* item);
void draw_ui();