#define _USE_MATH_DEFINES
#include "glibmm/utility.h"
#include "mainwindow.h"
#include "ai_client.h"
#include "sigc++/functors/mem_fun.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <regex>
#include <thread>
#include <curl/curl.h>
#include <webkit/webkit.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <md4c-html.h>
#include <nlohmann/json.hpp>
#include <qrencode.h>
#include <vte/vte.h>

using json = nlohmann::json;

void MainWindow::setup_pomodoro() {
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

    auto* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 14);
    box->set_margin_start(20);
    box->set_margin_end(20);
    box->set_margin_top(20);
    box->set_margin_bottom(20);
    box->set_halign(Gtk::Align::CENTER);
    box->set_valign(Gtk::Align::CENTER);

    auto* title = Gtk::make_managed<Gtk::Label>("Pomodoro");
    title->add_css_class("page-title");
    title->set_halign(Gtk::Align::CENTER);

    pomo_time.add_css_class("pomo-time");
    pomo_status.add_css_class("pomo-status");
    pomo_progress.set_fraction(0.0);
    pomo_progress.set_margin_start(20);
    pomo_progress.set_margin_end(20);

    btn_pomo_start.add_css_class("pomo-btn");
    btn_pomo_reset.add_css_class("pomo-btn");
    btn_pomo_skip.add_css_class("pomo-btn");

    auto* btn_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    btn_box->set_halign(Gtk::Align::CENTER);
    btn_box->append(btn_pomo_start);
    btn_box->append(btn_pomo_reset);
    btn_box->append(btn_pomo_skip);

    btn_pomo_start.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_pomo_start));
    btn_pomo_reset.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_pomo_reset));
    btn_pomo_skip.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_pomo_skip));

    auto* settings_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 16);
    settings_box->set_halign(Gtk::Align::CENTER);
    settings_box->set_margin_top(8);

    auto* focus_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 4);
    focus_box->set_halign(Gtk::Align::CENTER);
    auto* focus_lbl = Gtk::make_managed<Gtk::Label>("Odaklanma");
    focus_lbl->add_css_class("pomo-setting-label");
    spin_pomo_duration.set_range(1, 120);
    spin_pomo_duration.set_value(25);
    spin_pomo_duration.set_increments(5, 10);
    spin_pomo_duration.set_numeric(true);
    spin_pomo_duration.set_wrap(false);
    spin_pomo_duration.add_css_class("pomo-spin");
    spin_pomo_duration.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_pomo_duration_changed));
    focus_box->append(*focus_lbl);
    focus_box->append(spin_pomo_duration);

    auto* break_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 4);
    break_box->set_halign(Gtk::Align::CENTER);
    auto* break_lbl = Gtk::make_managed<Gtk::Label>("Mola");
    break_lbl->add_css_class("pomo-setting-label");
    spin_pomo_break.set_range(1, 30);
    spin_pomo_break.set_value(5);
    spin_pomo_break.set_increments(1, 5);
    spin_pomo_break.set_numeric(true);
    spin_pomo_break.set_wrap(false);
    spin_pomo_break.add_css_class("pomo-spin");
    spin_pomo_break.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_pomo_break_changed));
    break_box->append(*break_lbl);
    break_box->append(spin_pomo_break);

    settings_box->append(*focus_box);
    settings_box->append(*break_box);

    box->append(*title);
    box->append(pomo_status);
    box->append(pomo_time);
    box->append(pomo_progress);
    box->append(*btn_box);
    box->append(*settings_box);
    sw->set_child(*box);
    stack.add(*sw, "pomodoro");
}

void MainWindow::setup_badges() {
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);

    auto* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    box->set_margin_start(16);
    box->set_margin_end(16);
    box->set_margin_top(16);
    box->set_margin_bottom(16);

    auto* title = Gtk::make_managed<Gtk::Label>("Rozetlerim");
    title->add_css_class("page-title");
    title->set_halign(Gtk::Align::START);
    box->append(*title);

    auto* sub = Gtk::make_managed<Gtk::Label>("Çalışma serini koruyarak rozetler kazan!");
    sub->set_halign(Gtk::Align::START);
    sub->add_css_class("dash-sub");
    sub->set_margin_bottom(8);
    box->append(*sub);

    auto* streak_lbl = Gtk::make_managed<Gtk::Label>("");
    streak_lbl->set_markup("Mevcut Seri: <b>" + std::to_string(study_streak) + " gün</b>");
    streak_lbl->set_halign(Gtk::Align::START);
    streak_lbl->set_margin_bottom(16);
    box->append(*streak_lbl);

    struct BadgeDef { std::string name; std::string resource; int required; std::string desc; };
    std::vector<BadgeDef> badges = {
        {
            "5 Gün Serisi",  
            "/org/ogrenci/merkezi/assets/rozet/5days.png", 
             5,  
             "5 gün ara vermeden çalış"
        },
        {
            "7 Gün Serisi",  
            "/org/ogrenci/merkezi/assets/rozet/7days.png",  
            7,  
            "7 gün ara vermeden çalış"
        },
        {
            "14 Gün Serisi", 
            "/org/ogrenci/merkezi/assets/rozet/14days.png", 
            14, 
            "14 gün ara vermeden çalış"
        },
        {
            "21 Gün",
            "/org/ogrenci/merkezi/assets/rozet/21days.png",
            21,
            "Ha gayret, 1 Ay olacak"
        },
        {
            "30 Gün Serisi",
            "/org/ogrenci/merkezi/assets/rozet/30days.png",
            30,
            "1 Ay, iyi İlerliyorsun!"
        },
        {
            "60 Gün Serisi",
            "/org/ogrenci/merkezi/assets/rozet/60days.png",
            60,
            "2 Ay, Sadık Öğrenci..."
        },
        {
            "90 Gün Serisi",
            "/org/ogrenci/merkezi/assets/rozet/90days.png",
            90,
            "Ders Ustası"
        },
        {
            "180 Gün Serisi",
            "/org/ogrenci/merkezi/assets/rozet/180days.png",
            180,
            "Ders Canavarı"
        },
        {
            "365 Gün Serisi",
            "/org/ogrenci/merkezi/assets/rozet/365days.png",
            365,
            "Akademik Canavar..."
        }
    };

    auto* flow = Gtk::make_managed<Gtk::FlowBox>();
    flow->set_max_children_per_line(3);
    flow->set_selection_mode(Gtk::SelectionMode::NONE);
    flow->set_column_spacing(16);
    flow->set_row_spacing(16);
    flow->set_homogeneous(true);
    flow->set_margin_top(4);

    for (auto& b : badges) {
        bool earned = study_streak >= b.required;

        auto* card = Gtk::make_managed<Gtk::Frame>();
        card->add_css_class("card");

        auto* vb = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);
        vb->set_halign(Gtk::Align::CENTER);
        vb->set_valign(Gtk::Align::CENTER);
        vb->set_margin_start(24);
        vb->set_margin_end(24);
        vb->set_margin_top(24);
        vb->set_margin_bottom(24);

        auto* img = Gtk::make_managed<Gtk::Image>();
        img->set_from_resource(b.resource);
        img->set_pixel_size(96);

        if (!earned) img->set_opacity(0.2);
        vb->append(*img);

        auto* nl = Gtk::make_managed<Gtk::Label>("");
        nl->set_markup("<b>" + b.name + "</b>");
        nl->set_halign(Gtk::Align::CENTER);
        nl->set_margin_top(4);
        if (!earned) nl->set_opacity(0.5);
        vb->append(*nl);

        auto* dl = Gtk::make_managed<Gtk::Label>(b.desc);
        dl->set_halign(Gtk::Align::CENTER);
        dl->set_wrap(true);
        dl->set_opacity(0.55);
        vb->append(*dl);

        if (earned) {
            auto* el = Gtk::make_managed<Gtk::Label>("Kazanıldı!");
            el->set_halign(Gtk::Align::CENTER);
            el->add_css_class("badge-earned");
            vb->append(*el);
        } else {
            auto* ll = Gtk::make_managed<Gtk::Label>("Kilitli");
            ll->set_halign(Gtk::Align::CENTER);
            ll->set_opacity(0.4);
            vb->append(*ll);
        }

        card->set_child(*vb);
        flow->append(*card);
    }

    box->append(*flow);
    sw->set_child(*box);
    stack.add(*sw, "badges");
}

void MainWindow::setup_flashcards() {
    flash_load_anki_exports();

    flash_back_btn.add_css_class("flash-back-btn");
    flash_back_btn.set_label("\u2190 Geri");
    flash_header_title.add_css_class("flash-page-title");

    flash_card_front.set_wrap(true);
    flash_card_front.set_max_width_chars(30);
    flash_card_back.set_wrap(true);
    flash_card_back.set_max_width_chars(30);
    flash_card_hint.set_wrap(true);
    flash_card_hint.set_max_width_chars(30);

    flash_back_btn.signal_clicked().connect([this]() {
        auto name = flash_stack.get_visible_child_name();
        if (name == "deck_detail") {
            flash_show_decks();
        } else if (name == "review") {
            flash_show_deck_detail(flash_current_deck_id);
        } else if (name == "card_edit") {
            flash_show_deck_detail(flash_current_deck_id);
        } else if (name == "ai_generate") {
            flash_show_deck_detail(flash_current_deck_id);
        }
    });

    auto* header_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    header_box->append(flash_back_btn);
    header_box->append(flash_header_title);
    header_box->set_margin_bottom(16);

    auto* page_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    page_box->set_margin_start(20);
    page_box->set_margin_end(20);
    page_box->set_margin_top(16);
    page_box->set_margin_bottom(16);
    page_box->append(*header_box);
    page_box->append(flash_stack);

    flash_stack.set_transition_type(Gtk::StackTransitionType::SLIDE_LEFT_RIGHT);
    flash_stack.set_transition_duration(300);

    flash_show_decks();

    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);
    sw->set_child(*page_box);
    stack.add(*sw, "flashcards");
}

void MainWindow::flash_show_decks() {
    auto* existing = flash_stack.get_child_by_name("decks");
    if (existing) {
        flash_stack.remove(*existing);
    }

    flash_back_btn.set_visible(false);
    flash_header_title.set_text("Bilgi Kartlar\u0131m");

    flash_refresh_all_deck_stats();

    auto* outer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 16);
    outer->set_valign(Gtk::Align::START);

    auto* section_title = Gtk::make_managed<Gtk::Label>("Kart Destelerin");
    section_title->add_css_class("flash-section-title");
    section_title->set_halign(Gtk::Align::START);
    outer->append(*section_title);

    auto* section_sub = Gtk::make_managed<Gtk::Label>(
        std::to_string(flash_decks.size()) + " deste \u2014 toplam " +
        std::to_string(flash_cards.size()) + " kart");
    section_sub->add_css_class("flash-section-subtitle");
    section_sub->set_halign(Gtk::Align::START);
    outer->append(*section_sub);

    auto* action_row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 12);
    action_row->set_margin_top(20);
    action_row->set_margin_bottom(8);

    auto* btn_new_deck = Gtk::make_managed<Gtk::Button>("+ Yeni Kart Destesi");
    btn_new_deck->add_css_class("flash-action-btn");
    btn_new_deck->signal_clicked().connect([this]() {
        auto dialog = Gtk::make_managed<Gtk::Dialog>();
        dialog->set_title("Yeni Kart Destesi");
        dialog->set_transient_for(*this);
        dialog->set_modal(true);
        dialog->set_default_size(350, 120);

        dialog->add_button("\u0130ptal", Gtk::ResponseType::CANCEL);
        dialog->add_button("Olu\u015ftur", Gtk::ResponseType::OK);

        auto* content = dialog->get_content_area();
        content->set_margin_start(16);
        content->set_margin_end(16);
        content->set_margin_top(16);
        content->set_margin_bottom(16);

        auto* entry = Gtk::make_managed<Gtk::Entry>();
        entry->set_placeholder_text("Deste ad\u0131 (ornek: Matematik Terimleri)");
        entry->set_hexpand(true);
        content->append(*entry);

        entry->signal_activate().connect([dialog]() {
            dialog->response(Gtk::ResponseType::OK);
        });

        dialog->signal_response().connect([this, dialog, entry](int response_id) {
            if (response_id == Gtk::ResponseType::OK) {
                auto name = entry->get_text();
                if (name.empty()) name = "Yeni Deste";
                auto* deck = &flash_decks.emplace_back();
                deck->id = flash_generate_id();
                deck->title = name;
                deck->created_at = std::time(nullptr);
                save_data();
                flash_show_deck_detail(deck->id);
            }
            dialog->hide();
        });

        dialog->show();
    });

    auto* btn_ai_deck = Gtk::make_managed<Gtk::Button>("AI ile Olu\u015ftur");
    btn_ai_deck->add_css_class("flash-ai-btn");
    btn_ai_deck->signal_clicked().connect([this]() {
        auto* deck = &flash_decks.emplace_back();
        deck->id = flash_generate_id();
        deck->title = "Yapay Zeka Destesi";
        deck->created_at = std::time(nullptr);
        save_data();
        flash_show_ai_generate(deck->id);
    });

    action_row->append(*btn_new_deck);
    action_row->append(*btn_ai_deck);
    outer->append(*action_row);

    if (flash_decks.empty()) {
        auto* empty_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
        empty_box->set_halign(Gtk::Align::CENTER);
        empty_box->set_valign(Gtk::Align::CENTER);
        empty_box->set_margin_top(60);

        auto* empty_icon = Gtk::make_managed<Gtk::Label>("\U0001F4C7");
        empty_icon->add_css_class("flash-empty-icon");

        auto* empty_title = Gtk::make_managed<Gtk::Label>("Hen\u00fcz kart desten yok");
        empty_title->add_css_class("flash-empty-title");

        auto* empty_desc = Gtk::make_managed<Gtk::Label>(
            "Ders notlar\u0131ndan veya s\u0131f\u0131rdan kart desteleri olu\u015fturarak\n"
            "s\u0131navlara daha etkili haz\u0131rlanabilirsin.");
        empty_desc->add_css_class("flash-empty-desc");

        empty_box->append(*empty_icon);
        empty_box->append(*empty_title);
        empty_box->append(*empty_desc);
        outer->append(*empty_box);
        flash_stack.add(*outer, "decks");
        flash_stack.set_visible_child(*outer);
        return;
    }

    auto* cards_vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    cards_vbox->set_margin_top(8);

    time_t now = std::time(nullptr);

    for (size_t i = 0; i < flash_decks.size(); i++) {
        auto& deck = flash_decks[i];

        auto* card = Gtk::make_managed<Gtk::Frame>();
        card->add_css_class("flash-deck-card");
        card->set_hexpand(true);

        auto* hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 16);
        hbox->set_margin_start(20);
        hbox->set_margin_end(20);
        hbox->set_margin_top(18);
        hbox->set_margin_bottom(18);

        auto* info_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 4);
        info_box->set_hexpand(true);

        auto* title_lbl = Gtk::make_managed<Gtk::Label>(deck.title);
        title_lbl->add_css_class("flash-deck-title");
        title_lbl->set_halign(Gtk::Align::START);

        std::string stats_str = std::to_string(deck.card_count) + " kart";
        if (deck.new_count > 0) stats_str += " \u00b7 " + std::to_string(deck.new_count) + " yeni";
        if (deck.due_count > 0) stats_str += " \u00b7 " + std::to_string(deck.due_count) + " tekrar";
        if (!deck.source_course.empty()) stats_str += " \u00b7 " + deck.source_course;

        auto* stats_lbl = Gtk::make_managed<Gtk::Label>(stats_str);
        stats_lbl->add_css_class("flash-deck-stats");
        stats_lbl->set_halign(Gtk::Align::START);

        info_box->append(*title_lbl);
        info_box->append(*stats_lbl);

        if (deck.last_reviewed_at > 0) {
            auto* time_lbl = Gtk::make_managed<Gtk::Label>();
            time_lbl->add_css_class("flash-deck-time");
            time_lbl->set_halign(Gtk::Align::START);

            int diff_days = (now - deck.last_reviewed_at) / 86400;
            if (diff_days == 0) time_lbl->set_text("Az \u00f6nce");
            else if (diff_days == 1) time_lbl->set_text("D\u00fcn");
            else time_lbl->set_text(std::to_string(diff_days) + " g\u00fcn \u00f6nce");

            info_box->append(*time_lbl);
        }

        auto* right_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);
        right_box->set_valign(Gtk::Align::CENTER);
        right_box->set_halign(Gtk::Align::CENTER);

        if (deck.due_count > 0 || deck.new_count > 0) {
            auto* study_btn = Gtk::make_managed<Gtk::Button>(" \u2192 \u00c7al\u0131\u015f");
            study_btn->add_css_class("flash-study-btn");
            study_btn->signal_clicked().connect([this, id = deck.id]() {
                flash_show_review(id);
            });
            right_box->append(*study_btn);
        }

        auto* arrow_lbl = Gtk::make_managed<Gtk::Label>("\u2192");
        arrow_lbl->add_css_class("flash-deck-arrow");

        if (deck.due_count > 0 || deck.new_count > 0) {
            right_box->append(*arrow_lbl);
        }

        hbox->append(*info_box);
        hbox->append(*right_box);
        card->set_child(*hbox);

        std::string deck_id = deck.id;
        auto gesture = Gtk::GestureClick::create();
        gesture->signal_released().connect([this, deck_id](int, double, double) {
            flash_show_deck_detail(deck_id);
        });
        card->add_controller(gesture);

        cards_vbox->append(*card);
    }

    outer->append(*cards_vbox);
    flash_stack.add(*outer, "decks");
    flash_stack.set_visible_child(*outer);
}

void MainWindow::flash_refresh_all_deck_stats() {
    for (auto& d : flash_decks) {
        flash_refresh_deck_stats(d.id);
    }
}

void MainWindow::flash_refresh_deck_stats(const std::string& deck_id) {
    auto it = std::find_if(flash_decks.begin(), flash_decks.end(),
        [&](const FlashDeck& d) { return d.id == deck_id; });
    if (it == flash_decks.end()) return;

    time_t now = std::time(nullptr);
    it->card_count = 0;
    it->new_count = 0;
    it->due_count = 0;
    it->review_count = 0;

    for (auto& card : flash_cards) {
        if (card.deck_id != deck_id) continue;
        it->card_count++;
        if (card.repetitions == 0) it->new_count++;
        else if (card.next_review_at <= now) it->due_count++;
        it->review_count += card.repetitions;
    }
}

void MainWindow::flash_show_deck_detail(const std::string& deck_id) {
    auto* old_detail = flash_stack.get_child_by_name("deck_detail");
    if (old_detail) flash_stack.remove(*old_detail);
    auto* old_review = flash_stack.get_child_by_name("review");
    if (old_review) flash_stack.remove(*old_review);

    auto it = std::find_if(flash_decks.begin(), flash_decks.end(),
        [&](const FlashDeck& d) { return d.id == deck_id; });
    if (it == flash_decks.end()) return;

    flash_back_btn.set_visible(true);
    flash_header_title.set_text(it->title);
    flash_current_deck_id = deck_id;

    flash_refresh_deck_stats(deck_id);

    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);

    auto* outer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    outer->set_halign(Gtk::Align::FILL);

    int learned = 0;
    for (auto& c : flash_cards)
        if (c.deck_id == deck_id && c.repetitions > 0) learned++;

    auto* stats_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    stats_box->set_margin_top(8);
    stats_box->set_margin_bottom(20);
    stats_box->add_css_class("flash-stats-row");

    auto make_stat = [](const std::string& val, const std::string& label) -> Gtk::Widget* {
        auto* vb = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 2);
        vb->set_halign(Gtk::Align::CENTER);
        vb->set_hexpand(true);
        auto* v = Gtk::make_managed<Gtk::Label>(val);
        v->add_css_class("flash-stat-value");
        auto* l = Gtk::make_managed<Gtk::Label>(label);
        l->add_css_class("flash-stat-label");
        vb->append(*v);
        vb->append(*l);
        return vb;
    };

    stats_box->append(*make_stat(std::to_string(it->card_count), "Kart"));
    stats_box->append(*make_stat(std::to_string(learned), "\u00d6\u011frenildi"));
    stats_box->append(*make_stat(std::to_string(it->due_count), "Tekrar"));
    stats_box->append(*make_stat(std::to_string(it->review_count), "Toplam"));

    outer->append(*stats_box);

    auto* action_row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
    action_row->set_margin_bottom(20);

    auto* study_btn = Gtk::make_managed<Gtk::Button>(
        it->due_count > 0 || it->new_count > 0 ? "\u25b6 \u00c7al\u0131\u015fmaya Ba\u015fla" : "\u25b6 G\u00f6zden Ge\u00e7ir");
    study_btn->add_css_class("flash-study-btn");
    study_btn->set_hexpand(true);
    study_btn->signal_clicked().connect([this, deck_id]() {
        flash_show_review(deck_id);
    });

    auto* ai_btn = Gtk::make_managed<Gtk::Button>("\u2728 AI'dan Kart \u00dcret");
    ai_btn->add_css_class("flash-ai-btn-sm");
    ai_btn->signal_clicked().connect([this, deck_id]() {
        flash_show_ai_generate(deck_id);
    });

    auto* export_btn = Gtk::make_managed<Gtk::Button>("\u2191 Anki'ye Aktar");
    export_btn->add_css_class("flash-action-btn-sm");
    export_btn->signal_clicked().connect([this, deck_id]() {
        flash_export_anki(deck_id);
    });

    action_row->append(*study_btn);
    action_row->append(*ai_btn);
    action_row->append(*export_btn);
    outer->append(*action_row);

    if (!it->source_course.empty() || !it->description.empty()) {
        auto* info_card = Gtk::make_managed<Gtk::Frame>();
        info_card->add_css_class("flash-info-card");
        auto* info_vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
        info_vbox->set_margin_start(16);
        info_vbox->set_margin_end(16);
        info_vbox->set_margin_top(14);
        info_vbox->set_margin_bottom(14);

        if (!it->source_course.empty()) {
            auto* course_lbl = Gtk::make_managed<Gtk::Label>("Ders: " + it->source_course);
            course_lbl->set_halign(Gtk::Align::START);
            course_lbl->add_css_class("flash-info-text");
            info_vbox->append(*course_lbl);
        }
        if (!it->description.empty()) {
            auto* desc_lbl = Gtk::make_managed<Gtk::Label>(it->description);
            desc_lbl->set_halign(Gtk::Align::START);
            desc_lbl->add_css_class("flash-info-text");
            desc_lbl->set_wrap(true);
            info_vbox->append(*desc_lbl);
        }
        info_card->set_child(*info_vbox);
        outer->append(*info_card);
    }

    auto* list_title = Gtk::make_managed<Gtk::Label>("Kartlar");
    list_title->add_css_class("flash-section-title");
    list_title->set_halign(Gtk::Align::START);
    list_title->set_margin_top(16);
    list_title->set_margin_bottom(4);
    outer->append(*list_title);

    auto* add_row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
    auto* add_card_btn = Gtk::make_managed<Gtk::Button>("+ Kart Ekle");
    add_card_btn->add_css_class("flash-add-btn");
    add_card_btn->signal_clicked().connect([this, deck_id]() {
        flash_show_card_edit(deck_id, "");
    });

    auto* rename_btn = Gtk::make_managed<Gtk::Button>("Yeniden Adland\u0131r");
    rename_btn->add_css_class("flash-action-btn-sm");
    rename_btn->signal_clicked().connect([this, deck_id]() {
        auto it = std::find_if(flash_decks.begin(), flash_decks.end(),
            [&](const FlashDeck& d) { return d.id == deck_id; });
        if (it == flash_decks.end()) return;

        auto dialog = Gtk::make_managed<Gtk::Dialog>();
        dialog->set_title("Desteyi Yeniden Adland\u0131r");
        dialog->set_transient_for(*this);
        dialog->set_modal(true);
        dialog->set_default_size(350, 120);

        dialog->add_button("\u0130ptal", Gtk::ResponseType::CANCEL);
        dialog->add_button("Kaydet", Gtk::ResponseType::OK);

        auto* content = dialog->get_content_area();
        content->set_margin_start(16);
        content->set_margin_end(16);
        content->set_margin_top(16);
        content->set_margin_bottom(16);

        auto* entry = Gtk::make_managed<Gtk::Entry>();
        entry->set_text(it->title);
        entry->set_placeholder_text("Deste ad\u0131");
        entry->set_hexpand(true);
        entry->select_region(0, -1);
        content->append(*entry);

        entry->signal_activate().connect([dialog]() {
            dialog->response(Gtk::ResponseType::OK);
        });

        dialog->signal_response().connect([this, dialog, entry, deck_id](int response_id) {
            if (response_id == Gtk::ResponseType::OK) {
                auto name = entry->get_text();
                if (!name.empty()) {
                    auto it = std::find_if(flash_decks.begin(), flash_decks.end(),
                        [&](const FlashDeck& d) { return d.id == deck_id; });
                    if (it != flash_decks.end()) {
                        it->title = name;
                        save_data();
                        flash_show_deck_detail(deck_id);
                    }
                }
            }
            dialog->hide();
        });

        dialog->show();
    });

    auto* delete_deck_btn = Gtk::make_managed<Gtk::Button>("Desteyi Sil");
    delete_deck_btn->add_css_class("flash-delete-btn");
    delete_deck_btn->signal_clicked().connect([this, deck_id]() {
        auto* dialog = new Gtk::MessageDialog(*this,
            "Bu desteyi silmek istedi\u011fine emin misin?",
            false, Gtk::MessageType::QUESTION, Gtk::ButtonsType::YES_NO, true);
        dialog->set_secondary_text("T\u00fcm kartlar kal\u0131c\u0131 olarak silinecek.");
        dialog->signal_response().connect([this, dialog, deck_id](int resp) {
            if (resp == GTK_RESPONSE_YES) {
                flash_delete_deck(deck_id);
                flash_show_decks();
            }
            delete dialog;
        });
        dialog->present();
    });

    add_row->append(*add_card_btn);
    add_row->append(*rename_btn);
    add_row->append(*delete_deck_btn);
    outer->append(*add_row);

    auto* cards_vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);
    cards_vbox->set_margin_top(12);

    int card_idx = 0;
    for (size_t i = 0; i < flash_cards.size(); i++) {
        auto& card = flash_cards[i];
        if (card.deck_id != deck_id) continue;
        card_idx++;

        auto* frame = Gtk::make_managed<Gtk::Frame>();
        frame->add_css_class("flash-list-card");
        frame->set_hexpand(true);

        auto* hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 14);
        hbox->set_margin_start(16);
        hbox->set_margin_end(16);
        hbox->set_margin_top(12);
        hbox->set_margin_bottom(12);

        auto* badge = Gtk::make_managed<Gtk::Label>(std::to_string(card_idx));
        badge->add_css_class("flash-list-badge");

        auto* text_vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 2);
        text_vbox->set_hexpand(true);

        auto* front_lbl = Gtk::make_managed<Gtk::Label>(card.front);
        front_lbl->add_css_class("flash-list-front");
        front_lbl->set_halign(Gtk::Align::START);
        front_lbl->set_ellipsize(Pango::EllipsizeMode::END);

        std::string meta = card.repetitions > 0
            ? std::to_string(card.repetitions) + " tekrar \u00b7 " + std::to_string(card.interval) + " g\u00fcn ara"
            : "Hen\u00fcz \u00e7al\u0131\u015f\u0131lmad\u0131";

        auto* meta_lbl = Gtk::make_managed<Gtk::Label>(meta);
        meta_lbl->add_css_class("flash-list-meta");
        meta_lbl->set_halign(Gtk::Align::START);

        text_vbox->append(*front_lbl);
        text_vbox->append(*meta_lbl);

        auto* arrow_lbl = Gtk::make_managed<Gtk::Label>("\u270e");
        arrow_lbl->add_css_class("flash-list-edit");

        hbox->append(*badge);
        hbox->append(*text_vbox);
        hbox->append(*arrow_lbl);
        frame->set_child(*hbox);

        std::string cid = card.id;
        auto gesture = Gtk::GestureClick::create();
        gesture->signal_released().connect([this, deck_id, cid](int, double, double) {
            flash_show_card_edit(deck_id, cid);
        });
        frame->add_controller(gesture);

        cards_vbox->append(*frame);
    }

    if (card_idx == 0) {
        auto* empty_lbl = Gtk::make_managed<Gtk::Label>(
            "Hen\u00fcz kart eklenmemi\u015f. AI ile kart olu\u015ftur veya elle ekle.");
        empty_lbl->add_css_class("flash-empty-sub");
        empty_lbl->set_margin_top(20);
        cards_vbox->append(*empty_lbl);
    }

    outer->append(*cards_vbox);
    sw->set_child(*outer);
    flash_stack.add(*sw, "deck_detail");
    flash_stack.set_visible_child(*sw);
}

void MainWindow::flash_show_review(const std::string& deck_id) {
    // Clear reusable member containers from previous review
    while (auto* child = flash_rate_box.get_first_child())
        flash_rate_box.remove(*child);

    auto* old_review = flash_stack.get_child_by_name("review");
    if (old_review) flash_stack.remove(*old_review);

    // Ensure member widgets are unparented before re-use
    auto unparent = [](Gtk::Widget* w) {
        if (w->get_parent()) w->unparent();
    };
    unparent(&flash_review_progress);
    unparent(&flash_card_front);
    unparent(&flash_card_back);
    unparent(&flash_card_hint);
    unparent(&flash_card_container);
    unparent(&flash_reveal_btn);
    unparent(&flash_rate_box);

    flash_back_btn.set_visible(true);

    auto it = std::find_if(flash_decks.begin(), flash_decks.end(),
        [&](const FlashDeck& d) { return d.id == deck_id; });
    if (it == flash_decks.end()) return;
    flash_header_title.set_text(it->title);
    flash_current_deck_id = deck_id;

    flash_build_review_queue(deck_id);

    if (flash_review_queue.empty()) {
        auto* done_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 16);
        done_box->set_valign(Gtk::Align::CENTER);
        done_box->set_halign(Gtk::Align::CENTER);
        done_box->set_margin_top(80);

        auto* icon = Gtk::make_managed<Gtk::Label>("\u2705");
        icon->add_css_class("flash-done-icon");

        auto* title = Gtk::make_managed<Gtk::Label>("Tebrikler!");
        title->add_css_class("flash-done-title");

        auto* desc = Gtk::make_managed<Gtk::Label>(
            "Bug\u00fcn tekrar\u0131 gereken t\u00fcm kartlar\u0131 tamamlad\u0131n.\n"
            "Yeni kart kalmad\u0131. Bir sonraki tekrar vakti geldi\u011finde\n"
            "tekrar g\u00f6r\u00fc\u015fmek \u00fczere!");
        desc->add_css_class("flash-done-desc");

        auto* btn = Gtk::make_managed<Gtk::Button>("Geri D\u00f6n");
        btn->add_css_class("flash-action-btn");
        btn->signal_clicked().connect([this, deck_id]() {
            flash_show_deck_detail(deck_id);
        });

        done_box->append(*icon);
        done_box->append(*title);
        done_box->append(*desc);
        done_box->append(*btn);

        flash_stack.add(*done_box, "review");
        flash_stack.set_visible_child(*done_box);
        return;
    }

    flash_current_index = 0;

    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);

    auto* outer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    outer->set_valign(Gtk::Align::CENTER);
    outer->set_halign(Gtk::Align::CENTER);
    outer->set_margin_top(20);

    flash_review_progress.set_markup(
        "1 / " + std::to_string(flash_review_queue.size()) +
        " \u2022 Kalan: " + std::to_string(flash_review_queue.size()));
    flash_review_progress.add_css_class("flash-review-progress");
    outer->append(flash_review_progress);

    auto* progress_bar = Gtk::make_managed<Gtk::ProgressBar>();
    progress_bar->set_fraction(0.0);
    progress_bar->add_css_class("flash-progress-bar");
    progress_bar->set_size_request(400, -1);
    outer->append(*progress_bar);

    outer->append(*Gtk::make_managed<Gtk::Label>(""));

    flash_card_container.add_css_class("flash-card-container");
    flash_card_container.set_size_request(520, -1);

    flash_card_front.add_css_class("flash-card-front");
    flash_card_back.add_css_class("flash-card-back");
    flash_card_hint.add_css_class("flash-card-hint");

    flash_card_front.set_wrap(true);
    flash_card_front.set_max_width_chars(30);
    flash_card_back.set_wrap(true);
    flash_card_back.set_max_width_chars(30);
    flash_card_hint.set_wrap(true);
    flash_card_hint.set_max_width_chars(30);

    auto* card_inner = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    card_inner->set_margin_start(32);
    card_inner->set_margin_end(32);
    card_inner->set_margin_top(48);
    card_inner->set_margin_bottom(32);
    card_inner->set_vexpand(true);
    card_inner->set_valign(Gtk::Align::FILL);

    card_inner->append(flash_card_front);

    flash_card_back.set_visible(false);
    flash_card_hint.set_visible(false);
    card_inner->append(flash_card_back);
    card_inner->append(flash_card_hint);

    flash_card_container.set_child(*card_inner);
    outer->append(flash_card_container);

    flash_reveal_btn.set_label("Cevab\u0131 G\u00f6ster");
    flash_reveal_btn.add_css_class("flash-reveal-btn");
    flash_reveal_btn.set_size_request(320, 50);
    flash_reveal_btn.set_margin_top(24);
    flash_reveal_conn.disconnect();
    flash_reveal_conn = flash_reveal_btn.signal_clicked().connect([this, progress_bar]() {
        flash_card_front.remove_css_class("flash-card-front");
        flash_card_front.add_css_class("flash-card-front-revealed");

        flash_card_back.set_visible(true);
        flash_card_hint.set_visible(true);
        flash_reveal_btn.set_visible(false);
        flash_rate_box.set_visible(true);

        flash_card_container.add_css_class("flash-card-flipped");
    });
    outer->append(flash_reveal_btn);

    flash_rate_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    flash_rate_box.set_spacing(10);
    flash_rate_box.set_margin_top(20);
    flash_rate_box.set_halign(Gtk::Align::CENTER);
    flash_rate_box.set_visible(false);

    auto make_rate_btn = [this, progress_bar](const std::string& label, const std::string& css, int grade) {
        auto* btn = Gtk::make_managed<Gtk::Button>(label);
        btn->add_css_class(css);
        btn->signal_clicked().connect([this, grade, progress_bar]() {
            flash_rate_card(grade);
            flash_show_next_card(progress_bar);
        });
        return btn;
    };

    flash_rate_box.append(*make_rate_btn("Unuttum", "flash-rate-0", 0));
    flash_rate_box.append(*make_rate_btn("Zor", "flash-rate-1", 2));
    flash_rate_box.append(*make_rate_btn("\u0130yi", "flash-rate-2", 4));
    flash_rate_box.append(*make_rate_btn("\u00c7ok Kolay", "flash-rate-3", 5));

    outer->append(flash_rate_box);

    flash_show_card_at_index(progress_bar);

    sw->set_child(*outer);
    flash_stack.add(*sw, "review");
    flash_stack.set_visible_child(*sw);
}

