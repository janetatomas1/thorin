# Measure functions and styling

Design notes. **Not implemented yet** except where marked.

## Problem

Yoga decides each widget's position and size; ImGui draws at `SetCursorPos(position)`.
Frame widgets (Slider, Drag, Input, ColorEdit, ...) take a width via `SetNextItemWidth`
but no height, so a `.height(24.0f)` only moves siblings in Yoga while ImGui draws at
`GetFrameHeight()`. Labels are drawn outside `SetNextItemWidth`, so a labelled widget
is also wider than its Yoga box. Widgets without an explicit size get 0 from Yoga.

## Parenthood (implemented)

The Yoga tree is the only record of parenthood. `Widget::parent()` follows
Yoga parent node → node context (`Layout*`) → `Layout::owner()`. Both the context and
the owner are re-set on move, so the result is always the parent's current address.

## Measure functions

Leaf widgets opt in from their constructor:

```cpp
layout().enable_measure();   // YGNodeSetMeasureFunc(node, trampoline)
```

The trampoline is a plain function: node context → `Layout` → `owner()` →
virtual `Widget::measure(width, widthMode, height, heightMode)`. It captures no widget
address, so moves need no re-registration. Containers never enable measure; Yoga
requires measured nodes to be leaves (`add_child` should assert this).

Yoga calls `measure` only from `YGNodeCalculateLayout`, i.e. from
`Layout::calculate_layout`, which runs inside the frame (after `make_current()`,
`NewFrame()` and `Begin()` in `GLBackend::update`). So `measure` may use ImGui
metrics such as `CalcTextSize` and `GetFrameHeight`. `calculate_layout` must not be
called outside a frame.

Yoga calls `measure` only for dimensions that are not set explicitly, so `.width()` /
`.height()` still win. Constraint modes are resolved with a shared helper:

| Mode      | Result                     |
|-----------|----------------------------|
| Exactly   | available                  |
| AtMost    | min(intrinsic, available)  |
| Undefined | intrinsic                  |

### Intrinsic sizes

| Widget                                               | Width                                                 | Height                   |
|------------------------------------------------------|-------------------------------------------------------|--------------------------|
| Text                                                 | `CalcTextSize`, wrapped at the available width        | wrapped text height      |
| Button                                               | text + 2 × `FramePadding.x`                           | text + 2 × `FramePadding.y` |
| Checkbox, RadioButton                                | frame-height square + `ItemInnerSpacing.x` + label    | `GetFrameHeight()`       |
| Slider, Drag, Input, TextInput, ColorEdit, Dropdown  | 16 × `FontSize` + `ItemInnerSpacing.x` + label        | `GetFrameHeight()`       |

- **Default field width** is a fixed multiple of the font size (16×, ImGui's own fallback
  for auto-resizing windows), not `CalcItemWidth()`. The latter is 65% of the window
  width, which Yoga's cache can't see, so it would go stale on resize.
- **Labels are part of the measured width.** `show()` gives the field
  `width() - labelWidth - ItemInnerSpacing.x`. Label width is
  `CalcTextSize(title, nullptr, true)` (text after `##` hidden); 0 when there is no
  visible label.
- **Explicit height** on a frame widget: push `ImGuiStyleVar_FramePadding` with
  `y = (height - FontSize) / 2` around the ImGui call. `Button` and
  `InputTextMultiline` take a size directly.
- Yoga's default `flex-shrink` is 0, so a row of intrinsic-width widgets that doesn't fit
  overflows unless children get `flex_shrink(1)` / `flex(1)`.

## Dirtying

Yoga caches measure results per node, keyed by the available size and modes. It can't see
state that `measure` reads from outside Yoga, so that state must call `Layout::mark_dirty()`
(`YGNodeMarkDirty`, which propagates to the root). `mark_dirty()` is a no-op on nodes
without a measure function, because Yoga aborts on those.

The global ImGui style is assumed fixed for now (no font, `FramePadding`, etc. changes).
With that, the only invalidating events are:

- `Widget::set_title`
- `Widget::set_parent` (the new tree may belong to a window with a different style)
- style wrapper setters (below)
- `Window::set_style` (below)

The value and format of a frame widget don't affect its measured size, so they don't dirty.

## Styling

### Per-widget style: wrappers

Styles are template wrappers that derive from the widget they style:

```cpp
template <WidgetConcept W, class S>
class Styled : public W {
    S style_;
public:
    using W::W;
    bool show() override {
        style_.push(); bool r = W::show(); style_.pop(); return r;
    }
    ImVec2 measure(float w, YGMeasureMode wm, float h, YGMeasureMode hm) override {
        style_.push(); auto s = W::measure(w, wm, h, hm); style_.pop(); return s;
    }
    S& style();   // setters call layout().mark_dirty()
};

template <WidgetConcept W> using Font = Styled<W, FontStyle>;   // PushFont / PopFont
```

