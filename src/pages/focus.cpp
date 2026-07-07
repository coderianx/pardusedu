#include "mainwindow.h"
#include <fstream>
#include <sstream>
#include <cstdlib>

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
