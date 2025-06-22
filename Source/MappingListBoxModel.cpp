#include "MappingListBoxModel.h"
#include "Mapping.h"

using namespace juce;

class GridComboBox final : public ComboBox
{
public:
    GridComboBox(const String& componentName, const int row)
    : ComboBox(componentName)
    , Row(row)
    {}

    int Row;
};

class GridToggleButton final : public ToggleButton
{
public:
    GridToggleButton(const String& componentName, const int row)
    : ToggleButton(componentName)
    , Row(row)
    {}

    int Row;
};

class GridTextButton final : public TextButton
{
public:
    GridTextButton(const String& componentName, const int row)
    : TextButton(componentName)
    , Row(row)
    {}

    int Row;
};

class GridTextEditor final : public TextEditor
{
public:
    GridTextEditor(const String& componentName, const int row)
    : TextEditor(componentName)
    , Row(row)
    {}

    int Row;
};

class PathSelectorComponent final : public Component
{
public:
    PathSelectorComponent(const int row): comboBox("Path", row)
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

                const auto file = fc.getResult();
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
            const auto processes = WindowHelper::GetProcesses();
            int index = 0;
            for (auto& info : processes)
            {
                comboBox.getRootMenu()->addItem(++index, info.Path, true, false, info.Icon);
            }
        }
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();

        browseButton.setBounds(bounds.removeFromRight(24));
        bounds.removeFromRight(2); // gap
        comboBox.setBounds(bounds);
    }
    
    GridComboBox comboBox;
    TextButton browseButton;
    FileChooser chooser {"Select executable...", File::getSpecialLocation(File::userHomeDirectory), "*.exe"};
};


void MappingListBoxModel::paintRowBackground( Graphics& g, const int rowNumber, int , int , bool )
{
    g.fillAll((*_mappings)[rowNumber]->isActive() ? Colours::green : Colours::white);
}

void MappingListBoxModel::paintCell(Graphics&, int, int, int, int, bool)
{
    
}

