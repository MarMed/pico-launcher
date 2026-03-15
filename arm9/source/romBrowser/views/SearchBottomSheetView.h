#pragma once
#include <array>
#include <memory>
#include "BottomSheetView.h"
#include "gui/views/Label2DView.h"
#include "../viewModels/SearchViewModel.h"

class MaterialColorScheme;
class IFontRepository;
class SearchKeyView;

class SearchBottomSheetView : public BottomSheetView
{
public:
    static constexpr int KEY_COUNT = 43;
    static constexpr int ROW_COUNT = 6;

    SearchBottomSheetView(SearchViewModel* viewModel,
        const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository);
    ~SearchBottomSheetView() override;

    void InitVram(const VramContext& vramContext) override;
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;
    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;
    View* MoveFocus(View* currentFocus, FocusMoveDirection direction, View* source) override;

    void Focus(FocusManager& focusManager) override;

private:
    SearchViewModel* _viewModel;
    const MaterialColorScheme* _materialColorScheme;
    Label2DView _titleLabel;
    Label2DView _queryLabel;
    Label2DView _resultCountLabel;
    std::array<std::unique_ptr<SearchKeyView>, KEY_COUNT> _keys;
    char _cachedQuery[RomBrowserController::SEARCH_QUERY_MAX_LENGTH + 1] = { 0 };
    int _cachedResultCount = -1;

    void UpdateLabels();
    void UpdateKeyPositions();
    int FindKeyIndex(const View* view) const;
    View* GetKeyAtRowColumn(int row, int column) const;
    void HandleKeyActivated(int keyIndex);
};
