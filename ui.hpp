#pragma once
#include <imgui.h>
#include <iostream>
#include "doki.hpp"

struct DokiExpression;

enum ZoomModes {
    Columns = 0,
    Zoom = 1,
};

const ImColor GREEN = ImColor(0, 128, 0, 255);
const ImColor YELLOW = ImColor(255, 255, 0, 255);
const ImColor YELLOWGREEN = ImColor(154, 205, 50, 255);

inline int selectedExpressionGroup;
inline char** expressionGroups = nullptr;
inline size_t tabCount;

inline double zoom = 1.0;
inline int columns = 3;
inline unsigned char zoomMode = ZoomModes::Columns;
inline const char* zoomModeNames[] = { "Columns:", "Zoom:" };

void down_press();
void up_press();
void update_warnings(const DokiExpression* item);
/// Begins the ImGui UI rendering process
void draw_ui();
