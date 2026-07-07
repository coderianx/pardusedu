#include "mainwindow.h"
#include "ai_client.h"
#include <nlohmann/json.hpp>
#include <webkit/webkit.h>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <regex>
#include <thread>
#include <mutex>

using json = nlohmann::json;

// ============================================================
// Weekly Analysis Functions
// ============================================================

static Gtk::Frame* make_section_card(const std::string& header_markup, Gtk::Widget& content) {
    auto* card = Gtk::make_managed<Gtk::Frame>();
    card->add_css_class("card");
    auto* vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    vbox->set_margin_start(14);
    vbox->set_margin_end(14);
    vbox->set_margin_top(10);
    vbox->set_margin_bottom(10);
    auto* hdr = Gtk::make_managed<Gtk::Label>();
    hdr->set_markup(header_markup);
    hdr->set_halign(Gtk::Align::START);
    hdr->add_css_class("weekly-section-header");
    vbox->append(*hdr);
    vbox->append(content);
    card->set_child(*vbox);
    return card;
}

void MainWindow::setup_weekly_analysis() {
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

    weekly_box.set_orientation(Gtk::Orientation::VERTICAL);
    weekly_box.set_margin_start(20);
    weekly_box.set_margin_end(20);
    weekly_box.set_margin_top(16);
    weekly_box.set_margin_bottom(16);
    weekly_box.set_spacing(12);

    auto* header_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    btn_weekly_back.add_css_class("flash-back-btn");
    btn_weekly_back.signal_clicked().connect([this]() {
        auto* child = stack.get_child_by_name("dashboard");
        if (child) stack.set_visible_child(*child);
    });
    weekly_header_label.add_css_class("flash-page-title");

    header_box->append(btn_weekly_back);
    header_box->append(weekly_header_label);
    header_box->set_margin_bottom(16);
    weekly_box.append(*header_box);

    weekly_stats_box.set_halign(Gtk::Align::CENTER);
    weekly_stats_box.set_margin_bottom(20);
    weekly_box.append(weekly_stats_box);

    // Daily study time chart


    weekly_ai_summary.set_wrap(true);
    weekly_ai_summary.set_max_width_chars(60);
    weekly_ai_summary.add_css_class("weekly-ai-text");
    weekly_ai_summary.set_halign(Gtk::Align::START);

    weekly_ai_advice.set_wrap(true);
    weekly_ai_advice.set_max_width_chars(60);
    weekly_ai_advice.add_css_class("weekly-ai-text");
    weekly_ai_advice.set_halign(Gtk::Align::START);

    weekly_ai_motivation.set_wrap(true);
    weekly_ai_motivation.set_max_width_chars(50);
    weekly_ai_motivation.add_css_class("weekly-motivation");
    weekly_ai_motivation.set_halign(Gtk::Align::CENTER);

    auto* summary_card = make_section_card("<b>Haftan\u0131n \u00d6zeti</b>", weekly_ai_summary);
    weekly_box.append(*summary_card);

    auto* advice_card = make_section_card("<b>AI De\u011ferlendirmesi</b>", weekly_ai_advice);
    weekly_box.append(*advice_card);

    auto* moti_card = make_section_card("<b>Motivasyon</b>", weekly_ai_motivation);
    moti_card->set_margin_bottom(8);
    weekly_box.append(*moti_card);

    auto* weak_frame = Gtk::make_managed<Gtk::Frame>();
    weak_frame->add_css_class("card");
    weekly_weak_deck_box.set_margin_start(14);
    weekly_weak_deck_box.set_margin_end(14);
    weekly_weak_deck_box.set_margin_top(10);
    weekly_weak_deck_box.set_margin_bottom(10);
    weekly_weak_deck_box.set_spacing(8);

    auto* weak_header = Gtk::make_managed<Gtk::Label>();
    weak_header->set_markup("<b>Zorlan\u0131lan Kartlar</b>");
    weak_header->set_halign(Gtk::Align::START);
    weak_header->add_css_class("weekly-section-header");
    weekly_weak_deck_box.append(*weak_header);

    weekly_weak_deck_info.set_halign(Gtk::Align::START);
    weekly_weak_deck_info.set_margin_top(2);
    weekly_weak_deck_info.set_margin_bottom(4);
    weekly_weak_deck_info.add_css_class("weekly-weak-info");
    weekly_weak_deck_box.append(weekly_weak_deck_info);

    btn_weekly_study_weak.add_css_class("flash-action-btn");
    btn_weekly_study_weak.set_halign(Gtk::Align::CENTER);
    btn_weekly_study_weak.set_margin_top(4);
    btn_weekly_study_weak.signal_clicked().connect([this]() {
        auto& report = get_current_weekly_report();
        if (!report.weak_deck_id.empty()) {
            ensure_page_built("flashcards");
            flash_show_review(report.weak_deck_id);
            auto* child = stack.get_child_by_name("flashcards");
            if (child) stack.set_visible_child(*child);
        }
    });
    weekly_weak_deck_box.append(btn_weekly_study_weak);

    weak_frame->set_child(weekly_weak_deck_box);
    weekly_box.append(*weak_frame);

    weekly_ai_dispatcher.connect(
        sigc::mem_fun(*this, &MainWindow::on_weekly_ai_response));

    sw->set_child(weekly_box);
    stack.add(*sw, "weekly_analysis");
}

