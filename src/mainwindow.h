#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm.h>
#include <giomm.h>
#include <glib.h>
#include <string>
#include <webkit/webkit.h>
#include <vector>
#include <map>

struct Task {
    std::string title;
    std::string category;
    std::string due_date;
    bool done;
    int priority;
};

struct CourseNote {
    std::string course;
    std::string content;
};

struct GradeEntry {
    std::string course;
    double midterm;
    double final;
    double average;
    char letter;
};

struct ScheduleEntry {
    std::string day;
    std::string time;
    std::string course;
    std::string location;
};

class MainWindow : public Gtk::ApplicationWindow {
public:
    MainWindow();

private:
    Gtk::Box root_box{Gtk::Orientation::VERTICAL};
    Gtk::Paned paned{Gtk::Orientation::HORIZONTAL};
    Gtk::Box sidebar_box{Gtk::Orientation::VERTICAL, 0};
    Gtk::Stack stack;
    Gtk::ListBox sidebar;

    bool dark_mode = true;
    Gtk::Button btn_theme;

    std::vector<Task> tasks;
    std::vector<CourseNote> course_notes;
    std::vector<GradeEntry> grades;
    std::vector<ScheduleEntry> schedule;
    int pomodoro_completed = 0;
    int pomodoro_minutes = 0;
    int study_streak = 0;

    Gtk::Label dash_welcome{"", Gtk::Align::CENTER};
    Gtk::Picture dash_logo;
    Gtk::Picture header_logo;

    Gtk::Label pomo_time{"25:00", Gtk::Align::CENTER};
    Gtk::Button btn_pomo_start{"Başlat"};
    Gtk::Button btn_pomo_reset{"Sıfırla"};
    Gtk::Button btn_pomo_skip{"Atla"};
    Gtk::Label pomo_status{"Hazır", Gtk::Align::CENTER};
    Gtk::ProgressBar pomo_progress;
    Gtk::SpinButton spin_pomo_duration;
    Gtk::SpinButton spin_pomo_break;
    Gtk::Label pomo_settings_label{"Süre Ayarla", Gtk::Align::CENTER};
    bool pomo_running = false;
    bool pomo_break = false;
    int pomo_seconds = 25 * 60;
    int pomo_total = 25 * 60;
    int pomo_duration_minutes = 25;
    int pomo_break_minutes = 5;
    sigc::connection pomo_timer_conn;

    Gtk::Entry task_title_entry;
    Gtk::Entry task_due_entry;
    Gtk::ComboBoxText task_category;
    Gtk::ComboBoxText task_priority;
    Gtk::Button btn_add_task{"Görev Ekle"};
    Gtk::Box task_list_box{Gtk::Orientation::VERTICAL, 4};
    Gtk::ScrolledWindow task_scrolled;
    Gtk::Label task_stats{"", Gtk::Align::START};

    Gtk::ListBox course_list;
    Gtk::Button btn_add_course{"Ders Ekle"};
    Gtk::Box note_editor_box{Gtk::Orientation::VERTICAL, 0};
    Gtk::Label note_title{"", Gtk::Align::START};
    Gtk::TextView note_view;
    Gtk::Button btn_save_note{"Kaydet"};
    Gtk::Button btn_delete_course{"Dersi Sil"};
    int selected_note_index = -1;

    Gtk::Label focus_status{"Hazır", Gtk::Align::CENTER};
    Gtk::Label focus_timer{"00:00", Gtk::Align::CENTER};
    Gtk::Button btn_focus_toggle{"Dikkat Modunu Başlat"};
    Gtk::Label focus_blocked{"", Gtk::Align::CENTER};
    bool focus_active = false;
    int focus_seconds = 0;
    sigc::connection focus_timer_conn;

    Gtk::ScrolledWindow linux_scrolled;
    Gtk::Box linux_commands{Gtk::Orientation::VERTICAL, 6};

    Gtk::Box schedule_box{Gtk::Orientation::VERTICAL, 12};
    Gtk::Grid schedule_grid;
    Gtk::Button btn_add_schedule{"Ders Ekle"};
    Gtk::Button btn_clear_schedule{"Programı Temizle"};
    std::vector<std::string> schedule_times = {"08:30", "09:30", "10:30", "11:30", "13:00", "14:00", "15:00", "16:00"};

    std::vector<std::string> blocked_sites = {
        "instagram.com", "www.instagram.com",
        "twitter.com", "www.twitter.com", "x.com", "www.x.com",
        "facebook.com", "www.facebook.com",
        "tiktok.com", "www.tiktok.com",
        "reddit.com", "www.reddit.com",
        "netflix.com", "www.netflix.com",
        "discord.com", "discord.gg",
        "twitch.tv", "www.twitch.tv"
    };

    void detect_theme();
    std::string get_username();
    std::string get_data_dir();
    void setup_data();
    void save_data();


    void setup_ui();
    void toggle_theme();
    void apply_theme();
    void setup_sidebar();
    void select_first_row();
    void add_item(const std::string& name, const std::string& id);
    void setup_pages();
    Gtk::Frame* make_card(Gtk::Widget& content);

    void setup_dashboard();
    void setup_pomodoro();
    void update_pomo_display();
    void on_pomo_start();
    void on_pomo_reset();
    void on_pomo_skip();
    void on_pomo_duration_changed();
    void on_pomo_break_changed();

    void setup_planner();
    void on_add_task();
    void refresh_task_list();
    Gtk::Box* make_task_row(size_t index);

    void setup_notes();
    void refresh_course_list();
    void on_course_select(Gtk::ListBoxRow* row);
    void on_add_course();
    void on_save_note();
    void on_delete_course();

    void setup_schedule();
    void build_schedule_grid();
    void on_time_clicked(size_t row_idx);
    void on_schedule_cell_clicked(const std::string& day, const std::string& time, const std::string& current_course, const std::string& current_location);
    void on_add_schedule();
    void on_clear_schedule();

    void setup_focus();
    void on_focus_toggle();
    void block_sites();
    void unblock_sites();

    void setup_linux();
    void filter_linux(const std::string& q);

    void setup_ai_chat();
    void process_ai_notes(std::string& ai_text);
    Gtk::Box ai_chat_box{Gtk::Orientation::VERTICAL, 8};
    Gtk::ScrolledWindow ai_scroll;
    Gtk::Entry ai_input;
    Gtk::Button btn_ai_send{};
    Gtk::Button btn_ai_key{};
    std::string ai_api_key;
    std::string ai_model = "llama-3.3-70b-versatile";
    void show_ai_key_dialog();

    void on_select(Gtk::ListBoxRow* row);

    Gtk::Box html_box{Gtk::Orientation::VERTICAL, 0};
    Gtk::ScrolledWindow html_scrolled;  
    void setup_python();
    void reload_python();
    WebKitWebView* python_webview = nullptr;

};

#endif
