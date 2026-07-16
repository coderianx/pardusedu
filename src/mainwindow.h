#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtkmm.h>
#include <giomm.h>
#include <glib.h>
#include <string>
#include <webkit/webkit.h>
#include <vector>
#include <map>
#include <unordered_set>
#include <mutex>
#include <thread>
#include <nlohmann/json.hpp>
#include "ai_client.h"
#include "parduslab.h"

// Koç data escaping for | pipe-separated files
inline std::string koc_escape(const std::string& s) {
    std::string r;
    r.reserve(s.size());
    for (char c : s) {
        if (c == '|') r += "\\p";
        else if (c == '\\') r += "\\\\";
        else r += c;
    }
    return r;
}
inline std::string koc_unescape(const std::string& s) {
    std::string r;
    r.reserve(s.size());
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            if (s[i+1] == 'p') r += '|';
            else if (s[i+1] == '\\') r += '\\';
            else { r += s[i]; r += s[i+1]; }
            i++;
        } else {
            r += s[i];
        }
    }
    return r;
}

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

struct FlashDeck {
    std::string id;
    std::string title;
    std::string description;
    std::string source_note;
    std::string source_course;
    int card_count = 0;
    int new_count = 0;
    int due_count = 0;
    int review_count = 0;
    long created_at = 0;
    long last_reviewed_at = 0;
};

struct FlashCard {
    std::string id;
    std::string deck_id;
    std::string front;
    std::string back;
    std::string hint;
    std::string tags;
    int interval = 0;
    int repetitions = 0;
    double ease_factor = 2.5;
    int lapses = 0;
    long next_review_at = 0;
    long last_reviewed_at = 0;
};

struct DailyStats {
    std::string date;
    int pomo_sessions = 0;
    int focus_minutes = 0;
    int cards_reviewed = 0;
    int cards_correct = 0;
    int cards_wrong = 0;
    int tasks_done = 0;
    int tasks_total = 0;
};

struct GunlukSoru {
    std::string tarih;
    std::string ders;
    std::string konu;
    int dogru = 0;
    int yanlis = 0;
    int bos = 0;
};

struct KocHedef {
    std::string hedef;
    std::string olusturma_tarihi;
    std::string opsiyonel_alan;
};

struct WeeklyReport {
    std::string week_start;
    std::string week_end;
    int total_pomo_sessions = 0;
    int total_focus_minutes = 0;
    int cards_reviewed = 0;
    int cards_correct = 0;
    int cards_wrong = 0;
    int tasks_completed = 0;
    int tasks_total = 0;
    int streak_count = 0;
    int notes_created = 0;
    int study_score = 0;
    std::string best_deck_id;
    std::string worst_deck_id;
    std::vector<DailyStats> daily_stats;
    std::vector<std::string> wrong_card_ids;
    std::vector<std::string> leech_card_ids;
    std::string weak_deck_id;
    long created_at = 0;
    std::string ai_summary;
    std::string ai_advice;
    std::string ai_motivation;
};

class MainWindow : public Gtk::ApplicationWindow {
public:
    MainWindow();
    ~MainWindow();

private:
    Gtk::Box root_box{Gtk::Orientation::VERTICAL};
    Gtk::Paned paned{Gtk::Orientation::HORIZONTAL};
    Gtk::Box sidebar_box{Gtk::Orientation::VERTICAL, 0};
    Gtk::Stack stack;
    Gtk::ListBox sidebar;

    bool dark_mode = true;
    bool sidebar_icons = false;
    Gtk::Button btn_theme;
    Gtk::Button btn_settings;
    Gtk::Image* save_icon_ptr = nullptr;
    Gtk::Image* settings_icon_ptr = nullptr;
    std::vector<std::pair<Gtk::Image*, std::string>> sidebar_icon_widgets;

    std::string cached_css;
    Glib::RefPtr<Gtk::CssProvider> css_provider_dark;
    Glib::RefPtr<Gtk::CssProvider> css_provider_light;

    std::vector<Task> tasks;
    std::vector<CourseNote> course_notes;
    std::vector<GradeEntry> grades;
    std::vector<ScheduleEntry> schedule;
    std::vector<FlashDeck> flash_decks;
    std::vector<FlashCard> flash_cards;
    int pomodoro_completed = 0;
    int pomodoro_minutes = 0;
    int weekly_pomo_sessions = 0;
    int weekly_pomo_minutes = 0;
    std::string current_week_start;
    int study_streak = 0;
    std::string last_streak_date;
    void check_week_change();

