#include "common.h"
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include "gui/Alignment.h"
#include "gui/GraphicsContext.h"
#include "gui/VramContext.h"
#include "gui/input/InputProvider.h"
#include "core/StringUtil.h"
#include "core/math/RgbMixer.h"
#include "themes/material/MaterialColorScheme.h"
#include "themes/IFontRepository.h"
#include "SearchBottomSheetView.h"

namespace
{
    constexpr int TITLE_LABEL_X = 20;
    constexpr int TITLE_LABEL_Y = 10;
    constexpr int QUERY_LABEL_X = 20;
    constexpr int QUERY_LABEL_Y = 26;
    constexpr int RESULT_LABEL_X = 20;
    constexpr int RESULT_LABEL_Y = 42;
    constexpr int GRID_TOP_Y = 54;
    constexpr int ROW_HEIGHT = 16;
    constexpr int ROW_SPACING = 1;
    constexpr int KEY_WIDTH = 24;
    constexpr int KEY_SPACING = 2;
    constexpr int SPACE_WIDTH = 46;
    constexpr int ACTION_WIDTH = 52;
    constexpr int ACTION_SPACING = 2;

    struct SearchKeySpec
    {
        enum class Action
        {
            Append,
            Backspace,
            Clear,
            Done
        };

        const char* label;
        int row;
        int column;
        int width;
        char character;
        Action action;
    };

    static const std::array<int, SearchBottomSheetView::ROW_COUNT> sRowLengths =
    {
        8, 8, 8, 8, 7, 4
    };

    static const std::array<int, SearchBottomSheetView::ROW_COUNT> sRowStarts =
    {
        0, 8, 16, 24, 32, 39
    };

    static const std::array<SearchKeySpec, SearchBottomSheetView::KEY_COUNT> sKeySpecs =
    {{
        { "A", 0, 0, KEY_WIDTH, 'A', SearchKeySpec::Action::Append },
        { "B", 0, 1, KEY_WIDTH, 'B', SearchKeySpec::Action::Append },
        { "C", 0, 2, KEY_WIDTH, 'C', SearchKeySpec::Action::Append },
        { "D", 0, 3, KEY_WIDTH, 'D', SearchKeySpec::Action::Append },
        { "E", 0, 4, KEY_WIDTH, 'E', SearchKeySpec::Action::Append },
        { "F", 0, 5, KEY_WIDTH, 'F', SearchKeySpec::Action::Append },
        { "G", 0, 6, KEY_WIDTH, 'G', SearchKeySpec::Action::Append },
        { "H", 0, 7, KEY_WIDTH, 'H', SearchKeySpec::Action::Append },
        { "I", 1, 0, KEY_WIDTH, 'I', SearchKeySpec::Action::Append },
        { "J", 1, 1, KEY_WIDTH, 'J', SearchKeySpec::Action::Append },
        { "K", 1, 2, KEY_WIDTH, 'K', SearchKeySpec::Action::Append },
        { "L", 1, 3, KEY_WIDTH, 'L', SearchKeySpec::Action::Append },
        { "M", 1, 4, KEY_WIDTH, 'M', SearchKeySpec::Action::Append },
        { "N", 1, 5, KEY_WIDTH, 'N', SearchKeySpec::Action::Append },
        { "O", 1, 6, KEY_WIDTH, 'O', SearchKeySpec::Action::Append },
        { "P", 1, 7, KEY_WIDTH, 'P', SearchKeySpec::Action::Append },
        { "Q", 2, 0, KEY_WIDTH, 'Q', SearchKeySpec::Action::Append },
        { "R", 2, 1, KEY_WIDTH, 'R', SearchKeySpec::Action::Append },
        { "S", 2, 2, KEY_WIDTH, 'S', SearchKeySpec::Action::Append },
        { "T", 2, 3, KEY_WIDTH, 'T', SearchKeySpec::Action::Append },
        { "U", 2, 4, KEY_WIDTH, 'U', SearchKeySpec::Action::Append },
        { "V", 2, 5, KEY_WIDTH, 'V', SearchKeySpec::Action::Append },
        { "W", 2, 6, KEY_WIDTH, 'W', SearchKeySpec::Action::Append },
        { "X", 2, 7, KEY_WIDTH, 'X', SearchKeySpec::Action::Append },
        { "Y", 3, 0, KEY_WIDTH, 'Y', SearchKeySpec::Action::Append },
        { "Z", 3, 1, KEY_WIDTH, 'Z', SearchKeySpec::Action::Append },
        { "0", 3, 2, KEY_WIDTH, '0', SearchKeySpec::Action::Append },
        { "1", 3, 3, KEY_WIDTH, '1', SearchKeySpec::Action::Append },
        { "2", 3, 4, KEY_WIDTH, '2', SearchKeySpec::Action::Append },
        { "3", 3, 5, KEY_WIDTH, '3', SearchKeySpec::Action::Append },
        { "4", 3, 6, KEY_WIDTH, '4', SearchKeySpec::Action::Append },
        { "5", 3, 7, KEY_WIDTH, '5', SearchKeySpec::Action::Append },
        { "6", 4, 0, KEY_WIDTH, '6', SearchKeySpec::Action::Append },
        { "7", 4, 1, KEY_WIDTH, '7', SearchKeySpec::Action::Append },
        { "8", 4, 2, KEY_WIDTH, '8', SearchKeySpec::Action::Append },
        { "9", 4, 3, KEY_WIDTH, '9', SearchKeySpec::Action::Append },
        { "-", 4, 4, KEY_WIDTH, '-', SearchKeySpec::Action::Append },
        { "_", 4, 5, KEY_WIDTH, '_', SearchKeySpec::Action::Append },
        { ".", 4, 6, KEY_WIDTH, '.', SearchKeySpec::Action::Append },
        { "Space", 5, 0, SPACE_WIDTH, ' ', SearchKeySpec::Action::Append },
        { "Back", 5, 1, ACTION_WIDTH, 0, SearchKeySpec::Action::Backspace },
        { "Clear", 5, 2, ACTION_WIDTH, 0, SearchKeySpec::Action::Clear },
        { "Done", 5, 3, ACTION_WIDTH, 0, SearchKeySpec::Action::Done }
    }};
}

