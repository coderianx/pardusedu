#include "mainwindow.h"
#include <nlohmann/json.hpp>
#include <webkit/webkit.h>
#include <md4c-html.h>
#include <sstream>
#include <fstream>
#include <regex>

using json = nlohmann::json;

static std::string dersler_course_css(const std::string& course) {
    if (course == "Matematik") return "dersler-course-math";
    if (course == "Fen Bilimleri") return "dersler-course-science";
    if (course.find("T\u00fcrk") != std::string::npos) return "dersler-course-turkish";
    if (course == "Sosyal Bilgiler") return "dersler-course-social";
    if (course == "\u0130ngilizce") return "dersler-course-english";
    return "";
}

static void dersler_cleanup_old(Gtk::Stack& stack) {
    auto* visible = stack.get_visible_child();
    if (!visible) return;
    auto* child = stack.get_first_child();
    while (child) {
        auto* next = child->get_next_sibling();
        if (child != visible)
            stack.remove(*child);
        child = next;
    }
}

void MainWindow::setup_dersler() {
    GError* error = nullptr;
    GBytes* bytes = g_resources_lookup_data(
        "/org/ogrenci/merkezi/assets/dersler/dersler.json",
        G_RESOURCE_LOOKUP_FLAGS_NONE,
        &error
    );
    if (bytes) {
        gsize size;
        const char* data = (const char*)g_bytes_get_data(bytes, &size);
        try { dersler_data = json::parse(std::string(data, size)); }
        catch (...) { dersler_data = json::object(); }
        g_bytes_unref(bytes);
    }

    dersler_page_box.set_margin_start(20);
    dersler_page_box.set_margin_end(20);
    dersler_page_box.set_margin_top(16);
    dersler_page_box.set_margin_bottom(16);

    dersler_back_btn.add_css_class("dersler-back-btn");
    dersler_header_title.add_css_class("page-title");

    dersler_back_btn.signal_clicked().connect([this]() {
        auto name = dersler_stack.get_visible_child_name();
        if (name == "content") {
            dersler_show_units(dersler_selected_grade, dersler_selected_course);
        } else if (name == "units") {
            dersler_show_courses(dersler_selected_grade);
        } else if (name == "courses") {
            dersler_show_grades();
        }
    });

    dersler_header_box.append(dersler_back_btn);
    dersler_header_box.append(dersler_header_title);
    dersler_header_box.set_margin_bottom(16);

    dersler_page_box.append(dersler_header_box);
    dersler_page_box.append(dersler_stack);

    dersler_stack.set_transition_type(Gtk::StackTransitionType::SLIDE_LEFT_RIGHT);
    dersler_stack.set_transition_duration(350);
    dersler_stack.property_transition_running().signal_changed().connect([this]() {
        if (!dersler_stack.property_transition_running().get_value())
            dersler_cleanup_old(dersler_stack);
    });

    dersler_show_grades();

    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);
    sw->set_child(dersler_page_box);
    stack.add(*sw, "dersler");
}

static Gtk::Widget* make_dersler_card(
    const std::string& arrow_text,
    const std::string& main_text,
    const std::string& sub_text,
    const std::string& css_class,
    int height
) {
    auto* btn = Gtk::make_managed<Gtk::Button>();
    if (!css_class.empty()) btn->add_css_class(css_class);
    btn->set_hexpand(true);
    btn->set_size_request(-1, height);

    auto* hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 16);
    hbox->set_margin_start(20);
    hbox->set_margin_end(20);

    auto* text_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 2);
    text_box->set_hexpand(true);
    text_box->set_halign(Gtk::Align::START);
    text_box->set_valign(Gtk::Align::CENTER);

    auto* main_lbl = Gtk::make_managed<Gtk::Label>(main_text);
    main_lbl->add_css_class("dersler-card-main");
    main_lbl->set_halign(Gtk::Align::START);

    text_box->append(*main_lbl);

    if (!sub_text.empty()) {
        auto* sub_lbl = Gtk::make_managed<Gtk::Label>(sub_text);
        sub_lbl->add_css_class("dersler-card-sub");
        sub_lbl->set_halign(Gtk::Align::START);
        text_box->append(*sub_lbl);
    }

    auto* arrow_lbl = Gtk::make_managed<Gtk::Label>(arrow_text);
    arrow_lbl->add_css_class("dersler-card-arrow");

    hbox->append(*text_box);
    hbox->append(*arrow_lbl);
    btn->set_child(*hbox);
    return btn;
}