    Gtk::Label dash_welcome{"", Gtk::Align::CENTER};
    Gtk::Picture dash_logo;
    Gtk::Image header_logo;
    Gtk::Image* dash_icon_streak = nullptr;
    Gtk::Image* dash_icon_pomo = nullptr;
    Gtk::Image* dash_icon_timer = nullptr;

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
    Gtk::Button btn_add_link_note{"+"};
    Gtk::Button btn_share_note;
    Gtk::Image* share_icon_ptr = nullptr;
    Gtk::Box note_editor_box{Gtk::Orientation::VERTICAL, 0};
    Gtk::Label note_title{"", Gtk::Align::START};
    Gtk::TextView note_view;
    Gtk::Button btn_save_note{"Kaydet"};
    Gtk::Button btn_delete_course{"Dersi Sil"};
    Gtk::Label* note_stats_label = nullptr;
    int selected_note_index = -1;

    Glib::RefPtr<Gtk::TextTag> tag_bold;
    Glib::RefPtr<Gtk::TextTag> tag_italic;
    Glib::RefPtr<Gtk::TextTag> tag_underline;
    Glib::RefPtr<Gtk::TextTag> tag_strikethrough;
    Glib::RefPtr<Gtk::TextTag> tag_h1;
    Glib::RefPtr<Gtk::TextTag> tag_h2;
    Glib::RefPtr<Gtk::TextTag> tag_h3;
    Glib::RefPtr<Gtk::TextTag> tag_font_inc;
    Glib::RefPtr<Gtk::TextTag> tag_font_dec;
    Gtk::ToggleButton btn_bold;
    Gtk::ToggleButton btn_italic;
    Gtk::ToggleButton btn_underline;
    Gtk::ToggleButton btn_strikethrough;
    Gtk::ToggleButton btn_h1;
    Gtk::ToggleButton btn_h2;
    Gtk::ToggleButton btn_h3;
    Gtk::Button btn_math;
    Gtk::Button btn_font_inc_w;
    Gtk::Button btn_font_dec_w;
    Gtk::Box format_toolbar{Gtk::Orientation::HORIZONTAL, 4};
    std::vector<std::string> note_formats;
    bool note_loading = false;

    Gtk::Label focus_status{"Hazır", Gtk::Align::CENTER};
    Gtk::Label focus_timer{"00:00", Gtk::Align::CENTER};
    Gtk::Button btn_focus_toggle{"Dikkat Modunu Başlat"};
    Gtk::Label focus_blocked{"", Gtk::Align::CENTER};
    bool focus_active = false;
    int focus_seconds = 0;
    sigc::connection focus_timer_conn;

    // --- PardusLab ---
    PardusLab* parduslab = nullptr;
    std::string current_lab_container_id;
    Gtk::Window* terminal_window = nullptr;
    GtkWidget* vte_widget = nullptr;
    Gtk::Label* terminal_hint = nullptr;
    Gtk::Label* lab_warning = nullptr;
    std::unordered_set<std::string> pages_built{"dashboard"};
    void ensure_page_built(const std::string& name);

    std::unordered_set<std::string> completed_challenges;

    std::string lab_cwd = "~";
    std::string lab_hostname = "parduslab";
    std::string lab_username = "root";
    std::vector<std::string> lab_users{"root"};
    Gtk::Widget* lab_insert_after = nullptr;
    bool lab_ready = false;
    bool lab_packages_installed = false;

    Gtk::ScrolledWindow linux_scrolled;
    Gtk::Box linux_commands{Gtk::Orientation::VERTICAL, 6};

    Gtk::Box schedule_box{Gtk::Orientation::VERTICAL, 12};
    Gtk::Grid schedule_grid;
    Gtk::Button btn_add_schedule{"Ders Ekle"};
    Gtk::Button btn_clear_schedule{"Programı Temizle"};
    std::vector<std::string> schedule_times = {"08:30", "09:30", "10:30", "11:30", "13:00", "14:00", "15:00", "16:00"};

