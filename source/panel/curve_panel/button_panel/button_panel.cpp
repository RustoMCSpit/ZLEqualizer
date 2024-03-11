// Copyright (C) 2023 - zsliu98
// This file is part of ZLEqualizer
//
// ZLEqualizer is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
//
// ZLEqualizer is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with ZLEqualizer. If not, see <https://www.gnu.org/licenses/>.

#include "button_panel.hpp"

namespace zlPanel {
    ButtonPanel::ButtonPanel(juce::AudioProcessorValueTreeState &parameters,
                             juce::AudioProcessorValueTreeState &parametersNA,
                             zlInterface::UIBase &base)
        : parametersRef(parameters), parametersNARef(parametersNA),
          uiBase(base) {
        for (size_t i = 0; i < zlState::bandNUM; ++i) {
            panels[i] = std::make_unique<FilterButtonPanel>(i, parameters, parametersNA, base);
            panels[i]->getDragger().getButton().onStateChange = [this]() {
                const auto idx = selectBandIdx.load();
                if (panels[idx]->getDragger().getButton().getToggleState()) {
                    juce::ScopedLock lock(wheelLock);
                    wheelAttachment.reset();
                    wheelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                        parametersRef, zlDSP::appendSuffix(zlDSP::Q::ID, selectBandIdx.load()), wheelSlider);
                    panels[idx]->getTargetDragger().getButton().setToggleState(false, juce::sendNotification);
                    panels[idx]->getSideDragger().getButton().setToggleState(false, juce::sendNotification);
                }
            };
            panels[i]->getTargetDragger().getButton().onStateChange = [this]() {
                const auto idx = selectBandIdx.load();
                if (panels[idx]->getTargetDragger().getButton().getToggleState()) {
                    juce::ScopedLock lock(wheelLock);
                    wheelAttachment.reset();
                    wheelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                        parametersRef, zlDSP::appendSuffix(zlDSP::targetQ::ID, selectBandIdx.load()), wheelSlider);
                    panels[idx]->getDragger().getButton().setToggleState(false, juce::sendNotification);
                    panels[idx]->getSideDragger().getButton().setToggleState(false, juce::sendNotification);
                }
            };
            panels[i]->getSideDragger().getButton().onStateChange = [this]() {
                const auto idx = selectBandIdx.load();
                if (panels[idx]->getSideDragger().getButton().getToggleState()) {
                    juce::ScopedLock lock(wheelLock);
                    wheelAttachment.reset();
                    wheelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                        parametersRef, zlDSP::appendSuffix(zlDSP::sideQ::ID, selectBandIdx.load()), wheelSlider);
                    panels[idx]->getDragger().getButton().setToggleState(false, juce::sendNotification);
                    panels[idx]->getTargetDragger().getButton().setToggleState(false, juce::sendNotification);
                }
            };
        }
        for (const auto &idx: IDs) {
            parametersNARef.addParameterListener(idx, this);
            parameterChanged(idx, parametersNARef.getRawParameterValue(idx)->load());
        }
        for (size_t i = 0; i < zlState::bandNUM; ++i) {
            addAndMakeVisible(panels[i].get());
        }
        addAndMakeVisible(lassoComponent);
    }

    ButtonPanel::~ButtonPanel() {
        for (const auto &idx: IDs) {
            parametersNARef.removeParameterListener(idx, this);
        }
    }

    void ButtonPanel::resized() {
        wheelSlider.setBounds(getLocalBounds());
        for (const auto &p: panels) {
            p->setBounds(getLocalBounds());
        }
    }

    void ButtonPanel::mouseDown(const juce::MouseEvent &event) {
        juce::ignoreUnused(event);
        for (size_t i = 0; i < zlState::bandNUM; ++i) {
            panels[i]->setSelected(false);
        }
        juce::ScopedLock lock(wheelLock);
        wheelAttachment.reset();

        lassoComponent.setColour(lassoComponent.lassoFillColourId, uiBase.getTextColor().withMultipliedAlpha(.25f));
        lassoComponent.setColour(lassoComponent.lassoOutlineColourId, uiBase.getTextColor().withMultipliedAlpha(.375f));
        lassoComponent.setVisible(true);
        lassoComponent.beginLasso(event, this);
    }

    void ButtonPanel::mouseUp(const juce::MouseEvent &event) {
        juce::ignoreUnused(event);
        for (auto *d : itemsSet) {
            d->getButton().setToggleState(true, juce::sendNotification);
        }
        lassoComponent.endLasso();
    }

    void ButtonPanel::mouseDrag(const juce::MouseEvent &event) {
        // juce::ignoreUnused(event);
        lassoComponent.dragLasso(event);
    }

    void ButtonPanel::mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel) {
        wheelSlider.mouseWheelMove(event, wheel);
    }

    void ButtonPanel::mouseDoubleClick(const juce::MouseEvent &event) {
        const auto idx = findAvailableBand();
        if (idx >= zlState::bandNUM) {
            return;
        }
        auto bound = getLocalBounds().toFloat();
        bound = bound.withSizeKeepingCentre(bound.getWidth(), bound.getHeight() - 2 * uiBase.getFontSize());
        const auto point = event.getPosition().toFloat();
        const auto x = point.getX(), y = point.getY();
        const auto freq = xtoFreq(x, bound);
        const auto db = yToDB(y, maximumDB.load(), bound);

        std::vector<std::string> initIDs;
        std::vector<float> initValues;

        if (freq < 20.f) {
            initIDs.emplace_back(zlDSP::fType::ID);
            initValues.emplace_back(zlDSP::fType::convertTo01(zlIIR::FilterType::highPass));
        } else if (freq < 50.f) {
            initIDs.emplace_back(zlDSP::fType::ID);
            initValues.emplace_back(zlDSP::fType::convertTo01(zlIIR::FilterType::lowShelf));
            initIDs.emplace_back(zlDSP::gain::ID);
            initValues.emplace_back(zlDSP::gain::convertTo01(
                juce::jlimit(-maximumDB.load(), maximumDB.load(), 2 * db)
            ));
        } else if (freq < 5000.f) {
            initIDs.emplace_back(zlDSP::fType::ID);
            initValues.emplace_back(zlDSP::fType::convertTo01(zlIIR::FilterType::peak));
            initIDs.emplace_back(zlDSP::gain::ID);
            initValues.emplace_back(zlDSP::gain::convertTo01(
                juce::jlimit(-maximumDB.load(), maximumDB.load(), db)
            ));
        } else if (freq < 15000.f) {
            initIDs.emplace_back(zlDSP::fType::ID);
            initValues.emplace_back(zlDSP::fType::convertTo01(zlIIR::FilterType::highShelf));
            initIDs.emplace_back(zlDSP::gain::ID);
            initValues.emplace_back(zlDSP::gain::convertTo01(
                juce::jlimit(-maximumDB.load(), maximumDB.load(), 2 * db)
            ));
        } else {
            initIDs.emplace_back(zlDSP::fType::ID);
            initValues.emplace_back(zlDSP::fType::convertTo01(zlIIR::FilterType::lowPass));
        }
        initIDs.emplace_back(zlDSP::freq::ID);
        initValues.emplace_back(zlDSP::freq::convertTo01(freq));
        initIDs.emplace_back(zlDSP::bypass::ID);
        initValues.emplace_back(zlDSP::bypass::convertTo01(false));

        for (size_t i = 0; i < initIDs.size(); ++i) {
            const auto paraID = zlDSP::appendSuffix(initIDs[i], idx);
            auto *_para = parametersRef.getParameter(paraID);
            _para->beginChangeGesture();
            _para->setValueNotifyingHost(initValues[i]);
            _para->endChangeGesture();
        }

        auto *para = parametersNARef.getParameter(zlState::selectedBandIdx::ID);
        para->beginChangeGesture();
        para->setValueNotifyingHost(zlState::selectedBandIdx::convertTo01(static_cast<int>(idx)));
        para->endChangeGesture();
    }

    void ButtonPanel::parameterChanged(const juce::String &parameterID, float newValue) {
        if (parameterID == zlState::selectedBandIdx::ID) {
            const auto idx = static_cast<size_t>(newValue);
            selectBandIdx.store(idx);
            for (size_t i = 0; i < zlState::bandNUM; ++i) {
                panels[i]->setSelected(i == idx);
            } {
                const juce::MessageManagerLock mmLock;
                panels[idx]->toFront(false);
            }
            juce::ScopedLock lock(wheelLock);
            wheelAttachment.reset();
            wheelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                parametersRef, zlDSP::appendSuffix(zlDSP::Q::ID, selectBandIdx.load()), wheelSlider);
            panels[idx]->getTargetDragger().getButton().setToggleState(false, juce::sendNotification);
            panels[idx]->getSideDragger().getButton().setToggleState(false, juce::sendNotification);
        } else if (parameterID == zlState::maximumDB::ID) {
            const auto idx = static_cast<size_t>(newValue);
            for (const auto &p: panels) {
                p->setMaximumDB(zlState::maximumDB::dBs[idx]);
            }
            maximumDB.store(zlState::maximumDB::dBs[idx]);
        }
    }

    void ButtonPanel::handleAsyncUpdate() {
        repaint();
    }

    size_t ButtonPanel::findAvailableBand() const {
        for (size_t i = 0; i < zlState::bandNUM; ++i) {
            const auto idx = zlState::appendSuffix(zlState::active::ID, i);
            const auto isActive = static_cast<bool>(parametersNARef.getRawParameterValue(idx)->load());
            if (!isActive) { return i; }
        }
        return zlState::bandNUM;
    }
} // zlPanel