void MainWindow::dersler_show_grades() {
    auto* existing = dersler_stack.get_child_by_name("grades");
    if (existing) {
        dersler_stack.set_visible_child(*existing);
        dersler_back_btn.set_visible(false);
        dersler_header_title.set_text("Dersler");
        dersler_selected_grade = 0;
        dersler_selected_course.clear();
        return;
    }

    dersler_back_btn.set_visible(false);
    dersler_header_title.set_text("Dersler");
    dersler_selected_grade = 0;
    dersler_selected_course.clear();

    auto* outer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 16);
    outer->set_valign(Gtk::Align::START);
    outer->set_margin_top(8);

    auto* title_lbl = Gtk::make_managed<Gtk::Label>("Sınıf Seçin");
    title_lbl->add_css_class("dersler-section-title");
    title_lbl->set_halign(Gtk::Align::START);
    outer->append(*title_lbl);

    auto* sub_lbl = Gtk::make_managed<Gtk::Label>("Görmek istediğiniz sınıfın derslerini seçin");
    sub_lbl->add_css_class("dersler-section-subtitle");
    sub_lbl->set_halign(Gtk::Align::START);
    outer->append(*sub_lbl);

    outer->append(*Gtk::make_managed<Gtk::Label>("")); // spacer

    auto* btn5 = Gtk::make_managed<Gtk::Button>();
    btn5->set_hexpand(true);
    btn5->set_size_request(-1, 80);
    {
        auto* hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 16);
        hbox->set_margin_start(20);
        hbox->set_margin_end(20);
        auto* text = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 2);
        text->set_hexpand(true);
        text->set_valign(Gtk::Align::CENTER);
        auto* main_lbl = Gtk::make_managed<Gtk::Label>("5. Sinif");
        main_lbl->set_halign(Gtk::Align::START);
        main_lbl->add_css_class("dersler-card-main");
        auto* sub_lbl = Gtk::make_managed<Gtk::Label>("Temel konular - 5 ders");
        sub_lbl->add_css_class("dersler-card-sub");
        sub_lbl->set_halign(Gtk::Align::START);
        text->append(*main_lbl);
        text->append(*sub_lbl);
        auto* arrow = Gtk::make_managed<Gtk::Label>("→");
        arrow->add_css_class("dersler-card-arrow");
        hbox->append(*text);
        hbox->append(*arrow);
        btn5->set_child(*hbox);
    }
    btn5->add_css_class("dersler-grade-btn");
    btn5->set_name("grade-5-btn");
    {
        auto p5 = Gtk::CssProvider::create();
        p5->load_from_data("button#grade-5-btn { background-color: rgb(99,102,241); background-image: none; border: none; border-radius: 16px; }");
        btn5->get_style_context()->add_provider(p5, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }

    auto* btn6 = Gtk::make_managed<Gtk::Button>();
    btn6->set_hexpand(true);
    btn6->set_size_request(-1, 80);
    {
        auto* hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 16);
        hbox->set_margin_start(20);
        hbox->set_margin_end(20);
        auto* text = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 2);
        text->set_hexpand(true);
        text->set_valign(Gtk::Align::CENTER);
        auto* main_lbl = Gtk::make_managed<Gtk::Label>("6. Sinif");
        main_lbl->set_halign(Gtk::Align::START);
        main_lbl->add_css_class("dersler-card-main");
        auto* sub_lbl = Gtk::make_managed<Gtk::Label>("Ileri duzey - 5 ders");
        sub_lbl->add_css_class("dersler-card-sub");
        sub_lbl->set_halign(Gtk::Align::START);
        text->append(*main_lbl);
        text->append(*sub_lbl);
        auto* arrow = Gtk::make_managed<Gtk::Label>("→");
        arrow->add_css_class("dersler-card-arrow");
        hbox->append(*text);
        hbox->append(*arrow);
        btn6->set_child(*hbox);
    }
    btn6->add_css_class("dersler-grade-btn");
    btn6->set_name("grade-6-btn");
    {
        auto p6 = Gtk::CssProvider::create();
        p6->load_from_data("button#grade-6-btn { background-color: rgb(236,72,153); background-image: none; border: none; border-radius: 16px; }");
        btn6->get_style_context()->add_provider(p6, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }

    btn5->signal_clicked().connect([this]() {
        dersler_selected_grade = 5;
        dersler_show_courses(5);
    });
    btn6->signal_clicked().connect([this]() {
        dersler_selected_grade = 6;
        dersler_show_courses(6);
    });

    outer->append(*btn5);
    outer->append(*btn6);

    dersler_stack.add(*outer, "grades");
    dersler_stack.set_visible_child(*outer);
}