void MainWindow::flash_build_review_queue(const std::string& deck_id) {
    flash_review_queue.clear();
    time_t now = std::time(nullptr);

    for (int i = 0; i < (int)flash_cards.size(); i++) {
        auto& card = flash_cards[i];
        if (card.deck_id != deck_id) continue;
        if (card.repetitions > 0 && card.next_review_at <= now) {
            flash_review_queue.push_back(i);
        }
    }

    int new_limit = 10;
    for (int i = 0; i < (int)flash_cards.size() && new_limit > 0; i++) {
        auto& card = flash_cards[i];
        if (card.deck_id != deck_id) continue;
        if (card.repetitions == 0) {
            flash_review_queue.push_back(i);
            new_limit--;
        }
    }

    int review_count = 0;
    for (auto idx : flash_review_queue) {
        if (flash_cards[idx].repetitions == 0) break;
        review_count++;
    }
    for (int i = review_count; i < (int)flash_review_queue.size(); i++) {
        int j = review_count + rand() % ((int)flash_review_queue.size() - review_count);
        std::swap(flash_review_queue[i], flash_review_queue[j]);
    }
}

void MainWindow::flash_show_card_at_index(Gtk::ProgressBar* progress_bar) {
    if (flash_current_index < 0 || flash_current_index >= (int)flash_review_queue.size())
        return;

    int card_idx = flash_review_queue[flash_current_index];
    auto& card = flash_cards[card_idx];

    flash_card_front.set_text(card.front);
    flash_card_back.set_text(card.back);
    flash_card_hint.set_text(card.hint.empty() ? "" : "\U0001F4A1 \u0130pucu: " + card.hint);

    flash_card_front.remove_css_class("flash-card-front-revealed");
    flash_card_front.add_css_class("flash-card-front");
    flash_card_back.set_visible(false);
    flash_card_hint.set_visible(false);
    flash_reveal_btn.set_visible(true);
    flash_rate_box.set_visible(false);
    flash_card_container.remove_css_class("flash-card-flipped");

    flash_review_progress.set_markup(
        std::to_string(flash_current_index + 1) + " / " +
        std::to_string(flash_review_queue.size()) +
        " \u2022 Kalan: " + std::to_string(flash_review_queue.size() - flash_current_index));

    if (progress_bar) {
        progress_bar->set_fraction(
            (double)flash_current_index / flash_review_queue.size());
    }
}

void MainWindow::flash_show_next_card(Gtk::ProgressBar* progress_bar) {
    flash_current_index++;
    if (flash_current_index >= (int)flash_review_queue.size()) {
        flash_show_review_done();
        return;
    }
    flash_show_card_at_index(progress_bar);
}

void MainWindow::flash_show_review_done() {
    auto* done_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 16);
    done_box->set_valign(Gtk::Align::CENTER);
    done_box->set_halign(Gtk::Align::CENTER);
    done_box->set_margin_top(60);

    int total = flash_review_queue.size();
    int learned = 0;
    for (auto idx : flash_review_queue)
        if (flash_cards[idx].repetitions > 0) learned++;

    auto* icon = Gtk::make_managed<Gtk::Label>("\U0001F389");
    icon->add_css_class("flash-done-icon");

    auto* title = Gtk::make_managed<Gtk::Label>("Tekrar Tamamland\u0131!");
    title->add_css_class("flash-done-title");

    auto* stats = Gtk::make_managed<Gtk::Label>(
        std::to_string(total) + " kart \u00e7al\u0131\u015f\u0131ld\u0131 \u00b7 " +
        std::to_string(learned) + " kart \u00f6\u011frenildi");
    stats->add_css_class("flash-done-stats");

    auto* btn = Gtk::make_managed<Gtk::Button>("Desteye D\u00f6n");
    btn->add_css_class("flash-action-btn");
    btn->signal_clicked().connect([this]() {
        flash_show_deck_detail(flash_current_deck_id);
    });

    done_box->append(*icon);
    done_box->append(*title);
    done_box->append(*stats);
    done_box->append(*btn);

    flash_stack.add(*done_box, "review");
    flash_stack.set_visible_child(*done_box);
}

void MainWindow::flash_rate_card(int grade) {
    if (flash_current_index < 0 || flash_current_index >= (int)flash_review_queue.size())
        return;

    int card_idx = flash_review_queue[flash_current_index];
    if (card_idx < 0 || card_idx >= (int)flash_cards.size()) return;

    auto& card = flash_cards[card_idx];
    time_t now = std::time(nullptr);

    if (grade == 0) {
        card.repetitions = 0;
        card.interval = 1;
        card.ease_factor = std::max(1.3, card.ease_factor - 0.2);
        card.lapses++;
    } else if (grade <= 2) {
        card.interval = 1;
        card.repetitions = 0;
    } else if (grade == 3) {
        if (card.repetitions == 0) card.interval = 1;
        else if (card.repetitions == 1) card.interval = 3;
        else card.interval = (int)std::round(card.interval * card.ease_factor);
        card.repetitions++;
    } else if (grade == 4) {
        if (card.repetitions == 0) card.interval = 1;
        else if (card.repetitions == 1) card.interval = 6;
        else card.interval = (int)std::round(card.interval * card.ease_factor);
        card.repetitions++;
    } else {
        if (card.repetitions == 0) card.interval = 3;
        else if (card.repetitions == 1) card.interval = 7;
        else card.interval = (int)std::round(card.interval * card.ease_factor * 1.3);
        card.repetitions++;
    }

    double ef = card.ease_factor + (0.1 - (5 - grade) * (0.08 + (5 - grade) * 0.02));
    card.ease_factor = std::max(1.3, ef);

    card.next_review_at = now + card.interval * 86400;
    card.last_reviewed_at = now;

    auto dit = std::find_if(flash_decks.begin(), flash_decks.end(),
        [&](const FlashDeck& d) { return d.id == card.deck_id; });
    if (dit != flash_decks.end()) dit->last_reviewed_at = now;

    save_data();
}

void MainWindow::flash_show_card_edit(const std::string& deck_id, const std::string& card_id) {
    auto* old_edit = flash_stack.get_child_by_name("card_edit");
    if (old_edit) flash_stack.remove(*old_edit);

    flash_back_btn.set_visible(true);

    auto dit = std::find_if(flash_decks.begin(), flash_decks.end(),
        [&](const FlashDeck& d) { return d.id == deck_id; });
    if (dit == flash_decks.end()) return;
    flash_header_title.set_text("Kart D\u00fczenle");
    flash_current_deck_id = deck_id;

    auto* outer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    outer->set_margin_top(8);
    outer->set_halign(Gtk::Align::CENTER);

    auto* form_card = Gtk::make_managed<Gtk::Frame>();
    form_card->add_css_class("flash-edit-card");
    form_card->set_size_request(560, -1);

    auto* form = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 16);
    form->set_margin_start(24);
    form->set_margin_end(24);
    form->set_margin_top(24);
    form->set_margin_bottom(24);

    FlashCard* existing = nullptr;
    for (auto& c : flash_cards) {
        if (c.id == card_id) { existing = &c; break; }
    }

    auto* front_lbl = Gtk::make_managed<Gtk::Label>("\u00d6n Y\u00fcz (Soru / Terim)");
    front_lbl->add_css_class("flash-edit-label");
    front_lbl->set_halign(Gtk::Align::START);

    auto front_buffer = Gtk::TextBuffer::create();
    auto* front_view = Gtk::make_managed<Gtk::TextView>();
    front_view->set_buffer(front_buffer);
    front_view->add_css_class("flash-edit-textview");
    front_view->set_wrap_mode(Gtk::WrapMode::WORD);
    front_view->set_size_request(-1, 60);
    if (existing) front_buffer->set_text(existing->front);

    auto* back_lbl = Gtk::make_managed<Gtk::Label>("Arka Y\u00fcz (Cevap / Tan\u0131m)");
    back_lbl->add_css_class("flash-edit-label");
    back_lbl->set_halign(Gtk::Align::START);

    auto back_buffer = Gtk::TextBuffer::create();
    auto* back_view = Gtk::make_managed<Gtk::TextView>();
    back_view->set_buffer(back_buffer);
    back_view->add_css_class("flash-edit-textview");
    back_view->set_wrap_mode(Gtk::WrapMode::WORD);
    back_view->set_size_request(-1, 120);
    if (existing) back_buffer->set_text(existing->back);

    auto* hint_lbl = Gtk::make_managed<Gtk::Label>("\u0130pucu (iste\u011fe ba\u011fl\u0131)");
    hint_lbl->add_css_class("flash-edit-label");
    hint_lbl->set_halign(Gtk::Align::START);

    auto* hint_entry = Gtk::make_managed<Gtk::Entry>();
    hint_entry->add_css_class("flash-edit-entry");
    hint_entry->set_placeholder_text("\u0130pucu metni...");
    if (existing) hint_entry->set_text(existing->hint);

    auto* tags_lbl = Gtk::make_managed<Gtk::Label>("Etiketler (virg\u00fclle ay\u0131r\u0131n)");
    tags_lbl->add_css_class("flash-edit-label");
    tags_lbl->set_halign(Gtk::Align::START);

    auto* tags_entry = Gtk::make_managed<Gtk::Entry>();
    tags_entry->add_css_class("flash-edit-entry");
    tags_entry->set_placeholder_text("etiket1, etiket2, ...");
    if (existing) tags_entry->set_text(existing->tags);

    auto* btn_row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
    btn_row->set_margin_top(8);
    btn_row->set_halign(Gtk::Align::END);

    auto* cancel_btn = Gtk::make_managed<Gtk::Button>("\u0130ptal");
    cancel_btn->add_css_class("flash-edit-cancel");
    cancel_btn->signal_clicked().connect([this]() {
        flash_show_deck_detail(flash_current_deck_id);
    });

    auto* save_btn = Gtk::make_managed<Gtk::Button>("Kaydet");
    save_btn->add_css_class("flash-edit-save");
    save_btn->signal_clicked().connect([this, deck_id, card_id, front_buffer,
                                          back_buffer, hint_entry, tags_entry]() {
        auto front_iter = front_buffer->begin();
        std::string front = front_buffer->get_text(front_iter, front_buffer->end());
        auto back_iter = back_buffer->begin();
        std::string back = back_buffer->get_text(back_iter, back_buffer->end());
        if (front.empty() || back.empty()) return;

        if (card_id.empty()) {
            FlashCard c;
            c.id = flash_generate_id();
            c.deck_id = deck_id;
            c.front = front;
            c.back = back;
            c.hint = hint_entry->get_text();
            c.tags = tags_entry->get_text();
            c.next_review_at = std::time(nullptr);
            flash_cards.push_back(c);
        } else {
            for (auto& c : flash_cards) {
                if (c.id == card_id) {
                    c.front = front;
                    c.back = back;
                    c.hint = hint_entry->get_text();
                    c.tags = tags_entry->get_text();
                    break;
                }
            }
        }

        save_data();
        flash_show_deck_detail(deck_id);
    });

    btn_row->append(*cancel_btn);
    btn_row->append(*save_btn);

    auto* ai_edit_btn = Gtk::make_managed<Gtk::Button>("\u2728 AI ile D\u00fczenle");
    ai_edit_btn->add_css_class("flash-ai-edit-btn");
    ai_edit_btn->signal_clicked().connect([this, front_buffer, back_buffer, hint_entry, ai_edit_btn]() {
        auto front_iter = front_buffer->begin();
        std::string front_text = front_buffer->get_text(front_iter, front_buffer->end());
        auto iter = back_buffer->begin();
        std::string back_text = back_buffer->get_text(iter, back_buffer->end());
        std::string hint_text = hint_entry->get_text();
        if (front_text.empty() && back_text.empty()) return;

        std::string prompt =
            "Su bilgi kartini daha iyi, daha aciklayici ve ogrenci dostu hale getir.\n"
            "Sadece kartin icerigini degistir, yeni bilgi EKLEME. \n"
            "Ciktiyi su formatta ver:\n"
            "ON: duzeltilmis on yuz\n"
            "ARKA: duzeltilmis arka yuz\n"
            "IPUCU: duzeltilmis ipucu\n"
            "Asil kart:\nON: " + front_text + "\nARKA: " + back_text + "\nIPUCU: " + hint_text;

        std::string front_capture = front_text;
        std::string back_capture = back_text;
        std::string hint_capture = hint_text;

        async_call_ai(prompt, "", [front_buffer, back_buffer, hint_entry,
                                   front_capture, back_capture, hint_capture, ai_edit_btn](std::string raw) {
            std::string reply = extract_ai_reply(raw);

            std::string new_front, new_back, new_hint;
            auto extract = [](const std::string& text, const std::string& prefix) -> std::string {
                size_t pos = text.find(prefix);
                if (pos == std::string::npos) return "";
                pos += prefix.size();
                size_t end = text.find('\n', pos);
                if (end == std::string::npos) end = text.size();
                std::string val = text.substr(pos, end - pos);
                val.erase(0, val.find_first_not_of(" \t\r\n"));
                val.erase(val.find_last_not_of(" \t\r\n") + 1);
                return val;
            };

            new_front = extract(reply, "ON:");
            new_back = extract(reply, "ARKA:");
            new_hint = extract(reply, "IPUCU:");

            if (new_front.empty()) new_front = front_capture;
            if (new_back.empty()) new_back = back_capture;
            if (new_hint.empty()) new_hint = hint_capture;

            Glib::signal_idle().connect_once([front_buffer, back_buffer, hint_entry,
                                                new_front, new_back, new_hint, ai_edit_btn]() {
                front_buffer->set_text(new_front);
                back_buffer->set_text(new_back);
                hint_entry->set_text(new_hint);
                ai_edit_btn->set_label("\u2728 AI ile D\u00fczenle");
                ai_edit_btn->set_sensitive(true);
            });
        });

        ai_edit_btn->set_label("D\u00fczenleniyor...");
        ai_edit_btn->set_sensitive(false);
    });

    form->append(*front_lbl);
    form->append(*front_view);
    form->append(*back_lbl);
    form->append(*back_view);
    form->append(*hint_lbl);
    form->append(*hint_entry);
    form->append(*tags_lbl);
    form->append(*tags_entry);
    form->append(*ai_edit_btn);
    form->append(*btn_row);

    form_card->set_child(*form);
    outer->append(*form_card);

    flash_stack.add(*outer, "card_edit");
    flash_stack.set_visible_child(*outer);
}

void MainWindow::flash_show_ai_generate(const std::string& deck_id) {
    auto* old_ai = flash_stack.get_child_by_name("ai_generate");
    if (old_ai) flash_stack.remove(*old_ai);

    flash_back_btn.set_visible(true);
    flash_header_title.set_text("AI ile Kart \u00dcret");
    flash_current_deck_id = deck_id;

    auto* outer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    outer->set_halign(Gtk::Align::CENTER);
    outer->set_margin_top(8);

    auto* form_card = Gtk::make_managed<Gtk::Frame>();
    form_card->add_css_class("flash-edit-card");
    form_card->set_size_request(560, -1);

    auto* form = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 16);
    form->set_margin_start(24);
    form->set_margin_end(24);
    form->set_margin_top(24);
    form->set_margin_bottom(24);

    auto* title_lbl = Gtk::make_managed<Gtk::Label>(
        "Hangi konudan kart olu\u015fturmak istiyorsun?");
    title_lbl->add_css_class("flash-edit-label");
    title_lbl->set_halign(Gtk::Align::START);
    title_lbl->set_wrap(true);

    auto* hint_lbl = Gtk::make_managed<Gtk::Label>(
        "AI, verdi\u011fin konuya g\u00f6re 5-10 aras\u0131 flashcard olu\u015fturacak.\n"
        "\u0130stersen bir ders notunu da yap\u0131\u015ft\u0131rabilirsin.");
    hint_lbl->add_css_class("flash-edit-hint");
    hint_lbl->set_halign(Gtk::Align::START);
    hint_lbl->set_wrap(true);

    auto* source_entry = Gtk::make_managed<Gtk::Entry>();
    source_entry->add_css_class("flash-edit-entry");
    source_entry->set_placeholder_text("\u00d6rn: Matematik 5. \u00dcnite - \u00dc\u00e7genler");

    auto source_buffer = Gtk::TextBuffer::create();
    auto* source_view = Gtk::make_managed<Gtk::TextView>();
    source_view->set_buffer(source_buffer);
    source_view->add_css_class("flash-edit-textview");
    source_view->set_wrap_mode(Gtk::WrapMode::WORD);
    source_view->set_size_request(-1, 180);

    auto card_count_adj = Gtk::Adjustment::create(8, 3, 20, 1);
    auto* count_spin = Gtk::make_managed<Gtk::SpinButton>(card_count_adj);
    count_spin->add_css_class("flash-spin");
    auto* count_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    auto* count_lbl = Gtk::make_managed<Gtk::Label>("Kart say\u0131s\u0131:");
    count_lbl->add_css_class("flash-edit-label");
    count_box->append(*count_lbl);
    count_box->append(*count_spin);

    auto* use_note_lbl = Gtk::make_managed<Gtk::Label>("Veya mevcut bir notu kullan:");
    use_note_lbl->add_css_class("flash-edit-label");
    use_note_lbl->set_halign(Gtk::Align::START);
    use_note_lbl->set_margin_top(8);

    auto* note_combo = Gtk::make_managed<Gtk::ComboBoxText>();
    note_combo->append("", "Se\u00e7iniz...");
    for (auto& n : course_notes) {
        note_combo->append(n.course, n.course);
    }
    note_combo->set_active_id("");

    auto* generate_btn = Gtk::make_managed<Gtk::Button>("\u2728 Kartlar\u0131 Olu\u015ftur");
    generate_btn->add_css_class("flash-ai-generate-btn");
    generate_btn->set_size_request(-1, 48);

    auto* status_lbl = Gtk::make_managed<Gtk::Label>("");
    status_lbl->add_css_class("flash-ai-status");
    status_lbl->set_halign(Gtk::Align::CENTER);

    generate_btn->signal_clicked().connect(
        [this, deck_id, source_entry, source_buffer, count_spin, note_combo,
         generate_btn, status_lbl]() {

        std::string topic = source_entry->get_text();
        auto iter = source_buffer->begin();
        std::string pasted = source_buffer->get_text(iter, source_buffer->end());
        std::string selected_note = note_combo->get_active_id();
        int count = (int)count_spin->get_value();

        std::string source_text;
        if (!selected_note.empty()) {
            for (auto& n : course_notes) {
                if (n.course == selected_note) {
                    source_text = n.content;
                    break;
                }
            }
        } else if (!pasted.empty()) {
            source_text = pasted;
        } else if (!topic.empty()) {
            source_text = topic;
        } else {
            status_lbl->set_text("L\u00fctfen bir konu veya not girin.");
            return;
        }

        std::string prompt =
            "Sen bir e\u011fitim asistan\u0131s\u0131n. Verilen kaynaktan bilgi kartlar\u0131 olu\u015ftur.\n"
            "Kart say\u0131s\u0131: " + std::to_string(count) + "\n\n"
            "Kaynak:\n" + source_text + "\n\n"
            "Kurallar:\n"
            "- Her kart\u0131n \u00f6n y\u00fcz\u00fc soru/terim, arka y\u00fcz\u00fc cevap/tan\u0131m\n"
            "- En \u00f6nemli noktalar\u0131 se\u00e7\n"
            "- A\u00e7\u0131k, net, \u00f6\u011frenci dostu ifadeler\n"
            "- Sadece verilen kaynaktaki bilgileri kullan, uydurma\n\n"
            "KESIN CIKTI FORMATI (her kart i\u00e7in aynen bu \u015fekilde):\n\n"
            "[KART]\n"
            "ON: soru veya terim\n"
            "ARKA: cevap veya tan\u0131m\n"
            "IPUCU: k\u0131sa ipucu (varsa)\n"
            "[KART_BITTI]\n";

        status_lbl->set_text("AI yan\u0131tlan\u0131yor...");
        generate_btn->set_sensitive(false);

        async_call_ai(prompt, "", [this, prompt, deck_id, status_lbl, generate_btn](std::string raw) {
            std::string reply = extract_ai_reply(raw);

            std::regex pattern(R"(\[KART\]([\s\S]*?)\[KART_BITTI\])", std::regex::icase);
            std::smatch match;
            std::string remaining = reply;
            int added = 0;

            while (std::regex_search(remaining, match, pattern)) {
                std::string block = match[1].str();

                auto extract_field = [](const std::string& text, const std::string& prefix) -> std::string {
                    size_t start = text.find(prefix);
                    if (start == std::string::npos) return "";
                    start += prefix.size();
                    if (start >= text.size()) return "";
                    size_t end = text.find('\n', start);
                    if (end == std::string::npos) end = text.size();
                    std::string val = text.substr(start, end - start);
                    val.erase(0, val.find_first_not_of(" \t\r\n"));
                    val.erase(val.find_last_not_of(" \t\r\n") + 1);
                    return val;
                };

                std::string front = extract_field(block, "ON:");
                std::string back = extract_field(block, "ARKA:");
                std::string hint = extract_field(block, "IPUCU:");

                if (!front.empty() && !back.empty()) {
                    FlashCard card;
                    card.id = flash_generate_id();
                    card.deck_id = deck_id;
                    card.front = front;
                    card.back = back;
                    card.hint = hint;
                    card.next_review_at = std::time(nullptr);
                    flash_cards.push_back(card);
                    added++;
                }

                remaining = match.suffix().str();
            }

            save_data();

            Glib::signal_idle().connect_once([this, deck_id, status_lbl, generate_btn, added]() {
                if (added > 0) {
                    flash_refresh_deck_stats(deck_id);
                    status_lbl->set_text(
                        std::to_string(added) + " kart ba\u015far\u0131yla olu\u015fturuldu! \u2705");
                    Glib::signal_timeout().connect([this, deck_id]() {
                        flash_show_deck_detail(deck_id);
                        return false;
                    }, 1500);
                } else {
                    status_lbl->set_text("Kart olu\u015fturulamad\u0131. Tekrar dene.");
                }
                generate_btn->set_sensitive(true);
            });
        });
    });

    form->append(*title_lbl);
    form->append(*hint_lbl);
    form->append(*source_entry);
    form->append(*source_view);
    form->append(*count_box);
    form->append(*use_note_lbl);
    form->append(*note_combo);
    form->append(*generate_btn);
    form->append(*status_lbl);

    form_card->set_child(*form);
    outer->append(*form_card);

    flash_stack.add(*outer, "ai_generate");
    flash_stack.set_visible_child(*outer);
}

std::string MainWindow::flash_generate_id() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    static int counter = 0;
    counter++;
    return "fc_" + std::to_string(ms) + "_" + std::to_string(counter);
}

void MainWindow::flash_delete_deck(const std::string& deck_id) {
    flash_cards.erase(
        std::remove_if(flash_cards.begin(), flash_cards.end(),
            [&](const FlashCard& c) { return c.deck_id == deck_id; }),
        flash_cards.end());
    flash_decks.erase(
        std::remove_if(flash_decks.begin(), flash_decks.end(),
            [&](const FlashDeck& d) { return d.id == deck_id; }),
        flash_decks.end());
    save_data();
}

void MainWindow::flash_delete_card(const std::string& card_id) {
    flash_cards.erase(
        std::remove_if(flash_cards.begin(), flash_cards.end(),
            [&](const FlashCard& c) { return c.id == card_id; }),
        flash_cards.end());
    save_data();
}

void MainWindow::flash_export_anki(const std::string& deck_id) {
    std::string dir_path = Glib::get_home_dir() + "/.local/share/ogrenci-merkezi/anki_export";
    g_mkdir_with_parents(dir_path.c_str(), 0755);

    std::string deck_name = "";
    for (auto& d : flash_decks) {
        if (d.id == deck_id) { deck_name = d.title; break; }
    }
    if (deck_name.empty()) return;

    std::string slug;
    for (char c : deck_name) {
        if (isalnum(c) || c == '-' || c == '_') slug += c;
        else if (c == ' ') slug += '_';
    }
    if (slug.empty()) slug = "deck";

    std::string path = dir_path + "/" + slug + ".tsv";
    std::ofstream f(path);
    f << "front\tback\ttags\n";
    for (auto& c : flash_cards) {
        if (c.deck_id != deck_id) continue;
        std::string esc_front = c.front;
        std::string esc_back = c.back;
        std::string esc_tags = c.tags;
        size_t p;
        while ((p = esc_front.find('\t')) != std::string::npos) esc_front.replace(p, 1, " ");
        while ((p = esc_front.find('\n')) != std::string::npos) esc_front.replace(p, 1, " ");
        while ((p = esc_back.find('\t')) != std::string::npos) esc_back.replace(p, 1, " ");
        while ((p = esc_back.find('\n')) != std::string::npos) esc_back.replace(p, 1, " ");
        while ((p = esc_tags.find('\t')) != std::string::npos) esc_tags.replace(p, 1, " ");
        f << esc_front << "\t" << esc_back << "\t" << esc_tags << "\n";
    }
    f.close();

    flash_show_decks();
}

void MainWindow::flash_load_anki_exports() {
    std::string dir_path = Glib::get_home_dir() + "/.local/share/ogrenci-merkezi/anki_export";
    g_mkdir_with_parents(dir_path.c_str(), 0755);

    GDir* dir = g_dir_open(dir_path.c_str(), 0, nullptr);
    if (!dir) return;

    const char* fname;
    while ((fname = g_dir_read_name(dir)) != nullptr) {
        std::string name(fname);
        if (name.size() < 4 || name.substr(name.size() - 4) != ".tsv") continue;

        std::string full_path = dir_path + "/" + name;
        std::ifstream f(full_path);
        if (!f.is_open()) continue;

        std::string deck_name = name.substr(0, name.size() - 4);
        std::string deck_id = "anki-" + deck_name;

        bool deck_exists = false;
        for (auto& d : flash_decks) {
            if (d.id == deck_id || d.title == deck_name) { deck_exists = true; break; }
        }
        if (!deck_exists) {
            FlashDeck nd;
            nd.id = deck_id;
            nd.title = deck_name;
            nd.description = "Anki'den i\u00e7e aktar\u0131ld\u0131";
            nd.created_at = std::time(nullptr);
            flash_decks.push_back(nd);
        }

        std::string line;
        std::getline(f, line); // header
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string front, back, tags;
            std::getline(ss, front, '\t');
            std::getline(ss, back, '\t');
            std::getline(ss, tags, '\t');

            bool exists = false;
            for (auto& c : flash_cards) {
                if (c.deck_id == deck_id && c.front == front && c.back == back) {
                    exists = true; break;
                }
            }
            if (!exists) {
                FlashCard nc;
                nc.id = flash_generate_id();
                nc.deck_id = deck_id;
                nc.front = front;
                nc.back = back;
                nc.tags = tags;
                nc.next_review_at = std::time(nullptr);
                nc.ease_factor = 2.5;
                nc.repetitions = 0;
                nc.hint = "";
                flash_cards.push_back(nc);
            }
        }
    }
    g_dir_close(dir);
    save_data();
}

void MainWindow::process_ai_flashcards(const std::string& ai_text, const std::string& deck_id) {
    std::regex pattern(R"(\[KART\]([\s\S]*?)\[KART_BITTI\])", std::regex::icase);
    std::smatch match;
    std::string remaining = ai_text;
    int added = 0;

    while (std::regex_search(remaining, match, pattern)) {
        std::string block = match[1].str();

        auto extract = [](const std::string& text, const std::string& prefix) -> std::string {
            size_t start = text.find(prefix);
            if (start == std::string::npos) return "";
            start += prefix.size();
            size_t end = text.find('\n', start);
            if (end == std::string::npos) end = text.size();
            std::string val = text.substr(start, end - start);
            val.erase(0, val.find_first_not_of(" \t\r\n"));
            val.erase(val.find_last_not_of(" \t\r\n") + 1);
            return val;
        };

        std::string front = extract(block, "ON:");
        std::string back = extract(block, "ARKA:");
        std::string hint = extract(block, "IPUCU:");

        if (!front.empty() && !back.empty()) {
            FlashCard c;
            c.id = flash_generate_id();
            c.deck_id = deck_id;
            c.front = front;
            c.back = back;
            c.hint = hint;
            c.next_review_at = std::time(nullptr);
            flash_cards.push_back(c);
            added++;
        }

        remaining = match.suffix().str();
    }

    if (added > 0) {
        save_data();
        flash_refresh_deck_stats(deck_id);
    }
}

void MainWindow::update_pomo_display() {
    int m = pomo_seconds / 60;
    int s = pomo_seconds % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << m << ":" << std::setw(2) << std::setfill('0') << s;
    pomo_time.set_text(oss.str());
    pomo_progress.set_fraction(1.0 - (double)pomo_seconds / pomo_total);
}

void MainWindow::on_pomo_start() {
    if (pomo_running) {
        pomo_running = false;
        btn_pomo_start.set_label("Başlat");
        if (pomo_timer_conn) pomo_timer_conn.disconnect();
        spin_pomo_duration.set_sensitive(true);
        spin_pomo_break.set_sensitive(true);
        return;
    }
    pomo_running = true;
    btn_pomo_start.set_label("Duraklat");
    pomo_status.set_text(pomo_break ? "Mola" : "Odaklan");
    spin_pomo_duration.set_sensitive(false);
    spin_pomo_break.set_sensitive(false);

    pomo_timer_conn = Glib::signal_timeout().connect([this]() {
        if (!pomo_running) return false;
        pomo_seconds--;
        if (pomo_seconds <= 0) {
            if (!pomo_break) { pomodoro_completed++; pomodoro_minutes += pomo_duration_minutes; }
            pomo_break = !pomo_break;
            pomo_seconds = pomo_break ? pomo_break_minutes * 60 : pomo_duration_minutes * 60;
            pomo_total = pomo_seconds;
            pomo_status.set_text(pomo_break ? "Mola" : "Odaklan");
            save_data();
        }
        update_pomo_display();
        return true;
    }, 1000);
}

void MainWindow::on_pomo_reset() {
    pomo_running = false;
    if (pomo_timer_conn) pomo_timer_conn.disconnect();
    pomo_break = false;
    pomo_seconds = pomo_duration_minutes * 60;
    pomo_total = pomo_duration_minutes * 60;
    btn_pomo_start.set_label("Başlat");
    pomo_status.set_text("Hazır");
    spin_pomo_duration.set_sensitive(true);
    spin_pomo_break.set_sensitive(true);
    update_pomo_display();
}

void MainWindow::on_pomo_skip() {
    pomo_running = false;
    if (pomo_timer_conn) pomo_timer_conn.disconnect();
    pomo_break = !pomo_break;
    pomo_seconds = pomo_break ? pomo_break_minutes * 60 : pomo_duration_minutes * 60;
    pomo_total = pomo_seconds;
    btn_pomo_start.set_label("Başlat");
    pomo_status.set_text(pomo_break ? "Mola" : "Odaklan");
    spin_pomo_duration.set_sensitive(true);
    spin_pomo_break.set_sensitive(true);
    update_pomo_display();
}

void MainWindow::on_pomo_duration_changed() {
    int val = (int)spin_pomo_duration.get_value();
    if (val > 0) {
        pomo_duration_minutes = val;
        if (!pomo_running && !pomo_break) {
            pomo_seconds = val * 60;
            pomo_total = val * 60;
            update_pomo_display();
        }
    }
}

void MainWindow::on_pomo_break_changed() {
    int val = (int)spin_pomo_break.get_value();
    if (val > 0) {
        pomo_break_minutes = val;
    }
}

