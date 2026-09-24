# Roadmap

Ideas for what to build next, grouped by area. Nothing here is implemented yet.
Design details for measuring and styling live in [measure.md](measure.md).

**Suggested order:** headless measure tests → explicit heights → automatic child rendering.
The tests verify the explicit-height work; automatic child rendering removes the most
boilerplate from every example.

## Finish the measure work

### Explicit heights on frame widgets

ImGui frame widgets (Slider, Drag, Input, TextInput, ColorEdit, Dropdown) take no height.
When Yoga gives one a fixed height, push `ImGuiStyleVar_FramePadding` with
`y = (height - FontSize) / 2` around the ImGui call, then pop. `Button` and
`InputTextMultiline` already take a size directly. After this, remove the hard-coded
`.height(24.0f)` from the slider, drag, input and coloredit examples.

This is the last step of the implementation order in [measure.md](measure.md).

### Headless measure tests

A test executable that needs no window:

1. `ImGui::CreateContext()`, build the font atlas, set `io.DisplaySize`.
2. `NewFrame()`, `Begin()`, draw each widget through `render()`.
3. Compare the widget's `measure()` result with ImGui's `GetItemRectSize()` for the same widget.

The GUI isn't run during development, so today measure functions are only checked by
reading ImGui's source. These tests would catch mistakes such as the ColorPicker's
vertical spacing (worked out from `ItemSize` / `BeginGroup`, never seen on screen), and
cover every future widget as it's added.

Group items (ColorEdit, ColorPicker, Input with step buttons) report the whole group's
rect after `EndGroup`, which is the size to compare against.

### RadioGroup's `&selected_` pointer

Each `RadioButton` stores `int* groupValue_`, pointing at its group's `selected_`. If the
`RadioGroup` is moved (for example as a member of a widget that moves), the options point at
the moved-from group. Parent lookup survives moves (see measure.md); this pointer doesn't.

Options:

- Look the value up through the parent: `static_cast<RadioGroup&>(*parent()).selected_`.
  Works because parenthood follows the Yoga tree.
- Have `RadioGroup`'s move operations rebind every option's pointer.

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