void MainWindow::navigate_to_weekly_analysis() {
    ensure_page_built("weekly_analysis");
    collect_weekly_data();
    render_weekly_analysis_ui();

    auto* child = stack.get_child_by_name("weekly_analysis");
    if (child) stack.set_visible_child(*child);
}

WeeklyReport& MainWindow::get_current_weekly_report() {
    if (weekly_reports.empty() || weekly_reports.back().created_at <
        (std::time(nullptr) - 7 * 86400)) {
        WeeklyReport dummy;
        dummy.week_start = "...";
        dummy.created_at = std::time(nullptr);
        weekly_reports.push_back(dummy);
    }
    return weekly_reports.back();
}

void MainWindow::render_weekly_analysis_ui() {
    auto& report = get_current_weekly_report();

    std::string date_str = report.week_start;
    if (date_str.size() > 10) date_str = date_str.substr(0, 10);
    std::string end_str = report.week_end;
    if (end_str.size() > 10) end_str = end_str.substr(0, 10);
    weekly_header_label.set_text(
        "Haftal\u0131k Analiz  " + date_str + " - " + end_str);

    while (auto* child = weekly_stats_box.get_first_child())
        weekly_stats_box.remove(*child);

    auto add_stat = [this](const std::string& val, const std::string& desc) {
        auto* vb = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 2);
        vb->set_halign(Gtk::Align::CENTER);
        vb->set_margin_start(16);
        vb->set_margin_end(16);
        vb->set_margin_top(12);
        vb->set_margin_bottom(12);

        auto* vl = Gtk::make_managed<Gtk::Label>(val);
        vl->add_css_class("stat-value");
        vb->append(*vl);

        auto* dl = Gtk::make_managed<Gtk::Label>(desc);
        dl->add_css_class("stat-label");
        vb->append(*dl);

        auto* fr = Gtk::make_managed<Gtk::Frame>();
        fr->add_css_class("stat-card");
        fr->set_child(*vb);
        weekly_stats_box.append(*fr);
    };

    double accuracy = report.cards_reviewed > 0
        ? (100.0 * report.cards_correct / report.cards_reviewed) : 0.0;

    std::string hours_str;
    {
        int h = report.total_focus_minutes / 60;
        int m = report.total_focus_minutes % 60;
        if (h > 0) hours_str = std::to_string(h) + "s " + std::to_string(m) + "dk";
        else hours_str = std::to_string(m) + "dk";
    }

    add_stat(std::to_string(report.study_score) + "/100", "Haftal\u0131k Skor");
    add_stat(std::to_string((int)accuracy) + "%", "Do\u011fruluk");
    add_stat(std::to_string(report.total_pomo_sessions), "Pomodoro");
    add_stat(hours_str, "\u00c7al\u0131\u015fma S\u00fcresi");

    if (report.ai_summary.empty()) {
        weekly_ai_summary.set_text(
            "AI analizi haz\u0131rlan\u0131yor...");
        weekly_ai_advice.set_text("");
        weekly_ai_motivation.set_text("");
    } else {
        weekly_ai_summary.set_text(report.ai_summary);
        weekly_ai_advice.set_markup(report.ai_advice);
        weekly_ai_motivation.set_markup(
            "<span font_style='italic'>\u201c" + report.ai_motivation + "\u201d</span>");
    }

    if (!report.weak_deck_id.empty()) {
        int count = 0;
        for (auto& c : flash_cards) {
            if (c.deck_id == report.weak_deck_id) count++;
        }
        weekly_weak_deck_info.set_text(
            std::to_string(count) + " kart \u00b7 \u00c7al\u0131\u015fmak i\u00e7in "
            "butona t\u0131kla");
        btn_weekly_study_weak.set_visible(true);
        weekly_weak_deck_box.set_visible(true);
    } else {
        weekly_weak_deck_info.set_text(
            "Bu hafta zorlan\u0131lan kart bulunmuyor");
        btn_weekly_study_weak.set_visible(false);
    }
}

