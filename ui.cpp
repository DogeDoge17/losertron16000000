#include "ui.hpp"


void update_warnings(const DokiExpression* item) {
	std::vector<std::string> groupIds;
    if (item == nullptr) {
        std::map<std::vector<std::string>, int> occurrences;

        for (const auto& selectedExpression : selectedExpressions) {
            std::vector<std::string> group;
            if constexpr (!girlDefaults.Groups.empty()) {
                for (const auto& ids : girlDefaults.Groups) {
                    for (const auto& grp : ids) {
	                    if (std::string uriStr = selectedExpression->uri.string(); uriStr.find(grp) != std::string::npos) {
                            group = ids;
                            break;
                        }
                    }
                    if (!group.empty()) break;
                }
            }

            if (group.empty()) continue;
            if (occurrences.contains(group))
                occurrences[group]++;
            else
                occurrences[group] = 0;
        }

        if (!occurrences.empty()) {
            const auto maxElement = std::ranges::max_element(occurrences, [](const auto& a, const auto& b) { return a.second < b.second; } );
            groupIds = maxElement->first;
        }
    } else {
        if constexpr (!girlDefaults.Groups.empty()) {
            for (const auto& ids : girlDefaults.Groups) {
                for (const auto& grp : ids) {
                    std::string uriStr = item->uri.string();
                    if (uriStr.find(grp) != std::string::npos) {
                        groupIds = ids;
                        break;
                    }
                }
                if (!groupIds.empty()) break;
            }
        }
    }

    for (auto &expressionv: all_expressions | std::views::values) {
	    for (auto& expGroup = expressionv; auto& image : expGroup) {
            if (groupIds.empty() &&
                (image.backgroundColor != GREEN && image.backgroundColor != YELLOWGREEN)) {
                image.backgroundColor = ImColor(0, 0, 0, 0);
                continue;
            }

            bool containsGroup = false;
            for (const auto& group : groupIds) {
	            if (std::string uriStr = image.uri.string(); uriStr.find(group) != std::string::npos) {
                    containsGroup = true;
                    break;
                }
            }

            if (!containsGroup) {
                image.backgroundColor = (image.backgroundColor == GREEN ||  image.backgroundColor == YELLOWGREEN) ? YELLOWGREEN : YELLOW;
            } else if (image.backgroundColor != GREEN && image.backgroundColor != YELLOWGREEN) {
                image.backgroundColor = ImColor(0, 0, 0, 0);
            }
        }
    }

    for (const auto & selectedExpression : selectedExpressions) {
        if (selectedExpression->backgroundColor == YELLOWGREEN) {
            bool containsGroup = false;
            for (const auto& group : groupIds) {
	            if (std::string uriStr = selectedExpression->uri.string(); uriStr.find(group) != std::string::npos) {
                    containsGroup = true;
                    break;
                }
            }

            if (containsGroup) {
                selectedExpression->backgroundColor = GREEN;
            }
        }
    }
}

void toggle_of_type(DokiExpression* item) {
	GirlieFolder* folder = nullptr;
	for (auto& folderr : girlDefaults.Folders) {
		if (folderr.Name == item->category) {
			folder = &folderr;
			break;
		}
	}

	const int max = std::ranges::any_of(folder->Bypass, [&](const std::string& bypass) { return item->uri.string().find(bypass) != std::string::npos; }) ? 1 : folder->Max;
	int itemCount = std::count_if(selectedExpressions.begin(), selectedExpressions.end(), [&](const DokiExpression* expr) { return expr->category == item->category; });

	for (int i = 0; i <= itemCount - max; i++) {
		auto location = std::ranges::find_if(selectedExpressions, [&](const DokiExpression* expr) { return expr->category == item->category; });
		if (location != selectedExpressions.end()) {
			(*location)->backgroundColor = ImColor(0, 0, 0, 0);
			selectedExpressions.erase(location);
		}
	}
}

/// <summary>
/// Toggles the pressed expression between selected and not selected
/// </summary>
void expression_click(DokiExpression* expression)
{
	if(const auto location = std::ranges::find(selectedExpressions, expression); location == selectedExpressions.end()) {
		toggle_of_type(expression);
		selectedExpressions.push_back(expression);
		expression->backgroundColor = GREEN;
		update_warnings(expression);
	}else {
		expression->backgroundColor = ImColor(0,0,0,0);
		selectedExpressions.erase(location);
	}

	sort_expressions();
	construct_doki();
	// await Task.Run(() => ConstructDoki(false));
}

