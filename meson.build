project('student-hub', 'cpp',
  version : '0.1',
  default_options : ['cpp_std=c++17']
)

gnome = import('gnome')

gtkmm_dep = dependency('gtkmm-4.0')
curl_dep = dependency('libcurl')

# ================= WEBKIT =================
webkit_dep = dependency('webkitgtk-6.0')

# ================= MD4C =================
md4c_dep = dependency('md4c-html')

# ================= QRencode =================
qrencode_dep = dependency('libqrencode')
vte_dep = dependency('vte-2.91-gtk4')

gresource = gnome.compile_resources('resources',
  'resources.gresource.xml',
  source_dir : '.',
  c_name : 'ogrenci'
) 

src_inc = include_directories('src')

executable(
  'student-hub',
  [
    'src/main.cpp',
    'src/data_manager.cpp',
    'src/ui_setup.cpp',
    'src/pages/pomodoro.cpp',
    'src/pages/badges.cpp',
    'src/pages/flashcards.cpp',
    'src/pages/planner.cpp',
    'src/pages/notes.cpp',
    'src/pages/schedule.cpp',
    'src/pages/focus.cpp',
    'src/pages/ai_koc.cpp',
    'src/pages/page_utils.cpp',
    'src/pages/linux.cpp',
    'src/pages/ai_chat.cpp',
    'src/pages/python.cpp',
    'src/pages/dersler.cpp',
    'src/pages/weekly_analysis.cpp',
    'src/pages/parduslab_pages.cpp',
    'src/ai_client.cpp',
    'src/calc_letter.cpp',
    'src/keyring.cpp',
    'src/parduslab.cpp'
  ],
  include_directories : src_inc,
  dependencies : [
    gtkmm_dep,
    curl_dep,
    webkit_dep,
    md4c_dep,
    qrencode_dep,
    vte_dep
  ],
  sources : gresource,
  install : false
)

subdir('tests')