class SearchKeyView : public View
{
public:
    typedef void (*action_t)(SearchKeyView* sender, void* arg);

    SearchKeyView(int width, const char* label,
        const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository)
        : _width(width)
        , _label(width, ROW_HEIGHT, 6, fontRepository->GetFont(FontType::Regular10))
        , _materialColorScheme(materialColorScheme)
    {
        _label.SetHorizontalAlignment(Alignment::Center);
        _label.SetText(label);
    }

    void InitVram(const VramContext& vramContext) override
    {
        _label.InitVram(vramContext);
    }

    void Update() override
    {
        _label.SetPosition(_position.x, _position.y);
    }

    void Draw(GraphicsContext& graphicsContext) override
    {
        const auto& baseBackground = _materialColorScheme->GetColor(md::sys::color::surfaceBright);
        const auto& baseForeground = _materialColorScheme->onSurfaceVariant;
        if (_isFocused)
        {
            auto focusedBackground = RgbMixer::Lerp(baseBackground, _materialColorScheme->onSurface, 18, 100);
            _label.SetBackgroundColor(focusedBackground);
            _label.SetForegroundColor(_materialColorScheme->onSurface);
        }
        else
        {
            _label.SetBackgroundColor(baseBackground);
            _label.SetForegroundColor(baseForeground);
        }

        _label.Draw(graphicsContext);
    }

    void VBlank() override
    {
        _label.VBlank();
    }

    Rectangle GetBounds() const override
    {
        return Rectangle(_position, _width, ROW_HEIGHT);
    }

    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override
    {
        if (inputProvider.Triggered(InputKey::A))
        {
            if (_action)
            {
                _action(this, _actionArg);
            }
            return true;
        }

        return View::HandleInput(inputProvider, focusManager);
    }

