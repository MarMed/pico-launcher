#include "common.h"
#include <string.h>
#include "core/StringUtil.h"
#include "SearchViewModel.h"

const char* SearchViewModel::GetSearchQuery() const
{
    return _romBrowserController->GetSearchQuery();
}

bool SearchViewModel::HasActiveSearch() const
{
    return GetSearchQuery()[0] != 0;
}

int SearchViewModel::GetResultCount() const
{
    const auto& romBrowserViewModel = _romBrowserController->GetRomBrowserViewModel();
    if (!romBrowserViewModel.IsValid())
    {
        return 0;
    }

    return romBrowserViewModel->GetFileInfoManager().GetItemCount();
}

void SearchViewModel::AppendCharacter(char character)
{
    char nextQuery[RomBrowserController::SEARCH_QUERY_MAX_LENGTH + 1];
    StringUtil::Copy(nextQuery, GetSearchQuery(), sizeof(nextQuery));
    u32 length = strlen(nextQuery);
    if (length >= RomBrowserController::SEARCH_QUERY_MAX_LENGTH)
    {
        return;
    }

    nextQuery[length] = character;
    nextQuery[length + 1] = 0;
    _romBrowserController->SetSearchQuery(nextQuery);
}

void SearchViewModel::Backspace()
{
    char nextQuery[RomBrowserController::SEARCH_QUERY_MAX_LENGTH + 1];
    StringUtil::Copy(nextQuery, GetSearchQuery(), sizeof(nextQuery));
    u32 length = strlen(nextQuery);
    if (length == 0)
    {
        return;
    }

    nextQuery[length - 1] = 0;
    _romBrowserController->SetSearchQuery(nextQuery);
}

void SearchViewModel::Clear()
{
    _romBrowserController->SetSearchQuery("");
}

void SearchViewModel::Close()
{
    _romBrowserController->HideSearch();
}
