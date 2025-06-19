#pragma once

#include "Main.h"
#include "Mapping.h"

class MappingListBoxModel final : public juce::TableListBoxModel
                                  , public juce::ComboBox::Listener
                                  , public juce::Button::Listener
                                  , public juce::TextEditor::Listener
                                  , public juce::ChangeListener
{
public:
    enum ColumnIds : int
    {
        Icon = 1,
        Path,
        Profile,
        Device,
        Active,
        Delete,
    };

    explicit MappingListBoxModel(juce::TableListBox& owner)
        : owner(owner)
    {
    }

    void setMappings(Mappings* mappings)
    {
        if (_mappings != nullptr)
        {
            // Remove listeners from old mappings
            for (auto& mapping : *_mappings)
                mapping->removeChangeListener(this);
        }

        _mappings = mappings;

        if (_mappings != nullptr)
        {
            // Add listeners to new mappings
            for (auto& mapping : *_mappings)
                mapping->addChangeListener(this);
        }
    }

private:
    Mappings* _mappings = nullptr;
    juce::TableListBox& owner;

public:
    int getNumRows() override
    {
        if (_mappings == nullptr)
        {
            return 0;
        }

        return static_cast<int>(_mappings->size());
    }

    // void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;
    void paintRowBackground(juce::Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;

    void paintCell(juce::Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    juce::Component* refreshComponentForCell(int rowNumber
                                             , int columnId
                                             , bool isRowSelected
                                             , juce::Component* existingComponentToUpdate) override;

    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked(juce::Button*) override;
    void textEditorTextChanged(juce::TextEditor&) override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
};
