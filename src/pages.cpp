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
#include <webkit/webkit.h>
#include <gdk/gdk.h>
#include <md4c-html.h>

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

    std::string markup = task.done ? "<s>" + task.title + "</s>" : task.title;
    auto* title = Gtk::make_managed<Gtk::Label>(markup);
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

    btn_add_course.add_css_class("action-btn");
    btn_add_course.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_add_course));
    left_box->append(btn_add_course);

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

    auto* btn_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    btn_box->set_margin_top(8);

    btn_save_note.add_css_class("action-btn");
    btn_save_note.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_save_note));

    btn_delete_course.add_css_class("delete-btn");
    btn_delete_course.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_delete_course));

    btn_box->append(btn_save_note);
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

void MainWindow::setup_ai_chat() {

    auto send_message = [this]() {

        std::string text = ai_input.get_text();
        if(text.empty()) return;

        // Kullanıcının mesajını sağ tarafta mavi bir balon içinde gösteriyoruz
        auto* user_msg = Gtk::make_managed<Gtk::Frame>();
        auto* user_lbl = Gtk::make_managed<Gtk::Label>(text);
        user_msg->add_css_class("user-msg");

        user_lbl->set_wrap(true);
        user_lbl->set_margin(8);

        user_msg->set_child(*user_lbl);
        user_msg->set_halign(Gtk::Align::END);

        ai_chat_box.append(*user_msg);

        // Asistanın cevabı için sol tarafta gri bir balon oluşturuyoruz
        auto* ai_msg = Gtk::make_managed<Gtk::Frame>();
        auto* ai_lbl = Gtk::make_managed<Gtk::Label>("Cevap alınıyor...");
        ai_msg->add_css_class("ai-msg");

        ai_lbl->set_wrap(true);
        ai_lbl->set_margin(8);
        ai_lbl->set_selectable(true);

        auto gesture = Gtk::GestureClick::create();
        gesture->set_button(3);

        gesture->signal_pressed().connect([ai_lbl](int, double, double) {
            auto clipboard = Gdk::Display::get_default()->get_clipboard();
            clipboard->set_text(ai_lbl->get_text());
        });

        ai_lbl->add_controller(gesture);
        
        ai_msg->set_child(*ai_lbl);
        ai_msg->set_halign(Gtk::Align::START);

        ai_chat_box.append(*ai_msg);

        ai_input.set_text("");

        std::string app_context;
        {
            std::string lower = text;
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
        std::string raw = call_ai(text, app_context);
        std::string clean = extract_ai_reply(raw);
        process_ai_notes(clean);

        if(!clean.empty())
            ai_lbl->set_text(clean);
        else
            ai_lbl->set_text("API cevap veremedi. Daha sonra dene.");

        // Sohbeti en alta kaydırarak yeni mesajı göstermesini sağlıyoruz
        auto adj = ai_scroll.get_vadjustment();
        adj->set_value(adj->get_upper());
    };

    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC,
                   Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);

    auto* page = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    page->set_margin_start(20);
    page->set_margin_end(20);
    page->set_margin_top(20);
    page->set_margin_bottom(20);

    auto* title_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
    title_box->set_hexpand(true);

    auto* title = Gtk::make_managed<Gtk::Label>("PardusEdu Yapay Zeka Asistanı");
    title->add_css_class("ai-page-title");
    title->set_hexpand(true);
    title->set_halign(Gtk::Align::CENTER);

    btn_ai_key.set_label("+");
    btn_ai_key.set_size_request(36, 36);
    btn_ai_key.add_css_class("ai-key-btn");
    btn_ai_key.set_halign(Gtk::Align::END);
    btn_ai_key.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::show_ai_key_dialog));

    title_box->append(*title);
    title_box->append(btn_ai_key);

    // Sohbet alanını hazırlıyoruz, bütün mesajlar bu kutunun içinde görünecek
    ai_chat_box.set_orientation(Gtk::Orientation::VERTICAL);
    ai_chat_box.set_spacing(8);
    ai_chat_box.set_vexpand(true);
    ai_chat_box.set_hexpand(true);

    ai_scroll.set_policy(Gtk::PolicyType::AUTOMATIC,
                         Gtk::PolicyType::AUTOMATIC);
    ai_scroll.set_vexpand(true);
    ai_scroll.set_child(ai_chat_box);

    // Kullanıcının mesaj yazması için giriş kutusu ve gönderme butonu
    ai_input.set_placeholder_text("PardusEdu Asistana soru sor...");
    ai_input.set_hexpand(true);

    auto* send_icon = Gtk::make_managed<Gtk::Image>();
    send_icon->set_from_resource("/org/ogrenci/merkezi/assets/send.svg");

    btn_ai_send.set_child(*send_icon);
    btn_ai_send.set_size_request(40, 40);
    btn_ai_send.set_hexpand(false);
    btn_ai_send.add_css_class("ai-send-btn");

    auto* input_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
    input_box->set_halign(Gtk::Align::FILL);

    // Yazı alanı mümkün olduğunca geniş olsun ki kullanıcı rahatça yazsın
    ai_input.set_hexpand(true);
    ai_input.signal_activate().connect(send_message);

    // Gönderme butonu küçük ve sabit dursun, asıl alanı yazı kutusu kaplasın
    btn_ai_send.set_hexpand(false);
    btn_ai_send.set_size_request(90, -1);
    btn_ai_send.set_halign(Gtk::Align::END);
    input_box->append(ai_input);
    input_box->append(btn_ai_send);

    // Kullanıcı butona tıklayınca veya Enter'a basınca mesajı gönderiyoruz
    btn_ai_send.signal_clicked().connect([this]() {

        std::string text = ai_input.get_text();
        if(text.empty()) return;

        // Kullanıcının yazdığı mesajı sohbete ekliyoruz
        auto* user_msg = Gtk::make_managed<Gtk::Frame>();
        auto* user_lbl = Gtk::make_managed<Gtk::Label>(text);
        user_msg->add_css_class("user-msg");

        user_lbl->set_wrap(true);
        user_lbl->set_margin(8);

        user_msg->set_child(*user_lbl);
        user_msg->set_halign(Gtk::Align::END);

        ai_chat_box.append(*user_msg);

        // Yapay zekanın cevabı için beklerken "Cevap alınıyor..." yazısı gösteriyoruz
        auto* ai_msg = Gtk::make_managed<Gtk::Frame>();
        auto* ai_lbl = Gtk::make_managed<Gtk::Label>("Cevap alınıyor...");
        ai_msg->add_css_class("ai-msg");

        ai_lbl->set_wrap(true);
        ai_lbl->set_margin(8);
        ai_lbl->set_selectable(true);
        ai_lbl->set_focusable(true);

        ai_msg->set_child(*ai_lbl);
        ai_msg->set_halign(Gtk::Align::START);

        ai_chat_box.append(*ai_msg);

        ai_input.set_text("");

        // API'ye istek atıp yapay zekanın cevabını alıyoruz
        std::string app_context;
        {
            std::string lower = text;
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
        std::string raw = call_ai(text, app_context);
        std::string clean = extract_ai_reply(raw);
        process_ai_notes(clean);
        
        if(!clean.empty()) {
            ai_lbl->set_text(clean);
        } else {
            ai_lbl->set_text("API cevap veremedi. Daha sonra dene.");
        }

        // En son mesaja odaklanmak için aşağı kaydırıyoruz
        auto adj = ai_scroll.get_vadjustment();
        adj->set_value(adj->get_upper());
    });

    // Sayfadaki tüm öğeleri birleştirip ana ekrana ekliyoruz
    page->append(*title_box);
    page->append(ai_scroll);
    page->append(*input_box);

    sw->set_child(*page);

    stack.add(*sw, "ai");
}