    void SetAction(action_t action, void* arg)
    {
        _action = action;
        _actionArg = arg;
    }

private:
    int _width;
    Label2DView _label;
    const MaterialColorScheme* _materialColorScheme;
    action_t _action = nullptr;
    void* _actionArg = nullptr;
};

SearchBottomSheetView::SearchBottomSheetView(SearchViewModel* viewModel,
    const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository)
    : _viewModel(viewModel)
    , _materialColorScheme(materialColorScheme)
    , _titleLabel(160, 16, 12, fontRepository->GetFont(FontType::Medium11))
    , _queryLabel(216, 16, RomBrowserController::SEARCH_QUERY_MAX_LENGTH + 8, fontRepository->GetFont(FontType::Regular10))
    , _resultCountLabel(216, 16, 16, fontRepository->GetFont(FontType::Regular10))
{
    _titleLabel.SetText(u"Search");
    AddChildTail(&_titleLabel);
    AddChildTail(&_queryLabel);
    AddChildTail(&_resultCountLabel);

    for (int i = 0; i < KEY_COUNT; i++)
    {
        _keys[i] = std::make_unique<SearchKeyView>(
            sKeySpecs[i].width, sKeySpecs[i].label, materialColorScheme, fontRepository);
        _keys[i]->SetAction([] (SearchKeyView* sender, void* arg)
        {
            auto self = reinterpret_cast<SearchBottomSheetView*>(arg);
            self->HandleKeyActivated(self->FindKeyIndex(sender));
        }, this);
        AddChildTail(_keys[i].get());
    }
}

SearchBottomSheetView::~SearchBottomSheetView() = default;

void SearchBottomSheetView::InitVram(const VramContext& vramContext)
{
    BottomSheetView::InitVram(vramContext);
}

void SearchBottomSheetView::Focus(FocusManager& focusManager)
{
    focusManager.Focus(_keys[0].get());
}

void SearchBottomSheetView::UpdateLabels()
{
    _titleLabel.SetPosition(TITLE_LABEL_X, _position.y + TITLE_LABEL_Y);
    _queryLabel.SetPosition(QUERY_LABEL_X, _position.y + QUERY_LABEL_Y);
    _resultCountLabel.SetPosition(RESULT_LABEL_X, _position.y + RESULT_LABEL_Y);

    const char* query = _viewModel->GetSearchQuery();
    if (strcmp(_cachedQuery, query) != 0)
    {
        StringUtil::Copy(_cachedQuery, query, sizeof(_cachedQuery));
        if (_cachedQuery[0] == 0)
        {
            _queryLabel.SetText(u"All items");
        }
        else
        {
            _queryLabel.SetText(_cachedQuery);
        }
    }

    int resultCount = _viewModel->GetResultCount();
    if (_cachedResultCount != resultCount)
    {
        _cachedResultCount = resultCount;
        char buffer[24];
        snprintf(buffer, sizeof(buffer), "%d matches", resultCount);
        _resultCountLabel.SetText(buffer);
    }
}

void SearchBottomSheetView::UpdateKeyPositions()
{
    for (int row = 0; row < ROW_COUNT; row++)
    {
        int rowStart = sRowStarts[row];
        int rowLength = sRowLengths[row];
        int rowY = _position.y + GRID_TOP_Y + row * (ROW_HEIGHT + ROW_SPACING);
        int spacing = row == ROW_COUNT - 1 ? ACTION_SPACING : KEY_SPACING;
        int rowWidth = 0;
        for (int column = 0; column < rowLength; column++)
        {
            rowWidth += sKeySpecs[rowStart + column].width;
        }
        rowWidth += (rowLength - 1) * spacing;

        int x = (256 - rowWidth) / 2;
        for (int column = 0; column < rowLength; column++)
        {
            auto& key = _keys[rowStart + column];
            key->SetPosition(x, rowY);
            key->Update();
            x += sKeySpecs[rowStart + column].width + spacing;
        }
    }
}

