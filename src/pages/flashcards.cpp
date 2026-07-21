#include "mainwindow.h"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <regex>
#include <thread>

using json = nlohmann::json;

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
    while (auto* child = flash_rate_box.get_first_child())
        flash_rate_box.remove(*child);

    auto* old_review = flash_stack.get_child_by_name("review");
    if (old_review) flash_stack.remove(*old_review);

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
