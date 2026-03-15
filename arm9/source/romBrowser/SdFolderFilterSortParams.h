#pragma once
#include "SdFolderSortType.h"
#include "SdFolderSortDirection.h"

class SdFolderFilterSortParams
{
public:
    SdFolderSortType sortType = SdFolderSortType::Name;
    SdFolderSortDirection sortDirection = SdFolderSortDirection::Ascending;
    const char* searchQuery = nullptr;

    SdFolderFilterSortParams() { }

    SdFolderFilterSortParams(
        SdFolderSortType sortType, SdFolderSortDirection sortDirection, const char* searchQuery = nullptr)
        : sortType(sortType), sortDirection(sortDirection), searchQuery(searchQuery) { }
};