void SearchBottomSheetView::Update()
{
    BottomSheetView::Update();
    UpdateLabels();
    UpdateKeyPositions();
}

void SearchBottomSheetView::Draw(GraphicsContext& graphicsContext)
{
    graphicsContext.SetClipArea(GetBounds());
    u32 oldPrio = graphicsContext.SetPriority(1);
    {
        _titleLabel.SetBackgroundColor(_materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _titleLabel.SetForegroundColor(_materialColorScheme->onSurface);
        _queryLabel.SetBackgroundColor(_materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _queryLabel.SetForegroundColor(_viewModel->HasActiveSearch()
            ? _materialColorScheme->onSurface
            : _materialColorScheme->onSurfaceVariant);
        _resultCountLabel.SetBackgroundColor(_materialColorScheme->GetColor(md::sys::color::surfaceContainerLow));
        _resultCountLabel.SetForegroundColor(_materialColorScheme->onSurfaceVariant);
        BottomSheetView::Draw(graphicsContext);
    }
    graphicsContext.SetPriority(oldPrio);
    graphicsContext.ResetClipArea();
}

bool SearchBottomSheetView::HandleInput(
    const InputProvider& inputProvider, FocusManager& focusManager)
{
    if (inputProvider.Triggered(InputKey::B))
    {
        _viewModel->Close();
        return true;
    }
    if (inputProvider.Triggered(InputKey::X))
    {
        _viewModel->Clear();
        return true;
    }
    if (inputProvider.Triggered(InputKey::Y))
    {
        _viewModel->Backspace();
        return true;
    }

    return false;
}

int SearchBottomSheetView::FindKeyIndex(const View* view) const
{
    for (int i = 0; i < KEY_COUNT; i++)
    {
        if (_keys[i].get() == view)
        {
            return i;
        }
    }

    return -1;
}

View* SearchBottomSheetView::GetKeyAtRowColumn(int row, int column) const
{
    if (row < 0 || row >= ROW_COUNT)
    {
        return nullptr;
    }
    if (column < 0 || column >= sRowLengths[row])
    {
        return nullptr;
    }

    return _keys[sRowStarts[row] + column].get();
}

View* SearchBottomSheetView::MoveFocus(View* currentFocus,
    FocusMoveDirection direction, View* source)
{
    int keyIndex = FindKeyIndex(currentFocus);
    if (keyIndex < 0)
    {
        return nullptr;
    }

    int row = 0;
    while (row + 1 < ROW_COUNT && keyIndex >= sRowStarts[row + 1])
    {
        row++;
    }
    int column = keyIndex - sRowStarts[row];

    switch (direction)
    {
        case FocusMoveDirection::Left:
            return GetKeyAtRowColumn(row, column - 1);

        case FocusMoveDirection::Right:
            return GetKeyAtRowColumn(row, column + 1);

        case FocusMoveDirection::Up:
            if (row == 0)
                return nullptr;
            return GetKeyAtRowColumn(row - 1, std::min(column, sRowLengths[row - 1] - 1));

        case FocusMoveDirection::Down:
            if (row == ROW_COUNT - 1)
                return nullptr;
            return GetKeyAtRowColumn(row + 1, std::min(column, sRowLengths[row + 1] - 1));
    }

    return nullptr;
}

void SearchBottomSheetView::HandleKeyActivated(int keyIndex)
{
    if (keyIndex < 0)
    {
        return;
    }

    const auto& keySpec = sKeySpecs[keyIndex];
    switch (keySpec.action)
    {
        case SearchKeySpec::Action::Append:
            _viewModel->AppendCharacter(keySpec.character);
            break;

        case SearchKeySpec::Action::Backspace:
            _viewModel->Backspace();
            break;

        case SearchKeySpec::Action::Clear:
            _viewModel->Clear();
            break;

        case SearchKeySpec::Action::Done:
            _viewModel->Close();
            break;
    }
}
