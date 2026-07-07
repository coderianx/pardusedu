#include "mainwindow.h"
#include "parduslab.h"
#include <vte/vte.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <regex>

using json = nlohmann::json;
namespace fs = std::filesystem;

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
                "PardusLab kullanmak için Terminalinizden:\nsudo apt install podman");
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