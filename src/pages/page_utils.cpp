#include "mainwindow.h"

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
    else if (name == "ai_koc") setup_ai_koc();

    pages_built.insert(name);
}