void MainWindow::dersler_show_courses(int grade) {
    dersler_back_btn.set_visible(true);
    dersler_header_title.set_text(std::to_string(grade) + ". Sinif Dersleri");

    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);

    auto* outer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    outer->set_halign(Gtk::Align::FILL);
    outer->set_margin_top(8);

    auto* section_title = Gtk::make_managed<Gtk::Label>(std::to_string(grade) + ". Sinif Dersleri");
    section_title->add_css_class("dersler-section-title");
    section_title->set_halign(Gtk::Align::START);
    outer->append(*section_title);

    std::string grade_key = std::to_string(grade);
    if (dersler_data.contains(grade_key)) {
        auto& courses = dersler_data[grade_key];

        std::vector<std::string> course_names;
        int total_units = 0;
        std::map<std::string, int> unit_counts;
        for (auto it = courses.begin(); it != courses.end(); ++it) {
            course_names.push_back(it.key());
            int cnt = it.value().size();
            unit_counts[it.key()] = cnt;
            total_units += cnt;
        }

        auto* section_sub = Gtk::make_managed<Gtk::Label>(
            std::to_string(course_names.size()) + " ders - " + std::to_string(total_units) + " unite"
        );
        section_sub->add_css_class("dersler-section-subtitle");
        section_sub->set_halign(Gtk::Align::START);
        outer->append(*section_sub);

        auto* cards_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
        cards_box->set_margin_top(16);

        for (size_t idx = 0; idx < course_names.size(); ++idx) {
            std::string course_name = course_names[idx];
            std::string css_class = dersler_course_css(course_name);
            int ucount = unit_counts[course_name];

            auto* card = make_dersler_card(
                "→", course_name,
                std::to_string(ucount) + " unite",
                "dersler-course-btn " + css_class, 72
            );

            std::string cn = course_name;
            auto* btn = dynamic_cast<Gtk::Button*>(card);
            if (btn) {
                btn->signal_clicked().connect([this, btn, grade, cn]() {
                    btn->add_css_class("dersler-btn-pulse");
                    dersler_selected_course = cn;
                    dersler_show_units(grade, cn);
                });
            }

            cards_box->append(*card);
        }

        outer->append(*cards_box);
    }

    sw->set_child(*outer);
    dersler_stack.add(*sw, "courses");
    dersler_stack.set_visible_child(*sw);
}

