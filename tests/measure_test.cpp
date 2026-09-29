// Checks each leaf widget's measure() against what ImGui actually draws, without a window.
// Yoga sizes the widget through measure(); render() then draws it, and GetItemRectSize() is
// the size ImGui laid out. Group widgets report the whole group's rect after EndGroup.

#include <gtest/gtest.h>

#include <functional>
#include <string>

#include <imgui.h>

#include <thorin/button.hpp>
#include <thorin/checkbox.hpp>
#include <thorin/coloredit.hpp>
#include <thorin/colorpicker.hpp>
#include <thorin/drag.hpp>
#include <thorin/dropdown.hpp>
#include <thorin/input.hpp>
#include <thorin/progressbar.hpp>
#include <thorin/separator.hpp>
#include <thorin/radiobutton.hpp>
#include <thorin/radiogroup.hpp>
#include <thorin/slider.hpp>
#include <thorin/text.hpp>
#include <thorin/textinput.hpp>
#include <thorin/widget.hpp>
#include <thorin/windowconfig.hpp>

using namespace thorin;

namespace {
    constexpr float displayWidth = 1280.0f;
    constexpr float displayHeight = 720.0f;
    // Yoga rounds boxes to whole pixels; ImGui doesn't.
    constexpr float tolerance = 1.0f;

    class Measure : public ::testing::Test {
    protected:
        // Leaves keep their intrinsic width instead of stretching to the column.
        Widget root;

        void SetUp() override {
            ImGui::CreateContext();

            ImGuiIO& io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.DisplaySize = ImVec2{displayWidth, displayHeight};
            io.DeltaTime = 1.0f / 60.0f;
            // Like ImGui's null renderer: the atlas builds on demand, and texture requests are
            // acknowledged after each frame without uploading anything.
            io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures | ImGuiBackendFlags_RendererHasVtxOffset;

            root.column(4.0f).align_items(YGAlignFlexStart);
        }

        void TearDown() override {
            ImGui::DestroyContext();
        }

        // One frame as GLBackend::update runs it: layout inside the frame, then draw.
        void frame(const std::function<void()>& draw) {
            ImGui::NewFrame();
            ImGui::SetNextWindowPos(ImVec2{0.0f, 0.0f});
            ImGui::SetNextWindowSize(ImVec2{displayWidth, displayHeight});
            ImGui::Begin("measure", nullptr, WindowConfig{}.windowFlags);

            root.layout().calculate_layout(displayWidth, displayHeight);
            draw();

            ImGui::End();
            ImGui::Render();

            if (auto textures = ImGui::GetDrawData()->Textures; textures != nullptr) {
                for (ImTextureData* texture : *textures) {
                    if (texture->Status == ImTextureStatus_WantCreate || texture->Status == ImTextureStatus_WantUpdates) {
                        texture->SetStatus(ImTextureStatus_OK);
                    } else if (texture->Status == ImTextureStatus_WantDestroy) {
                        texture->SetStatus(ImTextureStatus_Destroyed);
                    }
                }
            }
        }

        // Yoga's box and ImGui's drawn rect for one widget.
        struct Sizes {
            ImVec2 layout;
            ImVec2 drawn;
        };

        Sizes sizes(Widget& widget) {
            Sizes result{};
            frame([&] {
                widget.render();
                result.layout = widget.size();
                result.drawn = ImGui::GetItemRectSize();
            });
            return result;
        }

        void expect_matches(Widget& widget) {
            const Sizes s = sizes(widget);
            EXPECT_GT(s.layout.x, 0.0f) << widget.title();
            EXPECT_GT(s.layout.y, 0.0f) << widget.title();
            EXPECT_NEAR(s.layout.x, s.drawn.x, tolerance) << widget.title() << " width";
            EXPECT_NEAR(s.layout.y, s.drawn.y, tolerance) << widget.title() << " height";
        }