void MainWindow::setup_planner() {
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);

    auto* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    box->set_margin_start(16);
    box->set_margin_end(16);
    box->set_margin_top(16);
    box->set_margin_bottom(16);

    auto* title = Gtk::make_managed<Gtk::Label>("Görevler");
    title->add_css_class("page-title");
    title->set_halign(Gtk::Align::START);

    auto* form = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);
    form->add_css_class("task-form");

    auto* row1 = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    task_title_entry.set_placeholder_text("Görev / Ödev adı...");
    task_title_entry.set_hexpand(true);
    auto* lbl_date = Gtk::make_managed<Gtk::Label>("Tarih:");
    task_due_entry.set_placeholder_text("GG.AA.YYYY");
    task_due_entry.set_width_chars(12);
    row1->append(task_title_entry);
    row1->append(*lbl_date);
    row1->append(task_due_entry);

    auto* row2 = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    task_category.append("Genel");
    task_category.append("Ders Çalışma");
    task_category.append("Proje");
    task_category.append("Sınav");
    task_category.append("Ödev");
    task_category.set_active(0);

    task_priority.append("Düşük");
    task_priority.append("Orta");
    task_priority.append("Yüksek");
    task_priority.set_active(1);

    btn_add_task.add_css_class("action-btn");
    btn_add_task.set_hexpand(true);

    row2->append(task_category);
    row2->append(task_priority);
    row2->append(btn_add_task);

    form->append(*row1);
    form->append(*row2);

    btn_add_task.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_add_task));
    task_title_entry.signal_activate().connect(sigc::mem_fun(*this, &MainWindow::on_add_task));

    task_scrolled.set_child(task_list_box);
    task_scrolled.set_min_content_height(200);
    task_scrolled.set_vexpand(true);
    task_scrolled.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

    box->append(*title);
    box->append(*form);
    box->append(task_scrolled);
    box->append(task_stats);

    refresh_task_list();
    sw->set_child(*box);
    stack.add(*sw, "planner");
}

void MainWindow::on_add_task() {
    auto text = task_title_entry.get_text();
    if (text.empty()) return;
    tasks.push_back({text, task_category.get_active_text(), task_due_entry.get_text(), false, task_priority.get_active_row_number()});
    task_title_entry.set_text("");
    task_due_entry.set_text("");
    refresh_task_list();
    save_data();
}

void MainWindow::refresh_task_list() {
    while (Gtk::Widget* child = task_list_box.get_first_child()) task_list_box.remove(*child);

    int done = 0;
    for (size_t i = 0; i < tasks.size(); i++) {
        if (tasks[i].done) done++;
        auto* row = make_task_row(i);
        task_list_box.append(*row);
    }

    task_stats.set_markup(std::to_string(tasks.size()-done) + " bekliyor, <b>" + std::to_string(done) + "</b> tamamlandı");
}

Gtk::Box* MainWindow::make_task_row(size_t index) {
    auto& task = tasks[index];
    auto* row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);
    row->add_css_class("task-row");

    auto* check = Gtk::make_managed<Gtk::CheckButton>();
    check->set_active(task.done);
    check->signal_toggled().connect([this, index, check]() {
        if (index < tasks.size()) { tasks[index].done = check->get_active(); refresh_task_list(); save_data(); }
    });

    auto* info = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 2);
    info->set_hexpand(true);

    auto* title = Gtk::make_managed<Gtk::Label>();
    title->set_markup(task.done ? "<s>" + task.title + "</s>" : task.title);
    title->set_halign(Gtk::Align::START);
    title->set_wrap(true);

    std::string color = task.priority == 2 ? "#e74c3c" : task.priority == 1 ? "#f39c12" : "#2ecc71";
    std::string meta_text = "<small><span color='" + color + "'>●</span> " + task.category;
    if (!task.due_date.empty()) meta_text += " | " + task.due_date;
    meta_text += "</small>";
    auto* meta = Gtk::make_managed<Gtk::Label>(meta_text);
    meta->set_markup(meta_text);
    meta->set_halign(Gtk::Align::START);

    info->append(*title);
    info->append(*meta);

    auto* del = Gtk::make_managed<Gtk::Button>("Sil");
    del->add_css_class("delete-btn");
    del->signal_clicked().connect([this, index]() {
        if (index < tasks.size()) { tasks.erase(tasks.begin()+index); refresh_task_list(); save_data(); }
    });

    row->append(*check);
    row->append(*info);
    row->append(*del);
    return row;
}

void MainWindow::setup_notes() {
    auto* paned = Gtk::make_managed<Gtk::Paned>(Gtk::Orientation::HORIZONTAL);
    paned->set_position(200);
    paned->set_resize_start_child(false);

    auto* left_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);
    left_box->set_margin_start(12);
    left_box->set_margin_end(12);
    left_box->set_margin_top(12);
    left_box->set_margin_bottom(12);

    auto* title = Gtk::make_managed<Gtk::Label>("Dersler");
    title->add_css_class("page-title");
    title->set_halign(Gtk::Align::START);
    left_box->append(*title);

    course_list.set_selection_mode(Gtk::SelectionMode::SINGLE);
    course_list.signal_row_selected().connect(sigc::mem_fun(*this, &MainWindow::on_course_select));

    auto* course_sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    course_sw->set_child(course_list);
    course_sw->set_vexpand(true);
    left_box->append(*course_sw);

    auto* course_btn_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
    btn_add_course.add_css_class("action-btn");
    btn_add_course.set_hexpand(true);
    btn_add_course.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_add_course));
    course_btn_box->append(btn_add_course);

    btn_add_link_note.add_css_class("action-btn");
    btn_add_link_note.set_tooltip_text("Link notu ekle");
    btn_add_link_note.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_add_link_note));
    course_btn_box->append(btn_add_link_note);

    left_box->append(*course_btn_box);

    paned->set_start_child(*left_box);

    note_editor_box.set_margin_start(16);
    note_editor_box.set_margin_end(16);
    note_editor_box.set_margin_top(16);
    note_editor_box.set_margin_bottom(16);

    note_title.add_css_class("page-title");
    note_title.set_text("Bir ders seçin");
    note_editor_box.append(note_title);

    auto* sep = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::HORIZONTAL);
    sep->set_margin_top(4);
    sep->set_margin_bottom(8);
    note_editor_box.append(*sep);

    // Rich text tags
    auto nbuf = note_view.get_buffer();
    auto ttable = nbuf->get_tag_table();

    tag_bold = Gtk::TextTag::create("bold");
    tag_bold->property_weight() = Pango::Weight::BOLD;
    ttable->add(tag_bold);

    tag_italic = Gtk::TextTag::create("italic");
    tag_italic->property_style() = Pango::Style::ITALIC;
    ttable->add(tag_italic);

    tag_underline = Gtk::TextTag::create("underline");
    tag_underline->property_underline() = Pango::Underline::SINGLE;
    ttable->add(tag_underline);

    tag_strikethrough = Gtk::TextTag::create("strikethrough");
    tag_strikethrough->property_strikethrough() = true;
    ttable->add(tag_strikethrough);

    tag_h1 = Gtk::TextTag::create("h1");
    tag_h1->property_scale() = 2.0;
    tag_h1->property_weight() = Pango::Weight::BOLD;
    ttable->add(tag_h1);

    tag_h2 = Gtk::TextTag::create("h2");
    tag_h2->property_scale() = 1.5;
    tag_h2->property_weight() = Pango::Weight::BOLD;
    ttable->add(tag_h2);

    tag_h3 = Gtk::TextTag::create("h3");
    tag_h3->property_scale() = 1.2;
    tag_h3->property_weight() = Pango::Weight::BOLD;
    ttable->add(tag_h3);

    tag_font_inc = make_font_tag("font_inc", 1.3);
    tag_font_dec = make_font_tag("font_dec", 0.8);

    // Format toolbar — tüm butonlar tek bir cam çerçeve içinde
    format_toolbar.set_halign(Gtk::Align::START);
    format_toolbar.add_css_class("note-toolbar-glass");

    auto make_tool_toggle = [this](Gtk::ToggleButton& btn, const std::string& markup, const std::string& tip) {
        auto* lbl = Gtk::make_managed<Gtk::Label>();
        lbl->set_markup(markup);
        btn.set_child(*lbl);
        btn.add_css_class("note-tool-btn");
        btn.set_tooltip_text(tip);
        btn.set_has_frame(false);
        format_toolbar.append(btn);
    };

    auto make_tool_btn = [this](Gtk::Button& btn, const std::string& markup, const std::string& tip) {
        auto* lbl = Gtk::make_managed<Gtk::Label>();
        lbl->set_markup(markup);
        btn.set_child(*lbl);
        btn.add_css_class("note-tool-btn");
        btn.set_tooltip_text(tip);
        btn.set_has_frame(false);
        format_toolbar.append(btn);
    };

    make_tool_toggle(btn_bold, "<b>B</b>", "Kalın (Ctrl+B)");
    make_tool_toggle(btn_italic, "<i>İ</i>", "İtalik (Ctrl+I)");
    make_tool_toggle(btn_underline, "<u>U</u>", "Altı Çizili (Ctrl+U)");
    make_tool_toggle(btn_strikethrough, "<s>S</s>", "Üstü Çizili (Ctrl+S)");

    auto* sep1 = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::VERTICAL);
    sep1->add_css_class("note-tool-sep");
    format_toolbar.append(*sep1);

    make_tool_toggle(btn_h1, "<span size='x-large' weight='bold'>H1</span>", "Başlık 1");
    make_tool_toggle(btn_h2, "<span size='large' weight='bold'>H2</span>", "Başlık 2");
    make_tool_toggle(btn_h3, "<span size='medium' weight='bold'>H3</span>", "Başlık 3");

    auto* sep2 = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::VERTICAL);
    sep2->add_css_class("note-tool-sep");
    format_toolbar.append(*sep2);

    make_tool_btn(btn_font_dec_w, "<span size='small'>A⁻</span>", "Font küçült");
    make_tool_btn(btn_font_inc_w, "<span size='x-large'>A⁺</span>", "Font büyüt");

    note_editor_box.append(format_toolbar);

    btn_bold.signal_toggled().connect([this]() { apply_tag_to_selection(btn_bold, tag_bold); });
    btn_italic.signal_toggled().connect([this]() { apply_tag_to_selection(btn_italic, tag_italic); });
    btn_underline.signal_toggled().connect([this]() { apply_tag_to_selection(btn_underline, tag_underline); });
    btn_strikethrough.signal_toggled().connect([this]() { apply_tag_to_selection(btn_strikethrough, tag_strikethrough); });
    btn_h1.signal_toggled().connect([this]() { apply_tag_to_selection(btn_h1, tag_h1); });
    btn_h2.signal_toggled().connect([this]() { apply_tag_to_selection(btn_h2, tag_h2); });
    btn_h3.signal_toggled().connect([this]() { apply_tag_to_selection(btn_h3, tag_h3); });

    btn_font_inc_w.signal_clicked().connect([this]() {
        auto buf = note_view.get_buffer();
        Gtk::TextIter start, end;
        if (buf->get_selection_bounds(start, end))
            buf->apply_tag(tag_font_inc, start, end);
        else
            buf->apply_tag(tag_font_inc, buf->get_insert()->get_iter(), buf->get_insert()->get_iter());
    });
    btn_font_dec_w.signal_clicked().connect([this]() {
        auto buf = note_view.get_buffer();
        Gtk::TextIter start, end;
        if (buf->get_selection_bounds(start, end))
            buf->apply_tag(tag_font_dec, start, end);
        else
            buf->apply_tag(tag_font_dec, buf->get_insert()->get_iter(), buf->get_insert()->get_iter());
    });

    // Apply active formatting to newly inserted text
    nbuf->signal_insert().connect([this](const Gtk::TextIter& pos, const Glib::ustring&, int length) {
        if (length <= 0 || note_loading || selected_note_index < 0) return;
        // signal_insert fires AFTER insertion; pos points AFTER the inserted text
        int soff = pos.get_offset() - length;
        if (soff < 0) return;
        auto buf = note_view.get_buffer();
        auto s = buf->get_iter_at_offset(soff);
        auto e = buf->get_iter_at_offset(soff + length);
        if (btn_bold.get_active() && tag_bold) buf->apply_tag(tag_bold, s, e);
        if (btn_italic.get_active() && tag_italic) buf->apply_tag(tag_italic, s, e);
        if (btn_underline.get_active() && tag_underline) buf->apply_tag(tag_underline, s, e);
        if (btn_strikethrough.get_active() && tag_strikethrough) buf->apply_tag(tag_strikethrough, s, e);
        if (btn_h1.get_active() && tag_h1) buf->apply_tag(tag_h1, s, e);
        if (btn_h2.get_active() && tag_h2) buf->apply_tag(tag_h2, s, e);
        if (btn_h3.get_active() && tag_h3) buf->apply_tag(tag_h3, s, e);
    });

    auto* note_sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    note_sw->set_vexpand(true);
    note_sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

    note_view.set_wrap_mode(Gtk::WrapMode::WORD);
    note_view.set_left_margin(40);
    note_view.set_right_margin(40);
    note_view.set_top_margin(30);
    note_view.set_bottom_margin(30);
    note_view.set_editable(true);
    note_view.set_cursor_visible(true);

    auto key_controller = Gtk::EventControllerKey::create();
    key_controller->signal_key_pressed().connect(
        [this](guint keyval, guint, Gdk::ModifierType state) -> bool {
            if ((state & Gdk::ModifierType::CONTROL_MASK) == Gdk::ModifierType())
                return false;
            switch (gdk_keyval_to_upper(keyval)) {
            case GDK_KEY_B:
                btn_bold.set_active(!btn_bold.get_active());
                return true;
            case GDK_KEY_I:
                btn_italic.set_active(!btn_italic.get_active());
                return true;
            case GDK_KEY_U:
                btn_underline.set_active(!btn_underline.get_active());
                return true;
            case GDK_KEY_S:
                btn_strikethrough.set_active(!btn_strikethrough.get_active());
                return true;
            default:
                return false;
            }
        },
        false
    );
    note_view.add_controller(key_controller);

    auto note_provider = Gtk::CssProvider::create();
    note_provider->load_from_data(
        "textview { background: #ffffff; color: #222222; }"
        "textview text { background: #ffffff; color: #222222; }"
        "textview > border { background: #ffffff; }"
    );
    auto note_display = Gdk::Display::get_default();
    Gtk::StyleContext::add_provider_for_display(note_display, note_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 100);

    note_sw->set_child(note_view);
    note_editor_box.append(*note_sw);

    note_stats_label = Gtk::make_managed<Gtk::Label>("");
    note_stats_label->set_halign(Gtk::Align::END);
    note_stats_label->set_margin_top(4);
    note_stats_label->set_margin_bottom(2);
    note_stats_label->set_margin_end(4);
    note_stats_label->add_css_class("note-stats");
    note_editor_box.append(*note_stats_label);

    auto update_stats = [this]() {
        if (!note_stats_label) return;
        auto buf = note_view.get_buffer();
        if (!buf || selected_note_index < 0) {
            note_stats_label->set_text("");
            return;
        }
        auto text = buf->get_text(false);
        int chars = text.size();
        int words = 0;
        bool in_word = false;
        for (char c : text) {
            if (c == ' ' || c == '\n' || c == '\t') { in_word = false; }
            else if (!in_word) { words++; in_word = true; }
        }
        note_stats_label->set_text(
            std::to_string(words) + " kelime · " + std::to_string(chars) + " karakter");
    };
    note_view.get_buffer()->signal_changed().connect(update_stats);

    auto* btn_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    btn_box->set_margin_top(8);

    btn_save_note.add_css_class("action-btn");
    btn_save_note.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_save_note));

    btn_share_note.add_css_class("action-btn");
    share_icon_ptr = Gtk::make_managed<Gtk::Image>();
    share_icon_ptr->set_from_resource(dark_mode
        ? "/org/ogrenci/merkezi/assets/share.svg"
        : "/org/ogrenci/merkezi/assets/share_dark.svg");
    share_icon_ptr->set_pixel_size(18);
    btn_share_note.set_child(*share_icon_ptr);
    btn_share_note.set_tooltip_text("Notu paylaş");
    btn_share_note.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_share_note));

    btn_delete_course.add_css_class("delete-btn");
    btn_delete_course.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_delete_course));

    btn_box->append(btn_save_note);
    btn_box->append(btn_share_note);
    btn_box->append(btn_delete_course);
    note_editor_box.append(*btn_box);

    auto* right_sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    right_sw->set_child(note_editor_box);
    right_sw->set_vexpand(true);

    paned->set_end_child(*right_sw);

    refresh_course_list();
    stack.add(*paned, "notes");
}

void MainWindow::refresh_course_list() {
    while (Gtk::Widget* child = course_list.get_first_child()) course_list.remove(*child);

    for (size_t i = 0; i < course_notes.size(); i++) {
        auto* row = Gtk::make_managed<Gtk::ListBoxRow>();
        row->add_css_class("course-row");
        auto* label = Gtk::make_managed<Gtk::Label>(course_notes[i].course);
        label->set_halign(Gtk::Align::START);
        label->set_margin_start(10);
        label->set_margin_top(6);
        label->set_margin_bottom(6);
        row->set_child(*label);
        course_list.append(*row);
    }
}

void MainWindow::on_course_select(Gtk::ListBoxRow* row) {
    if (!row) return;
    int idx = row->get_index();
    if (idx >= 0 && idx < (int)course_notes.size()) {
        selected_note_index = idx;
        note_title.set_text(course_notes[idx].course);
        note_loading = true;
        note_view.get_buffer()->set_text(course_notes[idx].content);
        note_loading = false;
        load_note_format(idx);
        update_format_buttons();
    }
}

void MainWindow::on_add_course() {
    auto dialog = Gtk::make_managed<Gtk::Dialog>();
    dialog->set_title("Yeni Ders");
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->set_default_size(300, 100);

    dialog->add_button("İptal", Gtk::ResponseType::CANCEL);
    dialog->add_button("Ekle", Gtk::ResponseType::OK);

    auto* content = dialog->get_content_area();
    content->set_margin_start(16);
    content->set_margin_end(16);
    content->set_margin_top(16);
    content->set_margin_bottom(16);

    auto* entry = Gtk::make_managed<Gtk::Entry>();
    entry->set_placeholder_text("Ders adı (örn: Matematik)");
    entry->set_hexpand(true);
    content->append(*entry);

    dialog->signal_response().connect([this, dialog, entry](int response_id) {
        if (response_id == Gtk::ResponseType::OK) {
            auto name = entry->get_text();
            if (!name.empty()) {
                course_notes.push_back({name, ""});
                refresh_course_list();
                save_data();
            }
        }
        dialog->hide();
    });

    dialog->show();
}

static std::string json_escape(const std::string& s) {
    std::string out;
    for (char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\t': out += "\\t"; break;
            case '\r': out += "\\r"; break;
            default: out += c;
        }
    }
    return out;
}

static size_t share_WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    s->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void MainWindow::on_add_link_note() {
    auto dialog = Gtk::make_managed<Gtk::Dialog>();
    dialog->set_title("Link Notu Ekle");
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->set_default_size(400, 200);

    dialog->add_button("İptal", Gtk::ResponseType::CANCEL);
    dialog->add_button("Ekle", Gtk::ResponseType::OK);

    auto* content = dialog->get_content_area();
    content->set_margin_start(16);
    content->set_margin_end(16);
    content->set_margin_top(16);
    content->set_margin_bottom(16);
    content->set_spacing(12);

    auto* title_entry = Gtk::make_managed<Gtk::Entry>();
    title_entry->set_placeholder_text("Not başlığı");
    title_entry->set_hexpand(true);
    content->append(*title_entry);

    auto* link_entry = Gtk::make_managed<Gtk::Entry>();
    link_entry->set_placeholder_text("Link (URL)");
    link_entry->set_hexpand(true);
    content->append(*link_entry);

    auto* status_label = Gtk::make_managed<Gtk::Label>("");
    status_label->set_halign(Gtk::Align::START);
    status_label->set_visible(false);
    content->append(*status_label);

    dialog->signal_response().connect([this, dialog, title_entry, link_entry, status_label](int response_id) {
        if (response_id == Gtk::ResponseType::OK) {
            auto title = title_entry->get_text();
            auto link = link_entry->get_text();
            if (!title.empty() && !link.empty()) {
                status_label->set_text("Sunucudan getiriliyor...");
                status_label->set_visible(true);

                dialog->set_sensitive(false);

                std::thread([this, dialog, title, link]() {
                    CURL* curl = curl_easy_init();
                    std::string response;

                    if (curl) {
                        curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
                        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
                        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, share_WriteCallback);
                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
                        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
                        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

                        CURLcode res = curl_easy_perform(curl);
                        curl_easy_cleanup(curl);

                        if (res != CURLE_OK) {
                            response = "ERROR: " + std::string(curl_easy_strerror(res));
                        }
                    } else {
                        response = "ERROR: Failed to initialize curl";
                    }

                    Glib::signal_idle().connect_once([this, dialog, title, link, response]() {
                        dialog->set_sensitive(true);

                        if (response.rfind("ERROR:", 0) == 0) {
                            auto* err_dialog = Gtk::make_managed<Gtk::MessageDialog>(*this,
                                "Hata", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK);
                            err_dialog->set_secondary_text("Not getirilemedi:\n" + response.substr(6));
                            err_dialog->signal_response().connect([err_dialog](int) { err_dialog->hide(); });
                            err_dialog->show();
                            return;
                        }

                        std::string saved_content;
                        try {
                            auto j = json::parse(response);
                            saved_content = j.value("content", "");
                        } catch (const json::parse_error&) {}

                        if (saved_content.empty()) {
                            auto* err_dialog = Gtk::make_managed<Gtk::MessageDialog>(*this,
                                "Hata", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK);
                            err_dialog->set_secondary_text("Geçersiz yanıt: content bulunamadı.");
                            err_dialog->signal_response().connect([err_dialog](int) { err_dialog->hide(); });
                            err_dialog->show();
                            return;
                        }

                        course_notes.push_back({title, saved_content});
                        refresh_course_list();
                        save_data();

                        int new_idx = (int)course_notes.size() - 1;
                        selected_note_index = new_idx;
                        note_title.set_text(course_notes[new_idx].course);
                        note_view.get_buffer()->set_text(course_notes[new_idx].content);
                        auto* last_row = course_list.get_row_at_index(new_idx);
                        if (last_row) course_list.select_row(*last_row);

                        dialog->hide();
                    });
                }).detach();
                return;
            }
        }
        dialog->hide();
    });

    dialog->show();
}

void MainWindow::on_save_note() {
    if (selected_note_index >= 0 && selected_note_index < (int)course_notes.size()) {
        auto buf = note_view.get_buffer();
        Gtk::TextIter start, end;
        buf->get_bounds(start, end);
        course_notes[selected_note_index].content = buf->get_text(start, end, false);
        save_note_format(selected_note_index);
        save_data();
    }
}

void MainWindow::on_delete_course() {
    if (selected_note_index >= 0 && selected_note_index < (int)course_notes.size()) {
        if (selected_note_index < (int)note_formats.size())
            note_formats.erase(note_formats.begin() + selected_note_index);
        course_notes.erase(course_notes.begin() + selected_note_index);
        selected_note_index = -1;
        note_title.set_text("Bir ders seçin");
        note_view.get_buffer()->set_text("");
        refresh_course_list();
        save_data();
    }
}

Glib::RefPtr<Gtk::TextTag> MainWindow::make_font_tag(const std::string& name, double scale) {
    auto tag = Gtk::TextTag::create(name);
    tag->property_scale_set() = true;
    tag->property_scale() = scale;
    note_view.get_buffer()->get_tag_table()->add(tag);
    return tag;
}

void MainWindow::apply_tag_to_selection(Gtk::ToggleButton& btn, const Glib::RefPtr<Gtk::TextTag>& tag) {
    auto buf = note_view.get_buffer();
    Gtk::TextIter start, end;
    if (buf->get_selection_bounds(start, end)) {
        if (btn.get_active())
            buf->apply_tag(tag, start, end);
        else
            buf->remove_tag(tag, start, end);
    } else {
        auto iter = buf->get_insert()->get_iter();
        if (btn.get_active())
            buf->apply_tag(tag, iter, iter);
        else
            buf->remove_tag(tag, iter, iter);
    }
}

void MainWindow::update_format_buttons() {
    auto buf = note_view.get_buffer();
    if (!buf) return;
    auto iter = buf->get_insert()->get_iter();

    auto check = [&](const Glib::RefPtr<Gtk::TextTag>& tag) -> bool {
        if (!tag) return false;
        Gtk::TextIter s, e;
        if (buf->get_selection_bounds(s, e))
            return s.has_tag(tag) || e.has_tag(tag);
        return iter.has_tag(tag);
    };

    btn_bold.set_active(check(tag_bold));
    btn_italic.set_active(check(tag_italic));
    btn_underline.set_active(check(tag_underline));
    btn_strikethrough.set_active(check(tag_strikethrough));
    btn_h1.set_active(check(tag_h1));
    btn_h2.set_active(check(tag_h2));
    btn_h3.set_active(check(tag_h3));
}

void MainWindow::save_note_format(size_t index) {
    if (index >= note_formats.size())
        note_formats.resize(index + 1, "");

    auto buf = note_view.get_buffer();
    Gtk::TextIter start, end;
    buf->get_bounds(start, end);
    int len = end.get_offset();

    json j = json::array();
    auto record_tag = [&](const Glib::RefPtr<Gtk::TextTag>& tag, const std::string& tname) {
        int rs = -1;
        for (int i = 0; i <= len; i++) {
            auto it = (i < len) ? buf->get_iter_at_offset(i) : end;
            bool has = it.has_tag(tag);
            if (has && rs < 0) rs = i;
            else if (!has && rs >= 0) {
                json r;
                r["s"] = rs; r["e"] = i; r["t"] = tname;
                j.push_back(r);
                rs = -1;
            }
        }
    };

    record_tag(tag_bold, "b");
    record_tag(tag_italic, "i");
    record_tag(tag_underline, "u");
    record_tag(tag_strikethrough, "s");
    record_tag(tag_h1, "h1");
    record_tag(tag_h2, "h2");
    record_tag(tag_h3, "h3");
    record_tag(tag_font_inc, "fi");
    record_tag(tag_font_dec, "fd");

    note_formats[index] = j.dump();
}

void MainWindow::load_note_format(size_t index) {
    auto buf = note_view.get_buffer();
    if (index >= note_formats.size() || note_formats[index].empty())
        return;

    try {
        auto j = json::parse(note_formats[index]);
        auto tag_for_name = [this](const std::string& t) -> Glib::RefPtr<Gtk::TextTag> {
            if (t == "b") return tag_bold;
            if (t == "i") return tag_italic;
            if (t == "u") return tag_underline;
            if (t == "s") return tag_strikethrough;
            if (t == "h1") return tag_h1;
            if (t == "h2") return tag_h2;
            if (t == "h3") return tag_h3;
            if (t == "fi") return tag_font_inc;
            if (t == "fd") return tag_font_dec;
            return {};
        };
        for (auto& r : j) {
            int s = r["s"].get<int>();
            int e = r["e"].get<int>();
            auto tag = tag_for_name(r["t"].get<std::string>());
            if (tag) {
                auto sit = buf->get_iter_at_offset(s);
                auto eit = buf->get_iter_at_offset(e);
                buf->apply_tag(tag, sit, eit);
            }
        }
    } catch (const json::parse_error&) {}
}

void MainWindow::on_share_note() {
    if (selected_note_index < 0 || selected_note_index >= (int)course_notes.size()) {
        auto msg = Gtk::make_managed<Gtk::MessageDialog>(*this, "Uyarı", false, Gtk::MessageType::INFO, Gtk::ButtonsType::OK);
        msg->set_secondary_text("Lütfen önce bir not seçin.");
        msg->signal_response().connect([msg](int) { msg->hide(); });
        msg->show();
        return;
    }

    auto buf = note_view.get_buffer();
    Gtk::TextIter start, end;
    buf->get_bounds(start, end);
    std::string content = buf->get_text(start, end, false);
    if (content.empty()) {
        auto msg = Gtk::make_managed<Gtk::MessageDialog>(*this, "Uyarı", false, Gtk::MessageType::INFO, Gtk::ButtonsType::OK);
        msg->set_secondary_text("Not içeriği boş, paylaşılacak bir şey yok.");
        msg->signal_response().connect([msg](int) { msg->hide(); });
        msg->show();
        return;
    }

    auto* loading = Gtk::make_managed<Gtk::MessageDialog>(*this, "Paylaşılıyor...", false, Gtk::MessageType::INFO, Gtk::ButtonsType::NONE);
    loading->show();

    std::thread([this, content, loading]() {
        CURL* curl = curl_easy_init();
        std::string response;

        if (curl) {
            std::string note_title_text = course_notes[selected_note_index].course;
            std::string json = "{\"title\":\"" + json_escape(note_title_text) + "\",\"content\":\"" + json_escape(content) + "\"}";

            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");

            curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.1.214:8000/notes");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, share_WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

            CURLcode res = curl_easy_perform(curl);

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);

            if (res != CURLE_OK) {
                response = "ERROR: " + std::string(curl_easy_strerror(res));
            }
        } else {
            response = "ERROR: Failed to initialize curl";
        }

        Glib::signal_idle().connect_once([this, response, loading]() {
            loading->hide();

            if (response.rfind("ERROR:", 0) == 0) {
                auto err_msg = Gtk::make_managed<Gtk::MessageDialog>(*this, "Hata", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK);
                err_msg->set_secondary_text(response.substr(6));
                err_msg->signal_response().connect([err_msg](int) { err_msg->hide(); });
                err_msg->show();
                return;
            }

            std::string api_link, web_link;
            try {
                auto j = json::parse(response);
                api_link = j["data"].value("link", "");
                web_link = j["data"].value("web_viewer_link", "");
            } catch (const json::parse_error&) {}

            if (api_link.empty()) api_link = "http://192.168.1.214:8000";
            if (web_link.empty()) web_link = api_link;

            auto* result_dialog = Gtk::make_managed<Gtk::Dialog>();
            result_dialog->set_title("Not Paylaşıldı");
            result_dialog->set_transient_for(*this);
            result_dialog->set_modal(true);
            result_dialog->set_default_size(480, 260);

            result_dialog->add_button("Kapat", Gtk::ResponseType::CLOSE);

            auto* vbox = result_dialog->get_content_area();
            vbox->set_margin_start(16);
            vbox->set_margin_end(16);
            vbox->set_margin_top(16);
            vbox->set_margin_bottom(16);
            vbox->set_spacing(12);

            auto* lbl = Gtk::make_managed<Gtk::Label>();
            lbl->set_markup("<b>Notunuz paylaşıldı! Aşağıdaki linklerden kopyalayabilirsiniz:</b>");
            lbl->set_wrap(true);
            vbox->append(*lbl);

            auto make_link_row = [vbox](const std::string& label, const std::string& url) {
                auto* row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);

                auto* lbl = Gtk::make_managed<Gtk::Label>();
                lbl->set_markup("<b>" + label + ":</b>");
                lbl->set_size_request(20, -1);
                row->append(*lbl);

                auto* entry = Gtk::make_managed<Gtk::Entry>();
                entry->set_text(url);
                entry->set_hexpand(true);
                entry->set_editable(false);
                row->append(*entry);

                auto* btn = Gtk::make_managed<Gtk::Button>("Kopyala");
                btn->add_css_class("action-btn");
                btn->signal_clicked().connect([entry]() {
                    auto display = Gdk::Display::get_default();
                    if (display) {
                        auto clipboard = display->get_clipboard();
                        clipboard->set_text(entry->get_text());
                    }
                });
                row->append(*btn);

                vbox->append(*row);
            };

            make_link_row("API", api_link);
            make_link_row("Web", web_link);

            {
                auto* qr = QRcode_encodeString(web_link.c_str(), 0, QR_ECLEVEL_M, QR_MODE_8, 1);
                if (qr) {
                    int border = 2;
                    int dim = qr->width + 2 * border;
                    int pixel_size = 4;
                    int img_size = dim * pixel_size;

                    auto* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, img_size, img_size);
                    gdk_pixbuf_fill(pixbuf, 0xffffffff);

                    int stride = gdk_pixbuf_get_rowstride(pixbuf);
                    auto* pixels = gdk_pixbuf_get_pixels(pixbuf);

                    for (int y = 0; y < qr->width; y++) {
                        for (int x = 0; x < qr->width; x++) {
                            if (qr->data[y * qr->width + x] & 0x01) {
                                for (int py = 0; py < pixel_size; py++) {
                                    for (int px = 0; px < pixel_size; px++) {
                                        int ix = (x + border) * pixel_size + px;
                                        int iy = (y + border) * pixel_size + py;
                                        auto* p = pixels + iy * stride + ix * 3;
                                        p[0] = 0; p[1] = 0; p[2] = 0;
                                    }
                                }
                            }
                        }
                    }

                    auto ref = Glib::wrap(pixbuf, false);
                    QRcode_free(qr);

                    auto* qr_pic = Gtk::make_managed<Gtk::Picture>();
                    qr_pic->set_pixbuf(ref);
                    qr_pic->set_size_request(img_size, img_size);
                    auto* qr_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
                    qr_box->set_halign(Gtk::Align::CENTER);
                    qr_box->set_margin_top(8);
                    qr_box->append(*qr_pic);
                    vbox->append(*qr_box);
                }
            }

            result_dialog->signal_response().connect([result_dialog](int) {
                result_dialog->hide();
            });

            result_dialog->show();
        });
    }).detach();
}

void MainWindow::setup_schedule() {
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);

    auto* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    box->set_margin_start(16);
    box->set_margin_end(16);
    box->set_margin_top(16);
    box->set_margin_bottom(16);

    auto* title = Gtk::make_managed<Gtk::Label>("Ders Programı");
    title->add_css_class("page-title");
    title->set_halign(Gtk::Align::START);

    build_schedule_grid();

    schedule_grid.set_row_spacing(2);
    schedule_grid.set_column_spacing(2);
    schedule_grid.set_margin_top(8);

    btn_add_schedule.add_css_class("action-btn");
    btn_add_schedule.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_add_schedule));

    btn_clear_schedule.add_css_class("delete-btn");
    btn_clear_schedule.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_clear_schedule));

    auto* btn_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    btn_box->append(btn_add_schedule);
    btn_box->append(btn_clear_schedule);

    box->append(*title);
    box->append(schedule_grid);
    box->append(*btn_box);

    sw->set_child(*box);
    stack.add(*sw, "schedule");
}

