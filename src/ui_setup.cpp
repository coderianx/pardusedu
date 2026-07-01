#include "gtkmm/enums.h"
#include "gtkmm/label.h"
#include "gtkmm/object.h"
#include "mainwindow.h"
#include "resources.h"
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <ctime>

MainWindow::MainWindow() {
    set_title("Pardus Edu");
    set_default_size(1200, 750);

    detect_theme();
    setup_data();
    update_streak();
    check_week_change();
    setup_parduslab();
    setup_ui();
    apply_theme();
}

MainWindow::~MainWindow() {
    if (parduslab) {
        parduslab->cleanup_all();
        delete parduslab;
        parduslab = nullptr;
    }
}

void MainWindow::detect_theme() {
    auto settings = Gtk::Settings::get_default();
    if (settings) dark_mode = settings->property_gtk_application_prefer_dark_theme();
}

std::string MainWindow::get_username() {
    const char* user = getenv("USER");
    if (user && user[0]) return std::string(user);
    user = getenv("LOGNAME");
    if (user && user[0]) return std::string(user);
    return "Öğrenci";
}

void MainWindow::setup_ui() {
    set_child(root_box);

    auto* header = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    header->set_margin_start(16);
    header->set_margin_end(16);
    header->set_margin_top(8);
    header->set_margin_bottom(8);

    header_logo.set_from_resource("/org/ogrenci/merkezi/assets/pardus.png");
    header_logo.set_pixel_size(64);

    auto* title = Gtk::make_managed<Gtk::Label>("PardusEdu");
    title->add_css_class("app-title");
    title->set_hexpand(true);
    title->set_halign(Gtk::Align::START);

    auto* theme_icon = Gtk::make_managed<Gtk::Image>();
    theme_icon->set_from_resource("/org/ogrenci/merkezi/assets/sun.svg");
    theme_icon->set_pixel_size(20);
    btn_theme.set_child(*theme_icon);
    btn_theme.add_css_class("theme-btn");
    btn_theme.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::toggle_theme));

    auto* save_btn = Gtk::make_managed<Gtk::Button>();
    save_icon_ptr = Gtk::make_managed<Gtk::Image>();
    save_icon_ptr->set_from_resource(dark_mode
        ? "/org/ogrenci/merkezi/assets/save.svg"
        : "/org/ogrenci/merkezi/assets/save_dark.svg");
    save_icon_ptr->set_pixel_size(20);
    save_btn->set_child(*save_icon_ptr);
    save_btn->add_css_class("save-btn");
    save_btn->signal_clicked().connect([this]() { save_data(); });

    settings_icon_ptr = Gtk::make_managed<Gtk::Image>();
    settings_icon_ptr->set_from_resource(dark_mode
        ? "/org/ogrenci/merkezi/assets/settings.svg"
        : "/org/ogrenci/merkezi/assets/settings_dark.svg");
    settings_icon_ptr->set_pixel_size(20);
    btn_settings.set_child(*settings_icon_ptr);
    btn_settings.add_css_class("settings-btn");
    btn_settings.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::show_settings_dialog));

    header->append(header_logo);
    header->append(*title);
    header->append(btn_theme);
    header->append(btn_settings);
    header->append(*save_btn);

    root_box.append(*header);
    root_box.append(paned);

    stack.set_transition_type(Gtk::StackTransitionType::CROSSFADE);
    stack.set_transition_duration(200);
    setup_sidebar();
    setup_pages();
    select_first_row();
}

