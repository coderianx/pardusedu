#include "mainwindow.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cmath>

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
            if (!pomo_break) { pomodoro_completed++; pomodoro_minutes += pomo_duration_minutes; weekly_pomo_sessions++; weekly_pomo_minutes += pomo_duration_minutes; }
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
