// ==============================================================================
// Copyright (C) 2023 - zsliu98
// This file is part of ZLEComp
//
// ZLEComp is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
// ZLEComp is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEComp. If not, see <https://www.gnu.org/licenses/>.
// ==============================================================================

#ifndef ZL_NAME_LOOK_AND_FEEL_H
#define ZL_NAME_LOOK_AND_FEEL_H

#include <juce_gui_basics/juce_gui_basics.h>

#include "../interface_definitions.h"

namespace zlInterface {
    class NameLookAndFeel : public juce::LookAndFeel_V4 {
    public:
        explicit NameLookAndFeel(UIBase &base) {
            uiBase = &base;
        }

        void drawLabel(juce::Graphics &g, juce::Label &label) override {
            if (editable) {
                g.setColour(uiBase->getTextColor().withMultipliedAlpha(alpha.load()));
            } else {
                g.setColour(uiBase->getTextInactiveColor().withMultipliedAlpha(alpha.load()));
            }
            g.setFont(uiBase->getFontSize() * fontScale.load());
            g.drawText(label.getText(), label.getLocalBounds(), justification.load());
        }

        inline void setEditable(const bool f) { editable.store(f); }

        inline void setAlpha(const float x) { alpha.store(x); }

        inline void setFontScale(const float x) { fontScale.store(x); }

        inline void setJustification(const juce::Justification j) { justification.store(j); }

    private:
        std::atomic<bool> editable{true};
        std::atomic<float> alpha{1.f};
        std::atomic<float> fontScale{FontNormal};
        std::atomic<juce::Justification> justification{juce::Justification::centred};

        UIBase *uiBase;
    };
}

#endif //ZL_NAME_LOOK_AND_FEEL_H
