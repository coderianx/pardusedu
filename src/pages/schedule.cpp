#include "mainwindow.h"
#include <sstream>
#include <algorithm>

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
