# Roadmap

Ideas for what to build next, grouped by area. Nothing here is implemented yet.
Design details for measuring and styling live in [measure.md](measure.md).

## Finish the measure work (done)

- **Explicit heights on frame widgets.** Slider, Drag, Input, TextInput (single line),
  ColorEdit and Dropdown push `ImGuiStyleVar_FramePadding` with
  `y = (height - FontSize) / 2` when their box differs from `GetFrameHeight()`
  (`Widget::push_frame_height`). Frame-height squares (combo arrow, colour swatch) are
  measured with the exact height too (`Widget::frame_height`). The hard-coded heights are
  gone from the examples.
- **Headless measure tests.** `tests/measure_test.cpp` (GoogleTest, `ctest`) creates an
  ImGui context with no window, lays each widget out and compares Yoga's box with
  `GetItemRectSize()`. Covers every leaf and the main flag variants, including the
  ColorPicker, plus explicit boxes on frame widgets. Build with `-DTHORIN_BUILD_TESTS=OFF`
  to skip.
- **RadioGroup's `&selected_` pointer.** `RadioGroup`'s move operations re-point every
  option at the new group's `selected_`.

## Layout

### Automatic child rendering (done)

Implemented: `Widget::show()` renders its children by walking the Yoga tree, skipping
children with `display(YGDisplayNone)`. The examples no longer override `show()`, and
`RadioGroup::show()` calls the base. Original sketch:

```cpp
bool Widget::show() {
    bool changed = false;
    for (size_t i = 0; i < YGNodeGetChildCount(node); ++i) {
        // child node → context (Layout*) → owner() → render()
        changed |= child->render();
    }
    return changed;
}
```

- Removes the hand-written `show()` in every example
  (`changed |= a.render(); changed |= b.render(); ...`).
- Plain `Row`, `Column` and `Panel` containers come almost free: a `Widget` with
  `row()` / `column()` set.
- Children render in Yoga child order, which is insertion order.
- Leaves are unaffected: they override `show()` and have no children.
- Containers that draw something themselves (`RadioGroup`) can keep their override or call
  the base.

### Scroll containers

Content bigger than the window is simply cut off today. Combine Yoga
`overflow(YGOverflowScroll)` with `ImGui::BeginChild` / `EndChild`:

- The container's box becomes a child window; ImGui handles scrollbars and clipping.
- Children are positioned relative to the child window, so `render()`'s `SetCursorPos`
  must use positions relative to the scroll container, not the root.
- Yoga must lay the content out with unbounded size on the scroll axis. That size becomes
  the child window's content size.

## Styling

Designed in [measure.md](measure.md#styling):

- `Styled<W, S...>` with `Font`, `Color` and `Padding` parts. Styles are pushed in both
  `show()` and `measure()`.
- `Window::set_style`, which applies a per-window global style change and marks measured
  nodes dirty.
- Templated `RadioGroup<W = RadioButton>`, so each option can be a styled leaf.

## New widgets

Each follows the established pattern: `enable_measure()` in the constructor, `measure()`
based on ImGui's own sizing, and `mark_dirty()` in setters that change size.

### Simple leaves

| Widget           | ImGui call                              | Measure notes                                      |
|------------------|-----------------------------------------|----------------------------------------------------|
| ProgressBar      | `ProgressBar`                           | field width, frame height; takes a size directly   |
| Separator        | `Separator` / `SeparatorText`           | fills width; 1 px or text height                   |
| ColorButton      | `ColorButton`                           | takes a size; default frame-height square          |
| VSlider          | `VSliderScalar`                         | takes a size; needs an intrinsic height            |
| Image            | `Image`                                 | texture size, or aspect ratio via Yoga             |
| Selectable       | `Selectable`                            | label size; `SpanAllColumns` fills width           |
| PlotLines/Histogram | `PlotLines` / `PlotHistogram`        | field width + label; takes a graph size            |

### Collapsible containers

CollapsingHeader, TreeNode, TabBar:

- The header or tab strip is drawn by the container itself.
- When collapsed, or when a tab isn't selected, children get `display(YGDisplayNone)`, so Yoga
  gives them no space.
- Toggling marks the layout dirty. `display` is a style setter, so Yoga notices on its own.
- These containers draw something *and* have children, so they can't use a measure function.
  The header would be its own leaf child placed first.

### Overlays

Tooltip, Popup, Modal:

- They float above the layout rather than taking space in it, so they aren't children in the
  Yoga tree, or they are absolutely positioned.
- A widget outside the tree has no parent, so `window()` / `app()` can't walk up. These
  widgets need `window_` set directly (see measure.md, parenthood).
- Each has its own ImGui window, so it could have its own root `Layout`, with
  `calculate_layout` run inside its `Begin` / `End`.