void MainWindow::rate_weak_card_from_analysis(int grade) {
    (void)grade;
}

std::string MainWindow::build_weekly_ai_prompt(const WeeklyReport& data) {
    std::string daily_str;
    for (auto& d : data.daily_stats) {
        daily_str += "- " + d.date + ": "
                     + std::to_string(d.pomo_sessions) + " pomodoro, "
                     + std::to_string(d.cards_reviewed) + " kart ("
                     + std::to_string(d.cards_correct) + "D/"
                     + std::to_string(d.cards_wrong) + "Y), "
                     + std::to_string(d.tasks_done) + "/"
                     + std::to_string(d.tasks_total) + " g\u00f6rev\n";
    }

    return
        "Sen bir \u00f6\u011frenci ko\u00e7usun. "
        "A\u015fa\u011f\u0131daki haftal\u0131k \u00e7al\u0131\u015fma verilerini analiz et.\n\n"

        "Hafta: " + data.week_start + " - " + data.week_end + "\n\n"

        "Flashcard \u0130statistikleri:\n"
        "- Toplam tekrar: " + std::to_string(data.cards_reviewed) + "\n"
        "- Do\u011fru: " + std::to_string(data.cards_correct) + " (%"
          + (data.cards_reviewed > 0
             ? std::to_string(data.cards_correct * 100 / data.cards_reviewed)
             : "0") + ")\n"
        "- Yanl\u0131\u015f: " + std::to_string(data.cards_wrong) + "\n"
        "- En ba\u015far\u0131l\u0131 deste ID: " + data.best_deck_id + "\n"
        "- En zay\u0131f deste ID: " + data.worst_deck_id + "\n"
        "- Zorlan\u0131lan kart say\u0131s\u0131: "
          + std::to_string(data.wrong_card_ids.size()) + "\n\n"

        "Pomodoro \u0130statistikleri:\n"
        "- Tamamlanan seans: " + std::to_string(data.total_pomo_sessions) + "\n"
        "- Toplam odak s\u00fcresi: " + std::to_string(data.total_focus_minutes)
          + " dakika\n\n"

        "G\u00f6rev \u0130statistikleri:\n"
        "- Tamamlanan: " + std::to_string(data.tasks_completed) + " / "
          + std::to_string(data.tasks_total) + "\n\n"

        "G\u00fcnl\u00fck Detay:\n" + daily_str + "\n"

        "Sadece a\u015fa\u011f\u0131daki JSON format\u0131nda yan\u0131t ver, "
        "ba\u015fka metin ekleme:\n\n"

        "{\n"
        "  \"ozet\": \"Haftan\u0131n 2-3 c\u00fcmlelik \u00f6zeti\",\n"
        "  \"guclu_yonler\": [\"Madde 1\", \"Madde 2\"],\n"
        "  \"gelisme_alanlari\": [\"Madde 1\", \"Madde 2\"],\n"
        "  \"oneriler\": [\"Madde 1\", \"Madde 2\", \"Madde 3\"],\n"
        "  \"motivasyon\": \"K\u0131sa motivasyon mesaj\u0131\"\n"
        "}";
}

