#include "gtkmm/enums.h"
#include "gtkmm/label.h"
#include "gtkmm/object.h"
#include "mainwindow.h"
#include "resources.h"
#include <sstream>
#include <iomanip>
#include <map>

MainWindow::MainWindow() {
    set_title("Pardus Edu");
    set_default_size(1200, 750);

    detect_theme();
    setup_data();
    update_streak();
    setup_ui();
    apply_theme();
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

    header->append(header_logo);
    header->append(*title);
    header->append(btn_theme);
    header->append(*save_btn);

    root_box.append(*header);
    root_box.append(paned);

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

    if (share_icon_ptr) {
        share_icon_ptr->set_from_resource(dark_mode
            ? "/org/ogrenci/merkezi/assets/share.svg"
            : "/org/ogrenci/merkezi/assets/share_dark.svg");
    }

    auto reload = [](Gtk::Image* img, const char* path) { if (img) img->set_from_resource(path); };
    if (dash_icon_streak) reload(dash_icon_streak, "/org/ogrenci/merkezi/assets/streak.svg");
    if (dash_icon_pomo) reload(dash_icon_pomo, "/org/ogrenci/merkezi/assets/pomo.svg");
    if (dash_icon_timer) reload(dash_icon_timer, "/org/ogrenci/merkezi/assets/timer.svg");
}

void MainWindow::apply_theme() {
    auto settings = Gtk::Settings::get_default();
    if (settings) settings->property_gtk_application_prefer_dark_theme() = dark_mode;

    g_resources_register(ogrenci_get_resource());

    std::string css_str;
    auto file = Gio::File::create_for_uri("resource:///org/ogrenci/merkezi/assets/style.css");
    try {
        auto stream = file->read();
        char buffer[4096];
        while (true) {
            gsize bytes_read = stream->read(buffer, sizeof(buffer));
            if (bytes_read == 0) break;
            css_str.append(buffer, bytes_read);
        }
    } catch (const Glib::Error& err) {
        g_warning("CSS yüklenemedi: %s", err.what());
        return;
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
            {"__danger", "#e74c3c"},
            {"__success", "#2ecc71"},
            {"__btn_bg", "#dddddd"},
            {"__btn_text", "#333333"},
            {"__btn_hover", "#cccccc"}
        };
    }

    for (auto& [placeholder, value] : colors) {
        size_t pos = 0;
        while ((pos = css_str.find(placeholder, pos)) != std::string::npos) {
            css_str.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }

    auto provider = Gtk::CssProvider::create();
    provider->load_from_data(css_str);

    auto display = Gdk::Display::get_default();
    Gtk::StyleContext::add_provider_for_display(display, provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    reload_python();
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
    add_item("Pomodoro", "pomodoro");
    add_item("Görevler", "planner");
    add_item("Notlarım", "notes");
    add_item("Ders Programı", "schedule");
    add_item("Dikkat Modu", "focus");
    add_item("Linux", "linux");
    add_item("Yapay Zeka", "ai");;
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
    auto* label = Gtk::make_managed<Gtk::Label>(name);
    label->set_halign(Gtk::Align::START);
    label->set_margin_start(14);
    label->set_margin_top(5);
    label->set_margin_bottom(5);
    row->set_child(*label);
    row->set_name(id);
    sidebar.append(*row);
}

void MainWindow::setup_pages() {
    setup_dashboard();
    setup_pomodoro();
    setup_planner();
    setup_notes();
    setup_schedule();
    setup_focus();
    setup_linux();
    setup_ai_chat();
    setup_python();
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

    // --- Copyright ---
    auto* pardusedu = Gtk::make_managed<Gtk::Label>("©PardusEdu");
    pardusedu->set_halign(Gtk::Align::CENTER);
    pardusedu->add_css_class("pardusedu");
    pardusedu->set_margin_bottom(40);
    box->append(*pardusedu);

    sw->set_child(*box);
    stack.add(*sw, "dashboard");
}

void MainWindow::on_select(Gtk::ListBoxRow* row) {
    if (!row) return;
    auto name = row->get_name();
    auto* child = stack.get_child_by_name(name);
    if (child) stack.set_visible_child(*child);
}
