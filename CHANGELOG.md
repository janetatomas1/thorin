# Changelog

All notable changes to thorin. The format follows [Keep a Changelog](https://keepachangelog.com),
and versions follow [Semantic Versioning](https://semver.org); while the version is 0.x, minor
versions may break the API. The version itself is set in `project()` in `CMakeLists.txt` and
exposed as `THORIN_VERSION` in `thorin/version.hpp`. Plans live in [docs/roadmap.md](docs/roadmap.md).

## [Unreleased]

## [0.1.0] - 2026-10-07

First version: the features thorin has today.

### Application and windows

- `Thorin` app with a main loop (`exec()` / `exit()`), multiple windows, each with its own ImGui
  context, OpenGL context and root widget. `add_window<W>(config, args...)` creates the root widget
  in place.
- `WindowConfig`: size, title, ImGui window flags, SDL window flags, initial state (maximized,
  minimized). `Window` can maximize, minimize, resize and retitle at runtime.
- Actions: `add_action(fn, delay)` runs `fn` on a later dispatch, delayed by a number of frames;
  it can be called from any thread. `widget.post(fn)` does the same for a widget and drops the
  action if the widget is destroyed in the meantime.
- Widget lifecycle: `init()` / `destroy()` reach children recursively, `destroy()` in reverse order.
- OpenGL 3.3 backend on SDL3 (`GLBackend`), behind the `GPUBackend` interface.
- `thorin/version.hpp` with `THORIN_VERSION` and its major / minor / patch parts.

### Layout

- Yoga flexbox layout on every widget: margin, padding, position, size and min / max size, gap,
  flex direction / grow / shrink / basis / wrap, alignment and justification, as chainable setters.
- Measure functions for every leaf widget, based on ImGui's own sizing, so widgets get their
  natural size. Explicit heights on frame widgets are honoured through frame padding.
- Automatic child rendering in Yoga order; `display(YGDisplayNone)` children are skipped.
- `ChildWindow`: a scrolling, clipping container drawn as an ImGui child window.
- Headless measure tests (`tests/measure_test.cpp`, GoogleTest, `THORIN_BUILD_TESTS`).

### Styling

- Per-widget `Style`: colours by `ImGuiCol` and every ImGui style var, inherited by children,
  also applied while measuring. Getters return `std::optional`; `std::nullopt` removes a value.
- Fonts: `Window::load_font()` (file or memory), `Window::set_font()` for the window default,
  `Style::set_font()` / `set_font_size()` per widget.
- Colours: `from_hex(0xRRGGBBAA)` and the CSS named colours in `thorin::colors`.
- Tooltips on any widget (`set_tooltip`), enabled / disabled state (`set_enabled`).

### Widgets

- **Basic:** `Text`, `Button`, `Checkbox`, `RadioGroup` / `RadioButton`, `Separator` (horizontal,
  vertical, with title and title alignment), `ProgressBar` (overlay text, indeterminate mode).
- **Input:** `TextInput` (single line, multiline, hint), `Slider`, `Drag` and `Input` (int or
  float, 1-4 components, range, step, format), `ColorEdit` and `ColorPicker` (RGB / RGBA,
  reference colour), `Dropdown` (options, placeholder), `Selectable` (multi-select list).
- **Containers:** `Table` (headers, number cells or any widget as a cell), `TabWidget` (tabs added and
  removed at runtime, stretched or natural-width), `MenuBar` / `Menu` / `MenuItem` (submenus, separators,
  shortcuts, checkable items).
- **Overlays:** `Popup` and `Modal`, laid out by Yoga and positioned at their box; they nest.
- **Docking:** `DockArea` with `Pane`s: initial side and ratio, closable panes that take no space
  when closed, minimize / restore while floating.
- **OpenGL:** `RenderWidget` / `ImageWidget` draws a GL texture and owns a framebuffer to render
  into it (resize, UV flip); `ImageButton` is a clickable `RenderWidget`.

### Examples

One example per widget in `examples/`, plus `helloworld`.
