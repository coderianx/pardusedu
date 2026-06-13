#include "glibmm/utility.h"
#include "mainwindow.h"
#include "ai_client.h"
#include "sigc++/functors/mem_fun.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <regex>
#include <thread>
#include <curl/curl.h>
#include <webkit/webkit.h>
#include <gdk/gdk.h>
#include <md4c-html.h>
#include <nlohmann/json.hpp>
#include <qrencode.h>

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

        std::thread([this, prompt, front_buffer, back_buffer, hint_entry,
                      front_capture, back_capture, hint_capture, ai_edit_btn]() {
            std::string raw = call_ai(prompt, "");
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
        }).detach();

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

        std::thread([this, prompt, deck_id, status_lbl, generate_btn]() {
            std::string raw = call_ai(prompt, "");
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
        }).detach();
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
        note_view.get_buffer()->set_text(course_notes[idx].content);
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
        save_data();
    }
}

void MainWindow::on_delete_course() {
    if (selected_note_index >= 0 && selected_note_index < (int)course_notes.size()) {
        course_notes.erase(course_notes.begin() + selected_note_index);
        selected_note_index = -1;
        note_title.set_text("Bir ders seçin");
        note_view.get_buffer()->set_text("");
        refresh_course_list();
        save_data();
    }
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
        outer->append(*cb);

        auto click_box = Gtk::GestureClick::create();
        click_box->signal_released().connect([revealer, cb](int, double, double) {
            bool revealed = revealer->get_child_revealed();
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
            std::string final_user_text;
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
                std::string app_context;
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
                final_user_text = text_copy;
                // Store app_context to pass alongside
                // We need to call call_ai with app_context, so restructure
                std::string raw = call_ai(final_user_text, app_context);
                {
                    std::lock_guard<std::mutex> lock(ai_mutex);
                    pending_ai_response = raw;
                }
                ai_dispatcher.emit();
                return;
            }
            std::string raw = call_ai(final_user_text, "");
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
    std::string active_provider = ai_provider == AIProvider::GEMINI ? "gemini" : ai_provider == AIProvider::OPENROUTER ? "openrouter" : "groq";
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
    groq_model_combo->append("groq-compound", "Groq Compound");

    
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

    content->append(*groq_box);
    content->append(*or_box);
    content->append(*gemini_box);

    // Sağlayıcı değişince görünürlüğü ayarla
    auto update_visibility = [groq_box, or_box, gemini_box, provider_combo]() {
        std::string id = provider_combo->get_active_id();
        groq_box->set_visible(id == "groq");
        or_box->set_visible(id == "openrouter");
        gemini_box->set_visible(id == "gemini");
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

    btn_save->signal_clicked().connect([this, dialog, provider_combo, groq_key_entry, groq_model_combo, or_key_entry, or_model_combo, gemini_key_entry, gemini_model_combo]() {
        std::string prov = provider_combo->get_active_id();
        if (prov == "gemini") ai_provider = AIProvider::GEMINI;
        else if (prov == "openrouter") ai_provider = AIProvider::OPENROUTER;
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

        // Aktif sağlayıcının modelini uygula
        if (ai_provider == AIProvider::GROQ) {
            set_model(ai_model_groq);
        } else if (ai_provider == AIProvider::OPENROUTER) {
            set_model(ai_model_openrouter);
        } else {
            set_model(ai_model_gemini);
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