void MainWindow::toggle_theme() {
    dark_mode = !dark_mode;
    apply_theme();
    auto* icon = dynamic_cast<Gtk::Image*>(btn_theme.get_child());
    if (icon) {
        icon->set_from_resource(dark_mode
            ? "/org/ogrenci/merkezi/assets/moon.svg"
            : "/org/ogrenci/merkezi/assets/sun.svg");
    }

    auto* key_icon = Gtk::make_managed<Gtk::Image>();
    key_icon->set_from_resource(dark_mode
        ? "/org/ogrenci/merkezi/assets/settings.svg"
        : "/org/ogrenci/merkezi/assets/settings_dark.svg");
    key_icon->set_pixel_size(20);
    key_icon->set_visible(true);
    btn_ai_key.set_child(*key_icon);

    if (save_icon_ptr) {
        save_icon_ptr->set_from_resource(dark_mode
            ? "/org/ogrenci/merkezi/assets/save.svg"
            : "/org/ogrenci/merkezi/assets/save_dark.svg");
    }

    if (settings_icon_ptr) {
        settings_icon_ptr->set_from_resource(dark_mode
            ? "/org/ogrenci/merkezi/assets/settings.svg"
            : "/org/ogrenci/merkezi/assets/settings_dark.svg");
    }

    if (share_icon_ptr) {
        share_icon_ptr->set_from_resource(dark_mode
            ? "/org/ogrenci/merkezi/assets/share.svg"
            : "/org/ogrenci/merkezi/assets/share_dark.svg");
    }

    auto reload = [](Gtk::Image* img, const char* path) { if (img) img->set_from_resource(path); };
    if (dash_icon_streak) reload(dash_icon_streak, "/org/ogrenci/merkezi/assets/streak.svg");
    if (dash_icon_pomo) reload(dash_icon_pomo, "/org/ogrenci/merkezi/assets/pomo.svg");
    if (dash_icon_timer) reload(dash_icon_timer, "/org/ogrenci/merkezi/assets/timer.svg");

    for (auto& [img, base] : sidebar_icon_widgets) {
        std::string p = "/org/ogrenci/merkezi/assets/" + base + ".svg";
        std::string p_dark = "/org/ogrenci/merkezi/assets/" + base + "_dark.svg";
        img->set_from_resource(dark_mode ? p.c_str() : p_dark.c_str());
    }
}