void MainWindow::build_schedule_grid() {
    while (Gtk::Widget* child = schedule_grid.get_first_child()) schedule_grid.remove(*child);

    std::vector<std::string> days = {"Pazartesi", "Salı", "Çarşamba", "Perşembe", "Cuma"};

    auto* corner = Gtk::make_managed<Gtk::Label>("");
    corner->set_markup("<b>\\ Gün</b>");
    corner->set_halign(Gtk::Align::CENTER);
    corner->set_valign(Gtk::Align::CENTER);
    corner->set_margin_start(4);
    corner->set_margin_end(4);
    corner->add_css_class("schedule-corner");
    schedule_grid.attach(*corner, 0, 0, 1, 1);

    for (size_t c = 0; c < days.size(); c++) {
        auto* lbl = Gtk::make_managed<Gtk::Label>(days[c]);
        lbl->set_markup("<b>" + days[c] + "</b>");
        lbl->set_halign(Gtk::Align::CENTER);
        lbl->set_margin_start(4);
        lbl->set_margin_end(4);
        lbl->add_css_class("schedule-header");
        schedule_grid.attach(*lbl, c + 1, 0, 1, 1);
    }

    for (size_t r = 0; r < schedule_times.size(); r++) {
        auto* time_btn = Gtk::make_managed<Gtk::Button>(schedule_times[r]);
        time_btn->set_halign(Gtk::Align::CENTER);
        time_btn->set_valign(Gtk::Align::CENTER);
        time_btn->set_margin_start(4);
        time_btn->set_margin_end(4);
        time_btn->add_css_class("schedule-time-btn");

        size_t row_idx = r;
        time_btn->signal_clicked().connect([this, row_idx]() {
            on_time_clicked(row_idx);
        });

        schedule_grid.attach(*time_btn, 0, r + 1, 1, 1);

        for (size_t c = 0; c < days.size(); c++) {
            std::string entry_text = "";
            std::string location_text = "";
            for (auto& s : schedule) {
                if (s.day == days[c] && s.time == schedule_times[r]) {
                    entry_text = s.course;
                    location_text = s.location;
                    break;
                }
            }

            auto* cell = Gtk::make_managed<Gtk::Button>(entry_text.empty() ? "+" : entry_text);
            cell->set_hexpand(true);
            cell->set_halign(Gtk::Align::FILL);
            cell->set_valign(Gtk::Align::FILL);
            cell->set_margin_start(2);
            cell->set_margin_end(2);
            cell->add_css_class(entry_text.empty() ? "schedule-cell-empty" : "schedule-cell");

            if (!entry_text.empty() && !location_text.empty()) {
                cell->set_tooltip_text(location_text);
            }

            size_t day_idx = c;
            size_t time_idx = r;
            cell->signal_clicked().connect([this, days, day_idx, time_idx, entry_text, location_text]() {
                on_schedule_cell_clicked(days[day_idx], schedule_times[time_idx], entry_text, location_text);
            });

            schedule_grid.attach(*cell, c + 1, r + 1, 1, 1);
        }
    }
}

void MainWindow::on_time_clicked(size_t row_idx) {
    auto dialog = Gtk::make_managed<Gtk::Dialog>();
    dialog->set_title("Saat Düzenle");
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->set_default_size(250, 100);

    dialog->add_button("İptal", Gtk::ResponseType::CANCEL);
    dialog->add_button("Kaydet", Gtk::ResponseType::OK);

    auto* content = dialog->get_content_area();
    content->set_margin_start(16);
    content->set_margin_end(16);
    content->set_margin_top(16);
    content->set_margin_bottom(16);

    auto* entry = Gtk::make_managed<Gtk::Entry>();
    entry->set_placeholder_text("GG:DD (örn: 09:00)");
    entry->set_text(schedule_times[row_idx]);
    entry->set_hexpand(true);
    content->append(*entry);

    dialog->signal_response().connect([this, dialog, row_idx, entry](int response_id) {
        if (response_id == Gtk::ResponseType::OK) {
            auto new_time = entry->get_text();
            if (!new_time.empty()) {
                std::string old_time = schedule_times[row_idx];
                for (auto& s : schedule) {
                    if (s.time == old_time) s.time = new_time;
                }
                schedule_times[row_idx] = new_time;
                save_data();
                build_schedule_grid();
            }
        }
        dialog->hide();
    });

    dialog->show();
}

void MainWindow::on_schedule_cell_clicked(const std::string& day, const std::string& time, const std::string& current_course, const std::string& current_location) {
    auto dialog = Gtk::make_managed<Gtk::Dialog>();
    dialog->set_title("Ders Düzenle");
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->set_default_size(320, 180);

    dialog->add_button("Sil", -100);
    dialog->add_button("İptal", Gtk::ResponseType::CANCEL);
    dialog->add_button("Kaydet", Gtk::ResponseType::OK);

    auto* content = dialog->get_content_area();
    content->set_margin_start(16);
    content->set_margin_end(16);
    content->set_margin_top(16);
    content->set_margin_bottom(16);

    auto* info = Gtk::make_managed<Gtk::Label>(day + " - " + time);
    info->set_markup("<b>" + day + " | " + time + "</b>");
    info->set_halign(Gtk::Align::START);
    content->append(*info);

    auto* entry_course = Gtk::make_managed<Gtk::Entry>();
    entry_course->set_placeholder_text("Ders adı...");
    entry_course->set_text(current_course);
    entry_course->set_hexpand(true);
    content->append(*entry_course);

    auto* entry_location = Gtk::make_managed<Gtk::Entry>();
    entry_location->set_placeholder_text("Yer (örn: A-101)...");
    entry_location->set_text(current_location);
    entry_location->set_hexpand(true);
    content->append(*entry_location);

    dialog->signal_response().connect([this, dialog, day, time, entry_course, entry_location](int response_id) {
        if (response_id == Gtk::ResponseType::OK) {
            auto course = entry_course->get_text();
            auto location = entry_location->get_text();
            if (!course.empty()) {
                for (auto it = schedule.begin(); it != schedule.end(); ++it) {
                    if (it->day == day && it->time == time) { schedule.erase(it); break; }
                }
                schedule.push_back({day, time, course, location});
                save_data();
                build_schedule_grid();
            }
        } else if (response_id == -100) {
            for (auto it = schedule.begin(); it != schedule.end(); ++it) {
                if (it->day == day && it->time == time) { schedule.erase(it); break; }
            }
            save_data();
            build_schedule_grid();
        }
        dialog->hide();
    });

    dialog->show();
}

void MainWindow::on_add_schedule() {
    auto dialog = Gtk::make_managed<Gtk::Dialog>();
    dialog->set_title("Yeni Ders");
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->set_default_size(350, 220);

    dialog->add_button("İptal", Gtk::ResponseType::CANCEL);
    dialog->add_button("Ekle", Gtk::ResponseType::OK);

    auto* content = dialog->get_content_area();
    content->set_margin_start(16);
    content->set_margin_end(16);
    content->set_margin_top(16);
    content->set_margin_bottom(16);

    auto* day_combo = Gtk::make_managed<Gtk::ComboBoxText>();
    day_combo->append("Pazartesi");
    day_combo->append("Salı");
    day_combo->append("Çarşamba");
    day_combo->append("Perşembe");
    day_combo->append("Cuma");
    day_combo->set_active(0);

    auto* time_combo = Gtk::make_managed<Gtk::ComboBoxText>();
    for (auto& t : schedule_times) time_combo->append(t);
    time_combo->set_active(0);

    auto* entry_course = Gtk::make_managed<Gtk::Entry>();
    entry_course->set_placeholder_text("Ders adı...");
    entry_course->set_hexpand(true);

    auto* entry_location = Gtk::make_managed<Gtk::Entry>();
    entry_location->set_placeholder_text("Yer (örn: B-205)...");
    entry_location->set_hexpand(true);

    content->append(*day_combo);
    content->append(*time_combo);
    content->append(*entry_course);
    content->append(*entry_location);

    dialog->signal_response().connect([this, dialog, day_combo, time_combo, entry_course, entry_location](int response_id) {
        if (response_id == Gtk::ResponseType::OK) {
            auto course = entry_course->get_text();
            if (!course.empty()) {
                std::string day = day_combo->get_active_text();
                std::string time = time_combo->get_active_text();
                std::string location = entry_location->get_text();
                for (auto it = schedule.begin(); it != schedule.end(); ++it) {
                    if (it->day == day && it->time == time) { schedule.erase(it); break; }
                }
                schedule.push_back({day, time, course, location});
                save_data();
                build_schedule_grid();
            }
        }
        dialog->hide();
    });

    dialog->show();
}

void MainWindow::on_clear_schedule() {
    schedule.clear();
    save_data();
    build_schedule_grid();
}

void MainWindow::setup_focus() {
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

    auto* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 14);
    box->set_margin_start(30);
    box->set_margin_end(30);
    box->set_margin_top(30);
    box->set_margin_bottom(30);
    box->set_halign(Gtk::Align::CENTER);
    box->set_valign(Gtk::Align::CENTER);

    auto* title = Gtk::make_managed<Gtk::Label>("Dikkat Modu");
    title->add_css_class("page-title");
    title->set_halign(Gtk::Align::CENTER);

    auto* desc = Gtk::make_managed<Gtk::Label>("Dikkat dağıtıcı siteler engellenir.\nOdaklanarak çalışmaya devam edin.");
    desc->set_justify(Gtk::Justification::CENTER);
    desc->set_halign(Gtk::Align::CENTER);

    focus_status.add_css_class("pomo-status");
    focus_timer.add_css_class("focus-time");

    btn_focus_toggle.add_css_class("focus-btn-off");
    btn_focus_toggle.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_focus_toggle));

    std::string bt = "Engellenen: ";
    for (size_t i = 0; i < blocked_sites.size() && i < 6; i++) {
        bt += blocked_sites[i];
        if (i < 5 && i < blocked_sites.size()-1) bt += ", ";
    }
    focus_blocked.set_markup("<small>" + bt + "</small>");

    box->append(*title);
    box->append(*desc);
    box->append(focus_status);
    box->append(focus_timer);
    box->append(btn_focus_toggle);
    box->append(focus_blocked);

    sw->set_child(*box);
    stack.add(*sw, "focus");
}

void MainWindow::on_focus_toggle() {
    if (focus_active) {
        focus_active = false;
        focus_seconds = 0;
        if (focus_timer_conn) focus_timer_conn.disconnect();
        btn_focus_toggle.set_label("Dikkat Modunu Başlat");
        btn_focus_toggle.remove_css_class("focus-btn-on");
        btn_focus_toggle.add_css_class("focus-btn-off");
        focus_status.set_text("Hazır");
        focus_timer.set_text("00:00");
        unblock_sites();
    } else {
        focus_active = true;
        focus_seconds = 0;
        btn_focus_toggle.set_label("Durdur");
        btn_focus_toggle.remove_css_class("focus-btn-off");
        btn_focus_toggle.add_css_class("focus-btn-on");
        focus_status.set_text("Aktif");
        block_sites();

        focus_timer_conn = Glib::signal_timeout().connect([this]() {
            if (!focus_active) return false;
            focus_seconds++;
            int m = focus_seconds / 60;
            int s = focus_seconds % 60;
            std::ostringstream oss;
            oss << std::setw(2) << std::setfill('0') << m << ":" << std::setw(2) << std::setfill('0') << s;
            focus_timer.set_text(oss.str());
            return true;
        }, 1000);
    }
}

void MainWindow::block_sites() {
    unblock_sites();

    std::string path = "/tmp/student-hub-blocked";
    std::ofstream f(path);
    for (auto& s : blocked_sites) f << "127.0.0.1 " << s << "\n";
    f.close();

    std::string script = "/tmp/student-hub-block.sh";
    std::ofstream sf(script);
    sf << "#!/bin/bash\n";
    sf << "cat " << path << " >> /etc/hosts\n";
    sf.close();
    std::system("chmod +x /tmp/student-hub-block.sh");
    std::system("pkexec /tmp/student-hub-block.sh");
}

void MainWindow::unblock_sites() {
    std::string script = "/tmp/student-hub-unblock.sh";
    std::ofstream sf(script);
    sf << "#!/bin/bash\n";
    sf << "grep -v '127\\.0\\.0\\.1.*\\(instagram\\|twitter\\|facebook\\|tiktok\\|reddit\\|netflix\\|discord\\|twitch\\|x\\.com\\)' /etc/hosts > /tmp/hosts-clean && mv /tmp/hosts-clean /etc/hosts\n";
    sf.close();
    std::system("chmod +x /tmp/student-hub-unblock.sh");
    std::system("pkexec /tmp/student-hub-unblock.sh");
}

void MainWindow::ensure_page_built(const std::string& name) {
    if (pages_built.count(name)) return;

    if (name == "dersler") setup_dersler();
    else if (name == "badges") setup_badges();
    else if (name == "flashcards") setup_flashcards();
    else if (name == "pomodoro") setup_pomodoro();
    else if (name == "planner") setup_planner();
    else if (name == "notes") setup_notes();
    else if (name == "schedule") setup_schedule();
    else if (name == "focus") setup_focus();
    else if (name == "linux") setup_linux();
    else if (name == "ai") setup_ai_chat();
    else if (name == "python") setup_python();
    else if (name == "weekly_analysis") setup_weekly_analysis();

    pages_built.insert(name);
}

