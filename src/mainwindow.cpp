#include "mainwindow.h"
#include "resources.h"
#include <glib.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

MainWindow::MainWindow() {
    set_title("Pardus Edu");
    set_default_size(1200, 750);

    detect_theme();
    setup_data();
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

std::string MainWindow::get_data_dir() {
    const char* data = g_get_user_data_dir();
    if (!data) return "/tmp/ogrenci-merkezi";
    return std::string(data) + "/ogrenci-merkezi";
}

void MainWindow::setup_data() {
    auto dir = get_data_dir();
    fs::create_directories(dir);

    auto tf = dir + "/tasks.dat";
    if (fs::exists(tf)) {
        std::ifstream f(tf);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p1 = line.find('|');
            auto p2 = line.find('|', p1 + 1);
            auto p3 = line.find('|', p2 + 1);
            auto p4 = line.find('|', p3 + 1);
            if (p1 != std::string::npos && p2 != std::string::npos && p3 != std::string::npos && p4 != std::string::npos) {
                tasks.push_back({line.substr(0, p1), line.substr(p1+1, p2-p1-1), line.substr(p2+1, p3-p2-1), line[p3+1]=='1', std::stoi(line.substr(p4+1))});
            }
        }
    }

    auto pf = dir + "/pomodoro.dat";
    if (fs::exists(pf)) {
        std::ifstream f(pf);
        std::string line;
        if (std::getline(f, line)) {
            auto p = line.find('|');
            if (p != std::string::npos) {
                pomodoro_completed = std::stoi(line.substr(0, p));
                pomodoro_minutes = std::stoi(line.substr(p+1));
            }
        }
    }

    auto sf = dir + "/streak.dat";
    if (fs::exists(sf)) {
        std::ifstream f(sf);
        std::string line;
        if (std::getline(f, line)) study_streak = std::stoi(line);
    }

    auto nf = dir + "/notes.dat";
    if (fs::exists(nf)) {
        std::ifstream f(nf);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p = line.find('|');
            if (p != std::string::npos) {
                course_notes.push_back({line.substr(0, p), ""});
            }
        }
        for (size_t i = 0; i < course_notes.size(); i++) {
            auto cf = dir + "/note_" + std::to_string(i) + ".txt";
            if (fs::exists(cf)) {
                std::ifstream nf2(cf);
                std::string content((std::istreambuf_iterator<char>(nf2)), std::istreambuf_iterator<char>());
                course_notes[i].content = content;
            }
        }
    }

    auto gf = dir + "/grades.dat";
    if (fs::exists(gf)) {
        std::ifstream f(gf);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p1 = line.find('|');
            auto p2 = line.find('|', p1 + 1);
            auto p3 = line.find('|', p2 + 1);
            if (p1 != std::string::npos && p2 != std::string::npos && p3 != std::string::npos) {
                GradeEntry g;
                g.course = line.substr(0, p1);
                g.midterm = std::stod(line.substr(p1+1, p2-p1-1));
                g.final = std::stod(line.substr(p2+1, p3-p2-1));
                g.average = g.midterm * 0.4 + g.final * 0.6;
                g.letter = calc_letter(g.average);
                grades.push_back(g);
            }
        }
    }

    auto schf = dir + "/schedule.dat";
    if (fs::exists(schf)) {
        std::ifstream f(schf);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p1 = line.find('|');
            auto p2 = line.find('|', p1 + 1);
            auto p3 = line.find('|', p2 + 1);
            auto p4 = line.find('|', p3 + 1);
            if (p1 != std::string::npos && p2 != std::string::npos && p3 != std::string::npos && p4 != std::string::npos) {
                schedule.push_back({line.substr(0, p1), line.substr(p1+1, p2-p1-1), line.substr(p2+1, p3-p2-1), line.substr(p4+1)});
            }
        }
    }

    auto stf = dir + "/schedule_times.dat";
    if (fs::exists(stf)) {
        std::ifstream f(stf);
        std::string line;
        schedule_times.clear();
        while (std::getline(f, line)) {
            if (!line.empty()) schedule_times.push_back(line);
        }
        if (schedule_times.empty()) schedule_times = {"08:30", "09:30", "10:30", "11:30", "13:00", "14:00", "15:00", "16:00"};
    }
}

void MainWindow::save_data() {
    auto dir = get_data_dir();
    fs::create_directories(dir);

    { std::ofstream f(dir + "/tasks.dat"); for (auto& t : tasks) f << t.title << "|" << t.category << "|" << t.due_date << "|" << (t.done?"1":"0") << "|" << t.priority << "\n"; }
    { std::ofstream f(dir + "/pomodoro.dat"); f << pomodoro_completed << "|" << pomodoro_minutes << "\n"; }
    { std::ofstream f(dir + "/streak.dat"); f << study_streak << "\n"; }
    { std::ofstream f(dir + "/notes.dat"); for (auto& n : course_notes) f << n.course << "|\n"; }
    for (size_t i = 0; i < course_notes.size(); i++) {
        std::ofstream f(dir + "/note_" + std::to_string(i) + ".txt");
        f << course_notes[i].content;
    }
    { std::ofstream f(dir + "/grades.dat"); for (auto& g : grades) f << g.course << "|" << g.midterm << "|" << g.final << "|\n"; }
    { std::ofstream f(dir + "/schedule.dat"); for (auto& s : schedule) f << s.day << "|" << s.time << "|" << s.course << "|" << s.location << "\n"; }
    { std::ofstream f(dir + "/schedule_times.dat"); for (auto& t : schedule_times) f << t << "\n"; }
}

char MainWindow::calc_letter(double avg) {
    if (avg >= 90) return 'A';
    if (avg >= 80) return 'B';
    if (avg >= 70) return 'C';
    if (avg >= 60) return 'D';
    return 'F';
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

    auto* title = Gtk::make_managed<Gtk::Label>("Pardus Öğrenci Merkezi");
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
    auto* save_icon = Gtk::make_managed<Gtk::Image>();
    save_icon->set_from_resource("/org/ogrenci/merkezi/assets/save.svg");
    save_icon->set_pixel_size(20);
    save_btn->set_child(*save_icon);
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

    btn_ai_key.unset_child();
    auto* key_icon = Gtk::make_managed<Gtk::Image>();
    key_icon->set_from_resource(dark_mode
        ? "/org/ogrenci/merkezi/assets/settings.svg"
        : "/org/ogrenci/merkezi/assets/settings_dark.svg");
    key_icon->set_pixel_size(20);
    key_icon->set_visible(true);
    btn_ai_key.set_child(*key_icon);
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
            {"__blue", "#38BDF8"},
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
    // Tema değişince Python ders sayfasını da güncelliyoruz ki karanlık/aydınlık moda uygun olsun
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

void MainWindow::on_select(Gtk::ListBoxRow* row) {
    if (!row) return;
    auto name = row->get_name();
    auto* child = stack.get_child_by_name(name);
    if (child) stack.set_visible_child(*child);
}