    int dersler_selected_grade = 0;
    std::string dersler_selected_course;
    nlohmann::json dersler_data;
    int dersler_current_level = 0;
    WebKitWebView* dersler_webview = nullptr;
    Gtk::Box dersler_page_box{Gtk::Orientation::VERTICAL, 0};
    Gtk::Box dersler_header_box{Gtk::Orientation::HORIZONTAL, 8};
    Gtk::Button dersler_back_btn{"← Geri"};
    Gtk::Label dersler_header_title{"Dersler", Gtk::Align::START};
    Gtk::Stack dersler_stack;
    Gtk::ScrolledWindow dersler_content_sw;
    std::vector<sigc::connection> dersler_connections;

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
    void update_streak();


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
    void on_add_link_note();
    void on_share_note();
    void on_save_note();
    void on_delete_course();
    void apply_tag_to_selection(Gtk::ToggleButton& btn, const Glib::RefPtr<Gtk::TextTag>& tag);
    void update_format_buttons();
    void save_note_format(size_t index);
    void load_note_format(size_t index);
    Glib::RefPtr<Gtk::TextTag> make_font_tag(const std::string& name, double scale);

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

    // --- PardusLab ---
    void setup_parduslab();
    void on_lab_try_command(const std::string& command);
    void on_lab_open_terminal(const std::string& command_hint);
    void on_lab_close_terminal();
    void on_lab_reset_container();
    void on_lab_snapshot();
    void spawn_vte_shell(const std::string& container_id, const std::string& username);
    void spawn_terminal_session(const std::string& username, Gtk::Box* vbox, Gtk::Widget* after);
    void show_lab_error(const std::string& title, const std::string& msg);

    void setup_linux();
    void filter_linux(const std::string& q);

    void setup_ai_chat();
    void process_ai_notes(std::string& ai_text);
    Gtk::Box ai_chat_box{Gtk::Orientation::VERTICAL, 8};
    Gtk::ScrolledWindow ai_scroll;
    Gtk::Entry ai_input;
    Gtk::Button btn_ai_send{};
    Gtk::ToggleButton btn_ai_ddg{};
    Gtk::Button btn_ai_image{};
    std::string ai_image_path;
    std::string ai_image_base64;
    std::string ai_image_mime;
    Gtk::Box ai_image_preview_box{Gtk::Orientation::HORIZONTAL, 4};
    Gtk::Image ai_image_thumbnail;
    void ai_image_clear();
    Gtk::Button btn_ai_key{};
    Gtk::Label* ai_model_badge = nullptr;
    AIProvider ai_provider = AIProvider::GROQ;
    std::string ai_api_key_groq;
    std::string ai_api_key_openrouter;
    std::string ai_api_key_gemini;
    std::string ai_model_groq = "llama-3.3-70b-versatile";
    std::string ai_model_openrouter = "openai/gpt-4o-mini";
    std::string ai_model_gemini = "gemini-3.5-flash";
    std::string ai_model_ollama = "llama3.2";
    std::string ai_ollama_url = "http://localhost:11434";
    void show_ai_key_dialog();

    Glib::Dispatcher ai_dispatcher;
    std::string pending_ai_response;
    Gtk::Label* pending_ai_label = nullptr;
    std::mutex ai_mutex;
    bool ai_waiting = false;
    sigc::connection ai_loading_conn;
    int ai_dot_count = 0;
    void on_ai_response();

    void on_select(Gtk::ListBoxRow* row);
    void show_settings_dialog();
    void update_sidebar_icons();
    sigc::connection sidebar_glow_conn;

    Gtk::Box html_box{Gtk::Orientation::VERTICAL, 0};
    Gtk::ScrolledWindow html_scrolled;  
    void setup_python();
    void reload_python();
    void setup_badges();
    void setup_flashcards();
    void flash_show_decks();
    void flash_show_deck_detail(const std::string& deck_id);
    void flash_show_review(const std::string& deck_id);
    void flash_show_card_edit(const std::string& deck_id, const std::string& card_id = "");
    void flash_show_ai_generate(const std::string& deck_id);
    void process_ai_flashcards(const std::string& ai_text, const std::string& deck_id);
    void flash_build_review_queue(const std::string& deck_id);
    void flash_rate_card(int grade);
    void flash_refresh_deck_stats(const std::string& deck_id);
    void flash_refresh_all_deck_stats();
    void flash_show_card_at_index(Gtk::ProgressBar* progress_bar);
    void flash_show_next_card(Gtk::ProgressBar* progress_bar);
    void flash_show_review_done();
    std::string flash_generate_id();
    void flash_delete_deck(const std::string& deck_id);
    void flash_delete_card(const std::string& card_id);
    void flash_export_anki(const std::string& deck_id);
    void flash_load_anki_exports();

    // --- Weekly Analysis ---
    void setup_weekly_analysis();
    void collect_weekly_data();
    void render_weekly_analysis_ui();
    void navigate_to_weekly_analysis();
    void rate_weak_card_from_analysis(int grade);
    std::string build_weekly_ai_prompt(const WeeklyReport& data);
    void on_weekly_ai_response();
    WeeklyReport& get_current_weekly_report();
    std::vector<int> find_wrong_cards_this_week();
    std::vector<int> find_leech_cards();
    std::string create_weak_deck(const std::vector<int>& card_indices,
                                 const std::string& week_label);
    void cleanup_old_weak_decks();