void MainWindow::setup_linux() {
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);

    auto* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
    box->set_margin_start(16);
    box->set_margin_end(16);
    box->set_margin_top(16);
    box->set_margin_bottom(16);

    auto* title = Gtk::make_managed<Gtk::Label>("Linux Komutları");
    title->add_css_class("page-title");
    title->set_halign(Gtk::Align::START);

    auto* search = Gtk::make_managed<Gtk::Entry>();
    search->set_placeholder_text("Komut ara...");
    search->signal_changed().connect([this, search]() { filter_linux(search->get_text()); });

    lab_warning = Gtk::make_managed<Gtk::Label>("");
    lab_warning->add_css_class("lab-warning");
    lab_warning->set_halign(Gtk::Align::START);
    lab_warning->set_visible(!lab_ready);
    if (!lab_ready) {
        lab_warning->set_markup(
            "<b>PardusLab</b> aktif deil. Denemek iin: <tt>sudo apt install podman</tt>");
    }
    lab_warning->set_margin_bottom(8);

    struct Cmd { std::string cmd, desc, ex, detail; };
    std::vector<Cmd> cmds = {
        {"ls", "Dizin listele", "ls -la /home",
         "Belirtilen dizindeki dosya ve alt dizinleri listeler. Linux'ta en sık kullanılan komutlardan biridir.\n\n"
         "Seçenekler:\n"
         "  -a  Gizli dosyaları da gösterir (nokta ile başlayanlar)\n"
         "  -l  Uzun format (izin, sahip, boyut, tarih)\n"
         "  -h  Boyutları okunabilir formatta gösterir (KB, MB)\n"
         "  -R  Alt dizinleri özyinelemeli listeler\n"
         "  -t  Değişiklik zamanına göre sıralar\n"
         "  -r  Sıralamayı tersine çevirir\n\n"
         "Örnekler:\n"
         "  ls -lah /home/kullanici  → Detaylı, gizli dahil, okunabilir boyut\n"
         "  ls -lt /var/log          → En yeni dosyalar üstte\n"
         "  ls *.txt                 → Sadece .txt dosyaları"},
        {"cd", "Dizin değiştir", "cd /var/log",
         "Çalışma dizinini (current working directory) değiştirir. Kabuk oturumunuzun bulunduğu konumu belirler.\n\n"
         "Kullanım:\n"
         "  cd ..        → Bir üst dizine çık\n"
         "  cd ~         → Ev dizinine git (home)\n"
         "  cd -         → Önceki dizine geri dön\n"
         "  cd           → Ev dizinine git (parametresiz)\n"
         "  cd /         → Kök dizine git\n"
         "  cd /etc/nginx → Mutlak yol ile git\n\n"
         "İpucu: Tab tuşu ile otomatik tamamlama yapabilirsiniz."},
        {"pwd", "Çalışılan dizin", "pwd",
         "Print Working Directory. Şu anda bulunduğunuz dizinin tam (mutlak) yolunu ekrana yazdırır.\n\n"
         "Neden önemli?\n"
         "  Terminalde kaybolduğunuzda nerede olduğunuzu gösterir\n"
         "  Scriptlerde geçerli dizini öğrenmek için kullanılır\n"
         "  Göreceli yolların nereden başladığını belirler\n\n"
         "Örnek:\n"
         "  pwd  → /home/kullanici/Belgeler/Projeler"},
        {"cp", "Kopyala", "cp dosya.txt /yedek/",
         "Dosya veya dizinleri kopyalar. Orijinal dosya olduğu yerde kalır, bir kopyası oluşturulur.\n\n"
         "Seçenekler:\n"
         "  -r  Dizinleri özyinelemeli kopyalar (içerik dahil)\n"
         "  -i  Üzerine yazmadan önce onay sorar\n"
         "  -v  Kopyalanan dosyaları ekrana yazdırır\n"
         "  -p  İzinleri ve zaman damgalarını korur\n"
         "  -u  Sadece daha yeni dosyaları kopyalar\n\n"
         "Örnekler:\n"
         "  cp dosya.txt yedek.txt        → Dosya kopyala\n"
         "  cp -rv /kaynak /hedef         → Dizin kopyala\n"
         "  cp *.pdf /yedek/pdfler/       → Tüm PDF'leri kopyala"},
        {"mv", "Taşı / yeniden adlandır", "mv eski.txt yeni.txt",
         "Dosya veya dizinleri taşır ya da yeniden adlandırır. Taşıma işlemi sonrası kaynak silinir.\n\n"
         "Kullanım:\n"
         "  Aynı dizin içinde → Yeniden adlandırma\n"
         "  Farklı dizine     → Taşıma işlemi\n\n"
         "Seçenekler:\n"
         "  -i  Üzerine yazmadan önce sorar\n"
         "  -v  Taşınan dosyaları gösterir\n"
         "  -n  Mevcut dosyanın üzerine yazmaz\n\n"
         "Örnekler:\n"
         "  mv eski.txt yeni.txt          → Yeniden adlandır\n"
         "  mv dosya.txt /home/belgeler/  → Taşı\n"
         "  mv *.jpg /resimler/           → Tüm JPG'leri taşı"},
        {"rm", "Sil", "rm -rf /tmp/eski",
         "Dosya veya dizinleri siler. DİKKAT: Silinen dosyalar geri getirilemez!\n\n"
         "Seçenekler:\n"
         "  -r  Dizinleri özyinelemeli siler\n"
         "  -f  Zorla siler, onay sormaz, hata vermez\n"
         "  -i  Her dosya için onay sorar (güvenli)\n"
         "  -v  Silinen dosyaları gösterir\n\n"
         "⚠️ UYARI: rm -rf / komutu sistemi tamamen siler!\n\n"
         "Örnekler:\n"
         "  rm dosya.txt        → Tek dosya sil\n"
         "  rm -r proje/        → Dizin sil\n"
         "  rm -i *.log         → Her .log için sor"},
        {"mkdir", "Dizin oluştur", "mkdir -p a/b/c",
         "Yeni dizin (klasör) oluşturur.\n\n"
         "Seçenekler:\n"
         "  -p  Üst dizinleri de otomatik oluşturur\n"
         "  -v  Oluşturulan dizinleri gösterir\n"
         "  -m  İzinleri belirleyerek oluşturur\n\n"
         "Örnekler:\n"
         "  mkdir yeni_dizin              → Tek dizin oluştur\n"
         "  mkdir -p a/b/c/d              → İç içe dizinler\n"
         "  mkdir -m 700 gizli_dizin      → Sadece sahibi erişir\n"
         "  mkdir proje/{src,doc,test}    → Birden fazla dizin"},
        {"cat", "Dosya göster", "cat /etc/passwd",
         "Dosya içeriğini terminal'e yazdırır. Birden fazla dosyayı birleştirebilir.\n\n"
         "Seçenekler:\n"
         "  -n  Satır numaralarını gösterir\n"
         "  -b  Boş olmayan satırları numaralandırır\n"
         "  -E  Satır sonlarını $ ile gösterir\n\n"
         "Yönlendirme ile kullanım:\n"
         "  cat dosya1 dosya2 > birlesik.txt  → Birleştir\n"
         "  cat dosya >> mevcut.txt           → Ekleme yap\n"
         "  cat > yeni.txt                    → Klavyeden yaz\n\n"
         "İpucu: Uzun dosyalar için `less` veya `more` kullanın."},
        {"grep", "Metin ara", "grep 'hata' syslog",
         "Dosya veya giriş akışında metin araması yapar. Düzenli ifade (regex) destekler.\n\n"
         "Seçenekler:\n"
         "  -i  Büyük/küçük harf duyarsız\n"
         "  -n  Eşleşen satır numarasını gösterir\n"
         "  -r  Dizinlerde özyinelemeli ara\n"
         "  -v  EşleşMEYEN satırları gösterir\n"
         "  -c  Eşleşme sayısını gösterir\n"
         "  -l  Sadece dosya adını gösterir\n"
         "  -A 3  Eşleşmeden sonra 3 satır göster\n"
         "  -B 3  Eşleşmeden önce 3 satır göster\n\n"
         "Örnekler:\n"
         "  grep -i 'error' /var/log/syslog\n"
         "  grep -rn 'function' /proje/src/\n"
         "  ps aux | grep nginx"},
        {"find", "Dosya ara", "find / -name '*.log'",
         "Dosya ve dizin arar. Çok güçlü filtreleme seçenekleri vardır.\n\n"
         "Filtreler:\n"
         "  -name 'isim'     İsme göre ara (glob desteği)\n"
         "  -type f          Sadece dosyalar\n"
         "  -type d          Sadece dizinler\n"
         "  -size +10M       10MB'den büyük\n"
         "  -size -1k        1KB'den küçük\n"
         "  -mtime -7        Son 7 günde değişen\n"
         "  -perm 755        İzinlere göre\n"
         "  -user kullanici  Sahibine göre\n\n"
         "İşlemler:\n"
         "  -exec rm {} \\;  Bulunanları sil\n"
         "  -delete          Bulunanları sil (kısa)\n"
         "  -ls              Detaylı listele\n\n"
         "Örnekler:\n"
         "  find /home -name '*.pdf' -size +5M\n"
         "  find /tmp -type f -mtime +30 -delete"},
        {"chmod", "İzin değiştir", "chmod 755 betik.sh",
         "Dosya ve dizinlerin erişim izinlerini değiştirir.\n\n"
         "İzin değerleri:\n"
         "  4 = r (okuma)    → Dosyayı okuyabilir\n"
         "  2 = w (yazma)    → Dosyayı düzenleyebilir\n"
         "  1 = x (çalıştırma) → Programı çalıştırabilir\n\n"
         "Format: [sahip][grup][diğer]\n"
         "  755 → Sahip: rwx, Grup: r-x, Diğer: r-x\n"
         "  644 → Sahip: rw-, Grup: r--, Diğer: r--\n"
         "  700 → Sahip: rwx, Grup: ---, Diğer: ---\n\n"
         "Sembolik format:\n"
         "  chmod +x betik.sh    → Çalıştırma izni ekle\n"
         "  chmod -w dosya.txt   → Yazma izni kaldır\n"
         "  chmod u+rwx dosya    → Sahibe tüm izinler\n\n"
         "  -R  Özyinelemeli uygular"},
        {"chown", "Sahiplik değiştir", "chown kullanıcı:grup dosya",
         "Dosya ve dizinlerin sahibini ve grubunu değiştirir.\n\n"
         "Kullanım:\n"
         "  chown kullanici dosya     → Sadece sahip değiştir\n"
         "  chown :grup dosya         → Sadece grup değiştir\n"
         "  chown kullanici:grup dosya → İkisini birlikte\n\n"
         "Seçenekler:\n"
         "  -R  Özyinelemeli uygular\n"
         "  -v  Değişiklikleri gösterir\n\n"
         "⚠️ Sadece root kullanıcısı dosya sahibini değiştirebilir.\n\n"
         "Örnekler:\n"
         "  chown www-data:www-data /var/www/html\n"
         "  chown -R kullanici:users /home/kullanici"},
        {"ps", "Süreç listesi", "ps aux | grep nginx",
         "Çalışan süreçleri (process) listeler. Anlık bir görüntü verir.\n\n"
         "Formatlar:\n"
         "  ps aux    → Tüm kullanıcıların tüm süreçleri (BSD)\n"
         "  ps -ef    → Tüm süreçler tam format (System V)\n"
         "  ps -u kullanıcı → Belirli kullanıcının süreçleri\n"
         "  ps -p PID → Belirli PID'yi göster\n\n"
         "Çıktı sütunları:\n"
         "  USER  Süreci çalıştıran kullanıcı\n"
         "  PID   Süreç kimliği\n"
         "  %CPU  CPU kullanım yüzdesi\n"
         "  %MEM  Bellek kullanım yüzdesi\n"
         "  VSZ   Sanal bellek boyutu\n"
         "  RSS   Fiziksel bellek boyutu\n"
         "  STAT  Süreç durumu (R,S,Z,T)\n"
         "  COMMAND  Komut satırı\n\n"
         "İpucu: `ps aux --sort=-%cpu` CPU'ya göre sırala."},
        {"top", "Süreçler", "top",
         "Canlı süreç izleme aracı. Sistem kaynaklarını gerçek zamanlı gösterir.\n\n"
         "Üst bölüm:\n"
         "  Sistem çalışma süresi, kullanıcı sayısı, yük ortalaması\n"
         "  Toplam süreç sayısı (çalışan, uyuyan, zombie)\n"
         "  CPU kullanımı (user, system, idle, iowait)\n"
         "  Bellek kullanımı (toplam, kullanılan, boş)\n\n"
         "Kısayol tuşları:\n"
         "  q     → Çıkış\n"
         "  k     → Süreç sonlandır (PID sorar)\n"
         "  M     → Belleğe göre sırala\n"
         "  P     → CPU'ya göre sırala\n"
         "  T     → Çalışma süresine göre sırala\n"
         "  c     → Tam komut satırını göster\n"
         "  u     → Kullanıcıya göre filtrele\n"
         "  h     → Yardım\n\n"
         "Alternatif: `htop` daha renkli ve kullanıcı dostudur."},
        {"kill", "Süreç sonlandır", "kill -9 1234",
         "Süreçlere sinyal gönderir. Genellikle süreç sonlandırmak için kullanılır.\n\n"
         "Sinyaller:\n"
         "  -15 (SIGTERM)  → Nazik sonlandırma (varsayılan)\n"
         "                    Süreç temizlik yapabilir\n"
         "  -9  (SIGKILL)  → Zorla sonlandırma\n"
         "                    Süreç engellemeye çalışamaz\n"
         "  -1  (SIGHUP)   → Yeniden yükleme\n"
         "                    Konfigürasyon yeniden okunur\n"
         "  -18 (SIGCONT)  → Durdurulmuş süreci devam ettir\n"
         "  -19 (SIGSTOP)  → Süreci durdur\n\n"
         "İlgili komutlar:\n"
         "  killall isim  → İsme göre tüm süreçleri sonlandır\n"
         "  pkill isim    → İsme göre sinyal gönder\n"
         "  kill -l       → Tüm sinyalleri listele\n\n"
         "Örnekler:\n"
         "  kill 1234         → Nazik sonlandır\n"
         "  kill -9 1234      → Zorla sonlandır\n"
         "  killall firefox   → Tüm Firefox süreçlerini kapat"},
        {"tar", "Arşiv", "tar -xzf arsiv.tar.gz",
         "Dosya arşivleme ve sıkıştırma aracı. Linux'ta en yaygın arşiv formatıdır.\n\n"
         "Temel işlemler:\n"
         "  -c  Oluştur (create)\n"
         "  -x  Çıkar (extract)\n"
         "  -t  İçeriği listele\n"
         "  -f  Dosya adı belirt (her zaman son olmalı)\n\n"
         "Sıkıştırma:\n"
         "  -z  gzip (.tar.gz)   → Hızlı, yaygın\n"
         "  -j  bzip2 (.tar.bz2) → Daha iyi sıkıştırma\n"
         "  -J  xz (.tar.xz)     → En iyi sıkıştırma\n\n"
         "Diğer seçenekler:\n"
         "  -v  İşlem detaylarını gösterir\n"
         "  -C  Çıkarma dizini belirt\n"
         "  --exclude  Dosya hariç tut\n\n"
         "Örnekler:\n"
         "  tar -czf arsiv.tar.gz /dizin    → Oluştur\n"
         "  tar -xzf arsiv.tar.gz           → Çıkar\n"
         "  tar -tzf arsiv.tar.gz           → Listele\n"
         "  tar -xzf arsiv.tar.gz -C /hedef → Başka yere çıkar"},
        {"wget", "İndir", "wget https://ornek.com/dosya",
         "Web'den dosya indirme aracı. HTTP, HTTPS ve FTP destekler.\n\n"
         "Seçenekler:\n"
         "  -O dosya  Farklı isimle kaydet\n"
         "  -c        Kaldığı yerden devam et\n"
         "  -r        Özyinelemeli indir (site kopyala)\n"
         "  -i dosya  Dosyadan URL listesi oku\n"
         "  -b        Arka planda indir\n"
         "  -q        Sessiz mod\n"
         "  --limit-rate=1m  Hız sınırı koy\n"
         "  -P dizin  Kayıt dizini belirt\n"
         "  --user-agent  Kullanıcı ajanı değiştir\n\n"
         "Örnekler:\n"
         "  wget https://site.com/dosya.zip\n"
         "  wget -c https://site.com/buyuk.iso\n"
         "  wget -r -l 2 https://site.com\n"
         "  wget -i url_listesi.txt"},
        {"curl", "Veri aktar", "curl -O https://ornek.com/dosya",
         "URL ile veri transferi. wget'ten farklı olarak daha çok API ve HTTP istekleri için kullanılır.\n\n"
         "Seçenekler:\n"
         "  -O  Dosya adını URL'den al\n"
         "  -o isim  Dosya adını kendin belirle\n"
         "  -X POST  POST isteği gönder\n"
         "  -d 'veri'  POST verisi gönder\n"
         "  -H 'header'  HTTP header ekle\n"
         "  -u kullanıcı:şifre  Kimlik doğrulama\n"
         "  -L  Yönlendirmeleri takip et\n"
         "  -s  Sessiz mod\n"
         "  -I  Sadece header göster\n"
         "  -k  SSL sertifika kontrolünü atla\n\n"
         "Örnekler:\n"
         "  curl -O https://site.com/dosya.zip\n"
         "  curl -X POST -d '{\"key\":\"val\"}' https://api.site.com\n"
         "  curl -H 'Authorization: Bearer token' https://api.site.com\n"
         "  curl ifconfig.me  → Dış IP'ni öğren"},
        {"ssh", "Uz bağlantı", "ssh kullanıcı@sunucu",
         "Secure Shell. Uz bilgisayara güvenli bağlantı sağlar. Tüm iletişim şifrelidir.\n\n"
         "Seçenekler:\n"
         "  -p port  Port belirt (varsayılan: 22)\n"
         "  -i dosya  SSH anahtar dosyası kullan\n"
         "  -L port:hedef:port  Port yönlendirme (ileri)\n"
         "  -R port:hedef:port  Port yönlendirme (geri)\n"
         "  -X  X11 yönlendirme (GUI uygulamalar)\n"
         "  -C  Sıkıştırma kullan\n"
         "  -v  Detaylı çıktı (debug)\n\n"
         "Anahtar yönetimi:\n"
         "  ssh-keygen          → Yeni anahtar oluştur\n"
         "  ssh-copy-id sunucu  → Anahtarı sunucuya kopyala\n"
         "  ssh-agent           → Anahtar yöneticisi\n\n"
         "SSH config (~/.ssh/config):\n"
         "  Host myserver\n"
         "    HostName 192.168.1.100\n"
         "    User admin\n"
         "    Port 2222\n\n"
         "  ssh myserver  → Config'den bağlan"},
        {"scp", "Uz kopyala", "scp dosya kullanıcı@sunucu:/yol",
         "SSH üzerinden güvenli dosya kopyalama. Komut yapısı cp'ye benzer.\n\n"
         "Yönler:\n"
         "  Yerel → Uz:  scp dosya kullanıcı@sunucu:/yol\n"
         "  Uz → Yerel:  scp kullanıcı@sunucu:/dosya .\n"
         "  Uz → Uz:     scp sunucu1:/dosya sunucu2:/yol\n\n"
         "Seçenekler:\n"
         "  -r  Dizinleri özyinelemeli kopyala\n"
         "  -P port  Port belirt\n"
         "  -C  Sıkıştırarak aktar\n"
         "  -p  İzinleri ve zaman damgalarını koru\n"
         "  -l limit  Bant genişliği sınırı (Kbit/s)\n\n"
         "Örnekler:\n"
         "  scp rapor.pdf user@192.168.1.10:/home/user/\n"
         "  scp -r proje/ user@sunucu:/var/www/\n"
         "  scp -P 2222 dosya user@sunucu:~/\n\n"
         "İpucu: rsync daha hızlı ve akıllı senkronizasyon sağlar."},
        {"df", "Disk kullanım", "df -h",
         "Dosya sistemi disk kullanımını gösterir. Bağlı tüm bölümleri listeler.\n\n"
         "Seçenekler:\n"
         "  -h  Okunabilir format (GB, MB)\n"
         "  -T  Dosya sistemi türünü göster (ext4, xfs)\n"
         "  -i  Inode kullanımını göster\n"
         "  -t tip  Sadece belirli dosya sistemi\n"
         "  -x tip  Belirli dosya sistemini hariç tut\n\n"
         "Çıktı sütunları:\n"
         "  Filesystem  Dosya sistemi aygıtı\n"
         "  Size        Toplam boyut\n"
         "  Used        Kullanılan\n"
         "  Avail       Kullanılabilir\n"
         "  Use%        Kullanım yüzdesi\n"
         "  Mounted on  Bağlantı noktası\n\n"
         "Örnekler:\n"
         "  df -h              → Tüm bölümler\n"
         "  df -h /home        → /home bölümü\n"
         "  df -Th             → Tür bilgisi ile"},
        {"du", "Dizin boyutu", "du -sh /var",
         "Dizin ve dosyaların disk kullanımını hesaplar.\n\n"
         "Seçenekler:\n"
         "  -s  Sadece toplam boyut (summary)\n"
         "  -h  Okunabilir format\n"
         "  -c  Sonunda toplam göster\n"
         "  --max-depth=N  N seviye derinliğe in\n"
         "  -a  Tüm dosyaları göster (varsayılan: sadece dizin)\n"
         "  --time  Son değişiklik zamanını göster\n\n"
         "Örnekler:\n"
         "  du -sh /var/log           → /var/log toplam boyutu\n"
         "  du -sh *                  → Her dosya/dizin boyutu\n"
         "  du -h --max-depth=1 /home → Kullanıcı bazlı boyut\n"
         "  du -shc *.zip             → ZIP'ler + toplam\n\n"
         "İpucu: `du -sh * | sort -rh` en büyükten sırala."},
        {"free", "Bellek", "free -h",
         "RAM ve takas (swap) belleği kullanımını gösterir.\n\n"
         "Seçenekler:\n"
         "  -h  Okunabilir format (GB, MB)\n"
         "  -m  MB cinsinden\n"
         "  -g  GB cinsinden\n"
         "  -s N  Her N saniyede güncelle\n"
         "  -t  Toplam satırını ekle\n\n"
         "Çıktı sütunları:\n"
         "  total     Toplam fiziksel bellek\n"
         "  used      Kullanılan bellek\n"
         "  free      Boş bellek\n"
         "  shared    Paylaşılan bellek (tmpfs)\n"
         "  buff/cache  Önbellek ve tampon\n"
         "  available  Gerçekten kullanılabilir\n\n"
         "Önemli: Linux boş belleği önbellek olarak kullanır.\n"
         "\"available\" sütunu gerçek kullanılabilir belleği gösterir.\n\n"
         "Örnekler:\n"
         "  free -h           → Tek seferlik\n"
         "  free -h -s 2      → Her 2 saniyede güncelle"},
        {"uname", "Sistem bilgisi", "uname -a",
         "Sistem ve çekirdek bilgilerini gösterir.\n\n"
         "Seçenekler:\n"
         "  -a  Tüm bilgiler\n"
         "  -s  Çekirdek adı (Linux)\n"
         "  -n  Makine adı (hostname)\n"
         "  -r  Çekirdek sürümü\n"
         "  -v  Çekirdek versiyonu\n"
         "  -m  Makine mimarisi (x86_64, aarch64)\n"
         "  -o  İşletim sistemi adı\n\n"
         "Örnekler:\n"
         "  uname -a    → Tüm bilgiler\n"
         "  uname -r    → 6.1.0-18-amd64\n"
         "  uname -m    → x86_64\n\n"
         "Detaylı sistem bilgisi için:\n"
         "  lsb_release -a  → Dağıtım bilgisi\n"
         "  cat /etc/os-release → OS bilgisi"},
        {"whoami", "Kullanıcı", "whoami",
         "Geçerli kullanıcının adını gösterir.\n\n"
         "İlgili komutlar:\n"
         "  id        → UID, GID ve grup bilgileri\n"
         "  id kullanıcı → Başka kullanıcı bilgisi\n"
         "  groups    → Kullanıcının grupları\n"
         "  users     → Oturum açmış kullanıcılar\n"
         "  w         → Kullanıcılar ve aktiviteleri\n"
         "  last      → Son oturum açma kayıtları\n\n"
         "Örnekler:\n"
         "  whoami    → kullanici\n"
         "  id        → uid=1000(kullanici) gid=1000(kullanici)\n"
         "  id root   → uid=0(root) gid=0(root)"},
        {"sudo", "Yönetici çalıştır", "sudo apt update",
         "Komutu yönetici (root) yetkisiyle çalıştırır.\n\n"
         "Seçenekler:\n"
         "  -i  Root kabuğu aç\n"
         "  -s  Kabuk çalıştır\n"
         "  -u kullanıcı  Başka kullanıcı olarak çalıştır\n"
         "  -l  Yetkileri listele\n"
         "  -k  Yetki süresini sıfırla (şifre tekrar sorulur)\n"
         "  -v  Yetki süresini uzat\n\n"
         "Güvenlik:\n"
         "  /etc/sudoers dosyasında kimin ne yapabileceği belirlenir\n"
         "  visudo ile sudoers dosyası düzenlenir\n"
         "  Varsayılan olarak 15 dakika yetki süresi vardır\n\n"
         "Örnekler:\n"
         "  sudo apt update       → Paket listesini güncelle\n"
         "  sudo -i               → Root kabuğuna geç\n"
         "  sudo -u www-data ls   → www-data olarak listele\n"
         "  sudo -l               → Yetkilerini gör"},
        {"apt", "Paket yönet (Debian)", "apt install paket",
         "Debian/Ubuntu/Pardus tabanlı sistemlerde paket yöneticisi.\n\n"
         "Temel komutlar:\n"
         "  apt update            → Paket listesini güncelle\n"
         "  apt upgrade           → Tüm paketleri güncelle\n"
         "  apt full-upgrade      → Bağımlılıkları da güncelle\n"
         "  apt install paket     → Paket kur\n"
         "  apt remove paket      → Paket kaldır (config kalır)\n"
         "  apt purge paket       → Paket ve config sil\n"
         "  apt autoremove        → Gereksiz bağımlılıkları temizle\n"
         "  apt search anahtar    → Paket ara\n"
         "  apt show paket        → Paket detaylarını göster\n"
         "  apt list --installed  → Kurulu paketleri listele\n"
         "  apt list --upgradable → Güncellenebilir paketler\n\n"
         "İpucu: `apt` yerine `apt-get` scriptlerde tercih edilir.\n"
         "`apt` interaktif kullanım için daha uygun (renkli, ilerleme çubuğu)."},
        {"dnf", "Paket yönet (Fedora)", "dnf install paket",
         "Fedora/RHEL/CentOS tabanlı sistemlerde paket yöneticisi.\n\n"
         "Temel komutlar:\n"
         "  dnf update            → Tüm paketleri güncelle\n"
         "  dnf install paket     → Paket kur\n"
         "  dnf remove paket      → Paket kaldır\n"
         "  dnf search anahtar    → Paket ara\n"
         "  dnf info paket        → Paket detayları\n"
         "  dnf list installed    → Kurulu paketler\n"
         "  dnf list available    → Mevcut paketler\n"
         "  dnf history           → İşlem geçmişi\n"
         "  dnf history undo N    → N. işlemi geri al\n"
         "  dnf clean all         → Önbelleği temizle\n"
         "  dnf repolist          → Depo listesini göster\n\n"
         "Ek özellikler:\n"
         "  dnf group list        → Grup paketleri\n"
         "  dnf provides dosya    → Hangi paket sağlıyor\n"
         "  dnf download paket    → Sadece indir, kurma"},
        {"systemctl", "Servis yönet", "systemctl status sshd",
         "Systemd servis yönetimi. Linux servislerini başlatır, durdurur ve yönetir.\n\n"
         "Servis komutları:\n"
         "  systemctl start sshd      → Servisi başlat\n"
         "  systemctl stop sshd       → Servisi durdur\n"
         "  systemctl restart sshd    → Yeniden başlat\n"
         "  systemctl reload sshd     → Config yeniden yükle\n"
         "  systemctl status sshd     → Durum göster\n"
         "  systemctl enable sshd     → Otomatik başlat\n"
         "  systemctl disable sshd    → Otomatik başlatmayı kapat\n"
         "  systemctl is-active sshd  → Aktif mi kontrol et\n"
         "  systemctl is-enabled sshd → Otomatik başlatma durumu\n\n"
         "Genel komutlar:\n"
         "  systemctl list-units      → Aktif birimleri listele\n"
         "  systemctl list-unit-files → Tüm birim dosyaları\n"
         "  systemctl daemon-reload   → Config yeniden yükle\n"
         "  systemctl poweroff        → Bilgisayarı kapat\n"
         "  systemctl reboot          → Yeniden başlat"},
        {"journalctl", "Günlük", "journalctl -u nginx",
         "Systemd günlük kayıtlarını görüntüler. Tüm servis logları burada toplanır.\n\n"
         "Seçenekler:\n"
         "  -u birim     Belirli servise göre filtrele\n"
         "  -f           Canlı takip (tail -f gibi)\n"
         "  --since \"today\"   Bugünden beri\n"
         "  --since \"2024-01-01\" → Belirli tarihten beri\n"
         "  --until \"now\"  Şu ana kadar\n"
         "  -p err       Hata seviyesi ve üstü\n"
         "  -p 0..7      Öncelik seviyesi (0=emerg, 7=debug)\n"
         "  -n 50        Son 50 satır\n"
         "  --no-pager   Sayfalama olmadan göster\n"
         "  -e           Son sayfaya git\n"
         "  -b           Bu açılıştan beri\n"
         "  -b -1        Önceki açılıştan beri\n\n"
         "Örnekler:\n"
         "  journalctl -u nginx -f\n"
         "  journalctl -p err --since yesterday\n"
         "  journalctl --disk-usage  → Günlük boyutu"},
        {"man", "Kılavuz", "man ls",
         "Komut kılavuz (manual) sayfasını açar. Her komutun detaylı dokümantasyonu vardır.\n\n"
         "Man sayfası bölümleri:\n"
         "  NAME     Komut adı ve kısa açıklama\n"
         "  SYNOPSIS Kullanım formatı\n"
         "  DESCRIPTION Detaylı açıklama\n"
         "  OPTIONS  Seçenekler ve açıklamaları\n"
         "  EXAMPLES Örnekler\n"
         "  SEE ALSO İlgili komutlar\n\n"
         "Navigasyon:\n"
         "  q     → Çıkış\n"
         "  /arama → Metin ara\n"
         "  n     → Sonraki eşleşme\n"
         "  N     → Önceki eşleşme\n"
         "  space → Sayfa aşağı\n"
         "  b     → Sayfa yukarı\n"
         "  g     → Başa git\n"
         "  G     → Sona git\n\n"
         "Bölümler:\n"
         "  man 1 komut  → Kullanıcı komutları\n"
         "  man 2 komut  → Sistem çağrıları\n"
         "  man 3 komut  → C kütüphane fonksiyonları\n"
         "  man 5 dosya  → Dosya formatları\n"
         "  man -k anahtar → Konu ara (apropos)"},
        {"history", "Geçmiş", "history | grep ssh",
         "Kabuk komut geçmişini gösterir ve yönetir.\n\n"
         "Kullanım:\n"
         "  history       → Tüm geçmişi listele\n"
         "  history 20    → Son 20 komut\n"
         "  history -c    → Geçmişi temizle\n"
         "  history -d N  → N. satırı sil\n\n"
         "Geçmişten çalıştırma:\n"
         "  !n         → n. komutu çalıştır\n"
         "  !!         → Son komutu tekrar çalıştır\n"
         "  !abc       → abc ile başlayan son komutu çalıştır\n"
         "  !abc:p     → Çalıştırmadan önce göster\n"
         "  !$         → Son komutun son argümanı\n"
         "  Ctrl+R     → Geçmişte arama (interactive)\n\n"
         "Yapılandırma (~/.bashrc):\n"
         "  HISTSIZE=10000    → Bellekte tutulan\n"
         "  HISTFILESIZE=20000 → Dosyada tutulan\n"
         "  HISTCONTROL=ignoredups → Tekrarları kaydetme"},
        {"alias", "Kısayol", "alias ll='ls -la'",
         "Uzun komutlara kısa isimler (takma ad) tanımlar.\n\n"
         "Kullanım:\n"
         "  alias           → Tüm aliasları listele\n"
         "  alias ll='ls -la' → Yeni alias tanımla\n"
         "  unalias ll      → Alias kaldır\n"
         "  unalias -a      → Tüm aliasları kaldır\n\n"
         "Yaygın aliaslar:\n"
         "  alias ll='ls -la'\n"
         "  alias la='ls -A'\n"
         "  alias gs='git status'\n"
         "  alias gp='git push'\n"
         "  alias update='sudo apt update && sudo apt upgrade'\n\n"
         "Kalıcı yapmak için:\n"
         "  ~/.bashrc dosyasına ekleyin\n"
         "  source ~/.bashrc ile yeniden yükleyin\n\n"
         "İpucu: alias komutunun kendisi de aliaslanabilir!"},
        {"ln", "Bağlantı", "ln -s /hedef bağlantı",
         "Dosya bağlantısı (link) oluşturur. İki tür bağlantı vardır.\n\n"
         "Sembolik (yumuşak) bağlantı:\n"
         "  ln -s /hedef/link yol/baglanti\n"
         "  → Windows'taki kısayol gibidir\n"
         "  → Hedef silinirse bağlantı bozulur\n"
         "  → Farklı dosya sistemine bağlanabilir\n\n"
         "Sert (hard) bağlantı:\n"
         "  ln /hedef/link yol/baglanti\n"
         "  → Aynı inode'u paylaşır\n"
         "  → Hedef silinse de erişilebilir\n"
         "  → Sadece aynı dosya sisteminde\n"
         "  → Dizinlere uygulanamaz\n\n"
         "Seçenekler:\n"
         "  -f  Mevcut dosyanın üzerine yaz\n"
         "  -v  Oluşturulan bağlantıyı göster\n"
         "  -n  Sembolik bağlantıyı hedef olarak işleme\n\n"
         "Örnekler:\n"
         "  ln -s /var/www/html ~/web\n"
         "  ln -s /opt/proje/config.json ./config"},
        {"diff", "Karşılaştır", "diff dosya1 dosya2",
         "İki dosya arasındaki farkları gösterir.\n\n"
         "Seçenekler:\n"
         "  -u  Birleşik (unified) format\n"
         "  -c  Bağlam (context) formatı\n"
         "  -r  Dizinleri özyinelemeli karşılaştır\n"
         "  -y  Yan yana görünüm\n"
         "  -q  Sadece farklıysa bildir\n"
         "  -i  Büyük/küçük harf duyarsız\n"
         "  -w  Boşluk farklarını yok say\n"
         "  -B  Boş satır farklarını yok say\n\n"
         "Çıktı okuma (unified format):\n"
         "  --- dosya1  → Orijinal dosya\n"
         "  +++ dosya2  → Yeni dosya\n"
         "  -satır      → Silinen satır\n"
         "  +satır      → Eklenen satır\n"
         "   satır      → Değişmeyen satır\n\n"
         "Yama oluşturma:\n"
         "  diff -u eski.txt yeni.txt > yama.patch\n"
         "  patch eski.txt < yama.patch"},
        {"head", "İlk satırlar", "head -n 20 dosya.log",
         "Dosyanın ilk satırlarını gösterir.\n\n"
         "Seçenekler:\n"
         "  -n 20  İlk 20 satır\n"
         "  -c 100  İlk 100 bayt\n"
         "  -q  Dosya adını gösterme\n"
         "  -v  Her zaman dosya adını göster\n\n"
         "Birden fazla dosya:\n"
         "  head -n 5 dosya1.txt dosya2.txt\n"
         "  → Her dosyanın ilk 5 satırını gösterir\n"
         "  → Dosya adları başlık olarak yazılır\n\n"
         "Pipeline ile kullanım:\n"
         "  ls -la | head -n 10\n"
         "  cat /var/log/syslog | head -n 50\n\n"
         "İpucu: tail ile birleştirerek dosyanın başını ve sonunu görebilirsiniz."},
        {"tail", "Son satırlar", "tail -f /var/log/syslog",
         "Dosyanın son satırlarını gösterir. Canlı izleme için idealdir.\n\n"
         "Seçenekler:\n"
         "  -f  Canlı takip (dosya büyüdükçe gösterir)\n"
         "  -F  -f + dosya yeniden oluşturulursa takip et\n"
         "  -n 50  Son 50 satır\n"
         "  -c 100  Son 100 bayt\n"
         "  -q  Dosya adını gösterme\n"
         "  -v  Her zaman dosya adını göster\n"
         "  --pid=PID  PID bitince çık\n\n"
         "Canlı izleme:\n"
         "  tail -f /var/log/syslog\n"
         "  tail -f /var/log/nginx/access.log\n"
         "  tail -f /var/log/syslog | grep 'error'\n\n"
         "İpucu: Ctrl+C ile canlı takipten çıkabilirsiniz.\n"
         "Birden fazla dosyayı aynı anda izleyebilirsiniz:\n"
         "  tail -f /var/log/syslog /var/log/auth.log"},
        {"sort", "Sırala", "sort dosya.txt",
         "Metin dosyasının satırlarını sıralar.\n\n"
         "Seçenekler:\n"
         "  -r  Ters sıralama (Z→A, 9→0)\n"
         "  -n  Sayısal sıralama\n"
         "  -f  Büyük/küçük harf duyarsız\n"
         "  -u  Tekrarları kaldır (unique)\n"
         "  -R  Rastgele sıralama\n"
         "  -t:  Ayırıcı karakter belirt\n"
         "  -k2  2. sütuna göre sırala\n"
         "  -o dosya  Çıktıyı dosyaya yaz\n\n"
         "Örnekler:\n"
         "  sort dosya.txt              → Alfabetik sırala\n"
         "  sort -n sayilar.txt         → Sayısal sırala\n"
         "  sort -t: -k3 /etc/passwd    → 3. sütuna göre (UID)\n"
         "  sort -rn                    → Ters sayısal\n"
         "  sort dosya.txt -o sirali.txt → Dosyaya kaydet\n\n"
         "İpucu: uniq ile birleştirerek tekrarları bulun:\n"
         "  sort dosya.txt | uniq -c | sort -rn"},
        {"uniq", "Tekrarları kaldır", "sort dosya | uniq",
         "Yan yana tekrarlanan satırları filtreler. Sadece bitişik tekrarları algılar!\n\n"
         "⚠️ Önce sort kullanın, sonra uniq!\n\n"
         "Seçenekler:\n"
         "  -c  Her satırın tekrar sayısını göster\n"
         "  -d  Sadece tekrarlanan satırları göster\n"
         "  -u  Sadece benzersiz (tekrarsız) satırlar\n"
         "  -i  Büyük/küçük harf duyarsız\n"
         "  -w N  İlk N karakteri karşılaştır\n\n"
         "Örnekler:\n"
         "  sort dosya.txt | uniq          → Tekrarları temizle\n"
         "  sort dosya.txt | uniq -c       → Sayıları göster\n"
         "  sort dosya.txt | uniq -d       → Sadece tekrarlar\n"
         "  sort dosya.txt | uniq -u       → Sadece benzersizler\n\n"
         "Pratik kullanım:\n"
         "  history | awk '{print $2}' | sort | uniq -c | sort -rn\n"
         "  → En çok kullanılan komutları listele"},
        {"wc", "Satır/kelime sayısı", "wc -l dosya.txt",
         "Dosyadaki satır, kelime ve bayt sayısını sayar.\n\n"
         "Seçenekler:\n"
         "  -l  Satır sayısı (line)\n"
         "  -w  Kelime sayısı (word)\n"
         "  -c  Bayt sayısı (byte)\n"
         "  -m  Karakter sayısı (multibyte desteği)\n"
         "  -L  En uzun satır uzunluğu\n\n"
         "Çıktı formatı:\n"
         "  wc dosya.txt  → satır kelime bayt dosya\n"
         "  wc -l dosya.txt  → sadece satır sayısı\n\n"
         "Birden fazla dosya:\n"
         "  wc *.txt  → Her dosya + toplam\n\n"
         "Pipeline ile:\n"
         "  ls -1 | wc -l        → Dizindeki dosya sayısı\n"
         "  cat dosya.txt | wc -w → Kelime sayısı\n"
         "  grep 'hata' syslog | wc -l → Hata sayısı\n\n"
         "İpucu: `wc -l < dosya.txt` sadece sayıyı gösterir."},
        {"awk", "Metin işleme", "awk '{print $1}' dosya",
         "Güçlü metin işleme ve raporlama dili. Sütun bazlı işlem yapar.\n\n"
         "Temel yapı:\n"
         "  awk 'kural {eylem}' dosya\n\n"
         "Değişkenler:\n"
         "  $1, $2, ...  → Sütunlar\n"
         "  $0           → Tüm satır\n"
         "  $NF          → Son sütun\n"
         "  NF           → Sütun sayısı\n"
         "  NR           → Satır numarası\n"
         "  FS           → Ayırıcı (varsayılan: boşluk)\n\n"
         "Seçenekler:\n"
         "  -F ':'  Ayırıcı karakter belirt\n\n"
         "Örnekler:\n"
         "  awk '{print $1}' dosya         → İlk sütun\n"
         "  awk -F: '{print $1}' /etc/passwd → Kullanıcı adları\n"
         "  awk '$3 > 1000' dosya          → 3. sütun 1000'den büyük\n"
         "  awk '{sum += $1} END {print sum}' → Toplam hesapla\n"
         "  awk 'NR%2==0' dosya            → Çift satırlar\n"
         "  ps aux | awk '{print $1}' | sort | uniq -c → Kullanıcı bazlı süreç"},
        {"sed", "Akış düzenleyici", "sed 's/eski/yeni/g' dosya",
         "Stream Editor. Metin üzerinde otomatik düzenleme yapar.\n\n"
         "Temel komutlar:\n"
         "  s/eski/yeni/   → İlk eşleşmeyi değiştir\n"
         "  s/eski/yeni/g  → Tüm eşleşmeleri değiştir\n"
         "  s/eski/yeni/i  → Büyük/küçük harf duyarsız\n"
         "  d              → Satır sil\n"
         "  p              → Satır yazdır\n"
         "  a\\metin       → Satır sonuna ekle\n"
         "  i\\metin       → Satır başına ekle\n\n"
         "Adresleme:\n"
         "  5s/eski/yeni/  → Sadece 5. satırda\n"
         "  1,10s/eski/yeni/ → 1-10. satırlar arası\n"
         "  /pattern/s/eski/yeni/ → Desene uyan satırlarda\n"
         "  /pattern/d     → Desene uyan satırları sil\n\n"
         "Seçenekler:\n"
         "  -i  Dosyayı yerinde düzenle\n"
         "  -e  Birden fazla kural\n"
         "  -n  Sadece eşleşen satırları göster\n\n"
         "Örnekler:\n"
         "  sed 's/foo/bar/g' dosya\n"
         "  sed -i 's/eski/yeni/g' dosya\n"
         "  sed -e 's/a/b/' -e 's/c/d/' dosya"},
        {"cut", "Bölüm çıkar", "cut -d: -f1 /etc/passwd",
         "Satırlardan belirli bölümleri (sütunları) çıkarır.\n\n"
         "Seçenekler:\n"
         "  -d ':'  Ayırıcı karakter belirt\n"
         "  -f 1    1. sütunu al\n"
         "  -f 1,3  1. ve 3. sütunu al\n"
         "  -f 1-3  1'den 3'e kadar sütunları al\n"
         "  -c 1-5  1-5. karakterleri al\n"
         "  -c 1,5,10 → 1, 5 ve 10. karakterleri al\n"
         "  --complement  Seçilenleri hariç tut\n\n"
         "Örnekler:\n"
         "  cut -d: -f1 /etc/passwd        → Kullanıcı adları\n"
         "  cut -d: -f1,3 /etc/passwd      → Kullanıcı adı + UID\n"
         "  cut -c1-10 dosya.txt           → Her satırın ilk 10 karakteri\n"
         "  ps aux | cut -c1-20            → İlk 20 karakter\n\n"
         "İpucu: awk daha karmaşık işlemler için daha uygundur."},
        {"ping", "Bağlantı testi", "ping -c 4 google.com",
         "Ağ bağlantısını test eder. ICMP Echo Request paketleri gönderir.\n\n"
         "Seçenekler:\n"
         "  -c 4      4 paket gönder ve dur\n"
         "  -i 0.5    Paketler arası 0.5 saniye bekle\n"
         "  -s 1400   Paket boyutu (bayt)\n"
         "  -W 2      2 saniye zaman aşımı\n"
         "  -t TTL    Time-to-live değeri\n"
         "  -4        Sadece IPv4\n"
         "  -6        Sadece IPv6\n\n"
         "Çıktı okuma:\n"
         "  64 bytes from ... → Yanıt geldi\n"
         "  time=12.3 ms    → Gecikme (latency)\n"
         "  packet loss     → Kayıp paket yüzdesi\n\n"
         "Örnekler:\n"
         "  ping google.com          → Sürekli ping (Ctrl+C durdur)\n"
         "  ping -c 4 google.com     → 4 paket\n"
         "  ping -c 4 -i 0.2 site.com → Hızlı test\n"
         "  ping -s 1400 -c 4 site.com → Büyük paket testi\n\n"
         "İpucu: Yüksek gecikme (>100ms) veya paket kaybı ağ sorununa işaret eder."},
        {"ip", "Ağ yapılandırma", "ip addr show",
         "Linux ağ yapılandırma aracı. ifconfig'in modern ve daha güçlü alternatifidir.\n\n"
         "Alt komutlar:\n\n"
         "ip addr (Ağ adresleri):\n"
         "  ip addr show              → Tüm arayüzleri ve IP'leri göster\n"
         "  ip addr show eth0         → Sadece eth0 arayüzü\n"
         "  ip addr add 192.168.1.10/24 dev eth0 → IP ekle\n"
         "  ip addr del 192.168.1.10/24 dev eth0 → IP sil\n"
         "  ip addr flush dev eth0    → Tüm IP'leri temizle\n\n"
         "ip link (Arayüz yönetimi):\n"
         "  ip link show              → Tüm arayüzleri göster\n"
         "  ip link set eth0 up       → Arayüzü aktif et\n"
         "  ip link set eth0 down     → Arayüzü devre dışı bırak\n"
         "  ip link set eth0 mtu 9000 → MTU değiştir\n"
         "  ip link set eth0 promisc on → Promiscuous mod\n\n"
         "ip route (Yönlendirme tablosu):\n"
         "  ip route show             → Yönlendirme tablosu\n"
         "  ip route add default via 192.168.1.1 → Varsayılan ağ geçidi\n"
         "  ip route add 10.0.0.0/8 via 192.168.1.1 → Statik rota\n"
         "  ip route del 10.0.0.0/8   → Rota sil\n"
         "  ip route flush            → Tüm rotaları temizle\n\n"
         "ip neigh (ARP tablosu):\n"
         "  ip neigh show             → ARP tablosunu göster\n"
         "  ip neigh flush dev eth0   → ARP tablosunu temizle\n\n"
         "ip monitor (Canlı izleme):\n"
         "  ip monitor addr           → IP değişikliklerini izle\n"
         "  ip monitor route          → Rota değişikliklerini izle\n"
         "  ip monitor link           → Arayüz değişikliklerini izle\n\n"
         "Pratik örnekler:\n"
         "  ip -br addr show          → Kısa format (özet)\n"
         "  ip route get 8.8.8.8      → 8.8.8.8'e giden rota\n"
         "  ip -s link show eth0      → İstatistikler (bayt, paket)\n\n"
         "İpucu: `ip` komutu ifconfig'den daha hızlı ve daha fazla özellik sunar."},
        {"touch", "Dosya Oluşturur", "touch notlar.txt",
         "Dosya zaman damgasını değiştirir veya boş dosya oluşturur.\n\n"
         "Kullanım:\n"
         "  Dosya yoksa → Boş dosya oluşturur\n"
         "  Dosya varsa → Erişim ve değişiklik zamanını günceller\n\n"
         "Seçenekler:\n"
         "  -a  Sadece erişim zamanını değiştir\n"
         "  -m  Sadece değişiklik zamanını değiştir\n"
         "  -t YYYYAAGGssdd  Zaman belirt\n"
         "  -r ref_dosya  Referans dosyanın zamanını kullan\n"
         "  -c  Dosya yoksa oluşturma\n\n"
         "Örnekler:\n"
         "  touch yeni.txt              → Boş dosya oluştur\n"
         "  touch dosya.txt             → Zaman damgasını güncelle\n"
         "  touch -t 202401011200 dosya → 1 Ocak 2024 12:00\n"
         "  touch -r kaynak.txt hedef.txt → Kaynağın zamanını kopyala\n"
         "  touch *.txt                 → Tüm .txt dosyalarının zamanını güncelle\n\n"
         "İpucu: Birden fazla dosya oluşturmak için:\n"
         "  touch dosya1.txt dosya2.txt dosya3.txt"},
        {"tree", "Dizin ağacını gösterir", "tree /var/log",
         "Dizin yapısını ağaç formatında görselleştirir.\n\n"
         "Seçenekler:\n"
         "  -L 2      Maksimum 2 seviye derinlik\n"
         "  -a        Gizli dosyaları da göster\n"
         "  -d        Sadece dizinler (dosyalar yok)\n"
         "  -f        Tam yol göster\n"
         "  -h        Dosya boyutlarını göster\n"
         "  -p        İzinleri göster\n"
         "  -u        Sahip bilgisini göster\n"
         "  -g        Grup bilgisini göster\n"
         "  -s        Dosya boyutunu bayt cinsinden\n"
         "  -P '*.py'  Pattern ile filtrele\n"
         "  -I 'node_modules'  Hariç tut pattern\n"
         "  --dirsfirst  Dizinleri önce listele\n\n"
         "Örnekler:\n"
         "  tree                    → Mevcut dizin ağacı\n"
         "  tree -L 2 /home         → 2 seviye derinlik\n"
         "  tree -d /var            → Sadece dizinler\n"
         "  tree -P '*.cpp' src/    → Sadece .cpp dosyaları\n"
         "  tree -I 'build|*.o'     → build ve .o hariç\n\n"
         "Kurulum: Bazı sistemlerde kurulu gelmez.\n"
         "  sudo apt install tree"},
        {"file", "Dosya türünü öğren", "file image.png",
         "Dosyanın gerçek türünü belirler. Uzantıya değil içeriğe bakar.\n\n"
         "Nasıl çalışır?\n"
         "  Magic number (sihirli sayılar) ile dosya başlığını inceler\n"
         "  Dosya uzantısı yanıltıcı olsa bile doğru türü bulur\n\n"
         "Seçenekler:\n"
         "  -i  MIME türü göster (text/plain, image/png)\n"
         "  -b  Kısa format (dosya adı olmadan)\n"
         "  -z  Sıkıştırılmış dosya içeriğini göster\n"
         "  -L  Sembolik bağlantıyı takip et\n"
         "  -e ascii  Belirli testleri devre dışı bırak\n\n"
         "Örnekler:\n"
         "  file dosya.pdf          → PDF document, version 1.4\n"
         "  file resim.jpg          → JPEG image data, JFIF standard\n"
         "  file -i script.sh       → text/x-shellscript\n"
         "  file /dev/sda           → block special\n"
         "  file gizli_dosya        → ELF 64-bit (uzantısı .txt olsa bile)\n\n"
         "İpucu: Dosya uzantısı olmayan dosyaların türünü öğrenmek için idealdir."},
        {"stat", "Dosya Detayları", "stat dosya.txt",
         "Dosya hakkında detaylı meta veri bilgisi gösterir.\n\n"
         "Gösterilen bilgiler:\n"
         "  Dosya adı ve boyutu\n"
         "  Bloklar ve blok boyutu\n"
         "  Dosya türü (normal, dizin, sembolik bağlantı)\n"
         "  İzinler (octal ve okunabilir format)\n"
         "  Bağlar (hard link sayısı)\n"
         "  Sahip (UID) ve Grup (GID)\n"
         "  Erişim zamanı (atime) - son okuma\n"
         "  Değişiklik zamanı (mtime) - içerik değişikliği\n"
         "  Durum değişikliği (ctime) - metadata değişikliği\n"
         "  Doğum zamanı (birth) - oluşturma (destekleniyorsa)\n\n"
         "Seçenekler:\n"
         "  -f  Dosya sistemi bilgisi\n"
         "  -c 'format'  Formatlı çıktı\n"
         "  -t  Kısa format\n\n"
         "Formatlı çıktı örnekleri:\n"
         "  stat -c '%s' dosya.txt      → Sadece boyut\n"
         "  stat -c '%A' dosya.txt      → Sadece izinler\n"
         "  stat -c '%U' dosya.txt      → Sahip adı\n"
         "  stat -c '%n %s %A' dosya*   → Ad, boyut, izinler\n\n"
         "İpucu: Scriptlerde belirli bilgiyi almak için -c kullanın."},
        {"nslookup", "DNS sorgulama", "nslookup pardus.org.tr",
         "DNS sunucusundan alan adı (domain) sorgulama yapar.\n\n"
         "Kullanım:\n"
         "  nslookup alan_adi           → A kaydını sorgula\n"
         "  nslookup alan_adi dns_sunucu → Belirli DNS sunucusuna sor\n\n"
         "Kayıt türleri (interaktif mod):\n"
         "  set type=A      → IPv4 adresi\n"
         "  set type=AAAA   → IPv6 adresi\n"
         "  set type=MX     → Posta sunucuları\n"
         "  set type=NS     → Name server'lar\n"
         "  set type=CNAME  → Takma adlar\n"
         "  set type=TXT    → TXT kayıtları\n"
         "  set type=SOA    → Bölge bilgisi\n"
         "  set type=ANY    → Tüm kayıtlar\n\n"
         "Örnekler:\n"
         "  nslookup google.com\n"
         "  nslookup -type=MX gmail.com\n"
         "  nslookup google.com 8.8.8.8\n"
         "  nslookup\n"
         "  > set type=MX\n"
         "  > pardus.org.tr\n\n"
         "İpucu: dig komutu daha detaylı çıktı verir."},
        {"dig", "Gelişmiş DNS sorgulama", "dig pardus.org.tr",
         "Domain Information Groper. Detaylı ve esnek DNS sorgulama aracı.\n\n"
         "Temel kullanım:\n"
         "  dig alan_adi          → A kaydını sorgula\n"
         "  dig @dns_sunucu alan_adi → Belirli DNS'e sor\n"
         "  dig -t MX alan_adi    → MX kaydı sorgula\n\n"
         "Kayıt türleri:\n"
         "  -t A       → IPv4 adresi\n"
         "  -t AAAA    → IPv6 adresi\n"
         "  -t MX      → Posta sunucuları\n"
         "  -t NS      → Name server'lar\n"
         "  -t CNAME   → Takma ad\n"
         "  -t TXT     → TXT kayıtları (SPF, DKIM)\n"
         "  -t SOA     → Bölge yetkilisi\n"
         "  -t ANY     → Tüm kayıtlar\n\n"
         "Seçenekler:\n"
         "  +short     → Sadece sonuç (kısa çıktı)\n"
         "  +noall +answer → Sadece cevap bölümü\n"
         "  +trace     → Kökten itibaren izleme\n"
         "  -x IP      → Ters DNS (IP → alan adı)\n"
         "  +time=N    → Zaman aşımı (saniye)\n\n"
         "Örnekler:\n"
         "  dig google.com +short\n"
         "  dig -t MX gmail.com\n"
         "  dig @8.8.8.8 google.com\n"
         "  dig -x 8.8.8.8          → Ters DNS\n"
         "  dig google.com +trace   → DNS zincirini izle\n"
         "  dig +noall +answer google.com → Temiz çıktı"},
        {"useradd", "Yeni Kullanıcı ekleme", "useradd mehmet",
          "Sistemde yeni kullanıcı hesabı oluşturur.\n\n"
          "Seçenekler:\n"
          "  -m  Ev dizini otomatik oluştur\n"
          "  -s /bin/bash  Varsayılan kabuk belirt\n"
          "  -g ana_grup  Ana grup belirt\n"
          "  -G grup1,grup2  Ek gruplar ekle\n"
          "  -c 'Yorum'  Açıklama (gerçek ad vb.)\n"
          "  -e YYYY-AA-GG  Hesap bitiş tarihi\n"
          "  -d /özel/yol  Özel ev dizini\n"
          "  -u UID  Belirli UID ata\n"
          "  -r  Sistem kullanıcısı oluştur\n\n"
          "Kullanıcı oluşturduktan sonra:\n"
          "  passwd mehmet  → Şifre belirle\n\n"
          "Örnekler:\n"
          "  useradd -m -s /bin/bash mehmet\n"
          "  useradd -m -s /bin/bash -c 'Mehmet Yılmaz' mehmet\n"
          "  useradd -m -G sudo,users -s /bin/bash mehmet\n"
          "  useradd -r -s /usr/sbin/nologin servis\n\n"
          "İlgili komutlar:\n"
          "  userdel mehmet     → Kullanıcı sil\n"
          "  usermod -aG grup kullanıcı → Gruba ekle\n"
          "  passwd mehmet      → Şifre değiştir\n"
          "  chsh -s /bin/zsh kullanıcı → Kabuk değiştir"},
        {"basename", "Dosya adını göster", "basename /home/kullanici/belge.txt",
         "Bir dosya yolunun sadece dosya adı kısmını gösterir. Uzantıyı da isteğe bağlı kırpar.\n\n"
         "Seçenekler:\n"
         "  -a  Birden fazla yolu işle\n"
         "  -s .txt  Uzantıyı kırp\n"
         "  -z  Null ile sonlandır (find -print0 ile kullanım)\n\n"
         "Örnekler:\n"
         "  basename /usr/bin/ls              → ls\n"
         "  basename /var/log/syslog          → syslog\n"
         "  basename dosya.tar.gz .tar.gz     → dosya\n"
         "  basename -s .txt belge.txt rapor.txt → belge rapor\n\n"
         "İpucu: dirname ile ters işlemi yapar."},
        {"dirname", "Dizin yolunu göster", "dirname /home/kullanici/belge.txt",
         "Bir dosya yolunun sadece dizin kısmını gösterir.\n\n"
         "Seçenekler:\n"
         "  -z  Null ile sonlandır\n"
         "  -v  Her satırı detaylı göster (verbose)\n\n"
         "Örnekler:\n"
         "  dirname /usr/bin/ls              → /usr/bin\n"
         "  dirname /var/log/syslog          → /var/log\n"
         "  dirname /etc/nginx/nginx.conf    → /etc/nginx\n\n"
         "Pratik: cd $(dirname $(which python))  → Python dizinine git"},
        {"xargs", "Argümanları ilet", "find . -name '*.log' | xargs rm",
         "Standart girdiden (stdin) alınan verileri argüman olarak başka bir komuta iletir.\n\n"
         "Seçenekler:\n"
         "  -p  Her komuttan önce onay sor\n"
         "  -t  Komutları çalıştırmadan önce yazdır\n"
         "  -I {}  Yer tutucu belirt (dosya adı için)\n"
         "  -n 1  Her çağrıda 1 argüman kullan\n"
         "  -0  Null ile ayrılmış girdi (find -print0 ile)\n"
         "  --max-procs=4  Paralel çalıştırma\n\n"
         "Örnekler:\n"
         "  find . -name '*.bak' | xargs rm -f\n"
         "  ls *.txt | xargs -I {} mv {} /yedek/{}\n"
         "  cat url_list.txt | xargs -n1 curl -O\n"
         "  seq 1 10 | xargs -n1 echo 'Sayı:'\n\n"
         "İpucu: Dosya adlarında boşluk varsa -0 kullanın."},
        {"tee", "Çıktıyı böl", "ls -la | tee dosya_listesi.txt",
         "Komut çıktısını hem dosyaya yazar hem de ekranda gösterir. Boru hattını ikiye ayırır.\n\n"
         "Seçenekler:\n"
         "  -a  Dosyaya ekle (üzerine yazma)\n"
         "  -i  Ctrl+C sinyalini yok say\n"
         "  -p  Dosya yazma hatalarında uyar\n\n"
         "Örnekler:\n"
         "  echo 'Merhaba' | tee mesaj.txt\n"
         "  ping google.com | tee -a ping_log.txt\n"
         "  echo 'HOSTNAME,IP' | tee rapor.csv > /dev/null\n"
         "  ./script.sh | tee çıktı.log  → Çıktıyı hem izle hem kaydet\n\n"
         "İpucu: tee, shell scriptlerinde log tutmak için idealdir."},
        {"watch", "Periyodik çalıştır", "watch -n 2 df -h",
         "Bir komutu belirli aralıklarla tekrar çalıştırır ve çıktıyı canlı gösterir.\n\n"
         "Seçenekler:\n"
         "  -n 2    2 saniyede bir çalıştır\n"
         "  -d      Değişen satırları vurgula\n"
         "  -t      Başlık satırını gizle\n"
         "  -e      Hata durumunda don (freeze)\n"
         "  -b      Sadece çıktı değişirse bildir\n\n"
         "Örnekler:\n"
         "  watch -n 1 free -h           → Bellek değişimi\n"
         "  watch -d 'ls -la'            → Dosya değişikliklerini izle\n"
         "  watch -n 5 'ss -tlnp'        → Port değişiklikleri\n"
         "  watch 'ps aux | sort -nrk 4' → En çok RAM yiyenler\n\n"
         "İpucu: Ctrl+C ile durana kadar çalışır."},
        {"rsync", "Gelişmiş senkronizasyon", "rsync -av /kaynak/ /hedef/",
         "Dosya ve dizinleri akıllıca senkronize eder. Sadece değişen kısımları aktarır.\n\n"
         "Seçenekler:\n"
         "  -a      Arşiv modu (izin, sahiplik, sembolik link koru)\n"
         "  -v      Detaylı çıktı\n"
         "  -z      Sıkıştırarak aktar\n"
         "  -P      İlerleme göstergesi + kaldığından devam\n"
         "  --delete Hedefte olup kaynakta olmayanı sil\n"
         "  --exclude='*.tmp'  Belirli dosyaları hariç tut\n"
         "  -e 'ssh -p 2222'  SSH üzerinden özel port ile\n"
         "  --dry-run  Ne yapılacağını göster (gerçekten yapma)\n\n"
         "Örnekler:\n"
         "  rsync -avz /proje/ kullanici@sunucu:/yedek/\n"
         "  rsync -av --delete /kaynak/ /hedef/\n"
         "  rsync -av --exclude='node_modules' /proje/ /yedek/\n\n"
         "İpucu: scp'den daha hızlı ve akıllıdır."},
        {"cron", "Zamanlanmış görev", "crontab -e",
         "Belirli zamanlarda otomatik çalışması için görev planlar. Tarayıcıda çalışır.\n\n"
         "CRON formatı:\n"
         "  ┌───────── dakika (0-59)\n"
         "  │ ┌───────── saat (0-23)\n"
         "  │ │ ┌───────── gün (1-31)\n"
         "  │ │ │ ┌───────── ay (1-12)\n"
         "  │ │ │ │ ┌───────── haftanın günü (0-6, 0=Pazar)\n"
         "  * * * * * komut\n\n"
         "Komutlar:\n"
         "  crontab -e        → Zamanlayıcıyı düzenle\n"
         "  crontab -l        → Görevleri listele\n"
         "  crontab -r        → Tüm görevleri sil\n"
         "  crontab -u kullanıcı  → Başka kullanıcının cron'u (root)\n\n"
         "Örnekler:\n"
         "  0 9 * * * apt update          → Her gün 09:00'da\n"
         "  */30 * * * * /script.sh       → 30 dakikada bir\n"
         "  0 0 * * 1 logrotate           → Her Pazartesi 00:00\n"
         "  @reboot /script.sh            → Açılışta çalıştır\n\n"
         "İpucu: Çıktılar mail ile gönderilir. > /dev/null ile susturun."},
        {"lsblk", "Blok cihaz listele", "lsblk",
         "Diskler ve bölümler hakkında bilgi gösterir. Ağaç yapısıyla görselleştirir.\n\n"
         "Seçenekler:\n"
         "  -f  Dosya sistemi bilgisini göster\n"
         "  -m  Sahiplik ve izinleri göster\n"
         "  -l  Liste formatında (ağaç değil)\n"
         "  -t  Topoloji (sıra, döndürme hızı)\n"
         "  -d  Sadece diskler (bölümler yok)\n"
         "  -o ADI,SIZE,TIPO  Özel sütun seçimi\n"
         "  -p  Tam yol göster (/dev/sda1)\n"
         "  -J  JSON çıktı\n\n"
         "Örnekler:\n"
         "  lsblk                  → Tüm diskler\n"
         "  lsblk -f               → Hangi disk hangi format\n"
         "  lsblk -o ADI,BOYUT,TIP,NO  → Özel görünüm\n"
         "  lsblk -d -o ADI,SIZE   → Sadece diskler\n\n"
         "İpucu: fdisk /dev/sda ile detaylı bölüm düzenleme."},
        {"lsof", "Açık dosyalar", "lsof -i :80",
         "Hangi sürecin hangi dosyaları açtığını listeler. Linux'ta her şey dosyadır!\n\n"
         "Seçenekler:\n"
         "  -i :port     Belirli portu kullanan süreçler\n"
         "  -i TCP       TCP bağlantıları\n"
         "  -u kullanıcı  Kullanıcıya göre filtrele\n"
         "  -p PID       Belirli PID'nin dosyaları\n"
         "  +D /dizin    Dizindeki açık dosyalar\n"
         "  -t           Sadece PID'leri göster (script için)\n"
         "  -n           DNS çözümlemesi yapma (hızlı)\n"
         "  -P           Port isimlerini çözme\n\n"
         "Örnekler:\n"
         "  lsof | wc -l           → Açık dosya sayısı\n"
         "  lsof -i :443           → 443'ü kim kullanıyor\n"
         "  lsof -u kullanici      → Kullanıcının açık dosyaları\n"
         "  lsof /var/log/syslog   → Bu dosyayı kim kullanıyor\n\n"
         "İpucu: `lsof -i -P -n` ile hızlı ağ bağlantısı sorgusu."},
        {"ss", "Soket istatistiği", "ss -tlnp",
         "Ağ soketlerini ve bağlantıları gösterir. netstat'in modern, daha hızlı alternatifidir.\n\n"
         "Seçenekler:\n"
         "  -t  TCP soketleri\n"
         "  -u  UDP soketleri\n"
         "  -l  Dinlenen (listening) portlar\n"
         "  -n  Sayısal adres (DNS çözümlemesi yapma)\n"
         "  -p  Süreç bilgisi (PID/İsim)\n"
         "  -a  Tüm soketler (dinlenen + bağlı)\n"
         "  -s  İstatistik özeti\n"
         "  -4  Sadece IPv4\n"
         "  -6  Sadece IPv6\n"
         "  -e  Detaylı bilgi\n"
         "  -o  Zamanlayıcı bilgisi\n\n"
         "Örnekler:\n"
         "  ss -tlnp             → TCP dinlenen portlar + süreçler\n"
         "  ss -tup              → Tüm bağlı TCP/UDP\n"
         "  ss -s                → Bağlantı istatistikleri\n"
         "  ss -tlnp | grep 80   → 80'i kim dinliyor\n\n"
         "İpucu: netstat yerine ss kullanın. Daha hızlı ve güncel."},
        {"dpkg", "Debian paket yönetimi", "dpkg -i paket.deb",
         "Debian paketlerini (.deb) yönetmek için alt seviye paket yöneticisi.\n\n"
         "Temel komutlar:\n"
         "  dpkg -i paket.deb          → Paket kur\n"
         "  dpkg -r paket              → Paket kaldır (config kalır)\n"
         "  dpkg -P paket              → Paket ve config sil\n"
         "  dpkg -l                    → Tüm kurulu paketleri listele\n"
         "  dpkg -l | grep anahtar     → Paket ara\n"
         "  dpkg -s paket              → Paket durumu\n"
         "  dpkg -L paket              → Hangi dosyaları kurdu\n"
         "  dpkg -S /yol/dosya         → Dosya hangi paketten\n"
         "  dpkg --configure -a        → Yarı kurulu paketleri tamamla\n"
         "  dpkg-reconfigure paket     → Paketi yeniden yapılandır\n\n"
         "Örnekler:\n"
         "  dpkg -L nano | grep bin    → nano'nun binary'leri\n"
         "  dpkg -S /bin/ls            → ls hangi paketten\n"
         "  dpkg -l | grep ^ii | wc -l → Kurulu paket sayısı\n\n"
         "İpucu: apt daha kullanışlıdır, dpkg bağımlılıkları çözmez."},
        {"jq", "JSON işleme", "jq '.kullanicilar[].adi' data.json",
         "JSON verisini terminalde işlemek, filtrelemek ve dönüştürmek için hafif ve güçlü bir araç.\n\n"
         "Temel filtreler:\n"
         "  .           → Tüm nesne\n"
         "  .anahtar    → Belirli anahtar\n"
         "  .[].anahtar → Dizideki her öğenin anahtarı\n"
         "  .[0:2]      → İlk 2 eleman\n"
         "  select(.y > 10)  → Koşullu filtre\n\n"
         "Seçenekler:\n"
         "  -r      Raw çıktı (tırnak işareti olmadan)\n"
         "  -c      Compact (tek satırda)\n"
         "  -f dosya  Dosyadan jq programı oku\n"
         "  --arg k d  Değişken ata\n\n"
         "Örnekler:\n"
         "  curl api.site.com | jq '.'                  → Formatlı göster\n"
         "  cat data.json | jq '.products[].name'       → Tüm ürün adları\n"
         "  jq 'group_by(.kategori) | length' data.json → Gruplama\n"
         "  jq '.[] | select(.stok < 10)' data.json     → Kritik stok\n\n"
         "İpucu: API'lerden gelen JSON'u analiz etmek için mükemmel."},
        {"strace", "Sistem çağrılarını izle", "strace -e open,read ls",
         "Bir programın yaptığı tüm sistem çağrılarını (system calls) izler. Hata ayıklama için vazgeçilmezdir.\n\n"
         "Seçenekler:\n"
         "  -e syscall   Sadece belirli çağrıları göster\n"
         "  -p PID       Çalışan sürece bağlan\n"
         "  -f           Alt süreçleri de izle (fork)\n"
         "  -c           İstatistik özeti\n"
         "  -t           Zaman damgası ekle\n"
         "  -T           Her çağrının süresini göster\n"
         "  -o dosya     Çıktıyı dosyaya yaz\n"
         "  -s 100       Maksimum string uzunluğu\n"
         "  -e trace=network  Sadece ağ çağrıları\n"
         "  -e trace=file     Sadece dosya çağrıları\n\n"
         "Örnekler:\n"
         "  strace ls                   → ls'in tüm çağrıları\n"
         "  strace -e open ls /home     → Sadece open çağrıları\n"
         "  strace -p 1234              → 1234 PID'sini izle\n"
         "  strace -c firefox           → İstatistik topla\n\n"
         "İpucu: Program neden çöküyor? strace ile öğrenin!"},
        {"nc", "Ağ aracı", "nc -v google.com 80",
         "Netcat. Ağ bağlantıları için İsviçre çakısıdır. Port tarama, dosya transferi, sohbet.\n\n"
         "Seçenekler:\n"
         "  -v      Detaylı çıktı (verbose)\n"
         "  -l      Dinleme modu (server)\n"
         "  -p port  Port belirt\n"
         "  -n      DNS çözümlemesi yapma\n"
         "  -z      Sadece port tara (veri gönderme)\n"
         "  -w 5    Zaman aşımı (saniye)\n"
         "  -u      UDP (varsayılan: TCP)\n"
         "  -4      Sadece IPv4\n\n"
         "Örnekler:\n"
         "  nc -zv 192.168.1.1 1-1000 → Port tara\n"
         "  echo 'GET /' | nc google.com 80\n"
         "  nc -l -p 1234             → Sohbet server'ı\n"
         "  nc 192.168.1.10 1234      → Sohbet client'ı\n"
         "  tar -czf - /dizin | nc sunucu 9000 → Dosya transferi\n\n"
         "İpucu: netcat ile basit bir web sunucusu bile yapabilirsiniz."},
        {"tcpdump", "Ağ trafiği yakala", "tcpdump -i eth0 port 80",
         "Ağ trafiğini gerçek zamanlı yakalar ve analiz eder. Ağ sorunlarını bulmak için kullanılır.\n\n"
         "Seçenekler:\n"
         "  -i eth0    Arayüz belirt\n"
         "  -n         DNS çözümlemesi yapma\n"
         "  -X         Hex + ASCII göster\n"
         "  -A         Sadece ASCII (HTTP içerikleri)\n"
         "  -c 100     100 paket yakala ve dur\n"
         "  -w dosya   Ham çıktıyı dosyaya kaydet\n"
         "  -r dosya   Kaydedilmiş dosyayı oku\n"
         "  -s 0       Tüm paketi yakala (varsayılan: 68 bayt)\n\n"
         "Filtre örnekleri:\n"
         "  port 80                   → HTTP trafiği\n"
         "  host 192.168.1.1          → Belirli IP\n"
         "  src net 192.168.1.0/24    → Kaynak ağ\n"
         "  tcp and port 443          → HTTPS trafiği\n"
         "  not arp and not icmp      → ARP ve ping hariç\n\n"
         "Örnekler:\n"
         "  tcpdump -i eth0 -A port 80 | grep 'GET|POST|Host:'\n"
         "  tcpdump -i any -c 1000 -w trafik.pcap\n"
         "  tcpdump -r trafik.pcap port 443\n\n"
         "İpucu: Wireshark ile .pcap dosyalarını görsel analiz edin."},
        {"gpg", "Şifreleme", "gpg -c dosya.txt",
         "GNU Privacy Guard. Dosya ve mesaj şifreleme/imzalama aracı. Açık kaynak PGP uyarlamasıdır.\n\n"
         "Şifreleme:\n"
         "  gpg -c dosya.txt        → Simetrik şifrele (parola sorar)\n"
         "  gpg -e -a kullanıcı     → Açık anahtarla şifrele\n"
         "  gpg -d dosya.gpg        → Şifre çöz\n\n"
         "Anahtar yönetimi:\n"
         "  gpg --gen-key           → Yeni anahtar oluştur\n"
         "  gpg --list-keys         → Anahtarları listele\n"
         "  gpg --export -a 'Ad'    → Açık anahtarı dışa aktar\n"
         "  gpg --import anahtar.asc→ Anahtar içe aktar\n"
         "  gpg --keyserver keyserver.ubuntu.com --search-keys 'ad'\n\n"
         "İmzalama:\n"
         "  gpg --sign dosya        → İmzala (binary)\n"
         "  gpg --clearsign dosya   → İmzala (metin)\n"
         "  gpg --verify dosya.sig  → İmza doğrula\n\n"
         "Örnekler:\n"
         "  gpg -c gizli.txt          → Parola ile şifrele\n"
         "  gpg -d gizli.txt.gpg      → Çöz\n"
         "  gpg --encrypt --recipient ali@ornek.com dosya.txt\n\n"
         "İpucu: Şifrelenmiş yedekleme için idealdir."},
        {"openssl", "SSL/TLS aracı", "openssl s_client -connect google.com:443",
         "SSL/TLS protokolü ve çeşitli kriptografik işlemler için kapsamlı araç seti.\n\n"
         "Sertifika işlemleri:\n"
         "  openssl s_client -connect site.com:443  → Sertifika bilgisi\n"
         "  openssl x509 -in cert.pem -text -noout  → Sertifika oku\n"
         "  openssl s_client -showcerts -connect site.com:443 < /dev/null\n\n"
         "Anahtar ve CSR:\n"
         "  openssl genrsa -out key.pem 2048         → RSA anahtar üret\n"
         "  openssl req -new -key key.pem -out csr.pem → CSR oluştur\n"
         "  openssl req -x509 -days 365 -key key.pem -out cert.pem → Self-signed sertifika\n\n"
         "Hash ve şifreleme:\n"
         "  echo -n 'metin' | openssl dgst -sha256   → SHA-256 hash\n"
         "  openssl enc -aes-256-cbc -salt -in dosya -out dosya.enc → Şifrele\n"
         "  openssl enc -d -aes-256-cbc -in dosya.enc -out dosya → Çöz\n\n"
         "Rastgele veri:\n"
         "  openssl rand -hex 32  → 32 bayt rastgele (hex)\n"
         "  openssl rand -base64 16 → 16 bayt rastgele (base64)\n\n"
         "Örnekler:\n"
         "  openssl s_client -connect github.com:443 -tls1_3\n"
         "  openssl speed aes-256-cbc  → AES hız testi\n\n"
         "İpucu: openssl version ile sürümü kontrol edin."},
        {"git", "Sürüm kontrolü", "git status",
         "Dağıtık sürüm kontrol sistemi. kod değişikliklerini takip etmek ve ekiplerle paylaşmak için standart araç.\n\n"
         "Temel komutlar:\n"
         "  git init                  → Yeni repo başlat\n"
         "  git clone url            → Repo kopyala\n"
         "  git status               → Değişiklikleri göster\n"
         "  git add dosya            → Dosyayı stage ekle\n"
         "  git add .                → Tüm dosyaları stage ekle\n"
         "  git commit -m 'mesaj'    → Commit oluştur\n"
         "  git push origin main     → Uzaktaki repoya gönder\n"
         "  git pull                 → Uzaktaki repodan çek\n"
         "  git log --oneline        → Commit geçmişi (kısa)\n\n"
         "Dal (branch) işlemleri:\n"
         "  git branch               → Dalları listele\n"
         "  git branch yeni-dal      → Yeni dal oluştur\n"
         "  git checkout dal         → Dal değiştir\n"
         "  git merge dal            → Dalı birleştir\n"
         "  git branch -d dal        → Dal sil\n\n"
         "İleri seviye:\n"
         "  git diff                 → Değişiklikleri karşılaştır\n"
         "  git reset --hard HEAD    → Son commit'e dön\n"
         "  git stash                → Değişiklikleri geçici sakla\n"
         "  git remote -v            → Uzak repo adresleri\n"
         "  git tag v1.0             → Sürüm etiketi ekle\n\n"
         "Örnekler:\n"
         "  git add . && git commit -m 'güncelleme'\n"
         "  git log --oneline --graph --all\n"
         "  git diff --cached        → Stage edilmiş değişiklikler\n\n"
         "İpucu: .gitignore ile gereksiz dosyaları repodan hariç tutun."},
        {"make", "Derleme otomasyonu", "make",
         "Projeleri derlemek için otomasyon aracı. Makefile dosyasındaki kurallara göre çalışır.\n\n"
         "Temel kullanım:\n"
         "  make              → İlk hedefi derle\n"
         "  make all          → Tüm hedefleri derle\n"
         "  make clean        → Derleme dosyalarını temizle\n"
         "  make install      → Programı sisteme kur\n"
         "  make uninstall    → Kaldır\n"
         "  make test         → Testleri çalıştır\n\n"
         "Seçenekler:\n"
         "  -j4           4 iş parçacığı ile paralel derle\n"
         "  -f dosya      Farklı Makefile adı belirt\n"
         "  -n            Ne yapılacağını göster (çalıştırma)\n"
         "  -B            Tüm dosyaları yeniden derle\n"
         "  -C /dizin     Dizine git ve çalıştır\n\n"
         "Pratik değişkenler:\n"
         "  make CC=gcc CFLAGS='-O2'     → Derleyici belirt\n"
         "  make DESTDIR=/tmp/prefix install → Farklı yere kur\n\n"
         "Örnekler:\n"
         "  make -j$(nproc)          → Tüm çekirdeklerle derle\n"
         "  make clean && make       → Temiz derleme\n"
         "  make -n                  → Ne yapacak? göster\n\n"
         "İpucu: make -j$(nproc) ile derlemeyi hızlandırın."},
        {"which", "Komut yolunu bul", "which python3",
         "Bir komutun hangi dizinde olduğunu (tam yolunu) gösterir. PATH'te arar.\n\n"
         "Seçenekler:\n"
         "  -a        Tüm eşleşmeleri göster (ilk değil)\n"
         "  -s        Sessiz mod (çıktı yok, sadece çıkış kodu)\n\n"
         "Örnekler:\n"
         "  which bash              → /usr/bin/bash\n"
         "  which -a python         → Varsa tüm Python yolları\n"
         "  which gcc || echo 'Kurulu değil'\n\n"
         "İlgili komutlar:\n"
         "  whereis python3  → Binary, kaynak ve man sayfası\n"
         "  type ls          → Komut türünü göster\n"
         "  command -v ls    → POSIX uyumlu yol bulma\n\n"
         "İpucu: which size gerçek yolu söyler, type ise alias'ları da gösterir."},
        {"whereis", "Binary, kaynak ve man bul", "whereis python3",
         "Bir komutun binary, kaynak kod ve man sayfası dosyalarının yerini gösterir.\n\n"
         "Seçenekler:\n"
         "  -b        Sadece binary ara\n"
         "  -m        Sadece man sayfası ara\n"
         "  -s        Sadece kaynak kod ara\n"
         "  -B /dizin  Binary aranacak dizini belirt\n"
         "  -M /dizin  Man aranacak dizini belirt\n\n"
         "Örnekler:\n"
         "  whereis ls                → /usr/bin/ls /usr/share/man/man1/ls.1.gz\n"
         "  whereis -b python3        → Sadece binary\n"
         "  whereis -m git            → Sadece man sayfası\n"
         "  whereis nginx             → Nginx'in tüm dosyaları\n\n"
         "İpucu: which sadece binary yolunu, whereis daha fazlasını gösterir."},
        {"mount", "Dosya sistemi bağla", "mount /dev/sdb1 /mnt/usb",
         "Bir dosya sistemini (disk, bölüm, USB) belirli bir dizine bağlar (mount).\n\n"
         "Seçenekler:\n"
         "  -t vfat    Dosya sistemi türü belirt\n"
         "  -o ro      Salt okunur bağla\n"
         "  -o rw      Okuma-Yazma bağla\n"
         "  -o uid=1000  Sahipliği belirt\n"
         "  -o noexec  Çalıştırma iznini engelle\n"
         "  -a         /etc/fstab'daki tüm dosya sistemlerini bağla\n\n"
         "İlgili komutlar:\n"
         "  umount /mnt/usb          → Bağlı dosya sistemini çıkar\n"
         "  mount | column -t        → Tüm bağlı birimler (düzenli)\n"
         "  findmnt                  → Ağaç görünümü\n"
         "  df -h                    → Disk kullanımı\n\n"
         "Örnekler:\n"
         "  sudo mount /dev/sdb1 /mnt/usb\n"
         "  sudo mount -t iso9660 -o ro /dev/sr0 /mnt/cdrom\n"
         "  sudo mount -o uid=kullanici,gid=users /dev/sdb1 /mnt\n\n"
         "İpucu: Çıkarmadan önce mutlaka umount yapın!"},
        {"umount", "Dosya sistemi çıkar", "umount /mnt/usb",
         "Bağlı bir dosya sistemini güvenle çıkartır. Veri kaybını önlemek için çıkarmadan önce çalıştırılmalı.\n\n"
         "Seçenekler:\n"
         "  -a        Tüm dosya sistemlerini çıkar\n"
         "  -f        Zorla çıkar (meşgul dosya sistemi)\n"
         "  -l        Tembel çıkar (meşgul olmayınca)\n"
         "  -R        Alt bağlantıları da çıkar\n\n"
         "Sorun giderme:\n"
         "  umount: target is busy → Bir süreç kullanıyor\n"
         "  lsof /mnt/usb          → Hangi süreç? bul\n"
         "  fuser -km /mnt/usb     → Tüm süreçleri sonlandır\n\n"
         "Örnekler:\n"
         "  sudo umount /mnt/usb\n"
         "  sudo umount -l /mnt/usb  → Meşgulse zorla çıkar\n"
         "  sudo umount -a           → Tümünü çıkar (shutdown)\n\n"
         "İpucu: umount etmeden USB çekmeyin, veri kaybedebilirsiniz!"},
        {"uptime", "Sistem çalışma süresi", "uptime",
         "Sistemin ne kadar süredir çalıştığını, kullanıcı sayısını ve yük ortalamasını gösterir.\n\n"
         "Çıktı:\n"
         "  up 3 days, 2:15  → 3 gün 2 saat 15 dakikadır açık\n"
         "  3 users          → 3 kullanıcı oturum açmış\n"
         "  load average: 1.2, 0.8, 0.5\n\n"
         "Yük ortalaması (load average):\n"
         "  1.2 (son 1 dk)  0.8 (son 5 dk)  0.5 (son 15 dk)\n"
         "  Değer CPU çekirdek sayısına göre yorumlanır:\n"
         "  4 çekirdek için 4.0 = tam yük, >4 = aşırı yük\n\n"
         "Seçenek:\n"
         "  -p          İnsan okunabilir format (pretty)\n"
         "  -s          Sistemin başlatıldığı zamanı göster\n\n"
         "İlgili komutlar:\n"
         "  w           → uptime + kullanıcı aktiviteleri\n"
         "  top/htop    → Detaylı sistem yükü\n\n"
         "Örnekler:\n"
         "  uptime                  → 10:15 up 5 days, 2 users, load: 0.5\n"
         "  uptime -p               → up 5 days, 2 hours\n"
         "  uptime -s               → 2026-06-19 08:30:15"},
        {"hostname", "Makine adı göster", "hostname",
         "Sistemin ağ üzerindeki adını (hostname) gösterir veya değiştirir.\n\n"
         "Seçenekler:\n"
         "  -I        Tüm IP adreslerini göster\n"
         "  -i        IP adresini göster\n"
         "  -d        DNS alan adını göster\n"
         "  -f        Tam nitelikli alan adı (FQDN)\n"
         "  -A        Tüm FQDN adlarını göster\n"
         "  -s        Kısa ad (ilk noktaya kadar)\n"
         "  -y        NIS/YP alan adı\n\n"
         "Kalıcı değiştirme:\n"
         "  sudo hostnamectl set-hostname yeni-ad\n"
         "  → /etc/hostname dosyasını günceller\n\n"
         "Örnekler:\n"
         "  hostname          → pardus-laptop\n"
         "  hostname -I       → 192.168.1.10 10.0.0.5\n"
         "  hostname -f       → pardus-laptop.ev.ağı\n"
         "  hostname -i       → 127.0.1.1\n\n"
         "İpucu: hostname -I, ifconfig'e gerek kalmadan IP'yi gösterir."},
        {"printenv", "Ortam değişkenlerini göster", "printenv USER",
         "Tüm ortam değişkenlerini (environment variables) veya belirli bir değişkeni gösterir.\n\n"
         "Önemli ortam değişkenleri:\n"
         "  HOME       → Kullanıcının ev dizini (/home/kullanici)\n"
         "  USER       → Kullanıcı adı\n"
         "  PATH       → Komut arama yolları\n"
         "  SHELL      → Varsayılan kabuk (/bin/bash)\n"
         "  PWD        → Geçerli çalışma dizini\n"
         "  LANG       → Dil ve bölge ayarı\n"
         "  TERM       → Terminal türü (xterm-256color)\n"
         "  EDITOR     → Varsayılan metin düzenleyici\n\n"
         "İlgili komutlar:\n"
         "  env         → printenv ile aynı\n"
         "  set         → Tüm shell değişkenlerini gösterir\n"
         "  export AD=değer  → Yeni değişken tanımla\n"
         "  unset AD    → Değişkeni kaldır\n\n"
         "Örnekler:\n"
         "  printenv PATH              → /usr/bin:/bin:/usr/local/bin\n"
         "  printenv | grep -i proxy   → Proxy ayarlarını bul\n"
         "  export EDITOR=nano         → Varsayılan editör değiştir\n\n"
         "İpucu: printenv ile PATH'te hangi dizinler olduğunu görebilirsiniz."},
        {"type", "Komut türünü göster", "type ls",
         "Bir komutun türünü gösterir: alias, builtin, fonksiyon veya harici program.\n\n"
         "Komut türleri:\n"
         "  alias     → Kullanıcı tanımlı kısayol\n"
         "  builtin   → Kabuğun içindeki komut (cd, echo)\n"
         "  file      → Harici program (/usr/bin/ls)\n"
         "  function  → Shell fonksiyonu\n\n"
         "Seçenekler:\n"
         "  -t        Sadece türü göster (alias/builtin/file/function)\n"
         "  -p        Sadece yol göster (which gibi)\n"
         "  -a        Tüm eşleşmeleri göster\n"
         "  -f        Fonksiyon tanımını gizle\n\n"
         "Örnekler:\n"
         "  type ls          → ls is aliased to 'ls --color=auto'\n"
         "  type -t cd       → builtin\n"
         "  type -t python3  → file\n"
         "  type -a echo     → echo is a shell builtin\n\n"
         "İpucu: Bir komut nereden geliyor? type cevaplar!"},
        {"sensors", "Donanım sensörleri", "sensors",
         "CPU sıcaklığı, fan hızı, voltaj ve diğer donanım sensör değerlerini okur.\n\n"
         "Seçenekler:\n"
         "  -u        Ham değerleri göster\n"
         "  -j        JSON formatında çıktı\n"
         "  -A        Adaptör satırlarını gizle\n\n"
         "Örnek çıktı:\n"
         "  coretemp-isa-0000\n"
         "  Package id 0:  +45.0°C (high = +80.0°C, crit = +100.0°C)\n"
         "  Core 0:        +42.0°C\n"
         "  Core 1:        +44.0°C\n\n"
         "İzleme:\n"
         "  watch -n 2 sensors        → Canlı sıcaklık takibi\n"
         "  sensors | grep Core       → Sadece çekirdek sıcaklıkları\n\n"
         "Kurulum:\n"
         "  sudo apt install lm-sensors\n"
         "  sudo sensors-detect       → Sensörleri algıla\n\n"
         "İpucu: Aşırı ısınma durumunda fan temizliği veya macun yenileme gerekebilir."},
        {"sysctl", "Kernel parametreleri", "sysctl net.ipv4.ip_forward",
         "Linux çekirdek (kernel) parametrelerini okur ve çalışma anında değiştirir.\n\n"
         "Kullanım:\n"
         "  sysctl -a                    → Tüm parametreleri listele\n"
         "  sysctl parametre             → Belirli parametreyi oku\n"
         "  sysctl -w parametre=değer    → Parametre değiştir\n"
         "  sysctl -p                    → /etc/sysctl.conf'u yükle\n\n"
         "Önemli parametreler:\n"
         "  net.ipv4.ip_forward=1       → IP yönlendirme (router)\n"
         "  net.ipv6.conf.all.disable_ipv6=1  → IPv6 kapat\n"
         "  vm.swappiness=10            → Takas kullanımını azalt\n"
         "  kernel.hostname=pardus      → Hostname değiştir\n"
         "  fs.file-max=100000          → Maksimum dosya açma limiti\n\n"
         "Örnekler:\n"
         "  sysctl net.ipv4.ip_forward       → Oku: 0 veya 1\n"
         "  sudo sysctl -w net.ipv4.ip_forward=1  → Aktifleştir\n"
         "  sudo sysctl -p                    → Config dosyasını yükle\n\n"
         "İpucu: Değişiklikler kalıcı olsun istiyorsanız /etc/sysctl.conf veya /etc/sysctl.d/'a yazın."},
        {"nice", "Öncelikle çalıştır", "nice -n 10 ./script.sh",
         "Bir programı belirli bir öncelik (priority) değeriyle çalıştırır. Düşük öncelik = daha az CPU.\n\n"
         "Öncelik değerleri (niceness):\n"
         "  -20 (en yüksek öncelik)  → +19 (en düşük öncelik)\n"
         "  Varsayılan: 0\n"
         "  Sadece root düşük değer (-) verebilir\n\n"
         "İlgili komut:\n"
         "  renice -n 5 -p PID      → Çalışan sürecin önceliğini değiştir\n"
         "  renice -n -5 -u kullanici → Kullanıcının tüm süreçleri\n"
         "  top (PR ve NI sütunları) → Öncelikleri gör\n\n"
         "Örnekler:\n"
         "  nice -n 19 ./arkaplan.sh        → Arka planda düşük öncelik\n"
         "  nice -n -10 make -j4            → Daha yüksek öncelik (root)\n"
         "  renice -n 10 -p 1234            → PID 1234'ü yavaşlat\n"
         "  ps -eo pid,ni,cmd | sort -n -k2 → Önceliğe göre sırala\n\n"
         "İpucu: Uzun süren işlemleri nice ile düşük öncelikte çalıştırın, sistem yanıt vermeye devam etsin."},
        {"fdisk", "Disk bölümleme", "sudo fdisk -l",
         "Disk bölüm tablosunu görüntüleme ve düzenleme aracı. MBR ve GPT destekler.\n\n"
         "Seçenekler:\n"
         "  -l            Tüm disk bölümlerini listele\n"
         "  -b sektör     Sektör boyutu (512, 1024, 2048, 4096)\n"
         "  -u            Sektör sayılarını göster\n"
         "  -x            Ek bilgileri göster (LBA, sektör)\n\n"
         "İnteraktif komutlar (fdisk /dev/sda):\n"
         "  m    Yardım\n"
         "  p    Bölüm tablosunu göster\n"
         "  n    Yeni bölüm oluştur\n"
         "  d    Bölüm sil\n"
         "  t    Bölüm tipini değiştir\n"
         "  w    Değişiklikleri kaydet ve çık\n"
         "  q    Değişiklikleri kaydetmeden çık\n"
         "  g    Yeni GPT bölüm tablosu oluştur\n"
         "  o    Yeni MBR (DOS) bölüm tablosu oluştur\n\n"
         "Örnekler:\n"
         "  sudo fdisk -l              → Diskleri ve bölümleri listele\n"
         "  sudo fdisk /dev/sdb        → /dev/sdb'yi düzenle\n"
         "  echo -e 'g\\nn\\n\\n\\n+1G\\nw' | sudo fdisk /dev/sdb\n"
         "    → 1GB'lik GPT bölümü oluştur (script ile)\n\n"
         "⚠️ UYARI: Disk bölümleme veri kaybına neden olabilir!"},
        {"mkfs", "Dosya sistemi oluştur", "sudo mkfs.ext4 /dev/sdb1",
         "Bir disk bölümüne dosya sistemi (format) oluşturur. ext4, xfs, ntfs, vfat destekler.\n\n"
         "Dosya sistemi türleri:\n"
         "  mkfs.ext4 /dev/sdb1  → ext4 (Linux, varsayılan, günlüklü)\n"
         "  mkfs.xfs /dev/sdb1   → XFS (büyük dosyalar)\n"
         "  mkfs.btrfs /dev/sdb1 → Btrfs (snapshot, sıkıştırma)\n"
         "  mkfs.vfat /dev/sdb1  → FAT32 (USB, uyumluluk)\n"
         "  mkfs.ntfs /dev/sdb1  → NTFS (Windows)\n\n"
         "Seçenekler (ext4):\n"
         "  -L etiket     → Birim etiketi (label)\n"
         "  -m 0          → Root için ayrılan yüzde (0 = tamamı kullanılabilir)\n"
         "  -O ^has_journal  → Günlüksüz (daha hızlı ama güvensiz)\n"
         "  -E stride=32  → RAID yapılandırması\n\n"
         "Örnekler:\n"
         "  sudo mkfs.ext4 -L depo /dev/sdb1\n"
         "  sudo mkfs.vfat -n USB /dev/sdb1\n"
         "  sudo mkfs.ext4 -m 0 -L yedek /dev/sdb1\n\n"
         "⚠️ UYARI: Format tüm verileri siler! Çalıştırmadan önce emin olun.\n\n"
         "İpucu: mkfs.ext4 = mke2fs -t ext4, ikisi de aynıdır."},
        {"logrotate", "Günlük döndürme", "logrotate /etc/logrotate.conf",
         "Sistem günlük dosyalarını (log) otomatik olarak döndürür, sıkıştırır ve temizler.\n\n"
         "Yapılandırma (/etc/logrotate.d/):\n\n"
         "  /var/log/nginx/*.log {\n"
         "      daily          → Günlük döndür\n"
         "      rotate 7       → Son 7 günü sakla\n"
         "      compress       → Sıkıştır (gzip)\n"
         "      delaycompress  → 1 gün sonra sıkıştır\n"
         "      missingok      → Dosya yoksa hata verme\n"
         "      notifempty     → Boşsa döndürme\n"
         "      postrotate     → Döndürme sonrası\n"
         "          systemctl reload nginx\n"
         "      endscript\n"
         "  }\n\n"
         "Seçenekler:\n"
         "  daily/weekly/monthly  → Döndürme sıklığı\n"
         "  rotate N              → Saklanacak arşiv sayısı\n"
         "  compress/delaycompress → Sıkıştırma\n"
         "  size 100M             → 100MB olunca döndür\n"
         "  maxage 30             → 30 günden eskiyi sil\n"
         "  copytruncate          → Dosyayı taşıma, kopyala+kes (çalışan programlar için)\n\n"
         "Test:\n"
         "  sudo logrotate -d /etc/logrotate.conf  → Debug (ne yapacak?)\n"
         "  sudo logrotate -f /etc/logrotate.conf  → Zorla döndür\n\n"
         "İpucu: Çalışan servislerin log'ları için copytruncate kullanın."},
        {"parallel", "Paralel çalıştır", "parallel echo ::: 1 2 3 4",
         "Birden fazla işi aynı anda (paralel) çalıştırarak işlemleri hızlandırır.\n\n"
         "Temel kullanım:\n"
         "  parallel komut ::: arg1 arg2 arg3\n"
         "  parallel komut ::: dosya1 dosya2\n"
         "  cat liste | parallel komut\n\n"
         "Seçenekler:\n"
         "  -j 4        4 iş paralel (varsayılan: çekirdek sayısı)\n"
         "  -k           Sıralı çıktı (sırayı koru)\n"
         "  --progress   İlerleme göster\n"
         "  --dry-run    Ne yapacağını göster\n"
         "  -a dosya     Argümanları dosyadan oku\n"
         "  --bar        İlerleme çubuğu\n"
         "  --timeout 60  Zaman aşımı (saniye)\n"
         "  -j0          Maksimum paralellik (tüm çekirdekler)\n\n"
         "Yer tutucular:\n"
         "  {}           Tam argüman\n"
         "  {.}          Uzantısız ad\n"
         "  {/}          Sadece dosya adı (yolsuz)\n"
         "  {//}         Sadece dizin\n"
         "  {#}          Sıra numarası\n\n"
         "Örnekler:\n"
         "  parallel -j4 gzip ::: *.log              → 4 dosyayı aynı anda sıkıştır\n"
         "  parallel curl -O ::: url1 url2 url3      → Paralel indir\n"
         "  ls | parallel 'wc -l {} > {}.sayi'       → Her dosyaya wc uygula\n"
         "  seq 1 100 | parallel -j0 'echo {}^2 | bc' → 100 işlemi aynı anda\n\n"
         "İpucu: xargs -P4 de benzer iş yapar ama parallel çok daha güçlüdür.\n"
         "Kurulum: sudo apt install parallel"},
        {"who", "Oturum açmış kullanıcılar", "who",
         "Sisteme oturum açmış kullanıcıları, bağlantı zamanlarını ve IP adreslerini gösterir.\n\n"
         "Seçenekler:\n"
         "  -b       Sistemin son açılış zamanı\n"
         "  -r       Çalışma seviyesi (runlevel)\n"
         "  -u       Boşta kalma süresi dahil\n"
         "  -q       Sadece kullanıcı adları ve sayısı\n"
         "  -H       Sütun başlıklarını göster\n\n"
         "İlgili komutlar:\n"
         "  w        → who + kullanıcı aktiviteleri + yük\n"
         "  users    → Sadece kullanıcı adları\n"
         "  last     → Son oturum açma geçmişi\n"
         "  lastlog  → Her kullanıcının son girişi\n"
         "  finger   → Kullanıcı detay bilgisi\n\n"
         "Örnekler:\n"
         "  who                → kullanici  tty7   2026-06-24 10:15 (:0)\n"
         "  who -b             → system boot  2026-06-20 08:30\n"
         "  who -uH            → Detaylı + başlık\n"
         "  who -q             → kullanici root (2 users)\n"
         "  w                  → Detaylı kullanıcı durumu\n\n"
         "İpucu: who ve w sysadmin'ler için vazgeçilmez komutlardır."},
     };

    for (auto& c : cmds) {
        auto* outer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
        outer->set_name(c.cmd + "|" + c.desc);

        auto* cb = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 3);
        cb->add_css_class("cmd-card");

        auto* ct = Gtk::make_managed<Gtk::Label>("");
        ct->set_markup("<b>" + c.cmd + "</b>");
        ct->set_halign(Gtk::Align::START);
        ct->add_css_class("cmd-name");

        auto* cd = Gtk::make_managed<Gtk::Label>(c.desc);
        cd->set_halign(Gtk::Align::START);

        auto* ce = Gtk::make_managed<Gtk::Label>("");
        ce->set_markup("<tt>" + c.ex + "</tt>");
        ce->set_halign(Gtk::Align::START);
        ce->add_css_class("cmd-example");

        auto* detail_label = Gtk::make_managed<Gtk::Label>("");
        std::string escaped_detail;
        for (char ch : c.detail) {
            if (ch == '<') escaped_detail += "&lt;";
            else if (ch == '>') escaped_detail += "&gt;";
            else if (ch == '&') escaped_detail += "&amp;";
            else escaped_detail += ch;
        }
        detail_label->set_markup(escaped_detail);
        detail_label->set_halign(Gtk::Align::START);
        detail_label->set_wrap(true);
        detail_label->set_margin_top(6);
        detail_label->add_css_class("cmd-detail");

        auto* revealer = Gtk::make_managed<Gtk::Revealer>();
        revealer->set_transition_type(Gtk::RevealerTransitionType::SLIDE_DOWN);
        revealer->set_child(*detail_label);

        cb->append(*ct);
        cb->append(*cd);
        cb->append(*ce);
        cb->append(*revealer);

        auto* lab_row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
        lab_row->set_margin_top(8);

        auto* btn_try = Gtk::make_managed<Gtk::Button>("Dene");
        btn_try->add_css_class("lab-btn-try");
        btn_try->signal_clicked().connect([this, c_cmd = c.cmd, c_ex = c.ex]() {
            on_lab_try_command(c_cmd + " " + c_ex);
        });

        auto* btn_terminal = Gtk::make_managed<Gtk::Button>("Terminal");
        btn_terminal->add_css_class("lab-btn-terminal");
        btn_terminal->signal_clicked().connect([this, c_cmd = c.cmd]() {
            on_lab_open_terminal("Şimdi deneyebilirsin: " + c_cmd);
        });

        auto* output_label = Gtk::make_managed<Gtk::Label>("");
        output_label->add_css_class("lab-output");
        output_label->set_wrap(true);
        output_label->set_selectable(true);
        output_label->set_halign(Gtk::Align::START);
        output_label->set_visible(false);

        auto* output_revealer = Gtk::make_managed<Gtk::Revealer>();
        output_revealer->set_transition_type(Gtk::RevealerTransitionType::SLIDE_DOWN);
        output_revealer->set_child(*output_label);

        lab_row->append(*btn_try);
        lab_row->append(*btn_terminal);
        cb->append(*lab_row);
        cb->append(*output_revealer);

        outer->append(*cb);

        auto click_box = Gtk::GestureClick::create();
        click_box->signal_released().connect([revealer, cb, output_revealer, output_label](int, double, double) {
            bool revealed = revealer->get_child_revealed();
            output_revealer->set_reveal_child(false);
            output_label->set_visible(false);
            if (revealed) {
                revealer->set_reveal_child(false);
                cb->remove_css_class("cmd-card-expanded");
            } else {
                revealer->set_reveal_child(true);
                cb->add_css_class("cmd-card-expanded");
            }
        });

        cb->add_controller(click_box);

        linux_commands.append(*outer);
    }

    sw->set_child(linux_commands);

    box->append(*title);
    box->append(*search);
    box->append(*lab_warning);
    box->append(*sw);
    stack.add(*box, "linux");
}

