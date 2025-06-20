#pragma once
#include "Main.h"
#include "MappingListBoxModel.h"

class MappingTable final : public juce::TableListBox
{
public:
    MappingTable()
    {
        auto& hdr = getHeader();
        
        hdr.setStretchToFitActive(true);

        using ColumnIds = MappingListBoxModel::ColumnIds;
        
        hdr.addColumn("Icon", ColumnIds::Icon, 30, 20, 40);
        hdr.addColumn("Path", ColumnIds::Path, 120);
        hdr.addColumn("Profile", ColumnIds::Profile, 90);
        hdr.addColumn("Device", ColumnIds::Device, 90);
        hdr.addColumn("Active", ColumnIds::Active, 40, 40, 40);
        hdr.addColumn("Delete", ColumnIds::Delete, 60, 60, 80);

        hdr.resizeAllColumnsToFit(getWidth());
    }
};
