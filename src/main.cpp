#include "mainwindow.h"
#include <execinfo.h>
#include <csignal>
#include <cstdlib>
#include <iostream>

static void crash_handler(int sig) {
    void* frames[64];
    int n = backtrace(frames, 64);
    std::cerr << "\nSIGNAL " << sig << " (";
    switch (sig) {
        case SIGSEGV: std::cerr << "SIGSEGV"; break;
        case SIGABRT: std::cerr << "SIGABRT"; break;
        case SIGILL: std::cerr << "SIGILL"; break;
        case SIGFPE: std::cerr << "SIGFPE"; break;
        default: std::cerr << sig; break;
    }
    std::cerr << ") at student-hub:\n";
    backtrace_symbols_fd(frames, n, STDERR_FILENO);
    _exit(1);
}

int main(int argc, char* argv[]) {
    signal(SIGSEGV, crash_handler);
    signal(SIGABRT, crash_handler);
    signal(SIGILL, crash_handler);
    signal(SIGFPE, crash_handler);
    auto app = Gtk::Application::create("org.ogrenci.merkezi");
    return app->make_window_and_run<MainWindow>(argc, argv);
}