void MainWindow::filter_linux(const std::string& q) {
    auto* child = linux_commands.get_first_child();
    while (child) {
        auto name = child->get_name();
        auto p = name.find('|');
        if (p != std::string::npos) {
            auto cmd = name.substr(0, p);
            auto desc = name.substr(p+1);
            child->set_visible(q.empty() || cmd.find(q) != std::string::npos || desc.find(q) != std::string::npos);
        }
        child = child->get_next_sibling();
    }
}

void MainWindow::process_ai_notes(std::string& ai_text) {
    std::regex pattern(R"(\[NOT_ISLEMI:\s*(EKLE|GUNCELLE|SIL)\]([\s\S]*?)\[NOT_ISLEMI:\s*BITTI\])",
                       std::regex::icase);

    std::smatch match;
    std::string remaining = ai_text;
    std::string cleaned;
    bool modified = false;

    while (std::regex_search(remaining, match, pattern)) {
        cleaned += match.prefix().str();

        std::string command = match[1].str();
        std::string body = match[2].str();

        std::regex course_regex(R"(Ders:\s*(.+?)(?:\s*---|\n|$))");
        std::smatch course_match;
        std::string course_name;

        if (std::regex_search(body, course_match, course_regex)) {
            course_name = course_match[1].str();
            course_name.erase(0, course_name.find_first_not_of(" \t\r\n"));
            course_name.erase(course_name.find_last_not_of(" \t\r\n") + 1);
        }

        if (!course_name.empty()) {
            std::string content;
            auto sep_pos = body.find("---");
            if (sep_pos != std::string::npos) {
                content = body.substr(sep_pos + 3);
                content.erase(0, content.find_first_not_of(" \t\r\n"));
                content.erase(content.find_last_not_of(" \t\r\n") + 1);
            }

            if (command == "EKLE" || command == "GUNCELLE") {
                auto it = std::find_if(course_notes.begin(), course_notes.end(),
                    [&](const CourseNote& n) { return n.course == course_name; });

                if (it != course_notes.end()) {
                    if (!content.empty())
                        it->content = content;
                } else {
                    course_notes.push_back({course_name, content});
                }
                modified = true;
            } else if (command == "SIL") {
                auto it = std::find_if(course_notes.begin(), course_notes.end(),
                    [&](const CourseNote& n) { return n.course == course_name; });
                if (it != course_notes.end()) {
                    course_notes.erase(it);
                    modified = true;
                }
            }
        }

        remaining = match.suffix().str();
    }

    cleaned += remaining;
    ai_text = cleaned;

    if (modified) {
        refresh_course_list();
        if (selected_note_index >= 0 && selected_note_index < (int)course_notes.size()) {
            note_title.set_text(course_notes[selected_note_index].course);
            note_view.get_buffer()->set_text(course_notes[selected_note_index].content);
        } else if (course_notes.empty()) {
            selected_note_index = -1;
            note_title.set_text("Bir ders seçin");
            note_view.get_buffer()->set_text("");
        }
        save_data();
    }
}

