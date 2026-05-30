gtest_dep = dependency('gtest', main: true, required: true)
gtest_nomain_dep = dependency('gtest', required: true)
json_dep = dependency('nlohmann_json', method: 'cmake', required: false)
if not json_dep.found()
  json_dep = declare_dependency(
    include_directories: include_directories('/usr/include')
  )
endif

# calc_letter testi
test_calc_letter = executable('test_calc_letter',
  'test_calc_letter.cpp',
  '../src/calc_letter.cpp',
  dependencies: gtest_dep,
)
test('calc_letter', test_calc_letter)

# extract_ai_reply testi (bağımsız implementasyon)
test_extract_ai = executable('test_extract_ai',
  'test_extract_ai_reply.cpp',
  dependencies: [gtest_dep, json_dep],
)
test('extract_ai_reply', test_extract_ai)