- Pushing in **both** `show()` and `measure()` makes the measurement exact. One shared
  push/pop keeps the two from diverging.
- Inheritance, not a member: one `Widget`, one Yoga node, `W`'s full API, and the
  measure function `W` already registered.
- Wrappers stack: `Font<Padding<Slider<1, float>>>`.
- `W`'s setters return `W&`, so call `style()` before chaining `W` setters.

### Idea: one variadic `Styled<W, S...>` instead of nesting

Instead of nesting one wrapper per style (`Font<Color<Button>>`), a single wrapper takes
the widget and a list of style parts:

```cpp
template <WidgetConcept W, class... S>
class Styled : public W {
    std::tuple<S...> styles_;

    void push() { std::apply([](auto&... s) { (s.push(), ...); }, styles_); }
    void pop() {                                   // reverse order
        [&]<std::size_t... I>(std::index_sequence<I...>) {
            (std::get<sizeof...(S) - 1 - I>(styles_).pop(), ...);
        }(std::index_sequence_for<S...>{});
    }

public:
    using W::W;
    bool show() override { push(); bool r = W::show(); pop(); return r; }
    ImVec2 measure(float w, YGMeasureMode wm, float h, YGMeasureMode hm) override {
        push(); auto s = W::measure(w, wm, h, hm); pop(); return s;
    }

    template <class T> T& style() { return std::get<T>(styles_); }   // setters mark_dirty
};

Styled<Button, Font, Color, Padding> ok;
ok.style<Font>().set(bigFont);
```

- **Widget first.** A class template's parameter pack must be last, so
  `Styled<Font, Color, Button>` would need extra metaprogramming to pull out the last type.
  It gains only word order.
- **Style parts are plain structs** (`Font`, `Color`, `Padding`) with `push()` / `pop()`,
  not wrapper templates.
- **Push order is the listed order;** pop is the reverse.
- **Compared with nesting:**
  - flatter types and shorter error messages;
  - one class, vtable and `show()` / `measure()` per combination instead of one per layer;
  - a single push/pop shared by `show()` and `measure()`.
- **Order still matters:** `Styled<B, Font, Color>` and `Styled<B, Color, Font>` are
  distinct types.
- **Each part type may appear once,** because `style<T>()` looks it up by type. Several
  colours go in one `Color` part holding multiple entries (preferred), or one type per
  slot (`Color<ImGuiCol_Text>`).
- **It works as a container item type:** `RadioGroup<Styled<RadioButton, Font>>`
  satisfies `std::derived_from<W, RadioButton>`.
- **Short aliases are still possible:** `template <class W> using WithFont = Styled<W, Font>;`

### Wrappers are for leaves only

A container draws its children inside its own `show()`, but Yoga measures those children
separately. `Font<RadioGroup>` would draw the options in the pushed font while measuring
them with the global one. `Styled::show()` should assert that the node has a measure
function.

### Containers of homogeneous items: item type as a template parameter

Containers whose children are all the same kind take the item type as a template
parameter, so the style lives on each leaf and is measured correctly:

```cpp
template <class W = RadioButton>
    requires std::derived_from<W, RadioButton>
class RadioGroup : public Widget {
    std::vector<W> options_;
    ...
};

RadioGroup<>                   plain;
RadioGroup<Font<RadioButton>>  styled;   // every option measured and drawn with the font
```

- `Styled<RadioButton, S>` derives from `RadioButton`, so the constraint accepts any
  wrapper stack.
- The container becomes a template, so its implementation moves to the header.
- Keeping children in a `std::vector<W>` is safe across reallocation: moves keep the
  node context and owner up to date.
- Heterogeneous containers (a generic column/row) don't need this: each child is wrapped
  individually.
- Possible later extension, if CSS-like inheritance is needed: a virtual
  `push_style()`/`pop_style()` on `Widget` and a trampoline that pushes every ancestor's
  style, root to leaf, before measuring.

### Global style

Each window has its own ImGui context, so "global" style is per window. Changes go through
`Window::set_style(...)`. It applies the change with that window's context current, then
marks every measured node in the window's tree dirty. Editing `ImGui::GetStyle()` directly
is unsupported, because nothing marks the nodes dirty.

## Implementation order

1. Plumbing: `Layout::enable_measure()`, `Layout::mark_dirty()`, trampoline, leaf assert in
   `add_child`.
2. Slider as the reference widget, including label handling.
3. The remaining leaf widgets.
4. Remove the hard-coded `.height(24.0f)` from the examples.
5. `Styled` / `Font`, `Window::set_style`, templated `RadioGroup` (separate change).
