#include "MappingListBoxModel.h"

#include "Mapping.h"

using namespace juce;

class GridComboBox final : public ComboBox
{
public:
    GridComboBox(const String& componentName, int row)
        : ComboBox(componentName),
          row(row)
    {
    }

    int row;
};

class GridToggleButton final : public ToggleButton
{
public:
    GridToggleButton(const String& componentName, int row)
        : ToggleButton(componentName),
          row(row)
    {
    }

    int row;
};

class GridTextButton final : public TextButton
{
public:
    GridTextButton(const String& componentName, int row)
        : TextButton(componentName),
          row(row)
    {
    }

    int row;
};

class GridTextEditor final : public TextEditor
{
public:
    GridTextEditor(const String& componentName, int row)
        : TextEditor(componentName),
          row(row)
    {
    }

    int row;
};

class PathSelectorComponent : public Component
{
public:
    PathSelectorComponent(int row): comboBox("Path", row)
    {
        addAndMakeVisible(comboBox);
        addAndMakeVisible(browseButton);
        comboBox.setEditableText(true);

        // Add mouse listener to populate menu items on click
        comboBox.addMouseListener(this, false);

        browseButton.setButtonText("...");
        browseButton.onClick = [this]()
        {
            chooser.launchAsync(FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
            [this](const FileChooser& fc)
            {
                if (fc.getResult() == File{})
                    return;

                auto file = fc.getResult();
                comboBox.setText(file.getFullPathName(), sendNotification);
                // Notify about change
            });

        };
    }

    void mouseDown(const MouseEvent& event) override
    {
        if (event.eventComponent == &comboBox)
        {
            // Clear existing items
            comboBox.getRootMenu()->clear();
            
            // Populate with fresh process list
            auto processes = WindowHelper::getProcesses();
            int index = 0;
            for (auto& info : processes)
            {
                comboBox.getRootMenu()->addItem(++index, info.path, true, false, info.icon);
            }
        }
    }


    void resized() override
    {
        auto bounds = getLocalBounds();
        auto buttonWidth = 24;
        
        browseButton.setBounds(bounds.removeFromRight(buttonWidth));
        bounds.removeFromRight(2); // gap
        comboBox.setBounds(bounds);
    }
    
    GridComboBox comboBox;
    TextButton browseButton;
    FileChooser chooser {"Select executable...", File::getSpecialLocation(File::userHomeDirectory), "*.exe"};
};


void MappingListBoxModel::paintRowBackground( Graphics& g, int rowNumber, int , int , bool )
{
    g.fillAll((*_mappings)[rowNumber]->isActive() ? Colours::green : Colours::white);
}

void MappingListBoxModel::paintCell(Graphics&, int, int, int, int, bool)
{
    
}