Component* MappingListBoxModel::refreshComponentForCell( const int rowNumber, const int columnId, const bool isRowSelected,
                                                        Component* existingComponentToUpdate)
{
    const auto& mapping = (*_mappings)[rowNumber];
    
    if (columnId == Icon)
    {
        auto* image_component = dynamic_cast<ImageComponent*> (existingComponentToUpdate);

        if (image_component == nullptr)
        {
            image_component = new ImageComponent();
            image_component->setTooltip("Icon for mapped App");
        }
        
        if (image_component != nullptr)
        {
            if (std::cmp_less(rowNumber, _mappings->size()))
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

    if (columnId == Path)
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
        
        auto* pathSelectorComponent = dynamic_cast<PathSelectorComponent*> (existingComponentToUpdate);
        if (pathSelectorComponent == nullptr)
        {
            pathSelectorComponent = new PathSelectorComponent(rowNumber);
            pathSelectorComponent->comboBox.addListener(this);
        }
        
        if (pathSelectorComponent != nullptr)
        {
            pathSelectorComponent->comboBox.setText(mapping->path(), dontSendNotification);
            pathSelectorComponent->comboBox.setTooltip(mapping->path());
            pathSelectorComponent->browseButton.setTooltip("Browse for executable");
            
            if (mapping->isActualPath())
            {
                if (mapping->fileExists())
                    pathSelectorComponent->comboBox.setColour(ComboBox::textColourId, Colours::green);
                else
                    pathSelectorComponent->comboBox.setColour(ComboBox::textColourId, Colours::red);
            }
        }
        return pathSelectorComponent;
    }

    if (columnId == Profile)
    {
        auto* profileComboBox = dynamic_cast<GridComboBox*> (existingComponentToUpdate);
        if (profileComboBox == nullptr)
        {
            profileComboBox = new GridComboBox("Profile", rowNumber);
            profileComboBox->setTooltip("Name of Profile to switch to");
            profileComboBox->addListener(this);
        }
        
        if (profileComboBox != nullptr)
        {
            
            profileComboBox->clear();

            int selected_index = 0;
            int index = 0;
            for (auto& profile : mapping->deviceProfiles())
            {
                profileComboBox->addItem(profile, ++index);

                if (profile == mapping->profileName())
                {
                    selected_index = index;
                }
            }

            profileComboBox->setSelectedId(selected_index, dontSendNotification);
        }

        return profileComboBox;
    }

    if (columnId == Device)
    {
        auto* deviceComboBox = dynamic_cast<GridComboBox*> (existingComponentToUpdate);
        if (deviceComboBox == nullptr)
        {
            deviceComboBox = new GridComboBox("Device", rowNumber);
            deviceComboBox->setTooltip("Device to switch");
            deviceComboBox->addListener(this);
        }
        
        if (deviceComboBox != nullptr)
        {
            deviceComboBox->clear();

            int selected_index = 0;
            int index = 0;
            
            for (auto& device_id : mapping->deviceIds())
            {
                deviceComboBox->addItem(device_id.DisplayName, ++index);

                if (device_id.DeviceId == mapping->deviceId())
                {
                    selected_index = index;
                }
            }

            deviceComboBox->setSelectedId(selected_index, dontSendNotification);
        }

        return deviceComboBox;
    }

    if (columnId == Active)
    {
        auto* toggleButton = dynamic_cast<GridToggleButton*> (existingComponentToUpdate);
        if (toggleButton == nullptr)
        {
            toggleButton = new GridToggleButton("", rowNumber);
            toggleButton->setColour(ToggleButton::tickColourId, Colours::black);
            toggleButton->setColour(ToggleButton::tickDisabledColourId, Colours::black);
            toggleButton->setTooltip("Activate or deactivate this mapping");
            toggleButton->addListener(this);
        }
        
        if (toggleButton != nullptr)
        {
            toggleButton->setToggleState(mapping->isActive(), dontSendNotification);
        }

        return toggleButton;
    }

    if (columnId == Delete)
    {
        auto* gridTextButton = dynamic_cast<GridTextButton*> (existingComponentToUpdate);
        if (gridTextButton == nullptr)
        {
            gridTextButton = new GridTextButton("DELETE", rowNumber);
            gridTextButton->setTooltip("Delete this mapping");
            gridTextButton->setColour(TextButton::buttonColourId, Colours::red);
            gridTextButton->addListener(this);
        }

        return gridTextButton;
    }
    
    return TableListBoxModel::refreshComponentForCell(rowNumber, columnId, isRowSelected, existingComponentToUpdate);
}

void MappingListBoxModel::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
    const auto gridComboBox = dynamic_cast<GridComboBox*>(comboBoxThatHasChanged);
    
    if (gridComboBox && gridComboBox->getName() == "Profile")
    {
        // auto row = ???
        const auto& mapping = (*_mappings)[gridComboBox->Row];
        mapping->profileName(gridComboBox->getText());
    }

    if (gridComboBox && gridComboBox->getName() == "Path")
    {
        const auto& mapping = (*_mappings)[gridComboBox->Row];
        mapping->path(gridComboBox->getText());
        owner->updateContent();
    }

    if (gridComboBox && gridComboBox->getName() == "Device")
    {
        const auto& mapping = (*_mappings)[gridComboBox->Row];
        for (auto& [DisplayName, DeviceId] : mapping->deviceIds())
        {
            if (DisplayName == gridComboBox->getText())
            {
                mapping->deviceId(DeviceId);
            }
        }
    }
}

void MappingListBoxModel::buttonClicked(Button* sender)
{
    if (const auto gridToggleButton = dynamic_cast<GridToggleButton*>(sender))
    {
        (*_mappings)[gridToggleButton->Row]->isActive(gridToggleButton->getToggleState());
        owner->repaintRow(gridToggleButton->Row);
        return;
    }

    if (const auto gridTextButton = dynamic_cast<GridTextButton*>(sender))
    {
        const ModifierKeys modifiers = ModifierKeys::getCurrentModifiers();

        if (! modifiers.isCtrlDown())
        {
            if ( ! AlertWindow::showOkCancelBox(MessageBoxIconType::WarningIcon, "Delete Mapping?", "Do you really want to delete this mapping?"))
                return;       
        }
        
        _mappings->erase(_mappings->begin() + gridTextButton->Row);
        owner->updateContent();
        return;
    }
}

void MappingListBoxModel::textEditorTextChanged(TextEditor& text_editor)
{
    if (const GridTextEditor* grid_text_editor = dynamic_cast<GridTextEditor*>(&text_editor))
    {
        const auto& mapping = (*_mappings)[grid_text_editor->Row];
        mapping->path(grid_text_editor->getText());
    }
}

void MappingListBoxModel::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
    owner->updateContent();
}