void MainWindow::dersler_show_units(int grade, const std::string& course) {
    dersler_back_btn.set_visible(true);
    dersler_header_title.set_text(course);

    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);

    auto* outer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    outer->set_halign(Gtk::Align::FILL);
    outer->set_margin_top(8);

    auto* section_title = Gtk::make_managed<Gtk::Label>(course + " Uniteleri");
    section_title->add_css_class("dersler-section-title");
    section_title->set_halign(Gtk::Align::START);
    outer->append(*section_title);

    std::string grade_key = std::to_string(grade);
    if (dersler_data.contains(grade_key) && dersler_data[grade_key].contains(course)) {
        auto& units = dersler_data[grade_key][course];

        auto* section_sub = Gtk::make_managed<Gtk::Label>(
            std::to_string(units.size()) + " unite"
        );
        section_sub->add_css_class("dersler-section-subtitle");
        section_sub->set_halign(Gtk::Align::START);
        outer->append(*section_sub);

        auto* cards_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
        cards_box->set_margin_top(16);

        std::string uc_css = dersler_course_css(course);
        int u_idx = 1;
        for (auto& unit : units) {
            std::string title = unit["title"];
            std::string file = unit["file"];

            auto* frame = Gtk::make_managed<Gtk::Frame>();
            frame->add_css_class("dersler-unit-card");
            if (!uc_css.empty()) frame->add_css_class(uc_css + "-unit");
            frame->set_hexpand(true);

            auto* hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 14);
            hbox->set_margin_start(16);
            hbox->set_margin_end(16);
            hbox->set_margin_top(14);
            hbox->set_margin_bottom(14);

            auto* badge = Gtk::make_managed<Gtk::Label>(std::to_string(u_idx));
            badge->add_css_class("dersler-unit-badge");

            auto* arrow_lbl = Gtk::make_managed<Gtk::Label>("→");
            arrow_lbl->add_css_class("dersler-card-arrow");

            auto* text_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 2);
            text_box->set_halign(Gtk::Align::START);
            text_box->set_hexpand(true);

            auto* title_lbl = Gtk::make_managed<Gtk::Label>(title);
            title_lbl->add_css_class("dersler-unit-title");
            title_lbl->set_halign(Gtk::Align::START);
            title_lbl->set_wrap(true);

            auto* meta_lbl = Gtk::make_managed<Gtk::Label>("Unite " + std::to_string(u_idx));
            meta_lbl->add_css_class("dersler-unit-meta");
            meta_lbl->set_halign(Gtk::Align::START);

            text_box->append(*title_lbl);
            text_box->append(*meta_lbl);

            hbox->append(*badge);
            hbox->append(*text_box);
            hbox->append(*arrow_lbl);
            frame->set_child(*hbox);

            std::string f = file;
            auto gesture = Gtk::GestureClick::create();
            gesture->signal_released().connect([this, f](int, double, double) {
                dersler_show_content(f);
            });
            frame->add_controller(gesture);

            cards_box->append(*frame);
            u_idx++;
        }

        outer->append(*cards_box);
    }

    sw->set_child(*outer);
    dersler_stack.add(*sw, "units");
    dersler_stack.set_visible_child(*sw);
}

