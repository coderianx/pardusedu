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

executable(
  'student-hub',
  [
    'src/main.cpp',
    'src/data_manager.cpp',
    'src/ui_setup.cpp',
    'src/pages.cpp',
    'src/ai_client.cpp',
    'src/calc_letter.cpp',
    'src/parduslab.cpp'
  ],
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