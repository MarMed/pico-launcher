#pragma once
#include "../RomBrowserController.h"

class SearchViewModel
{
public:
    explicit SearchViewModel(IRomBrowserController* romBrowserController)
        : _romBrowserController(romBrowserController) { }

    const char* GetSearchQuery() const;
    bool HasActiveSearch() const;
    int GetResultCount() const;

    void AppendCharacter(char character);
    void Backspace();
    void Clear();
    void Close();

private:
    IRomBrowserController* _romBrowserController;
};