void MainWindow::dersler_show_content(const std::string& md_file) {
    dersler_back_btn.set_visible(true);
    dersler_header_title.set_text(dersler_selected_course);

    GError* error = nullptr;
    std::string path = "/org/ogrenci/merkezi/assets/dersler/" + md_file + ".md";
    GBytes* bytes = g_resources_lookup_data(path.c_str(), G_RESOURCE_LOOKUP_FLAGS_NONE, &error);
    std::string markdown;
    if (bytes) {
        gsize size;
        const char* data = (const char*)g_bytes_get_data(bytes, &size);
        markdown.assign(data, size);
        g_bytes_unref(bytes);
    } else {
        markdown = "# İçerik Bulunamadı\nBu ünite için ders notu henüz eklenmemiş.";
    }

    std::string html;
    auto md_callback = [](const MD_CHAR* text, MD_SIZE size, void* userdata) {
        auto* out = static_cast<std::string*>(userdata);
        out->append(text, size);
    };
    md_html(markdown.c_str(), markdown.size(), md_callback, &html, MD_FLAG_TABLES, 0);

    std::string css;
    if (dark_mode) {
        css = R"(
html, body { margin:0; padding:0; height:100%; }
body{ background:transparent; color:#e6edf3; font-family: Inter, sans-serif; }
.md-container { background:#161b22; padding:40px; line-height:1.8; max-width:1000px; margin:40px auto; border-radius:16px; box-shadow: 0 6px 18px rgba(0,0,0,0.4); }
h1 { color:#f0f6fc; border-bottom:1px solid #30363d; padding-bottom:12px; } h2 { color:#e6edf3; }
a { color:#58a6ff; text-decoration:none; }
a:hover { color:#79c0ff; text-decoration:underline; }
pre{ background:#0d1117; padding:16px; border-radius:12px; overflow:auto; border:1px solid #30363d; }
code{ color:#79c0ff; font-family: 'JetBrains Mono', monospace; font-size:0.9em; }
pre code{ color:#e6edf3; }
hr{ border:none; height:1px; background:#30363d; margin:40px 0; }
blockquote{border-left:4px solid #58a6ff; padding-left:16px; margin:16px 0; color:#8b949e; font-style:italic; background:#0d1117; border-radius:0 8px 8px 0; padding:12px 16px;}
table{border-collapse:collapse; width:100%; margin:16px 0; border-radius:8px; overflow:hidden;}
th,td{border:1px solid #30363d; padding:10px 14px; text-align:left;}
th{background:#161b22; color:#f0f6fc; font-weight:600;}
tr:nth-child(even){background:#0d1117;}
p,li{color:#e6edf3; line-height:1.9;}
)";
    } else {
        css = R"(
html, body { margin:0; padding:0; height:100%; }
body{ background:transparent; color:#1f2328; font-family: Inter, sans-serif; }
.md-container { background:#ffffff; padding:40px; line-height:1.8; max-width:1000px; margin:40px auto; border-radius:16px; box-shadow: 0 1px 3px rgba(0,0,0,0.08), 0 6px 18px rgba(0,0,0,0.04); }
h1 { color:#1f2328; border-bottom:1px solid #d0d7de; padding-bottom:12px; } h2 { color:#24292f; }
a { color:#0969da; text-decoration:none; }
a:hover { color:#0550ae; text-decoration:underline; }
pre{ background:#f6f8fa; padding:16px; border-radius:12px; overflow:auto; border:1px solid #d0d7de; }
code{ color:#cf222e; font-family: 'JetBrains Mono', monospace; font-size:0.9em; }
pre code{ color:#1f2328; }
hr{ border:none; height:1px; background:#d0d7de; margin:40px 0; }
blockquote{border-left:4px solid #0969da; padding-left:16px; margin:16px 0; color:#656d76; font-style:italic; background:#f6f8fa; border-radius:0 8px 8px 0; padding:12px 16px;}
table{border-collapse:collapse; width:100%; margin:16px 0; border-radius:8px; overflow:hidden;}
th,td{border:1px solid #d0d7de; padding:10px 14px; text-align:left;}
th{background:#f6f8fa; color:#1f2328; font-weight:600;}
tr:nth-child(even){background:#f6f8fa;}
p,li{color:#1f2328; line-height:1.9;}
)";
    }

    std::string final_html = "<!DOCTYPE html>\n<html><head><meta charset=\"utf-8\"><style>" + css +
        "</style></head><body><div class=\"md-container\">" + html + "</div></body></html>";

    auto* webview = WEBKIT_WEB_VIEW(webkit_web_view_new());
    auto* web_widget = Gtk::manage(Glib::wrap(GTK_WIDGET(webview)));
    web_widget->set_hexpand(true);
    web_widget->set_vexpand(true);

    GdkRGBA bg;
    gdk_rgba_parse(&bg, dark_mode ? "#0d1117" : "#f6f8fa");
    webkit_web_view_set_background_color(webview, &bg);
    webkit_web_view_load_html(webview, final_html.c_str(), nullptr);

    dersler_stack.add(*web_widget, "content");
    dersler_stack.set_visible_child(*web_widget);
}

// ============================================================
// Weekly Analysis Functions
// ============================================================
