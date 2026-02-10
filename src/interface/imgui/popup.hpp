#pragma once
#include "imgui.h"
#include <imgui-cocos.hpp>

namespace GDH {
    struct PopupMessage {
        std::string caption;
        float expiry_time;
        float start_time;
    };

    class Popup {
    public:
        static Popup& get() {
            static Popup instance;
            return instance;
        }

        Popup& operator=(const Popup&) = delete;
        Popup(const Popup&) = delete;

        float popupDuration = 5.0f;
        std::vector<PopupMessage> messages;

        void add(const std::string& caption) {
            float currentTime = static_cast<float>(ImGui::GetTime());
            messages.insert(messages.begin(), {caption, currentTime + popupDuration, currentTime});
        }

        void render() {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1);

            ImVec2 displaySize = ImGui::GetIO().DisplaySize;
            float currentY = displaySize.y - 10.0f;
            float spacing = 10.0f;

            messages.erase(std::remove_if(messages.begin(), messages.end(),
                [](const PopupMessage& msg) { return ImGui::GetTime() > msg.expiry_time; }), messages.end());

            for (const auto& message : messages) {
                float currentTime = static_cast<float>(ImGui::GetTime());

                float alpha = 1.0f;
                float fadeDuration = 0.1f;

                if (currentTime < message.start_time + fadeDuration) {
                    alpha = (currentTime - message.start_time) / fadeDuration;
                } else if (currentTime > message.expiry_time - fadeDuration) {
                    alpha = (message.expiry_time - currentTime) / fadeDuration;
                }

                ImVec2 textSize = ImGui::CalcTextSize(message.caption.c_str());
                float popupWidth = textSize.x + 20.0f;
                float popupHeight = textSize.y + 20.0f;

                ImGui::SetNextWindowPos({displaySize.x - popupWidth - 10.0f, currentY - popupHeight});
                ImGui::SetNextWindowSize({popupWidth, popupHeight});

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

                if (ImGui::Begin(("Popup##" + std::to_string(reinterpret_cast<std::uintptr_t>(&message))).c_str(),
                                nullptr,
                                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | 
                                ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoSavedSettings)) {
                    ImGui::TextUnformatted(message.caption.c_str());
                    ImGui::End();
                }

                ImGui::PopStyleVar();

                currentY -= (popupHeight + spacing);
            }
            ImGui::PopStyleVar();
        }
    private:
        Popup() = default;
    };
}