    Glib::Dispatcher weekly_ai_dispatcher;
    std::string pending_weekly_ai_response;
    std::mutex weekly_ai_mutex;
    bool weekly_ai_waiting = false;

    std::vector<GunlukSoru> gunluk_sorular;
    KocHedef koc_hedef;
    std::string koc_son_rapor;
    bool koc_rapor_bekliyor = false;

    Glib::Dispatcher koc_ai_dispatcher;
    std::string pending_koc_ai_response;
    std::mutex koc_ai_mutex;
    Gtk::Label* koc_rapor_lbl = nullptr;
    Gtk::Button* koc_plan_btn = nullptr;

    Glib::Dispatcher koc_plan_dispatcher;
    std::string pending_koc_plan_response;
    std::mutex koc_plan_mutex;
    bool koc_plan_bekliyor = false;
    std::string koc_plan_json;
    Gtk::Box* koc_plan_container = nullptr;
    Gtk::Box* koc_gorev_container = nullptr;
    bool koc_dispatchers_connected = false;

    Glib::Dispatcher koc_materyal_dispatcher;
    std::string pending_koc_materyal_response;
    std::mutex koc_materyal_mutex;
    bool koc_materyal_bekliyor = false;
    Gtk::Label* koc_materyal_lbl = nullptr;

    void setup_dersler();
    void dersler_show_grades();
    void dersler_show_courses(int grade);
    void dersler_show_units(int grade, const std::string& course);
    void dersler_show_content(const std::string& md_file);
    Gtk::Stack flash_stack;
    Gtk::Button flash_back_btn;
    Gtk::Label flash_header_title;
    Gtk::Label flash_card_front;
    Gtk::Label flash_card_back;
    Gtk::Label flash_card_hint;
    Gtk::Label flash_card_counter;
    Gtk::Frame flash_card_container;
    Gtk::Box flash_rate_box;
    Gtk::Button flash_reveal_btn;
    sigc::connection flash_reveal_conn;
    Gtk::Label flash_review_progress;
    int flash_current_index = -1;
    std::string flash_current_deck_id;
    std::vector<int> flash_review_queue;

    // --- Weekly Analysis UI ---
    std::vector<WeeklyReport> weekly_reports;
    Gtk::Box weekly_box{Gtk::Orientation::VERTICAL, 0};
    Gtk::ScrolledWindow weekly_scroll;
    Gtk::Button btn_weekly_back{"\u2190"};
    Gtk::Label weekly_header_label{"", Gtk::Align::CENTER};
    Gtk::Box weekly_stats_box{Gtk::Orientation::HORIZONTAL, 16};
    Gtk::Label weekly_ai_summary{"", Gtk::Align::START};
    Gtk::Label weekly_ai_advice{"", Gtk::Align::START};
    Gtk::Label weekly_ai_motivation{"", Gtk::Align::CENTER};
    Gtk::Box weekly_weak_deck_box{Gtk::Orientation::VERTICAL, 6};
    Gtk::Button btn_weekly_study_weak{"\u25b6 Bu Kartlar\u0131 \u00c7al\u0131\u015f"};
    Gtk::Label weekly_weak_deck_info{"", Gtk::Align::START};
    Gtk::Widget* weekly_chart_widget = nullptr;

    void setup_ai_koc();
    void koc_hedef_ekle();
    void koc_soru_ekle(const std::string& ders, const std::string& konu, int d, int y, int b);
    void koc_haftalik_rapor();
    void koc_soru_kaydet();
    void koc_hedef_kaydet();
    std::string koc_soru_logu_olustur();
    std::string koc_rapor_prompt();
    void on_koc_ai_response();
    void koc_plan_olustur();
    std::string koc_plan_prompt();
    void on_koc_plan_response();
    void koc_gorevleriguncelle();
    void koc_materyal_oner();
    std::string koc_materyal_prompt();
    void on_koc_materyal_response();
    Gtk::Widget* koc_plan_tablosu();
    void koc_bugunku_listeyi_guncelle(Gtk::Box* liste);
    void koc_hata_analizini_doldur(Gtk::Box* liste);
    void koc_hata_listesini_guncelle(Gtk::Box* liste);

    WebKitWebView* python_webview = nullptr;

};

#endif