void MainWindow::on_ai_response() {
    std::string result;
    {
        std::lock_guard<std::mutex> lock(ai_mutex);
        result = pending_ai_response;
        pending_ai_response.clear();
    }

    ai_waiting = false;
    ai_input.set_sensitive(true);
    btn_ai_send.set_sensitive(true);
    btn_ai_ddg.set_sensitive(true);
    ai_loading_conn.disconnect();

    if (pending_ai_label) {
        std::string display_text;
        if (!result.empty()) {
            display_text = extract_ai_reply(result);
            process_ai_notes(display_text);
        }
        if (display_text.empty())
            display_text = "API cevap veremedi. Daha sonra dene.";
        pending_ai_label->set_text(display_text);
        pending_ai_label->remove_css_class("ai-loading");
        pending_ai_label = nullptr;
    }

    auto adj = ai_scroll.get_vadjustment();
    adj->set_value(adj->get_upper());
}

void MainWindow::setup_ai_chat() {
    ai_dispatcher.connect(sigc::mem_fun(*this, &MainWindow::on_ai_response));

    // Ana sayfa (dış scroll sadece sayfa taşarsa diye)
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);

    auto* page = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    page->set_margin_start(20);
    page->set_margin_end(20);
    page->set_margin_top(20);
    page->set_margin_bottom(20);

    // ─── Header: Başlık + Model rozeti + Ayarlar ───────────────────────
    auto* header_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    header_box->add_css_class("ai-header-box");

    auto* title = Gtk::make_managed<Gtk::Label>("Yapay Zeka Asistanı");
    title->add_css_class("ai-page-title");
    title->set_halign(Gtk::Align::START);
    title->set_hexpand(false);

    // Model rozeti - model ID'yi okunabilir hale getir
    auto format_model_name = [](const std::string& id) -> std::string {
        std::string name = id;
        auto slash = name.find('/');
        if (slash != std::string::npos) name = name.substr(slash + 1);
        for (auto& c : name) {
            if (c == '-' || c == '_') c = ' ';
        }
        if (!name.empty()) name[0] = std::toupper(name[0]);
        return name;
    };

    ai_model_badge = Gtk::make_managed<Gtk::Label>(format_model_name(get_model()));
    ai_model_badge->add_css_class("ai-model-badge");
    ai_model_badge->set_halign(Gtk::Align::START);

    auto* spacer = Gtk::make_managed<Gtk::Label>();
    spacer->set_hexpand(true);

    auto* set_icon = Gtk::make_managed<Gtk::Image>();
    set_icon->set_from_resource(dark_mode
        ? "/org/ogrenci/merkezi/assets/settings.svg"
        : "/org/ogrenci/merkezi/assets/settings_dark.svg");
    set_icon->set_pixel_size(20);
    btn_ai_key.set_child(*set_icon);
    btn_ai_key.set_size_request(38, 38);
    btn_ai_key.add_css_class("ai-key-btn");
    btn_ai_key.set_halign(Gtk::Align::END);
    btn_ai_key.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::show_ai_key_dialog));

    header_box->append(*title);
    header_box->append(*ai_model_badge);
    header_box->append(*spacer);
    header_box->append(btn_ai_key);

    // ─── Chat mesaj alanı ──────────────────────────────────────────
    ai_chat_box.set_orientation(Gtk::Orientation::VERTICAL);
    ai_chat_box.set_spacing(6);
    ai_chat_box.set_vexpand(true);
    ai_chat_box.set_hexpand(true);

    ai_scroll.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    ai_scroll.set_vexpand(true);
    ai_scroll.set_child(ai_chat_box);

    // ─── Input alanı (ChatGPT tarzı bileşik) ──────────────────────
    ai_input.set_placeholder_text("PardusEdu Asistana soru sor...");

    auto* send_icon = Gtk::make_managed<Gtk::Image>();
    send_icon->set_from_resource("/org/ogrenci/merkezi/assets/send.svg");
    send_icon->set_pixel_size(20);

    btn_ai_send.set_child(*send_icon);
    btn_ai_send.set_size_request(44, 44);
    btn_ai_send.add_css_class("ai-send-btn");

    auto* ddg_icon = Gtk::make_managed<Gtk::Image>();
    ddg_icon->set_from_resource("/org/ogrenci/merkezi/assets/duckduckgo.svg");
    ddg_icon->set_pixel_size(22);
    btn_ai_ddg.set_child(*ddg_icon);
    btn_ai_ddg.set_size_request(44, 44);
    btn_ai_ddg.set_tooltip_text("Aktifse mesaj gönderiminde DuckDuckGo araştırması yapılıp AI analiz eder");
    btn_ai_ddg.add_css_class("ai-ddg-btn");

    auto* input_container = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    input_container->add_css_class("ai-input-container");

    ai_input.set_hexpand(true);
    input_container->append(ai_input);
    input_container->append(btn_ai_ddg);
    input_container->append(btn_ai_send);

    // Focus - container'ın border'ı tüm genişliği kaplasın
    auto fc = Gtk::EventControllerFocus::create();
    fc->signal_enter().connect([input_container]() {
        input_container->add_css_class("focused");
    });
    fc->signal_leave().connect([input_container]() {
        input_container->remove_css_class("focused");
    });
    ai_input.add_controller(fc);

    // ─── Send mesaj (DDG toggle aktifse araştırma + AI) ────────────
    auto send_message = [this]() {
        if (ai_waiting) return;
        std::string text = ai_input.get_text();
        if (text.empty()) return;

        bool ddg_active = btn_ai_ddg.get_active();

        auto* user_lbl = Gtk::make_managed<Gtk::Label>(ddg_active ? "Araştır: " + text : text);
        user_lbl->add_css_class("user-msg");
        user_lbl->set_wrap(true);
        user_lbl->set_selectable(true);
        user_lbl->set_halign(Gtk::Align::END);
        ai_chat_box.append(*user_lbl);

        pending_ai_label = Gtk::make_managed<Gtk::Label>(
            ddg_active ? "DuckDuckGo Üzerinden ara\u015ft\u0131r\u0131l\u0131yor.." : "Cevap al\u0131n\u0131yor");
        pending_ai_label->add_css_class("ai-msg");
        pending_ai_label->add_css_class("ai-loading");
        pending_ai_label->set_wrap(true);
        pending_ai_label->set_selectable(true);
        pending_ai_label->set_halign(Gtk::Align::START);

        auto gesture = Gtk::GestureClick::create();
        gesture->set_button(3);
        gesture->signal_pressed().connect([this](int, double, double) {
            if (pending_ai_label) {
                auto clipboard = Gdk::Display::get_default()->get_clipboard();
                clipboard->set_text(pending_ai_label->get_text());
            }
        });
        pending_ai_label->add_controller(gesture);

        ai_chat_box.append(*pending_ai_label);

        ai_dot_count = 0;
        ai_loading_conn = Glib::signal_timeout().connect([this, ddg_active]() {
            if (!pending_ai_label) return false;
            ai_dot_count = (ai_dot_count + 1) % 4;
            std::string dots(ai_dot_count, '.');
            std::string msg = ddg_active
                ? "DuckDuckGo ara\u015ft\u0131r\u0131l\u0131yor"
                : "Cevap al\u0131n\u0131yor";
            pending_ai_label->set_text(msg + dots);
            return true;
        }, 400);

        auto adj = ai_scroll.get_vadjustment();
        adj->set_value(adj->get_upper());

        ai_input.set_text("");

        ai_waiting = true;
        ai_input.set_sensitive(false);
        btn_ai_send.set_sensitive(false);
        btn_ai_ddg.set_sensitive(false);

        std::string text_copy = text;
        bool do_ddg = ddg_active;

        std::thread([this, text_copy, do_ddg]() {
            std::string final_user_text = text_copy;
            std::string app_context;

            if (do_ddg) {
                std::string ddg_raw = duckduckgo_search(text_copy);
                if (ddg_raw.empty() || ddg_raw.rfind("DuckDuckGo hatas\u0131", 0) == 0) {
                    final_user_text = "Kullanıcının sorusu: " + text_copy +
                        "\n\nNOT: DuckDuckGo araması başarısız oldu. "
                        "Kendi bilginle cevap ver.";
                } else {
                    std::string ddg_summary;
                    try {
                        auto j = json::parse(ddg_raw);
                        std::string abstract = j.value("Abstract", "");
                        std::string heading = j.value("Heading", "");
                        auto results = j["Results"];
                        auto topics = j["RelatedTopics"];

                        if (!heading.empty())
                            ddg_summary += "Başlık: " + heading + "\n";
                        if (!abstract.empty())
                            ddg_summary += "Özet: " + abstract + "\n";

                        if (results.is_array()) {
                            for (auto& r : results) {
                                if (r.contains("Text") && !r["Text"].empty())
                                    ddg_summary += "- " + r["Text"].get<std::string>() + "\n";
                            }
                        }
                        if (topics.is_array()) {
                            int added = 0;
                            for (auto& t : topics) {
                                if (added >= 10) break;
                                if (t.is_object() && t.contains("Text") && !t["Text"].empty()) {
                                    ddg_summary += "- " + t["Text"].get<std::string>() + "\n";
                                    added++;
                                } else if (t.is_object() && t.contains("Topics") && t["Topics"].is_array()) {
                                    for (auto& st : t["Topics"]) {
                                        if (added >= 10) break;
                                        if (st.contains("Text") && !st["Text"].empty()) {
                                            ddg_summary += "- " + st["Text"].get<std::string>() + "\n";
                                            added++;
                                        }
                                    }
                                }
                            }
                        }
                    } catch (const json::parse_error&) {
                        ddg_summary = "DuckDuckGo yanıtı ayrıştırılamadı.";
                    }
                    final_user_text =
                        "Kullanıcı şunu sordu: " + text_copy +
                        "\n\nDuckDuckGo arama sonuçları:\n" + ddg_summary +
                        "\n\nYukarıdaki arama sonuçlarına göre kullanıcının sorusunu "
                        "detaylıca yanıtla. Kaynak olarak arama sonuçlarını kullan, "
                        "kendi bilgini de ekleyerek zenginleştir. "
                        "Yanıtın en sonuna • DuckDuckGo Üzerinden Araştırıldı yaz. "
                        "Yanıtında maddeler halinde ve açıklayıcı ol.";
                }
            } else {
                std::string lower = text_copy;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                static const std::vector<std::string> keywords = {
                    "not", "ders", "görev", "gorev", "ödev", "odev",
                    "oku", "analiz", "program", "takvim", "çalışma", "calisma"
                };
                bool needs = false;
                for (auto& kw : keywords)
                    if (lower.find(kw) != std::string::npos) { needs = true; break; }
                if (needs) {
                    for (auto& n : course_notes) {
                        app_context += "- Ders: " + n.course + "\n";
                        if (!n.content.empty())
                            app_context += "  İçerik:\n" + n.content + "\n\n";
                    }
                }
            }

            std::string raw = call_ai(final_user_text, app_context);
            {
                std::lock_guard<std::mutex> lock(ai_mutex);
                pending_ai_response = raw;
            }
            ai_dispatcher.emit();
        }).detach();
    };

    ai_input.signal_activate().connect(send_message);
    btn_ai_send.signal_clicked().connect(send_message);

    // ─── Sayfayı birleştir ────────────────────────────────────────
    page->append(*header_box);
    page->append(ai_scroll);
    page->append(*input_container);

    sw->set_child(*page);
    stack.add(*sw, "ai");
}