void MainWindow::apply_theme() {
    auto settings = Gtk::Settings::get_default();
    if (settings) settings->property_gtk_application_prefer_dark_theme() = dark_mode;

    g_resources_register(ogrenci_get_resource());

    auto& cached = dark_mode ? css_provider_dark : css_provider_light;
    if (cached) {
        auto display = Gdk::Display::get_default();
        Gtk::StyleContext::add_provider_for_display(display, cached, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
        reload_python();
        return;
    }

    if (cached_css.empty()) {
        auto file = Gio::File::create_for_uri("resource:///org/ogrenci/merkezi/assets/style.css");
        try {
            auto stream = file->read();
            char buffer[8192];
            while (true) {
                gsize bytes_read = stream->read(buffer, sizeof(buffer));
                if (bytes_read == 0) break;
                cached_css.append(buffer, bytes_read);
            }
        } catch (const Glib::Error& err) {
            g_warning("CSS yüklenemedi: %s", err.what());
            return;
        }
    }

    std::map<std::string, std::string> colors;
    if (dark_mode) {
        colors = {
            {"__bg_color", "#121212"},
            {"__sidebar_bg", "#1a1a1a"},
            {"__card_bg", "#1e1e1e"},
            {"__border", "#2e2e2e"},
            {"__text_color", "#e0e0e0"},
            {"__muted", "#aaaaaa"},
            {"__hover_bg", "#2a2a2a"},
            {"__accent", "#0076a8"},
            {"__accent_dark", "#005f8a"},
            {"__accent_light", "#00a8e8"},
            {"__on_accent", "#ffffff"},
            {"__danger", "#e74c3c"},
            {"__success", "#2ecc71"},
            {"__btn_bg", "#333333"},
            {"__btn_text", "#ffffff"},
            {"__btn_hover", "#444444"}
        };
    } else {
        colors = {
            {"__bg_color", "#f0f0f0"},
            {"__sidebar_bg", "#ffffff"},
            {"__card_bg", "#ffffff"},
            {"__border", "#dddddd"},
            {"__text_color", "#333333"},    
            {"__muted", "#888888"},
            {"__hover_bg", "#e8f4f8"},
            {"__accent", "#0076a8"},
            {"__accent_dark", "#005f8a"},
            {"__accent_light", "#00a8e8"},
            {"__on_accent", "#000000"},
            {"__danger", "#e74c3c"},
            {"__success", "#2ecc71"},
            {"__btn_bg", "#dddddd"},
            {"__btn_text", "#333333"},
            {"__btn_hover", "#cccccc"}
        };
    }

    std::vector<std::pair<std::string, std::string>> sorted(colors.begin(), colors.end());
    std::sort(sorted.begin(), sorted.end(), [](auto& a, auto& b) {
        return a.first.size() > b.first.size();
    });

    std::string css_str;
    css_str.reserve(cached_css.size());
    for (size_t i = 0; i < cached_css.size(); ) {
        if (cached_css[i] == '_' && i + 1 < cached_css.size() && cached_css[i+1] == '_') {
            bool matched = false;
            for (auto& [key, val] : sorted) {
                if (cached_css.compare(i, key.size(), key) == 0) {
                    css_str += val;
                    i += key.size();
                    matched = true;
                    break;
                }
            }
            if (!matched) { css_str += cached_css[i]; i++; }
        } else {
            css_str += cached_css[i];
            i++;
        }
    }

    auto provider = Gtk::CssProvider::create();
    provider->load_from_data(css_str);
    cached = provider;

    auto display = Gdk::Display::get_default();
    Gtk::StyleContext::add_provider_for_display(display, provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    reload_python();
}

void MainWindow::check_week_change() {
    time_t now = std::time(nullptr);
    struct tm* tm_now = std::localtime(&now);
    int current_wday = tm_now->tm_wday;
    if (current_wday == 0) current_wday = 7;
    time_t monday = now - (current_wday - 1) * 86400;

    char buf[11];
    struct tm* tm_monday = std::localtime(&monday);
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm_monday);
    std::string this_week(buf);

    if (current_week_start.empty()) {
        current_week_start = this_week;
        save_data();
    } else if (current_week_start != this_week) {
        if (weekly_pomo_sessions > 0 || weekly_pomo_minutes > 0) {
            collect_weekly_data();
        }
        weekly_pomo_sessions = 0;
        weekly_pomo_minutes = 0;
        current_week_start = this_week;
        save_data();
    }
}

void MainWindow::setup_sidebar() {
    sidebar_box.add_css_class("sidebar-box");
    sidebar.set_selection_mode(Gtk::SelectionMode::SINGLE);

    auto* stitle = Gtk::make_managed<Gtk::Label>("Menü");
    stitle->add_css_class("sidebar-title");
    stitle->set_margin_start(14);
    stitle->set_margin_top(12);
    stitle->set_margin_bottom(6);
    stitle->set_halign(Gtk::Align::START);
    sidebar_box.append(*stitle);

    sidebar_box.append(sidebar);

    add_item("Panel", "dashboard");
    add_item("Dersler", "dersler");
    add_item("Rozetlerim", "badges");
    add_item("Flashcards", "flashcards");
    add_item("Pomodoro", "pomodoro");
    add_item("Görevler", "planner");
    add_item("Notlarım", "notes");
    add_item("Ders Programı", "schedule");
    add_item("Dikkat Modu", "focus");
    add_item("Linux", "linux");
    add_item("Yapay Zeka", "ai");;
    add_item("Koç", "ai_koc");
    add_item("Python Öğren", "python");

    sidebar.signal_row_selected().connect(sigc::mem_fun(*this, &MainWindow::on_select));

    paned.set_start_child(sidebar_box);
    paned.set_position(200);
    paned.set_resize_start_child(false);
    paned.set_shrink_start_child(false);
}

void MainWindow::select_first_row() {
    auto* row = sidebar.get_row_at_index(0);
    if (row) sidebar.select_row(*row);
}

void MainWindow::add_item(const std::string& name, const std::string& id) {
    auto* row = Gtk::make_managed<Gtk::ListBoxRow>();
    auto* hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    hbox->set_margin_start(10);
    hbox->set_margin_top(5);
    hbox->set_margin_bottom(5);

    auto* icon = Gtk::make_managed<Gtk::Image>();
    static const std::map<std::string, std::string> icon_map = {
        {"dashboard", "sidebar_dashboard"},
        {"dersler", "sidebar_dersler"},
        {"badges", "sidebar_badges"},
        {"flashcards", "sidebar_flashcards"},
        {"pomodoro", "sidebar_pomodoro"},
        {"planner", "sidebar_planner"},
        {"notes", "sidebar_notes"},
        {"schedule", "sidebar_schedule"},
        {"focus", "sidebar_focus"},
        {"linux", "sidebar_linux"},
        {"ai", "sidebar_ai"},
        {"ai_koc", "sidebar_ai_koc"},
        {"python", "sidebar_python"}
    };
    auto it = icon_map.find(id);
    if (it != icon_map.end()) {
        std::string base = it->second;
        std::string path = "/org/ogrenci/merkezi/assets/" + base + ".svg";
        std::string path_dark = "/org/ogrenci/merkezi/assets/" + base + "_dark.svg";
        icon->set_from_resource(dark_mode ? path.c_str() : path_dark.c_str());
        icon->set_pixel_size(18);
        icon->set_visible(sidebar_icons);
        sidebar_icon_widgets.push_back({icon, base});
    }

    auto* label = Gtk::make_managed<Gtk::Label>(name);
    label->set_halign(Gtk::Align::START);

    hbox->append(*icon);
    hbox->append(*label);
    row->set_child(*hbox);
    row->set_name(id);
    sidebar.append(*row);
}

void MainWindow::setup_pages() {
    setup_dashboard();
    paned.set_end_child(stack);
}

Gtk::Frame* MainWindow::make_card(Gtk::Widget& content) {
    auto* frame = Gtk::make_managed<Gtk::Frame>();
    frame->add_css_class("card");
    content.set_margin_start(12);
    content.set_margin_end(12);
    content.set_margin_top(8);
    content.set_margin_bottom(8);
    frame->set_child(content);
    return frame;
}

void MainWindow::setup_dashboard() {
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);

    auto* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    box->set_halign(Gtk::Align::CENTER);

    // --- Welcome ---
    dash_logo.set_resource("/org/ogrenci/merkezi/assets/pardus.svg");
    dash_logo.set_content_fit(Gtk::ContentFit::SCALE_DOWN);
    dash_logo.set_size_request(30, 30);
    dash_logo.set_margin_bottom(12);
    dash_logo.set_margin_top(60);

    std::string user = get_username();
    dash_welcome.set_markup("Hoş geldin, <b>" + user + "</b>!");
    dash_welcome.add_css_class("dash-welcome");
    dash_welcome.set_halign(Gtk::Align::CENTER);
    dash_welcome.set_valign(Gtk::Align::CENTER);
    dash_welcome.set_margin_bottom(8);

    auto* sub = Gtk::make_managed<Gtk::Label>("Bugün çalışmaya hazır mısın?");
    sub->set_halign(Gtk::Align::CENTER);
    sub->add_css_class("dash-sub");

    box->append(dash_logo);
    box->append(dash_welcome);
    box->append(*sub);

    // --- Stats Cards ---
    auto* stats_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 20);
    stats_box->set_halign(Gtk::Align::CENTER);
    stats_box->set_margin_top(48);
    stats_box->set_margin_bottom(32);

    auto make_stat = [this](const std::string& icon_path, const std::string& val, const std::string& desc, Gtk::Image*& out_icon) -> Gtk::Frame* {
        auto* vb = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 4);
        vb->set_halign(Gtk::Align::CENTER);
        vb->set_margin_start(28);
        vb->set_margin_end(28);
        vb->set_margin_top(16);
        vb->set_margin_bottom(16);

        auto* ic = Gtk::make_managed<Gtk::Image>();
        ic->set_from_resource(icon_path);
        ic->set_pixel_size(34);
        ic->add_css_class("stat-icon");
        vb->append(*ic);
        out_icon = ic;

        auto* vl = Gtk::make_managed<Gtk::Label>(val);
        vl->add_css_class("stat-value");
        vb->append(*vl);

        auto* dl = Gtk::make_managed<Gtk::Label>(desc);
        dl->add_css_class("stat-label");
        vb->append(*dl);

        auto* fr = Gtk::make_managed<Gtk::Frame>();
        fr->add_css_class("stat-card");
        fr->set_child(*vb);
        return fr;
    };

    stats_box->append(*make_stat("/org/ogrenci/merkezi/assets/streak.svg", std::to_string(study_streak), "Günlük Seri", dash_icon_streak));
    stats_box->append(*make_stat("/org/ogrenci/merkezi/assets/pomo.svg", std::to_string(pomodoro_completed), "Pomodoro", dash_icon_pomo));
    stats_box->append(*make_stat("/org/ogrenci/merkezi/assets/timer.svg", std::to_string(pomodoro_minutes) + "dk", "Çalışma Süresi", dash_icon_timer));

    box->append(*stats_box);

    // --- Weekly Analysis Button ---
    auto* btn_weekly = Gtk::make_managed<Gtk::Button>("Haftal\u0131k Analizlerim");
    btn_weekly->set_halign(Gtk::Align::CENTER);
    btn_weekly->set_margin_bottom(16);
    btn_weekly->add_css_class("dash-weekly-btn");
    btn_weekly->signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::navigate_to_weekly_analysis));
    box->append(*btn_weekly);

    // --- Copyright ---
    auto* pardusedu = Gtk::make_managed<Gtk::Label>("©PardusEdu");
    pardusedu->set_halign(Gtk::Align::CENTER);
    pardusedu->add_css_class("pardusedu");
    pardusedu->set_margin_bottom(40);
    box->append(*pardusedu);

    sw->set_child(*box);
    stack.add(*sw, "dashboard");
}