void MainWindow::collect_weekly_data() {
    time_t now = std::time(nullptr);
    struct tm* tm_now = std::localtime(&now);
    int current_wday = tm_now->tm_wday;
    if (current_wday == 0) current_wday = 7;
    time_t monday = now - (current_wday - 1) * 86400;
    time_t sunday = monday + 6 * 86400;

    char buf[11];
    struct tm* tm_monday = std::localtime(&monday);
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm_monday);
    std::string week_start(buf);
    struct tm* tm_sunday = std::localtime(&sunday);
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", tm_sunday);
    std::string week_end(buf);

    WeeklyReport report;
    report.week_start = week_start;
    report.week_end = week_end;
    report.created_at = now;
    report.streak_count = study_streak;
    report.total_pomo_sessions = weekly_pomo_sessions;
    report.total_focus_minutes = weekly_pomo_minutes;
    report.tasks_completed = 0;
    report.tasks_total = (int)tasks.size();
    report.notes_created = (int)course_notes.size();

    for (auto& t : tasks) {
        if (t.done) report.tasks_completed++;
    }

    report.cards_reviewed = 0;
    report.cards_correct = 0;
    report.cards_wrong = 0;

    report.daily_stats.clear();
    int total_day_cards = 0;
    std::vector<int> day_card_counts(7, 0);

    for (int di = 0; di < 7; di++) {
        time_t day_sec = monday + di * 86400;
        time_t day_end = day_sec + 86400;
        struct tm* tm_day = std::localtime(&day_sec);
        char dbuf[11];
        std::strftime(dbuf, sizeof(dbuf), "%Y-%m-%d", tm_day);

        DailyStats ds;
        ds.date = dbuf;
        ds.cards_reviewed = 0;

        for (auto& card : flash_cards) {
            if (card.last_reviewed_at >= day_sec &&
                card.last_reviewed_at < day_end) {
                ds.cards_reviewed++;
            }
        }

        day_card_counts[di] = ds.cards_reviewed;
        total_day_cards += ds.cards_reviewed;

        ds.tasks_done = report.tasks_completed;
        ds.tasks_total = report.tasks_total;
        ds.pomo_sessions = 0;
        ds.focus_minutes = 0;

        report.daily_stats.push_back(ds);
    }

    // Distribute pomodoro and minutes proportionally by card activity
    int active_days = 0;
    for (int di = 0; di < 7; di++) {
        if (day_card_counts[di] > 0) active_days++;
    }
    if (active_days == 0 && weekly_pomo_sessions > 0) active_days = 7;

    if (active_days > 0) {
        int base_pomo = weekly_pomo_sessions / active_days;
        int base_min = weekly_pomo_minutes / active_days;
        int rem_pomo = weekly_pomo_sessions % active_days;
        int rem_min = weekly_pomo_minutes % active_days;
        for (int di = 0; di < 7; di++) {
            if (day_card_counts[di] > 0 || weekly_pomo_sessions > 0) {
                report.daily_stats[di].pomo_sessions = base_pomo + (rem_pomo > 0 ? 1 : 0);
                report.daily_stats[di].focus_minutes = base_min + (rem_min > 0 ? 1 : 0);
                if (rem_pomo > 0) rem_pomo--;
                if (rem_min > 0) rem_min--;
            }
        }
    }

    // Study score: weighted from multiple factors
    double score = 0;
    // Pomodoro factor (max 30 points): 10+ sessions = full
    score += std::min(30.0, (double)weekly_pomo_sessions * 3.0);
    // Task factor (max 25 points): 80% completion = full
    double task_rate = report.tasks_total > 0 ? (double)report.tasks_completed / report.tasks_total : 0;
    score += std::min(25.0, task_rate * 31.25);
    // Card factor (max 25 points): 50+ cards = full
    score += std::min(25.0, (double)total_day_cards * 0.5);
    // Streak factor (max 20 points): 7+ streak = full
    score += std::min(20.0, (double)study_streak * 2.857);
    report.study_score = (int)std::round(score);

    auto weak_indices = find_wrong_cards_this_week();
    auto leech_indices = find_leech_cards();
    std::vector<int> all_weak;
    all_weak.insert(all_weak.end(), weak_indices.begin(), weak_indices.end());
    all_weak.insert(all_weak.end(), leech_indices.begin(), leech_indices.end());
    std::sort(all_weak.begin(), all_weak.end());
    all_weak.erase(std::unique(all_weak.begin(), all_weak.end()), all_weak.end());

    if (!all_weak.empty()) {
        report.weak_deck_id = create_weak_deck(all_weak, week_start);
    }

    weekly_reports.push_back(report);
    save_data();

    std::string prompt = build_weekly_ai_prompt(report);
    weekly_ai_waiting = true;
    async_call_ai_json(prompt, [this](std::string response) {
        {
            std::lock_guard<std::mutex> lock(weekly_ai_mutex);
            pending_weekly_ai_response = response;
        }
        weekly_ai_dispatcher.emit();
    });
}