void draw_expressions(const float room) {
	ImGui::BeginChild("tabs", ImVec2(room, 0), false, ImGuiWindowFlags_ChildWindow); {
		const ImVec2 originalPadding = ImGui::GetStyle().ItemSpacing;
		ImGui::GetStyle().ItemSpacing = ImVec2(1, 1);

		const float tabWidth = ImGui::GetContentRegionAvail().x / tabCount - ImGui::GetStyle().ItemSpacing.x;

		const int lastIndex = selectedExpressionGroup;
		for (int i = 0; i < tabCount; i++) {
			ImGui::PushID(i);

			if (lastIndex == i)
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyle().Colors[ImGuiCol_ButtonActive]);

			if (ImGui::Button(expressionGroups[i], ImVec2(tabWidth, 0))) {
				selectedExpressionGroup = i;
			}

			if (lastIndex == i)
				ImGui::PopStyleColor();

			ImGui::PopID();

			if (i < tabCount - 1)
				ImGui::SameLine();
		}
		ImGui::Separator();

		ImGui::BeginChild("expressions", ImVec2(0, 0), true, ImGuiWindowFlags_ChildWindow);
		std::vector<DokiExpression> &expressions = all_expressions[expressionGroups[selectedExpressionGroup]];
    // TODO: change zoom to predefined column sizes
		const float targetSize = 100.0f * zoom;
		const float spacingX = ImGui::GetStyle().ItemSpacing.x;

		const int columns = std::clamp(
			static_cast<int>((ImGui::GetContentRegionAvail().x + spacingX) / (targetSize + spacingX)), 1,
			static_cast<int>(expressions.size()));

		if (ImGui::BeginTable("expressionGrid", columns, ImGuiTableFlags_SizingStretchSame)) {
			for (int i = 0; i < expressions.size(); i++) {
				ImGui::TableNextColumn();

				ImGui::PushID(i);
				ImVec2 uv1 = ImVec2(expressions[i].bounds.u0, expressions[i].bounds.v0);
				ImVec2 uv0 = ImVec2(expressions[i].bounds.u1, expressions[i].bounds.v1);

				if (const float size = ImGui::GetContentRegionAvail().x; ImGui::ImageButton("expressionButton", (ImTextureID)expressions[i].texture.id, ImVec2(size, size), uv0, uv1, expressions[i].backgroundColor , ImVec4(1, 1, 1, 1))) {
					expression_click(&expressions[i]);
				}
				ImGui::PopID();

				if ((i + 1) % columns != 0)
					ImGui::SameLine();
			}
		}
		ImGui::EndTable();

		ImGui::EndChild();
		ImGui::GetStyle().ItemSpacing = originalPadding;
	}
	ImGui::EndChild();
}

static float ratio = 0.5f;

void draw_ui() {
	constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
	                                          ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
	ImGui::Begin("LOSERTRON16000000", nullptr, window_flags);

	ImVec2 windowAvailSize = ImGui::GetContentRegionAvail();
	float splitterThickness = 8.0f;

	float leftPaneWidth = std::clamp(windowAvailSize.x * ratio, windowAvailSize.x * 0.1f, windowAvailSize.x * 0.9f);

	draw_expressions(leftPaneWidth);

	ImGui::SameLine();

	ImVec2 originalPadding = ImGui::GetStyle().FramePadding;
	ImGui::GetStyle().ItemSpacing = ImVec2(0, 0);
	ImGui::InvisibleButton("##splitter", ImVec2(splitterThickness, windowAvailSize.y));
	if (ImGui::IsItemHovered())
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	if (ImGui::IsItemActive()) {
		ratio += (ImGui::GetIO().MouseDelta.x / windowAvailSize.x);
		ratio = std::clamp(ratio, 0.1f, 0.9f);
	}
	ImGui::GetStyle().FramePadding = originalPadding;
	ImGui::SameLine();

	ImGui::BeginChild("##rightPane", ImVec2(0, 0), false);


	const float footerH = ImGui::GetFrameHeightWithSpacing();
	ImVec2 avail = ImGui::GetContentRegionAvail();

	ImGui::BeginChild("##imageArea", ImVec2(avail.x, avail.y - footerH), false);
	{
		ImVec2 imgAvail = ImGui::GetContentRegionAvail();
		float side = std::max(0.0f, std::min(imgAvail.x, imgAvail.y));
		ImVec2 cur = ImGui::GetCursorPos();
		ImVec2 offset = ImVec2((imgAvail.x - side) * 0.5f, (imgAvail.y - side) * 0.5f);
		ImGui::SetCursorPos(ImVec2(cur.x + offset.x, cur.y + offset.y));

		ImGui::Image(
			(ImTextureID)renderedDokiTexture,
			ImVec2(side, side),
			ImVec2(renderedDokiBounds.u1, renderedDokiBounds.v1),
			ImVec2(renderedDokiBounds.u0, renderedDokiBounds.v0),
			ImVec4(1, 1, 1, 1),
			ImVec4(1, 0, 0, 0)
		);
	}
	ImGui::EndChild();

	ImVec2 imgAvail = ImGui::GetContentRegionAvail();
	const float widthItem = ImGui::GetContentRegionAvail().x / 3;

	ImGui::SetNextItemWidth(widthItem);
	if (ImGui::Combo("##characterSelector", &girl, girlsvDISP, girlsc)) {
		init_doki();
	}

	// gap
	ImGui::SameLine(0, widthItem);
	ImGui::SetNextItemWidth(widthItem);
	if (ImGui::Button("Save Doki", ImVec2(widthItem, 0))) {
		save_doki();
	}

	ImGui::EndChild();

	ImGui::End();
}