void MainWindow::show_ai_key_dialog() {
    auto* dialog = new Gtk::Dialog();
    dialog->set_title("Yapay Zeka Ayarları");
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->set_default_size(420, 220);

    auto* content = dialog->get_content_area();
    content->set_margin(16);
    content->set_spacing(12);

    // Kullanıcının Groq API anahtarını girmesi için bir metin kutusu ekliyoruz
    auto* key_lbl = Gtk::make_managed<Gtk::Label>(
        "Groq API Anahtarı:\n(https://console.groq.com/keys adresinden alabilirsiniz)");
    key_lbl->set_wrap(true);
    key_lbl->set_halign(Gtk::Align::START);

    auto* key_entry = Gtk::make_managed<Gtk::Entry>();
    key_entry->set_placeholder_text("gsk_...");
    key_entry->set_text(ai_api_key);
    key_entry->set_hexpand(true);

    // Hangi yapay zeka modelini kullanmak istediğini seçmesi için bir liste ekliyoruz
    auto* model_lbl = Gtk::make_managed<Gtk::Label>("Model:");
    model_lbl->set_halign(Gtk::Align::START);

    auto* model_combo = Gtk::make_managed<Gtk::ComboBoxText>();
    model_combo->append("llama-3.1-8b-instant", "Llama 3.1 8B");
    model_combo->append("llama-3.3-70b-versatile", "Llama 3.3 70B");
    model_combo->append("meta-llama/llama-4-scout-17b-16e-instruct", "Llama 4 17B 16E");
    model_combo->append("openai/gpt-oss-120b", "OpenAi GPT OSS 120B");
    model_combo->append("openai/gpt-oss-20b", "OpenAi GPT OSS 20B");
    model_combo->append("openai/gpt-oss-safeguard-20b", "OpenAi GPT OSS SafeGuard 20B");
    model_combo->append("qwen/qwen3-32b", "Qwen 3 32B");
    model_combo->append("groq/compound", "Groq Compound");
    model_combo->set_active_id(ai_model);

    // Kullanıcının ayarları kaydetmesi veya iptal etmesi için butonlar
    auto* btn_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    btn_box->set_halign(Gtk::Align::END);

    auto* btn_cancel = Gtk::make_managed<Gtk::Button>("İptal");
    auto* btn_save = Gtk::make_managed<Gtk::Button>("Kaydet");
    btn_save->add_css_class("suggested-action");

    btn_box->append(*btn_cancel);
    btn_box->append(*btn_save);

    content->append(*key_lbl);
    content->append(*key_entry);
    content->append(*model_lbl);
    content->append(*model_combo);
    content->append(*btn_box);

    btn_cancel->signal_clicked().connect([dialog]() {
        dialog->close();
    });

    btn_save->signal_clicked().connect([this, dialog, key_entry, model_combo]() {
        std::string key = key_entry->get_text();
        if (!key.empty()) {
            ai_api_key = key;
            set_api_key(key);
        }
        ai_model = model_combo->get_active_id();
        set_model(ai_model);
        save_data();
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