void MainWindow::on_weekly_ai_response() {
    std::lock_guard<std::mutex> lock(weekly_ai_mutex);
    if (pending_weekly_ai_response.empty()) return;

    auto& report = get_current_weekly_report();
    try {
        auto j = json::parse(pending_weekly_ai_response);
        report.ai_summary = j.value("ozet", "");
        report.ai_advice = "";
        if (j.contains("guclu_yonler")) {
            report.ai_advice += "\u2705 G\u00fc\u00e7l\u00fc Y\u00f6nler:\n";
            for (auto& g : j["guclu_yonler"])
                report.ai_advice += "\u2022 " + g.get<std::string>() + "\n";
        }
        if (j.contains("gelisme_alanlari")) {
            report.ai_advice += "\n\U0001f4a1 Geli\u015ftirilmesi Gerekenler:\n";
            for (auto& g : j["gelisme_alanlari"])
                report.ai_advice += "\u2022 " + g.get<std::string>() + "\n";
        }
        if (j.contains("oneriler")) {
            report.ai_advice += "\n\U0001f4cc \u00d6neriler:\n";
            for (auto& o : j["oneriler"])
                report.ai_advice += "\u2022 " + o.get<std::string>() + "\n";
        }
        report.ai_motivation = j.value("motivasyon", "");
    } catch (...) {
        report.ai_summary = "AI yan\u0131t\u0131 ayr\u0131\u015ft\u0131r\u0131lamad\u0131.";
    }

    pending_weekly_ai_response.clear();
    weekly_ai_waiting = false;
    save_data();
    render_weekly_analysis_ui();
}

std::vector<int> MainWindow::find_wrong_cards_this_week() {
    std::vector<int> result;
    time_t now = std::time(nullptr);
    time_t week_ago = now - 7 * 86400;

    for (int i = 0; i < (int)flash_cards.size(); i++) {
        auto& card = flash_cards[i];
        if (card.last_reviewed_at >= week_ago) {
            if (card.ease_factor < 1.8 || card.lapses > card.repetitions + 1) {
                result.push_back(i);
            }
        }
    }

    return result;
}

std::vector<int> MainWindow::find_leech_cards() {
    std::vector<int> result;
    for (int i = 0; i < (int)flash_cards.size(); i++) {
        auto& card = flash_cards[i];
        if (card.lapses >= 3 && card.ease_factor < 1.5) {
            result.push_back(i);
        }
    }
    return result;
}

std::string MainWindow::create_weak_deck(
    const std::vector<int>& card_indices,
    const std::string& week_label)
{
    cleanup_old_weak_decks();

    FlashDeck deck;
    deck.id = "weak_" + std::to_string(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
    deck.title = "Zay\u0131f Kartlar \u2014 " + week_label;
    deck.description = "Bu hafta zorlan\u0131lan kartlar ("
                       + std::to_string((int)card_indices.size()) + " kart)";
    deck.created_at = std::time(nullptr);
    deck.last_reviewed_at = 0;
    deck.card_count = (int)card_indices.size();

    flash_decks.push_back(deck);

    for (int idx : card_indices) {
        if (idx < 0 || idx >= (int)flash_cards.size()) continue;
        auto& src = flash_cards[idx];
        FlashCard copy;
        copy.id = flash_generate_id();
        copy.deck_id = deck.id;
        copy.front = src.front;
        copy.back = src.back;
        copy.hint = src.hint;
        copy.tags = "zayif,haftalik";
        copy.interval = 0;
        copy.repetitions = 0;
        copy.ease_factor = 2.5;
        copy.lapses = 0;
        copy.next_review_at = std::time(nullptr);
        copy.last_reviewed_at = 0;
        flash_cards.push_back(copy);
    }

    save_data();
    return deck.id;
}
