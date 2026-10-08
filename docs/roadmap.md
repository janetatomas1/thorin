
# Roadmap

Ideas for what to build next, grouped by area. Sections marked (done) record what was built.
Design details for measuring live in [measure.md](measure.md); its styling section describes an
earlier `Styled<W, S...>` design that was replaced by `Style` (see below).

## Next

Before writing a real app with thorin, in this order:

1. **Frame pacing, `Continuous` only** (see [Frame pacing](#frame-pacing)): `FrameConfig` with
   `Continuous` and `swapInterval`. `OnDemand` and redraw requests wait.
2. **RenderWidget: render callback, auto-size, depth** (see [OpenGL](#opengl)), together since
   they all touch `RenderWidget`.
3. **RenderWidget: mouse input** (see [OpenGL](#opengl)).

Then the app, to find out what is actually missing.

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

### Scroll containers (done)

`ChildWindow` draws its Yoga box as an ImGui child window; ImGui handles scrollbars and
clipping.

- Its layout is an origin (`Layout::set_origin`): children's positions are relative to it,
  which is what `SetCursorPos` inside `BeginChild` expects.
- The scroll range is ImGui's own, from the items drawn in the child window, plus the
  trailing Yoga padding and border. Popups inside it are drawn in their own window, so
  they don't stretch it.
- Scrollbars are given room through the Yoga border on the right / bottom edge.

## Styling (done)

Every widget owns a `Style` (`Widget::style()`), holding only the values that were set:

- Colours by index: `color(ImGuiCol)` / `color(ImGuiCol, value)`. Each style var has a named
  getter / setter (`set_frame_padding`, `set_alpha`, ...), kept apart by type in a float and an
  `ImVec2` vector. Getters return `std::optional`; setting `std::nullopt` removes the value.
- `Widget::draw()` pushes the style around `show()`, so children inherit it like in ImGui. It is
  popped before the tooltip.
- `measure_trampoline` pushes the styles of the widget and all its ancestors, root first,
  before calling `measure()`. Layout runs before anything is drawn, so this is how a widget is
  measured with the style it is drawn with.
- Style-var setters mark the widget's whole subtree dirty (colours don't): the leaves under a
  container inherit its vars, and `Layout::mark_dirty()` alone does nothing on a container.
- `Style` holds a `Widget*` for that; it is moved with the widget and can't be copied.

- **Fonts.** `Style::set_font(ImFont*)` and `set_font_size()` push `PushFont` with the rest.
  An `ImFont*` belongs to one window's atlas; `Window::load_font()` (file or memory) adds one
  there and returns it, so a font is loaded once and shared between styles.
  `Window::set_font()` makes one the window's primary font (`ImGuiIO::FontDefault`).
- **Widget shortcuts.** `Widget` forwards every `Style` getter and setter, like it does for
  `Layout`.
- **Re-parenting.** `set_parent()` marks the whole subtree dirty, since it now inherits a
  different style.
- **Widgets drawn outside Yoga** (menu entries, table cells) are drawn inside their owner's
  `draw()`, so they inherit its style. Owners that measure them (`Table`, `MenuBar`) push the
  entry's own style around it.
- **Window-wide style** is ImGui's global style; there is no thorin wrapper for it.

Still open:

- Changing a table cell's or menu's style doesn't dirty the owner, so its measured size stays
  stale until something else does.

## New widgets

Each follows the established pattern: `enable_measure()` in the constructor, `measure()`
based on ImGui's own sizing, and `mark_dirty()` in setters that change size.

### Simple leaves

| Widget           | ImGui call                              | Measure notes                                      |
|------------------|-----------------------------------------|----------------------------------------------------|
| ColorButton      | `ColorButton`                           | takes a size; default frame-height square          |
| VSlider          | `VSliderScalar`                         | takes a size; needs an intrinsic height            |
| Image            | `Image`                                 | texture size, or aspect ratio via Yoga             |
| PlotLines/Histogram | `PlotLines` / `PlotHistogram`        | field width + label; takes a graph size            |

### Done

`Separator` (with `SeparatorText` title, vertical variant, title alignment) and `ProgressBar`
(overlay text, indeterminate mode).

### Collapsible containers

CollapsingHeader, TreeNode. (`TabWidget` is done and follows this pattern; by default its tabs
share the strip's width equally, `set_stretch(false)` gives them their natural width.)

- The header or tab strip is drawn by the container itself.
- When collapsed, or when a tab isn't selected, children get `display(YGDisplayNone)`, so Yoga
  gives them no space.
- Toggling marks the layout dirty. `display` is a style setter, so Yoga notices on its own.
- These containers draw something *and* have children, so they can't use a measure function.
  The header would be its own leaf child placed first.

### Overlays (done)

- **Tooltip:** `Widget::set_tooltip`, shown while the widget's box is hovered.
- **Popup, Modal:** ordinary Yoga children, absolutely positioned (no space taken in the
  parent) and origin layouts. `show()` opens the ImGui popup at the Yoga box's screen
  position and size; children are laid out inside it. `Modal` is a `Popup` with
  `BeginPopupModal`. They nest in each other and in `ChildWindow`.

### Docking (done)

- **DockArea:** an ImGui dock space over its Yoga box. Panes are added with `add_pane()` and
  joined on the next dispatch; `remove_pane()` is deferred too.
- **Pane:** a widget drawn in its own ImGui window, absolutely positioned and an origin layout
  like `Popup`, sized to its window each frame. `set_dock(side)` and `set_ratio()` set where it
  starts and how much of the area it takes; edges and ratios are respected when the area is first
  built. Panes can be closable (a closed pane keeps its children and takes no space, `open()`
  brings it back) and minimized while floating. The layout isn't saved between runs.

## Widget lifecycle (done)

- **init / destroy:** `Widget::init()` and `destroy()` reach the children recursively, `init()`
  in Yoga order and `destroy()` in reverse.
- **post():** `widget.post(fn)` queues `fn` to run on the next dispatch with the widget as its
  own type. Widgets are looked up by id at that point, so the action is dropped if the widget
  was destroyed meanwhile, and a moved widget gets it at its new address. Callbacks such as
  `Button`'s go through it.

## OpenGL

`RenderWidget` draws a texture and owns a framebuffer with it as the colour attachment. Rendering
into it is done by hand in the parent's `show()`; a real 3D view needs more.

- **Render callback.** `set_on_render([](globjects::Framebuffer& fb, int width, int height) {...})`.
  `RenderWidget::show()` binds the framebuffer, sets `glViewport` to the texture size, calls it,
  unbinds and restores the viewport, then draws the image.
- **Auto-size.** An option to resize the texture to the Yoga box whenever the box changes, times
  the display scale: windows use `SDL_WINDOW_HIGH_PIXEL_DENSITY`, so the box in points is not the
  pixel count. Resize only on change, since `resize()` reallocates.
- **Depth attachment.** An optional depth renderbuffer, resized with the texture.
- **Mouse input.** For camera controls: hovered, mouse position relative to the view, drag delta
  per button, wheel. ImGui has these after the `Image` call (`IsItemHovered`, `GetMousePos()` minus
  `GetItemRectMin()`, `GetMouseDragDelta`, `io.MouseWheel`); an `InvisibleButton` over the image
  keeps drags from moving the window.

Later: sharing GL objects between windows. Each window has its own context and nothing is
shared (`SDL_GL_SHARE_WITH_CURRENT_CONTEXT`); only needed when several windows show the same
textures or meshes.

## Frame pacing

The main loop sleeps a hard-coded `SDL_Delay(20)` per iteration (`WindowManager::update`) and
every window swaps with vsync on (`SDL_GL_SetSwapInterval(1)` in `GLBackend::init`). The two
waits stack: the swap waits for the refresh after the sleep, about 30 fps at 60 Hz, and each
extra window can cost another refresh.

Replace both with an app-wide `FrameConfig` and a per-window swap interval:

```cpp
enum class FramePolicy { Continuous, OnDemand };

struct FrameConfig {
    FramePolicy policy = FramePolicy::Continuous;
    int targetFps = 60;         // Continuous: the cap. OnDemand: max rate while redrawing. 0 = uncapped
    int idleTimeoutMs = 500;    // OnDemand: wake at least this often, -1 = only on events
    int minSleepMs = 1;         // always yield a little, even when a frame is over budget
};

// WindowConfig
int swapInterval = 0;           // 1 = vsync, 0 = off, -1 = adaptive (fall back to 1 if it fails)
```

- **Continuous** sleeps `max(budget - elapsed, minSleep)` after each frame (`SDL_GetTicksNS`,
  `SDL_DelayPrecise`). It covers a frame-rate cap, a fixed delay (`targetFps = 0`, the sleep is
  `minSleep`), and uncapped (`targetFps = 0`, `minSleepMs = 0`).
- **OnDemand** blocks in `SDL_WaitEventTimeout(nullptr, idleTimeoutMs)` while nothing asks
  for a redraw, and runs like Continuous while something does. `idleTimeoutMs = -1` is purely
  event-driven.
- **Vsync** is `swapInterval`, separate from the policy; pure vsync is `Continuous` with
  `targetFps = 0` and `swapInterval = 1`.
- The policy is a `switch` in a `WindowManager::wait()` called where `SDL_Delay(20)` is now.

### Redraw requests

`Thorin::request_redraw(int frames = 2)` keeps an OnDemand loop drawing for that many frames.
Every SDL event requests a redraw (ImGui needs a frame or two to settle hover and layout), and
an animating widget (e.g. `RenderWidget`) requests one from its `render()` every frame it is
drawn.

Needed before OnDemand is usable:

- `ActionManager` delays count dispatches, so delayed actions stall while the loop sleeps. Keep
  redrawing while the delay ring has pending actions.
- `add_action` is thread-safe, but a sleeping loop won't see it until the timeout. It should
  also `SDL_PushEvent` a user event to wake the loop.

Later: skip `update()` for minimized or occluded windows (`SDL_WINDOW_MINIMIZED`,
`SDL_WINDOW_OCCLUDED`).

## Tools

### Inspector

An overlay any thorin app can toggle, like browser devtools:

- The widget tree, walked through the Yoga tree (`Layout::child()`, `owner()`), with each
  widget's type, title and id.
- The selected widget's Yoga box drawn over the app (margin, border, padding, content), plus
  its computed position and size, measure results and flex properties.
- Its `Style`: the values set on it and the ones it inherits from ancestors.
- Picking: hover a widget in the app to select it in the tree.

Mostly reads data that already exists, and makes debugging layout and styling much faster.