void MainWindow::show_settings_dialog() {
    auto* dialog = new Gtk::Dialog();
    dialog->set_title("Ayarlar");
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->set_default_size(400, 170);

    auto* content = dialog->get_content_area();
    content->set_margin(20);
    content->set_spacing(12);

    auto* header_lbl = Gtk::make_managed<Gtk::Label>("");
    header_lbl->set_markup("<b>G\u00f6r\u00fcn\u00fcm</b>");
    header_lbl->set_halign(Gtk::Align::START);
    content->append(*header_lbl);

    auto* icons_row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    icons_row->set_margin_top(4);
    icons_row->add_css_class("settings-row");

    auto* icons_label = Gtk::make_managed<Gtk::Label>("Sidebar \u0130konlar\u0131");
    icons_label->set_halign(Gtk::Align::START);
    icons_label->set_hexpand(true);

    auto* icons_switch = Gtk::make_managed<Gtk::Switch>();
    icons_switch->set_active(sidebar_icons);
    icons_switch->set_halign(Gtk::Align::END);

    icons_row->append(*icons_label);
    icons_row->append(*icons_switch);
    content->append(*icons_row);

    auto* btn_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    btn_box->set_halign(Gtk::Align::END);
    btn_box->set_margin_top(16);

    auto* btn_cancel = Gtk::make_managed<Gtk::Button>("\u0130ptal");
    auto* btn_save = Gtk::make_managed<Gtk::Button>("Kaydet");
    btn_save->add_css_class("suggested-action");

    btn_box->append(*btn_cancel);
    btn_box->append(*btn_save);
    content->append(*btn_box);

    btn_cancel->signal_clicked().connect([dialog]() {
        dialog->close();
    });

    btn_save->signal_clicked().connect([this, dialog, icons_switch]() {
        sidebar_icons = icons_switch->get_active();
        update_sidebar_icons();
        save_data();
        dialog->close();
    });

    dialog->show();
}

void MainWindow::update_sidebar_icons() {
    for (auto& [img, base] : sidebar_icon_widgets) {
        img->set_visible(sidebar_icons);
    }
}

void MainWindow::on_select(Gtk::ListBoxRow* row) {
    if (!row) return;
    auto name = row->get_name();
    ensure_page_built(name);
    auto* child = stack.get_child_by_name(name);
    if (child) stack.set_visible_child(*child);

    row->add_css_class("sidebar-glow");
    int idx = row->get_index();
    if (sidebar_glow_conn.connected()) sidebar_glow_conn.disconnect();
    sidebar_glow_conn = Glib::signal_timeout().connect([this, idx]() {
        auto* r = sidebar.get_row_at_index(idx);
        if (r) r->remove_css_class("sidebar-glow");
        return false;
    }, 350);
}