#include "mainwindow.h"
#include <sstream>
#include <algorithm>

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
