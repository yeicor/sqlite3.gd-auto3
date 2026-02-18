# SQLite3.gd

A cross-platform (desktop, mobile, web) Godot GDExtension for SQLite3 database integration. 
 
See [demo/demo.gd](demo/demo.gd) for examples of use.

You can download prebuilt versions from the [Releases](https://github.com/yeicor/sqlite3.gd-auto3/releases) section or from the [Actions](https://github.com/yeicor/sqlite3.gd-auto3/actions) tab.

## Using this as a Template

To start a new project based on this template:

1. Rename the project: Update `project(sqlite3.gd CXX)` in `CMakeLists.txt`, and rename `demo/addons/sqlite3.gd/` to match it.
2. Update dependencies: Modify `vcpkg_ports/gdext/vcpkg.json`, and follow their instructions to link them in `CMakeLists.txt`.
3. You may need to edit `vcpkg_ports/` and `vcpkg_triplets/` to fix the builds.
4. Implement your bindings in `src/`, write docs in `doc_classes/`, and write tests/demo in `demo/` (see examples from this project).
5. Update this README with your project's description and links.
