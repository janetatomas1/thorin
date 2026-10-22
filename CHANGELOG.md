# Changelog

All notable changes to thorin. The format follows [Keep a Changelog](https://keepachangelog.com),
and versions follow [Semantic Versioning](https://semver.org); while the version is 0.x, minor
versions may break the API. The version itself is set in `project()` in `CMakeLists.txt` and
exposed as `THORIN_VERSION` in `thorin/version.hpp`. Plans live in [docs/roadmap.md](docs/roadmap.md).

## [Unreleased]

### Added

- `RenderWidget::set_clear_color()`: clears the texture (and the depth buffer, with `depth()`) before
  each render callback. Empty by default.
- `THORIN_GL` CMake option (ON by default): builds the OpenGL backend and the GL widgets, and links
  glbinding / globjects only when on.
- `Thorin::set_backend_factory()` / `make_backend()`: the app makes each window's backend. Defaults
  to `GLBackend` with `THORIN_GL`.

### Changed

- `Window` takes its backend in the constructor (`Window(std::unique_ptr<GPUBackend>, widget)`)
  instead of creating a `GLBackend` itself; use `app.make_backend(config)`.
- `WindowConfig::sdlFlags` no longer includes `SDL_WINDOW_OPENGL`; `GLBackend` adds it.

## [0.2.0] - 2026-10-20

### Added

- `TreeNode`: a collapsible node with an ImGui tree node header. Its children can be any widgets;
  they are indented by `IndentSpacing` and hidden from Yoga while it is closed. Nest TreeNodes to
  build a tree; `set_flags(ImGuiTreeNodeFlags_Framed)` gives a collapsing header. ImGui's tree
  lines work (`DrawLinesFull` / `DrawLinesToNodes` on the nodes, or `style.TreeLinesFlags`).
- `TreeView`: a scrolling, bordered child window for a tree of TreeNodes, with `expand_all()` and
  `collapse_all()`.
- `ChartWidget`: an ImPlot plot at the widget's Yoga box, with axes (label, fixed limits, tick
  format) and a legend. It owns its series: `LineSeries`, `ScatterSeries` and `BarSeries`, each
  with a stable id, a colour and its axes. Their values are owned (`SeriesBuffer`), borrowed
  (`SeriesView`) or a fixed-capacity ring (`SeriesRing`) for live data.
- `Chart3DWidget`: the ImPlot3D counterpart, with `Line3DSeries` and `Scatter3DSeries`.
- Themes: `Thorin::set_theme(Theme)` sets the ImGui, ImPlot and ImPlot3D styles and the font of
  every window, including open ones. Built in: `dark`, `light`, `classic`, `matrix`, `nord`,
  `blueprint`, `one_dark`.
- Fonts: `Font` holds TTF bytes without an ImGui context; `Thorin::add_font()` / `remove_font()`.
  Inter, JetBrains Mono and Noto Sans are bundled (`Font::inter()`, ...), and can be left out with
  `THORIN_BUNDLE_FONTS=OFF`.
- Window backgrounds: `Image` (decoded with stb_image, `Thorin::add_image()`), drawn stretched
  behind every widget with `Window::set_background()` or `Thorin::set_background()` for all
  windows. A `BackgroundPainter` draws a custom background on top of it
  (`Window::set_background_painter()`, or a factory in `Thorin::set_background_painter()`).
- Window icons: `Thorin::set_icon()` for every window, `Window::set_icon()` for one.
- Logging (spdlog): `Thorin::set_log_config(LogConfig)` with console, rotating file and an
  in-memory buffer (`log_memory()`). thorin logs to its own `"thorin"` logger with a separate
  level; `make_logger(name)` makes app loggers that share the sinks.
- `LogWidget`: the in-memory log with a level filter, a text filter, Clear and Copy; follows new
  messages while scrolled to the bottom.
- `thorin::clipboard`: `set_text()`, `text()`, `has_text()`, shared with ImGui's text inputs.
- `Table`: `set_column_width()` and `set_column_stretch()`, and a constructor for a table without
  a header row.
- `RenderWidget::set_capture_mouse()`: an invisible button over the view, so camera controls can drag
  and use the wheel without moving or scrolling the window. `hovered()` / `active()` tell the render
  callback whether the input from `ImGui::GetIO()` is meant for the view.
- Inspector: a devtools-like window, toggled with F12 or `Window::set_inspector()`. It shows the
  widget tree, draws the selected widget's box over the app, and edits its layout properties.
  Built with `THORIN_INSPECTOR` (on by default).
- Examples: `zion`, a system monitor (CPU, processes, system info, logs), and `chart` /
  `chart3d`.
- Tests: colour parsing, `Series` and `ActionManager`, next to the measure tests.

- CMake package: `find_package(thorin)` and `thorin::thorin`, from the build tree (`thorin_DIR`) or
  after `cmake --install`. The app provides the same dependencies, e.g. through the same Conan toolchain.
- thorin can be used with `add_subdirectory()`; examples and tests then stay off by default
  (`THORIN_BUILD_EXAMPLES`, `THORIN_BUILD_TESTS`).

### Changed

- `WindowConfig::iniFile` sets the ImGui settings file for that window's context. It is empty by
  default, which turns saving off. Before, every window wrote to the same `imgui.ini` in the
  working directory and they overwrote each other's settings.
- Themes default to larger text (`style.FontSizeBase` 0 means 19.5 px, 1.5x ImGui's 13 px; the
  built-in themes use 22 px) and roomier table cells.
- `Pane` resizes its Yoga box to its ImGui window as soon as the window changes size, and
  requests a redraw.

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