void MainWindow::show_ai_key_dialog() {
    auto* dialog = new Gtk::Dialog();
    dialog->set_title("Yapay Zeka Ayarları");
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->set_default_size(540, 520);

    auto* content = dialog->get_content_area();
    content->set_margin(16);
    content->set_spacing(12);

    // Sağlayıcı seçimi
    auto* provider_lbl = Gtk::make_managed<Gtk::Label>("Sağlayıcı:");
    provider_lbl->set_halign(Gtk::Align::START);

    auto* provider_combo = Gtk::make_managed<Gtk::ComboBoxText>();
    provider_combo->append("groq", "Groq Cloud");
    provider_combo->append("openrouter", "OpenRouter");
    provider_combo->append("gemini", "Google Gemini");
    provider_combo->append("ollama", "Ollama (Yerel)");
    std::string active_provider = ai_provider == AIProvider::GEMINI ? "gemini" : ai_provider == AIProvider::OPENROUTER ? "openrouter" : ai_provider == AIProvider::OLLAMA ? "ollama" : "groq";
    provider_combo->set_active_id(active_provider);

    content->append(*provider_lbl);
    content->append(*provider_combo);

    auto* sep = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::HORIZONTAL);
    sep->set_margin_top(8);
    sep->set_margin_bottom(8);
    content->append(*sep);

    // Groq Cloud bölümü
    auto* groq_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);

    auto* groq_header = Gtk::make_managed<Gtk::Label>("");
    groq_header->set_markup("<b>Groq Cloud</b>");
    groq_header->set_halign(Gtk::Align::START);

    auto* groq_key_lbl = Gtk::make_managed<Gtk::Label>(
        "API Anahtarı:\n(https://console.groq.com/keys adresinden alabilirsiniz)");
    groq_key_lbl->set_wrap(true);
    groq_key_lbl->set_halign(Gtk::Align::START);

    auto* groq_key_entry = Gtk::make_managed<Gtk::Entry>();
    groq_key_entry->set_placeholder_text("gsk_...");
    groq_key_entry->set_text(ai_api_key_groq);
    groq_key_entry->set_hexpand(true);

    auto* groq_model_lbl = Gtk::make_managed<Gtk::Label>("Model:");
    groq_model_lbl->set_halign(Gtk::Align::START);

    auto* groq_model_combo = Gtk::make_managed<Gtk::ComboBoxText>();
    groq_model_combo->append("llama-3.1-8b-instant", "Llama 3.1 8B");
    groq_model_combo->append("llama-3.3-70b-versatile", "Llama 3.3 70B");
    groq_model_combo->append("meta-llama/llama-4-scout-17b-16e-instruct", "Llama 4 Scout 17B-16E");
    groq_model_combo->append("openai/gpt-oss-120b", "OpenAi GPT OSS 120B");
    groq_model_combo->append("openai/gpt-oss-20b", "OpenAi GPT OSS 20B");
    groq_model_combo->append("openai/gpt-oss-safeguard-20b", "OpenAİ GPT OSS SafeGuard 20B");
    groq_model_combo->append("qwen/qwen3-32b", "Qwen 3 32B");
    groq_model_combo->append("qwen/qwen3.6-27b", "Qwen 3.6 27B");
    groq_model_combo->append("allam-2-7b", "ALLaM 2 7B");
    groq_model_combo->append("groq-compound", "Groq Compound");
    groq_model_combo->append("groq/compound-mini", "Groq Compound Mini");

    
    groq_model_combo->set_active_id(ai_model_groq);
    groq_box->append(*groq_header);
    groq_box->append(*groq_key_lbl);
    groq_box->append(*groq_key_entry);
    groq_box->append(*groq_model_lbl);
    groq_box->append(*groq_model_combo);

    // OpenRouter bölümü
    auto* or_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);

    auto* or_header = Gtk::make_managed<Gtk::Label>("");
    or_header->set_markup("<b>OpenRouter</b>");
    or_header->set_halign(Gtk::Align::START);

    auto* or_key_lbl = Gtk::make_managed<Gtk::Label>(
        "API Anahtarı:\n(https://openrouter.ai/keys adresinden alabilirsiniz)");
    or_key_lbl->set_wrap(true);
    or_key_lbl->set_halign(Gtk::Align::START);

    auto* or_key_entry = Gtk::make_managed<Gtk::Entry>();
    or_key_entry->set_placeholder_text("sk-or-...");
    or_key_entry->set_text(ai_api_key_openrouter);
    or_key_entry->set_hexpand(true);

    auto* or_model_lbl = Gtk::make_managed<Gtk::Label>("Model:");
    or_model_lbl->set_halign(Gtk::Align::START);

    auto* or_model_combo = Gtk::make_managed<Gtk::ComboBoxText>();
    // Google
    or_model_combo->append("google/gemma-4-31b-it", "Google Gemma 4 31B");
    or_model_combo->append("google/gemini-3.5-flash", "Gemini 3.5 Flash");
    or_model_combo->append("google/gemini-2.5-flash", "Gemini 2.5 Flash");
    or_model_combo->append("google/gemini-3.1-flash-lite", "Gemini 3.1 Flash Lite");
    or_model_combo->append("google/gemini-2.5-pro", "Gemini 2.5 Pro");
    or_model_combo->append("google/gemini-3.1-pro-preview", "Gemini 3.1 Pro");
    // Qwen
    or_model_combo->append("qwen/qwen3.6-flash", "Qwen 3.6 Flash");
    or_model_combo->append("qwen/qwen3-coder", "Qwen 3 Coder");
    or_model_combo->append("qwen/qwen3.7-max", "Qwen 3.7 Max");
    or_model_combo->append("qwen/qwen3-next-80b-a3b-instruct:free", "Qwen3 Next Free");
    or_model_combo->append("qwen/qwen3.6-flash", "Qwen 3.6 Flash");
    // Deepseek
    or_model_combo->append("deepseek/deepseek-r1", "Deepseek R1");
    or_model_combo->append("deepseek/deepseek-v4-pro", "Deepseek V4 Pro");
    or_model_combo->append("deepseek/deepseek-v4-flash:nitro", "Deepseek V4 Flash Nitro");
    // Mistral
    or_model_combo->append("mistralai/mistral-small-2603", "Mistral Small 4");
    // Kimi
    or_model_combo->append("moonshotai/kimi-k2-thinking", "Kimi K2");
    or_model_combo->append("moonshotai/kimi-k2.6", "Kimi K2.6");
    // Claude
    or_model_combo->append("anthropic/claude-haiku-4.5", "Claude Haiku 4.5");
    or_model_combo->append("anthropic/claude-sonnet-4.6", "Claude Sonnet 4.6");
    // GPT
    or_model_combo->append("openai/gpt-5-mini", "ChatGPT 5 Mini");
    or_model_combo->append("openai/gpt-4o-mini", "ChatGPT 4o Mini");
    or_model_combo->append("openai/gpt-4.1-mini", "ChatGPT 4.1 Mini");

    

    or_model_combo->set_active_id(ai_model_openrouter);

    or_box->append(*or_header);
    or_box->append(*or_key_lbl);
    or_box->append(*or_key_entry);
    or_box->append(*or_model_lbl);
    or_box->append(*or_model_combo);

    // Google Gemini bölümü
    auto* gemini_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);

    auto* gemini_header = Gtk::make_managed<Gtk::Label>("");
    gemini_header->set_markup("<b>Google Gemini</b>");
    gemini_header->set_halign(Gtk::Align::START);

    auto* gemini_key_lbl = Gtk::make_managed<Gtk::Label>(
        "API Anahtarı:\n(https://aistudio.google.com/apikey adresinden alabilirsiniz)");
    gemini_key_lbl->set_wrap(true);
    gemini_key_lbl->set_halign(Gtk::Align::START);

    auto* gemini_key_entry = Gtk::make_managed<Gtk::Entry>();
    gemini_key_entry->set_placeholder_text("AI...");
    gemini_key_entry->set_text(ai_api_key_gemini);
    gemini_key_entry->set_hexpand(true);

    auto* gemini_model_lbl = Gtk::make_managed<Gtk::Label>("Model:");
    gemini_model_lbl->set_halign(Gtk::Align::START);

    auto* gemini_model_combo = Gtk::make_managed<Gtk::ComboBoxText>();  
        
    // Gemini Flash Lite Models
    gemini_model_combo->append("gemini-2.5-flash-lite", "Gemini 2.5 Flash Lite");
    gemini_model_combo->append("gemini-3.1-flash-lite", "Gemini 3.1 Flash Lite");
    // Gemini Flash Models
    gemini_model_combo->append("gemini-2.5-flash", "Gemini 2.5 Flash");
    gemini_model_combo->append("gemini-3.5-flash", "Gemini 3.5 Flash");
    // Gemini Pro Models
    gemini_model_combo->append("gemini-2.5-pro", "Gemini 2.5 Pro");
    gemini_model_combo->append("gemini-3-pro-preview", "Gemini 3 Pro");
    gemini_model_combo->append("gemini-3.1-pro-preview", "Gemini 3.1 Pro");
    

    gemini_model_combo->set_active_id(ai_model_gemini);

    gemini_box->append(*gemini_header);
    gemini_box->append(*gemini_key_lbl);
    gemini_box->append(*gemini_key_entry);
    gemini_box->append(*gemini_model_lbl);
    gemini_box->append(*gemini_model_combo);

    // Ollama bölümü
    auto* ollama_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);

    auto* ollama_header = Gtk::make_managed<Gtk::Label>("");
    ollama_header->set_markup("<b>Ollama (Yerel)</b>");
    ollama_header->set_halign(Gtk::Align::START);

    auto* ollama_url_lbl = Gtk::make_managed<Gtk::Label>(
        "Ollama Sunucu Adresi:\n(http://localhost:11434 varsayılan)");
    ollama_url_lbl->set_wrap(true);
    ollama_url_lbl->set_halign(Gtk::Align::START);

    auto* ollama_url_entry = Gtk::make_managed<Gtk::Entry>();
    ollama_url_entry->set_placeholder_text("http://localhost:11434");
    ollama_url_entry->set_text(ai_ollama_url);
    ollama_url_entry->set_hexpand(true);

    auto* ollama_model_lbl = Gtk::make_managed<Gtk::Label>("Model:");
    ollama_model_lbl->set_halign(Gtk::Align::START);

    auto* ollama_model_combo = Gtk::make_managed<Gtk::ComboBoxText>();
    ollama_model_combo->append("llama3.1:8b", "Ollama 3.1 8B");
    ollama_model_combo->append("medgemma1.5:4b", "Medgemma 1.5 4B");
    ollama_model_combo->append("granite4:3b", "Granite4 3B");
    ollama_model_combo->append("gemma3:latest", "Gemma-3 4B");
    ollama_model_combo->append("gemma3:27b-cloud", "Gemma-3 27B Cloud");
    ollama_model_combo->append("gemma4:31b-cloud", "Gemma-4 31B Cloud");
    ollama_model_combo->append("deepseek-v3.1:671b-cloud", "Deepseek v3.1 Cloud");
    ollama_model_combo->append("deepseek-v4-flash:cloud", "Deepseek v4 flash Cloud");
    ollama_model_combo->append("qwen3.5:397b-cloud", "Qwen3.5 Cloud");
    ollama_model_combo->append("minimax-m3:cloud", "Minimax-m3 Cloud");
    ollama_model_combo->append("kimi-k2.6:cloud", "Kimi k2.6 Cloud");


    ollama_model_combo->set_active_id(ai_model_ollama);

    ollama_box->append(*ollama_header);
    ollama_box->append(*ollama_url_lbl);
    ollama_box->append(*ollama_url_entry);
    ollama_box->append(*ollama_model_lbl);
    ollama_box->append(*ollama_model_combo);

    content->append(*groq_box);
    content->append(*or_box);
    content->append(*gemini_box);
    content->append(*ollama_box);

    // Sağlayıcı değişince görünürlüğü ayarla
    auto update_visibility = [groq_box, or_box, gemini_box, ollama_box, provider_combo]() {
        std::string id = provider_combo->get_active_id();
        groq_box->set_visible(id == "groq");
        or_box->set_visible(id == "openrouter");
        gemini_box->set_visible(id == "gemini");
        ollama_box->set_visible(id == "ollama");
    };
    provider_combo->signal_changed().connect(update_visibility);
    update_visibility();

    // Kullanıcının ayarları kaydetmesi veya iptal etmesi için butonlar
    auto* btn_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    btn_box->set_halign(Gtk::Align::END);
    btn_box->set_margin_top(12);

    auto* btn_cancel = Gtk::make_managed<Gtk::Button>("İptal");
    auto* btn_save = Gtk::make_managed<Gtk::Button>("Kaydet");
    btn_save->add_css_class("suggested-action");

    btn_box->append(*btn_cancel);
    btn_box->append(*btn_save);

    content->append(*btn_box);

    btn_cancel->signal_clicked().connect([dialog]() {
        dialog->close();
    });

    btn_save->signal_clicked().connect([this, dialog, provider_combo, groq_key_entry, groq_model_combo, or_key_entry, or_model_combo, gemini_key_entry, gemini_model_combo, ollama_url_entry, ollama_model_combo]() {
        std::string prov = provider_combo->get_active_id();
        if (prov == "gemini") ai_provider = AIProvider::GEMINI;
        else if (prov == "openrouter") ai_provider = AIProvider::OPENROUTER;
        else if (prov == "ollama") ai_provider = AIProvider::OLLAMA;
        else ai_provider = AIProvider::GROQ;
        set_provider(ai_provider);

        // Groq ayarlarını kaydet
        std::string groq_key = groq_key_entry->get_text();
        if (!groq_key.empty()) {
            ai_api_key_groq = groq_key;
            set_groq_api_key(groq_key);
        }
        ai_model_groq = groq_model_combo->get_active_id();

        // OpenRouter ayarlarını kaydet
        std::string or_key = or_key_entry->get_text();
        if (!or_key.empty()) {
            ai_api_key_openrouter = or_key;
            set_openrouter_api_key(or_key);
        }
        ai_model_openrouter = or_model_combo->get_active_id();

        // Gemini ayarlarını kaydet
        std::string gemini_key = gemini_key_entry->get_text();
        if (!gemini_key.empty()) {
            ai_api_key_gemini = gemini_key;
            set_gemini_api_key(gemini_key);
        }
        ai_model_gemini = gemini_model_combo->get_active_id();

        // Ollama ayarlarını kaydet
        std::string ollama_url = ollama_url_entry->get_text();
        if (!ollama_url.empty()) {
            ai_ollama_url = ollama_url;
            set_ollama_url(ollama_url);
        }
        ai_model_ollama = ollama_model_combo->get_active_id();
        set_ollama_model(ai_model_ollama);

        // Aktif sağlayıcının modelini uygula
        if (ai_provider == AIProvider::GROQ) {
            set_model(ai_model_groq);
        } else if (ai_provider == AIProvider::OPENROUTER) {
            set_model(ai_model_openrouter);
        } else if (ai_provider == AIProvider::GEMINI) {
            set_model(ai_model_gemini);
        } else {
            set_model(ai_model_ollama);
        }

        save_data();

        // Model rozetini güncelle
        if (ai_model_badge) {
            std::string model_id = get_model();
            std::string display = model_id;
            auto slash = display.find('/');
            if (slash != std::string::npos) display = display.substr(slash + 1);
            for (auto& c : display) {
                if (c == '-' || c == '_') c = ' ';
            }
            if (!display.empty()) display[0] = std::toupper(display[0]);
            ai_model_badge->set_text(display);
        }

        dialog->close();
    });

    dialog->signal_close_request().connect([dialog]() -> bool {
        delete dialog;
        return true;
    }, false);

    dialog->present();
}

void MainWindow::setup_python() {

    // Python dersleri için kaydırılabilir bir alan oluşturuyoruz
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_hexpand(true);
    sw->set_vexpand(true);

    // İçerikleri düzenli bir şekilde yerleştirmek için dikey bir kutu kullanıyoruz
    auto* box = Gtk::make_managed<Gtk::Box>(
        Gtk::Orientation::VERTICAL,
        0
    );
    box->set_hexpand(true);
    box->set_vexpand(true);

    // Web tabanlı ders içeriğini göstermek için WebKit tarayıcı bileşenini kullanıyoruz
    python_webview = WEBKIT_WEB_VIEW(webkit_web_view_new());

    auto* web_widget =
        Gtk::manage(Glib::wrap(GTK_WIDGET(python_webview)));

    web_widget->set_hexpand(true);
    web_widget->set_vexpand(true);

    // Python derslerini HTML'e çevirip gösteriyoruz, tema değişince de yeniden yüklüyoruz
    reload_python();

    // WebKit bileşenini kutuya ekleyip kaydırılabilir alanla ana sayfaya yerleştiriyoruz
    box->append(*web_widget);
    sw->set_child(*box);

    stack.add(*sw, "python");

    // Sayfanın tüm alanı kaplaması için genişlik ve yükseklik ayarlarını yapıyoruz
    stack.set_hexpand(true);
    stack.set_vexpand(true);
}

void MainWindow::reload_python() {
   if (!python_webview) return;

    GError* error = nullptr;
    GBytes* bytes = g_resources_lookup_data(
        "/org/ogrenci/merkezi/docs/python/python.md",
        G_RESOURCE_LOOKUP_FLAGS_NONE,
        &error
    );

    std::string markdown;
    if (bytes) {
        gsize size;
        const char* data = (const char*)g_bytes_get_data(bytes, &size);
        markdown.assign(data, size);
        g_bytes_unref(bytes);
    } else {
        markdown = "# HATA\nMarkdown yüklenemedi";
    }

    std::string html;
    auto callback = [](
        const MD_CHAR* text,
        MD_SIZE size,
        void* userdata
    ) {
        auto* out = static_cast<std::string*>(userdata);
        out->append(text, size);
    };

    md_html(
        markdown.c_str(),
        markdown.size(),
        callback,
        &html,
        0,
        0
    );

    // Aydınlık ve karanlık mod için ayrı CSS stilleri hazırlıyoruz
    std::string css_light = R"(
html, body { margin:0; padding:0; height:100%; }
body{ background:transparent; color:#111111 !important; font-family: Inter, sans-serif; }
.md-container { background:#ffffff !important; padding:40px; line-height:1.8; max-width:1000px; margin:40px auto; border-radius:16px; box-shadow: 0 6px 18px rgba(16,24,40,0.06); }
h1 { color:#1f2937; } h2 { color:#374151; }
a { color:#1d4ed8 !important; text-decoration:none; }
a:hover { color:#2563eb !important; text-decoration:underline; }
pre{ background:#f3f4f6 !important; padding:16px; border-radius:12px; overflow:auto; border:1px solid #d1d5db !important; }
code{ color:#2563eb !important; font-family: monospace; }
hr{ border:none; height:1px; background:#e5e7eb; margin:40px 0; }
)";

    std::string css_dark = R"(
html, body { margin:0; padding:0; height:100%; }
body{ background:transparent; color:#e6eef6 !important; font-family: Inter, sans-serif; }
.md-container { background:#0b0c0d !important; padding:40px; line-height:1.8; max-width:1000px; margin:40px auto; border-radius:16px; box-shadow: 0 6px 18px rgba(2,6,23,0.6); }
h1 { color:#e6eef6 !important; } h2 { color:#cbd5e1 !important; }
a { color:#7dd3fc !important; text-decoration:none; }
a:hover { color:#38bdf8 !important; text-decoration:underline; }
pre{ background:#0f1720 !important; padding:16px; border-radius:12px; overflow:auto; border:1px solid #1f2933 !important; }
code{ color:#93c5fd !important; font-family: monospace; }
hr{ border:none; height:1px; background:#1f2933; margin:40px 0; }
)";

    std::string final_html = std::string("\n\n<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"utf-8\">\n<style>\n")
        + (dark_mode ? css_dark : css_light)
        + std::string("\n</style>\n</head>\n<body>\n\n<div class=\"md-container\">\n")
        + html
        + std::string("\n</div>\n</body>\n</html>\n");

    // WebView arka plan rengini temaya göre ayarlıyoruz, bazı platformlarda CSS geçmeyebiliyor
    GdkRGBA bg;
    gdk_rgba_parse(&bg, (dark_mode ? "#111213" : "#ffffff"));
    webkit_web_view_set_background_color(python_webview, &bg);

    webkit_web_view_load_html(
        python_webview,
        final_html.c_str(),
        nullptr
    );
}

static std::string dersler_course_css(const std::string& course) {
    if (course == "Matematik") return "dersler-course-math";
    if (course == "Fen Bilimleri") return "dersler-course-science";
    if (course.find("Türk") != std::string::npos) return "dersler-course-turkish";
    if (course == "Sosyal Bilgiler") return "dersler-course-social";
    if (course == "İngilizce") return "dersler-course-english";
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

    add_stat(std::to_string(report.study_score) + "/100", "Haftal\u0131k Skor");
    add_stat(std::to_string((int)accuracy) + "%", "Do\u011fruluk");
    add_stat(std::to_string(report.total_pomo_sessions), "Pomodoro");
    add_stat(std::to_string(report.total_focus_minutes) + "dk", "\u00c7al\u0131\u015fma");

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
    report.total_pomo_sessions = pomodoro_completed;
    report.total_focus_minutes = pomodoro_minutes;
    report.tasks_completed = 0;
    report.tasks_total = (int)tasks.size();
    report.notes_created = (int)course_notes.size();

    for (auto& t : tasks) {
        if (t.done) report.tasks_completed++;
    }

    report.cards_reviewed = 0;
    report.cards_correct = 0;
    report.cards_wrong = 0;

    // Populate daily stats for chart
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

        for (auto& t : tasks) {
            if (t.done) ds.tasks_done++;
        }
        ds.pomo_sessions = report.total_pomo_sessions / 7;
        ds.tasks_total = (int)tasks.size();

        report.daily_stats.push_back(ds);
    }

    // Distribute focus minutes proportionally by card activity
    for (int di = 0; di < 7; di++) {
        if (total_day_cards > 0) {
            report.daily_stats[di].focus_minutes =
                report.total_focus_minutes * day_card_counts[di] / total_day_cards;
        }
        if (report.daily_stats[di].focus_minutes == 0 && report.total_focus_minutes > 0) {
            report.daily_stats[di].focus_minutes = report.total_focus_minutes / 7;
        }
    }

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

// ========== PardusLab Callbacks ==========

void MainWindow::setup_parduslab() {
    parduslab = new PardusLab();
    lab_ready = false;

    Glib::signal_idle().connect_once([this]() {
        if (!parduslab->podman_available()) {
            if (lab_warning)
                lab_warning->set_text("Podman kurulu deil. PardusLab iin: sudo apt install podman");
            return;
        }
        lab_ready = true;
        parduslab->cleanup_all();

        GBytes* bytes = g_resources_lookup_data(
            "/org/ogrenci/merkezi/assets/parduslab/challenges.json",
            G_RESOURCE_LOOKUP_FLAGS_NONE, nullptr);
        if (bytes) {
            gsize size = 0;
            const char* data = static_cast<const char*>(g_bytes_get_data(bytes, &size));
            std::string tmp = "/tmp/parduslab_challenges.json";
            std::ofstream f(tmp);
            if (f.is_open()) {
                f.write(data, size);
                f.close();
                parduslab->load_challenges(tmp);
            }
            g_bytes_unref(bytes);
        }

        if (lab_warning)
            lab_warning->set_visible(false);
    });
}

void MainWindow::on_lab_try_command(const std::string& command) {
    if (!lab_ready || current_lab_container_id.empty()) {
        current_lab_container_id = parduslab->start_container();
        if (current_lab_container_id.empty()) {
            show_lab_error("Podman kurulu deil",
                "PardusLab kullanmak iin:\nsudo apt install podman");
            return;
        }
        lab_ready = true;
    }

    std::string output = parduslab->exec_command(
        current_lab_container_id, command
    );

    auto* child = linux_commands.get_first_child();
    while (child) {
        auto name = child->get_name();
        auto p = name.find('|');
        if (p != std::string::npos) {
            std::string cmd_name = name.substr(0, p);
            if (command.find(cmd_name) == 0) {
                auto* outer_box = dynamic_cast<Gtk::Box*>(child);
                if (outer_box) {
                    auto* card = dynamic_cast<Gtk::Box*>(outer_box->get_first_child());
                    if (card) {
                        auto children = card->get_children();
                        for (auto* w : children) {
                            auto* rev = dynamic_cast<Gtk::Revealer*>(w);
                            if (rev) {
                                auto* label = dynamic_cast<Gtk::Label*>(rev->get_child());
                                if (label) {
                                    std::string es;
                                    for (char ch : output) {
                                        if (ch == '<') es += "&lt;";
                                        else if (ch == '>') es += "&gt;";
                                        else if (ch == '&') es += "&amp;";
                                        else es += ch;
                                    }
                                    label->set_markup("<tt>$ " + command + "</tt>\n" + es);
                                    label->set_visible(true);
                                    rev->set_reveal_child(true);
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
            }
        }
        child = child->get_next_sibling();
    }
}

void MainWindow::spawn_vte_shell(const std::string& container_id, const std::string& username) {
    if (vte_widget) {
        auto* parent = gtk_widget_get_parent(vte_widget);
        if (parent) gtk_box_remove(GTK_BOX(parent), vte_widget);
        vte_widget = nullptr;
    }

    GtkWidget* vte = vte_terminal_new();
    VteTerminal* term = VTE_TERMINAL(vte);

    vte_terminal_set_scrollback_lines(term, 10000);

    GdkRGBA bg = {0.05, 0.05, 0.05, 1.0};
    GdkRGBA fg = {0.94, 0.94, 0.94, 1.0};
    vte_terminal_set_color_background(term, &bg);
    vte_terminal_set_color_foreground(term, &fg);

    GdkRGBA pal[16] = {
        {0.12,0.12,0.12,1.0}, {0.90,0.20,0.20,1.0},
        {0.00,1.00,0.53,1.0}, {0.95,0.75,0.15,1.0},
        {0.20,0.45,0.85,1.0}, {0.65,0.30,0.80,1.0},
        {0.15,0.80,0.80,1.0}, {0.80,0.80,0.80,1.0},
        {0.30,0.30,0.30,1.0}, {1.00,0.40,0.40,1.0},
    };
    vte_terminal_set_colors(term, nullptr, nullptr, pal, 16);

    std::string home = (username == "root") ? "/root" : "/home/" + username;
    const char* argv[] = {
        "podman", "exec", "-it", "-u", username.c_str(),
        "-w", home.c_str(),
        container_id.c_str(), "/bin/bash", nullptr
    };
    vte_terminal_spawn_async(term, VTE_PTY_DEFAULT, nullptr,
        const_cast<char**>(argv), nullptr, G_SPAWN_SEARCH_PATH,
        nullptr, nullptr, nullptr, -1, nullptr, nullptr, nullptr);

    gtk_widget_set_vexpand(vte, true);
    gtk_widget_set_hexpand(vte, true);
    vte_widget = vte;
}

void MainWindow::spawn_terminal_session(const std::string& username, Gtk::Box* vbox, Gtk::Widget* after) {
    lab_username = username;

    if (username != "root") {
        std::string cmd = "podman exec " + current_lab_container_id +
            " /bin/bash -c 'useradd -m " + username + " 2>/dev/null; "
            "echo " + username + ":" + username + "123 | chpasswd; "
            "usermod -aG sudo " + username + " 2>/dev/null'";
        system((cmd + " >/dev/null 2>&1").c_str());

        cmd = "podman exec " + current_lab_container_id +
            " /bin/bash -c 'cat > /home/" + username + "/.bashrc <<\"EOF\"\n"
            "if ! command -v sudo >/dev/null 2>&1; then\n"
            "    echo \"PardusLab kuruluyor... lutfen bekleyin.\"\n"
            "fi\n"
            "EOF\n"
            "chown " + username + ":" + username + " /home/" + username + "/.bashrc'";
        system((cmd + " >/dev/null 2>&1").c_str());

        if (!lab_packages_installed) {
            system(("podman exec -d " + current_lab_container_id +
                " /bin/bash -c 'DEBIAN_FRONTEND=noninteractive "
                "apt-get install -y --no-install-recommends sudo nano >/dev/null 2>&1'"
                ).c_str());
            lab_packages_installed = true;
        }
    }

    if (vte_widget) {
        auto* parent = gtk_widget_get_parent(vte_widget);
        if (parent) gtk_box_remove(GTK_BOX(parent), vte_widget);
        vte_widget = nullptr;
    }

    spawn_vte_shell(current_lab_container_id, username);
    if (vte_widget)
        gtk_box_insert_child_after(GTK_BOX(vbox->gobj()), vte_widget, GTK_WIDGET(after->gobj()));
}

void MainWindow::on_lab_open_terminal(const std::string& command_hint) {
    if (terminal_window) {
        if (terminal_hint)
            terminal_hint->set_markup("<b>" + command_hint + "</b>");
        terminal_window->present();
        return;
    }

    if (!lab_ready || current_lab_container_id.empty()) {
        current_lab_container_id = parduslab->start_container();
        if (current_lab_container_id.empty()) {
            show_lab_error("Container baltlamad",
                "Podman ile container baltlamad.\n"
                "Podman kurulumunu kontrol edin.");
            return;
        }
        lab_ready = true;
    }

    terminal_window = Gtk::make_managed<Gtk::Window>();
    terminal_window->set_title("Terminal");
    terminal_window->set_default_size(900, 550);
    terminal_window->set_transient_for(*this);
    terminal_window->set_modal(true);
    terminal_window->add_css_class("terminal-window");

    auto* vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);

    auto* top_bar = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    top_bar->add_css_class("lab-topbar");

    terminal_hint = Gtk::make_managed<Gtk::Label>("");
    terminal_hint->set_markup("<b>" + command_hint + "</b>");
    terminal_hint->set_hexpand(true);
    terminal_hint->set_halign(Gtk::Align::START);
    terminal_hint->set_margin_start(12);
    terminal_hint->add_css_class("lab-hint");

    auto* user_area = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    user_area->add_css_class("lab-user-area");

    auto* user_lbl = Gtk::make_managed<Gtk::Label>("root");
    user_lbl->add_css_class("lab-user-lbl");

    auto* user_arrow = Gtk::make_managed<Gtk::MenuButton>();
    user_arrow->set_icon_name("pan-down-symbolic");
    user_arrow->add_css_class("lab-user-arrow");
    user_arrow->set_has_frame(false);

    auto* popover = Gtk::make_managed<Gtk::Popover>();
    popover->add_css_class("lab-user-popover");

    auto* pop_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    pop_box->add_css_class("lab-pop-box");

    auto make_user_btn = [this, vbox, top_bar, user_lbl, popover](const std::string& name) -> Gtk::Button* {
        auto* btn = Gtk::make_managed<Gtk::Button>(name);
        btn->add_css_class("lab-pop-opt");
        btn->signal_clicked().connect([this, name, vbox, top_bar, user_lbl, popover]() {
            popover->popdown();
            user_lbl->set_text(name);
            spawn_terminal_session(name, vbox, top_bar);
        });
        return btn;
    };

    pop_box->append(*make_user_btn("root"));
    lab_insert_after = pop_box->get_first_child();

    auto* sep = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::HORIZONTAL);
    sep->add_css_class("lab-pop-sep");
    pop_box->append(*sep);

    auto* opt_new = Gtk::make_managed<Gtk::Button>("+ Yeni kullanici...");
    opt_new->add_css_class("lab-pop-opt");
    opt_new->signal_clicked().connect([this, vbox, top_bar, user_area, user_lbl, popover, pop_box, sep, opt_new, make_user_btn]() {
        popover->popdown();

        if (auto* arrow = dynamic_cast<Gtk::MenuButton*>(user_area->get_last_child()))
            arrow->set_visible(false);
        user_lbl->set_visible(false);

        auto* entry = Gtk::make_managed<Gtk::Entry>();
        entry->set_placeholder_text("kullanici adi");
        entry->set_max_length(32);
        entry->set_width_chars(12);
        entry->add_css_class("lab-user-entry");
        user_area->append(*entry);
        entry->grab_focus();

        entry->signal_activate().connect([this, entry, vbox, top_bar, user_area, user_lbl,
                                          popover, pop_box, sep, opt_new, make_user_btn]() {
            auto name = entry->get_text();
            if (name.empty()) return;
            user_area->remove(*entry);
            user_lbl->set_text(name);
            user_lbl->set_visible(true);
            if (auto* arrow = dynamic_cast<Gtk::MenuButton*>(user_area->get_last_child()))
                arrow->set_visible(true);
            spawn_terminal_session(name, vbox, top_bar);

            if (std::find(lab_users.begin(), lab_users.end(), name) == lab_users.end()) {
                lab_users.push_back(name);
                auto* btn = make_user_btn(name);
                pop_box->insert_child_after(*btn, *lab_insert_after);
                lab_insert_after = btn;
            }
        });
    });

    pop_box->append(*opt_new);
    popover->set_child(*pop_box);
    user_arrow->set_popover(*popover);

    user_area->append(*user_lbl);
    user_area->append(*user_arrow);
    top_bar->append(*terminal_hint);
    top_bar->append(*user_area);
    vbox->append(*top_bar);

    auto* btn_row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    btn_row->set_margin(8);
    btn_row->set_halign(Gtk::Align::CENTER);

    auto* btn_reset = Gtk::make_managed<Gtk::Button>("Sıfırla");
    btn_reset->add_css_class("lab-btn-reset");
    btn_reset->signal_clicked().connect([this]() { on_lab_reset_container(); });

    auto* btn_close = Gtk::make_managed<Gtk::Button>("Kapat");
    btn_close->add_css_class("lab-btn-close");
    btn_close->signal_clicked().connect([this]() { terminal_window->close(); });

    btn_row->append(*btn_reset);
    btn_row->append(*btn_close);
    vbox->append(*btn_row);

    spawn_terminal_session("root", vbox, top_bar);

    terminal_window->set_child(*vbox);
    terminal_window->signal_close_request().connect([this]() -> bool {
        terminal_window->set_visible(false);
        return true;
    }, false);
    terminal_window->present();
}

void MainWindow::on_lab_close_terminal() {
    if (terminal_window)
        terminal_window->set_visible(false);
}

void MainWindow::on_lab_reset_container() {
    if (!current_lab_container_id.empty()) {
        parduslab->stop_container(current_lab_container_id);
        parduslab->remove_container(current_lab_container_id);
        current_lab_container_id.clear();
    }
    current_lab_container_id = parduslab->start_container();
    lab_ready = !current_lab_container_id.empty();
    lab_cwd = "~";
    lab_packages_installed = false;
    lab_users.clear();
    lab_users.push_back("root");

    if (terminal_window) {
        terminal_window->set_visible(false);
        delete terminal_window;
        terminal_window = nullptr;
        vte_widget = nullptr;
        terminal_hint = nullptr;
        lab_insert_after = nullptr;
        Glib::signal_idle().connect_once([this]() {
            on_lab_open_terminal("Terminal sfrlandi");
        });
    }
}

void MainWindow::on_lab_snapshot() {
    if (current_lab_container_id.empty()) return;

    auto now = std::time(nullptr);
    auto* tm = std::localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "parduslab_%Y%m%d_%H%M%S", tm);

    std::string tag = buf;
    parduslab->snapshot(current_lab_container_id, tag);
}

void MainWindow::show_lab_error(const std::string& title, const std::string& msg) {
    auto* dialog = new Gtk::MessageDialog(
        *this, title, false,
        Gtk::MessageType::ERROR, Gtk::ButtonsType::OK, true
    );
    dialog->set_secondary_text(msg);
    dialog->signal_response().connect([dialog](int) {
        dialog->close();
        delete dialog;
    });
    dialog->present();
}

void MainWindow::cleanup_old_weak_decks() {
    std::vector<std::string> weak_ids;
    for (auto& d : flash_decks) {
        if (d.id.rfind("weak_", 0) == 0) {
            weak_ids.push_back(d.id);
        }
    }

    for (auto& wid : weak_ids) {
        flash_cards.erase(
            std::remove_if(flash_cards.begin(), flash_cards.end(),
                [&](FlashCard& c) { return c.deck_id == wid; }),
            flash_cards.end());
        flash_decks.erase(
            std::remove_if(flash_decks.begin(), flash_decks.end(),
                [&](FlashDeck& d) { return d.id == wid; }),
            flash_decks.end());
    }
}