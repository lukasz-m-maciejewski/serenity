/*
 * Copyright (c) 2022, MacDue <macdue@dueutil.tech>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "HighlightWidget.h"

#include <Applications/MouseSettings/HighlightWidgetGML.h>
#include <LibGUI/ConnectionToWindowServer.h>

HighlightWidget::HighlightWidget()
{
    load_from_gml(highlight_widget_gml);

    m_highlight_preview = find_descendant_of_type_named<GUI::Frame>("preview_frame")->add<MouseSettings::HighlightPreviewWidget>(palette());

    auto current_highlight_color = GUI::ConnectionToWindowServer::the().get_cursor_highlight_color();
    auto current_highlight_color_no_alpha = current_highlight_color;
    current_highlight_color_no_alpha.set_alpha(255);
    m_highlight_color_input = *find_descendant_of_type_named<GUI::ColorInput>("highlight_color_input");
    m_highlight_color_input->set_color(current_highlight_color_no_alpha);
    m_highlight_color_input->on_change = [&]() {
        m_highlight_preview->set_color(highlight_color());
        set_modified(true);
    };

    m_highlight_opacity_slider = *find_descendant_of_type_named<GUI::Slider>("highlight_opacity_slider");
    m_highlight_opacity_slider->set_value(current_highlight_color.alpha());
    m_highlight_opacity_slider->on_change = [&](int) {
        m_highlight_preview->set_color(highlight_color());
        set_modified(true);
    };

    m_highlight_radius_slider = *find_descendant_of_type_named<GUI::Slider>("highlight_radius_slider");
    m_highlight_radius_slider->set_value(GUI::ConnectionToWindowServer::the().get_cursor_highlight_radius());
    m_highlight_radius_slider->on_change = [&](int) {
        m_highlight_preview->set_radius(highlight_radius());
        set_modified(true);
    };

    m_highlight_preview->set_color(highlight_color());
    m_highlight_preview->set_radius(highlight_radius());
}

Gfx::Color HighlightWidget::highlight_color()
{
    auto color = m_highlight_color_input->color();
    color.set_alpha(m_highlight_opacity_slider->value());
    return color;
}

int HighlightWidget::highlight_radius()
{
    auto current_value = m_highlight_radius_slider->value();
    if (current_value <= m_highlight_radius_slider->min())
        return 0;
    return current_value;
}

void HighlightWidget::apply_settings()
{
    GUI::ConnectionToWindowServer::the().async_set_cursor_highlight_radius(highlight_radius());
    GUI::ConnectionToWindowServer::the().async_set_cursor_highlight_color(highlight_color());
}

void HighlightWidget::reset_default_values()
{
    constexpr auto default_highlight_color = Gfx::Color::NamedColor::Yellow;
    constexpr auto default_highlight_opacity = 80; // (in range of 0-255)
    // Disable the highlighting by default.
    // The range of radii you can configure the highlight to is 20 to 60px,
    // anything less than that is treated as 'no highlighting'.
    constexpr auto default_highlight_radius_length = 19;
    m_highlight_color_input->set_color(default_highlight_color);
    m_highlight_opacity_slider->set_value(default_highlight_opacity);
    m_highlight_radius_slider->set_value(default_highlight_radius_length);
}