Component* MappingListBoxModel::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected,
                                                        Component* existingComponentToUpdate)
{
    auto& mapping = (*_mappings)[rowNumber];
    
    if (columnId == static_cast<int>(Icon))
    {
        auto* image_component = dynamic_cast<ImageComponent*> (existingComponentToUpdate);

        if (image_component == nullptr)
        {
            image_component = new ImageComponent();
            image_component->setTooltip("Icon for mapped App");
        }
        
        if (image_component != nullptr)
        {
            if (rowNumber < static_cast<int>(_mappings->size()))
            {
                image_component->setImage(mapping->exeIcon());
            }
            else
            {
                image_component->setImage(Image());
            }
            return image_component;
        }
        return nullptr;
    }

    if (columnId == static_cast<int>(Path))
    {
        if (mapping->isDefault())
        {
            auto* label = dynamic_cast<Label*> (existingComponentToUpdate);
            if (label == nullptr)
            {
                label = new Label("",mapping->path());
                label->setColour(Label::textColourId, Colours::lightblue);
                label->setTooltip("DEFAULT is used if no other App matches");
            }

            return label;
        }
        else
        {
            auto* path_selector_component = dynamic_cast<PathSelectorComponent*> (existingComponentToUpdate);
            if (path_selector_component == nullptr)
            {
                path_selector_component = new PathSelectorComponent(rowNumber);
                path_selector_component->comboBox.addListener(this);
            }
        
            if (path_selector_component != nullptr)
            {
                path_selector_component->comboBox.setText(mapping->path(), dontSendNotification);
                path_selector_component->comboBox.setTooltip(mapping->path());
                path_selector_component->browseButton.setTooltip("Browse for executable");
            
                if (mapping->isActualPath())
                {
                    if (mapping->fileExists())
                        path_selector_component->comboBox.setColour(ComboBox::textColourId, Colours::green);
                    else
                        path_selector_component->comboBox.setColour(ComboBox::textColourId, Colours::red);
                }
            }
            return path_selector_component;
        }
    }

    if (columnId == Profile)
    {
        auto* combo_box = dynamic_cast<GridComboBox*> (existingComponentToUpdate);
        if (combo_box == nullptr)
        {
            combo_box = new GridComboBox("Profile", rowNumber);
            combo_box->setTooltip("Name of Profile to switch to");
            combo_box->addListener(this);
        }
        
        if (combo_box != nullptr)
        {
            
            combo_box->clear();

            int selected_index = 0;
            int index = 0;
            for (auto& profile : mapping->deviceProfiles())
            {
                combo_box->addItem(profile, ++index);

                if (profile == mapping->profileName())
                {
                    selected_index = index;
                }
            }

            combo_box->setSelectedId(selected_index, dontSendNotification);
        }

        return combo_box;
    }

    if (columnId == Device)
    {
        auto* combo_box = dynamic_cast<GridComboBox*> (existingComponentToUpdate);
        if (combo_box == nullptr)
        {
            combo_box = new GridComboBox("Device", rowNumber);
            combo_box->setTooltip("Device to switch");
            combo_box->addListener(this);
        }
        
        if (combo_box != nullptr)
        {
            combo_box->clear();

            int selected_index = 0;
            int index = 0;
            
            for (auto& device_id : mapping->deviceIds())
            {
                combo_box->addItem(device_id.DisplayName, ++index);

                if (device_id.DeviceId == mapping->deviceId())
                {
                    selected_index = index;
                }
            }

            combo_box->setSelectedId(selected_index, dontSendNotification);
        }

        return combo_box;
    }

    if (columnId == Active)
    {
        auto* toggle_button = dynamic_cast<GridToggleButton*> (existingComponentToUpdate);
        if (toggle_button == nullptr)
        {
            toggle_button = new GridToggleButton("", rowNumber);
            toggle_button->setColour(ToggleButton::tickColourId, Colours::black);
            toggle_button->setColour(ToggleButton::tickDisabledColourId, Colours::black);
            toggle_button->setTooltip("Activate or deactivate this mapping");
            toggle_button->addListener(this);
        }
        
        if (toggle_button != nullptr)
        {
            toggle_button->setToggleState(mapping->isActive(), dontSendNotification);
        }

        return toggle_button;
    }

    if (columnId == Delete)
    {
        auto* grid_text_button = dynamic_cast<GridTextButton*> (existingComponentToUpdate);
        if (grid_text_button == nullptr)
        {
            grid_text_button = new GridTextButton("DELETE", rowNumber);
            grid_text_button->setTooltip("Delete this mapping");
            grid_text_button->setColour(TextButton::buttonColourId, Colours::red);
            grid_text_button->addListener(this);
        }

        return grid_text_button;
    }
    
    return TableListBoxModel::refreshComponentForCell(rowNumber, columnId, isRowSelected, existingComponentToUpdate);
}

void MappingListBoxModel::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    auto grid_combo_box = dynamic_cast<GridComboBox*>(comboBoxThatHasChanged);
    
    if (grid_combo_box && grid_combo_box->getName() == "Profile")
    {
        // auto row = ???
        auto& mapping = (*_mappings)[grid_combo_box->row];
        mapping->profileName(grid_combo_box->getText());
    }

    if (grid_combo_box && grid_combo_box->getName() == "Path")
    {
        auto& mapping = (*_mappings)[grid_combo_box->row];
        mapping->path(grid_combo_box->getText());
        owner.updateContent();
    }

    if (grid_combo_box && grid_combo_box->getName() == "Device")
    {
        auto& mapping = (*_mappings)[grid_combo_box->row];
        for (auto& device_id : mapping->deviceIds())
        {
            if (device_id.DisplayName == grid_combo_box->getText())
            {
                mapping->deviceId(device_id.DeviceId);
            }
        }
    }
}

void MappingListBoxModel::buttonClicked(Button* sender)
{
    if (auto grid_toggle_button = dynamic_cast<GridToggleButton*>(sender))
    {
        (*_mappings)[grid_toggle_button->row]->isActive(grid_toggle_button->getToggleState());
        owner.repaintRow(grid_toggle_button->row);
        return;
    }

    if (auto grid_text_button = dynamic_cast<GridTextButton*>(sender))
    {
        ModifierKeys modifiers = ModifierKeys::getCurrentModifiers();

        if (! modifiers.isCtrlDown())
        {
            if ( ! AlertWindow::showOkCancelBox(MessageBoxIconType::WarningIcon, "Delete Mapping?", "Do you really want to delete this mapping?"))
                return;       
        }
        
        _mappings->erase(_mappings->begin() + grid_text_button->row);
        owner.updateContent();
        return;
    }
}

void MappingListBoxModel::textEditorTextChanged(TextEditor& text_editor)
{
    if (GridTextEditor* grid_text_editor = dynamic_cast<GridTextEditor*>(&text_editor))
    {
        auto& mapping = (*_mappings)[grid_text_editor->row];
        mapping->path(grid_text_editor->getText());
    }
}

void MappingListBoxModel::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
    owner.updateContent();
}