        // Explicit box on a frame widget: ImGui must draw exactly that box.
        void expect_box(Widget& widget, float width, float height) {
            widget.width(width).height(height);
            const Sizes s = sizes(widget);
            EXPECT_NEAR(s.layout.x, width, tolerance) << widget.title() << " layout width";
            EXPECT_NEAR(s.layout.y, height, tolerance) << widget.title() << " layout height";
            EXPECT_NEAR(s.drawn.x, width, tolerance) << widget.title() << " drawn width";
            EXPECT_NEAR(s.drawn.y, height, tolerance) << widget.title() << " drawn height";
        }
    };
}

// Intrinsic sizes

TEST_F(Measure, Button) {
    Button button("Click me", &root);
    expect_matches(button);
}

TEST_F(Measure, ButtonHiddenLabelSuffix) {
    Button button("Save##toolbar", &root);
    expect_matches(button);
}

TEST_F(Measure, Checkbox) {
    Checkbox checkbox("Enable feature", nullptr, &root);
    expect_matches(checkbox);
}

TEST_F(Measure, RadioButton) {
    int value = 0;
    RadioButton radio("Option", &value, 0, &root);
    expect_matches(radio);
}

TEST_F(Measure, Text) {
    Text text("Hello, world!", &root);
    expect_matches(text);
}

TEST_F(Measure, TextWrapsAtWidth) {
    Text text("The quick brown fox jumps over the lazy dog, again and again and again.", &root);
    text.width(120.0f);
    const Sizes s = sizes(text);
    EXPECT_GT(s.layout.y, ImGui::GetFontSize() * 1.5f) << "expected several lines";
    EXPECT_NEAR(s.layout.y, s.drawn.y, tolerance);
}

TEST_F(Measure, Slider) {
    SliderFloat<> slider("Volume", &root);
    expect_matches(slider);
}

TEST_F(Measure, SliderMultiComponent) {
    SliderInt<3> slider("Color", &root);
    expect_matches(slider);
}

TEST_F(Measure, Drag) {
    DragFloat<2> drag("Position", &root);
    expect_matches(drag);
}

TEST_F(Measure, InputWithStepButtons) {
    InputInt<> input("Count", &root);
    expect_matches(input);
}

TEST_F(Measure, InputWithoutStepButtons) {
    InputFloat<> input("Weight", &root);
    expect_matches(input);
}

TEST_F(Measure, InputMultiComponent) {
    InputFloat<3> input("Offset", &root);
    expect_matches(input);
}

TEST_F(Measure, TextInput) {
    TextInput input("Name", &root);
    expect_matches(input);
}

TEST_F(Measure, TextInputMultiline) {
    TextInput input("Notes", &root);
    input.set_multiline(true);
    expect_matches(input);
}

TEST_F(Measure, Dropdown) {
    Dropdown dropdown("Color", &root);
    dropdown.add_option("Red").add_option("Green");
    expect_matches(dropdown);
}

TEST_F(Measure, DropdownWidthFitPreview) {
    Dropdown dropdown("Color", &root);
    dropdown.add_option("Red").add_option("Dark green");
    dropdown.set_selected(1);
    dropdown.set_flags(ImGuiComboFlags_WidthFitPreview);
    expect_matches(dropdown);
}

TEST_F(Measure, DropdownNoPreview) {
    Dropdown dropdown("Color", &root);
    dropdown.set_flags(ImGuiComboFlags_NoPreview);
    expect_matches(dropdown);
}

TEST_F(Measure, ColorEdit) {
    ColorEdit<> edit("Tint", &root);
    expect_matches(edit);
}

TEST_F(Measure, ColorEditNoInputs) {
    ColorEdit<3> edit("Swatch", &root);
    edit.set_flags(ImGuiColorEditFlags_NoInputs);
    expect_matches(edit);
}

TEST_F(Measure, ColorPicker) {
    ColorPicker<> picker("Accent", &root);
    expect_matches(picker);
}

TEST_F(Measure, ColorPickerRgb) {
    ColorPicker<3> picker("Background", &root);
    expect_matches(picker);
}

TEST_F(Measure, ColorPickerWithReference) {
    ColorPicker<> picker("Accent", &root);
    picker.set_reference({1.0f, 0.0f, 0.0f, 1.0f});
    expect_matches(picker);
}

