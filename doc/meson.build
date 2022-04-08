doxygen_data = configuration_data()
doxygen_data.set('PACKAGE_NAME', 'libdsm')
doxygen_data.set('BDSM_PACKAGE_VERSION', meson.project_version())
doxygen_data.set('abs_top_srcdir', meson.source_root())

doxyfile = configure_file(
  input: 'Doxyfile.in',
  output: 'Doxyfile',
  configuration: doxygen_data,
  install: false)
doc_dir = join_paths(get_option('datadir'), 'doc', 'libdsm')
custom_target('doc',
           command: [doxygen, doxyfile],
           input: doxyfile,
           output: 'html',
           install: true,
           install_dir: doc_dir)
