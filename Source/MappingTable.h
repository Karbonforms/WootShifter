#pragma once
#include "Main.h"
#include "Mapping.h"
#include "MappingListBoxModel.h"

class MappingTable final : public juce::TableListBox
{
public:
    MappingTable()
    {
        auto& header = getHeader();
        
        header.setStretchToFitActive(true);

        using ColumnIds = MappingListBoxModel::ColumnIds;
        
        header.addColumn("Icon", ColumnIds::Icon, 30, 20, 40);
        header.addColumn("Path", ColumnIds::Path, 120);
        header.addColumn("Profile", ColumnIds::Profile, 90);
        header.addColumn("Device", ColumnIds::Device, 90);
        header.addColumn("Active", ColumnIds::Active, 40, 40);
        header.addColumn("Delete", ColumnIds::Delete, 60, 60, 80);
    
        header.resizeAllColumnsToFit(getWidth());
    }
};