TEST_F(Measure, ColorPickerAlphaBar) {
    ColorPicker<> picker("Accent", &root);
    picker.set_flags(ImGuiColorEditFlags_AlphaBar);
    expect_matches(picker);
}

TEST_F(Measure, ColorPickerNoSidePreview) {
    ColorPicker<> picker("Accent", &root);
    picker.set_flags(ImGuiColorEditFlags_NoSidePreview);
    expect_matches(picker);
}

TEST_F(Measure, ProgressBar) {
    ProgressBar bar(&root);
    bar.set_fraction(0.5f);
    expect_matches(bar);
}

// Separators stretch across the column (align_self), so the root's width.
TEST_F(Measure, Separator) {
    Separator separator("", &root);
    const Sizes s = sizes(separator);
    EXPECT_NEAR(s.layout.x, displayWidth, tolerance);
    EXPECT_NEAR(s.drawn.x, displayWidth, tolerance);
    EXPECT_NEAR(s.layout.y, s.drawn.y, tolerance);
}

TEST_F(Measure, SeparatorText) {
    Separator separator("Section", &root);
    expect_matches(separator);
}

TEST_F(Measure, SeparatorTextLeftWidth) {
    Separator separator("Section", &root);
    separator.set_left_width(40.0f).width(300.0f);
    expect_matches(separator);
}

TEST_F(Measure, SeparatorExplicitWidth) {
    Separator separator("", &root);
    separator.width(200.0f);
    const Sizes s = sizes(separator);
    EXPECT_NEAR(s.drawn.x, 200.0f, tolerance);
}

TEST_F(Measure, ExplicitBoxProgressBar) {
    ProgressBar bar(&root);
    expect_box(bar, 300.0f, 32.0f);
}

TEST_F(Measure, ExplicitBoxSlider) {
    SliderFloat<> slider("Volume", &root);
    expect_box(slider, 300.0f, 32.0f);
}

TEST_F(Measure, ExplicitBoxDrag) {
    DragInt<2> drag("Range", &root);
    expect_box(drag, 300.0f, 32.0f);
}

TEST_F(Measure, ExplicitBoxInput) {
    InputInt<> input("Count", &root);
    expect_box(input, 300.0f, 32.0f);
}

TEST_F(Measure, ExplicitBoxTextInput) {
    TextInput input("Name", &root);
    expect_box(input, 300.0f, 32.0f);
}

TEST_F(Measure, ExplicitBoxColorEdit) {
    ColorEdit<> edit("Tint", &root);
    expect_box(edit, 300.0f, 32.0f);
}

TEST_F(Measure, ExplicitBoxDropdown) {
    Dropdown dropdown("Color", &root);
    dropdown.add_option("Red");
    expect_box(dropdown, 300.0f, 32.0f);
}

TEST_F(Measure, ExplicitHeightShrinksFrame) {
    SliderFloat<> slider("Volume", &root);
    expect_box(slider, 300.0f, 16.0f);
}

TEST_F(Measure, ExplicitHeightWidensSquares) {
    // With an exact height the swatch and combo arrow are that tall and just as wide.
    ColorEdit<3> swatch("Swatch", &root);
    swatch.set_flags(ImGuiColorEditFlags_NoInputs).height(32.0f);
    expect_matches(swatch);

    Dropdown dropdown("Color", &root);
    dropdown.set_flags(ImGuiComboFlags_NoPreview).height(32.0f);
    expect_matches(dropdown);
}

// Moves

TEST_F(Measure, MovedRadioGroupKeepsSelection) {
    RadioGroup group(&root);
    group.add_option("A");
    group.add_option("B");

    RadioGroup moved(std::move(group));
    moved.at(1).select();
    EXPECT_EQ(moved.selected(), 1);
    EXPECT_TRUE(moved.at(1).selected());
    EXPECT_FALSE(moved.at(0).selected());
}

TEST_F(Measure, MoveAssignedRadioGroupKeepsSelection) {
    RadioGroup group(&root);
    group.add_option("A");
    group.add_option("B");

    RadioGroup target;
    target = std::move(group);
    target.at(0).select();
    target.set_selected(1);
    EXPECT_TRUE(target.at(1).selected());
}
