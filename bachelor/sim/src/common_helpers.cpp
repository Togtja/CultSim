#pragma once

#include "common_helpers.h"

namespace cs
{
unsigned count_set_bits(uint64_t x)
{
    unsigned sum{};
    while (x != 0)
    {
        sum += x & 1u;
        x >>= 1u;
    }
    return sum;
}

bool close_enough(glm::vec2 pos, glm::vec2 pos2, float threshold)
{
    glm::bvec2 boolvec = glm::epsilonEqual(pos, pos2, glm::vec2(threshold));
    return boolvec.x && boolvec.y;
}

float sigmoid(const float x)
{
    return exp(x) / (exp(x) + 1.f);
}

void cs_auto_table_headers()
{
    auto& g         = *ImGui::GetCurrentContext();
    int rowCount    = g.CurrentTable->ColumnsCount;
    auto row_height = ImGui::GetTextLineHeight();
    ImGui::TableNextRow(ImGuiTableRowFlags_Headers, row_height);
    for (int i = 0; i < rowCount; i++)
    {
        auto name = ImGui::TableGetColumnName(i);
        ImGui::Text("%s", name);
        if (i < rowCount - 1)
        {
            ImGui::TableNextCell();
        }
    }
}
} // namespace cs
