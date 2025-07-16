# Harbour

Harbour is a C++ project manager and build tool designed to simplify the creation, building, and running of C++ projects. It provides a command-line interface (CLI) to manage your projects, handle dependencies, and streamline your development workflow.

-----

## Features

  * **Project Creation**: Quickly scaffold a new C++ project with a predefined directory structure and a `CMakeLists.txt` file.
  * **Build System**: Uses **CMake** for robust and cross-platform builds.
  * **Dependency Management**: Automatically clones and sets up dependencies for graphics programming, including **GLFW**, **GLM**, and **GLAD**.
  * **Command-Line Interface**: A simple and intuitive CLI to create, build, and run your projects.
  * **Configuration**: Each project has a `.harbourConfig` file to manage project-specific settings.
  * **Debug and Release Builds**: Easily switch between debug and release build configurations.
  * **clangd Support**: Generates a `compile_commands.json` file in your project's root for easy integration with the `clangd` language server.

-----

## Installation

To install Harbour, you can use the provided shell script:

```bash
sh install.sh
```

This will build the project and place the `Harbour` executable in the `build/release/bin/release` directory. You will need to add this directory to your system's **PATH** to run `Harbour` from any location.

Alternatively, you can add an alias to your shell's configuration file (`.bashrc`, `.zshrc`, etc.):

```bash
alias harbour='/path/to/your/harbour/build/release/bin/release/Harbour'
```

-----

## Usage

Harbour provides several commands to manage your projects.

### `new`

The **`new`** command creates a new C++ project.

```bash
harbour new [options] <project_name>
```

**Options:**

  * **`-v <version>`**: Sets the C++ standard version (e.g., 17, 20). The default is 17.
  * **`-b <bin_dir>`**: Sets the output directory for the binary. The default is `bin`.
  * **`-l <lib_dir>`**: Sets the output directory for libraries. The default is `lib`.
  * **`-g`**: Enables a debug-friendly setup with a `debug.hpp` header.
  * **`-G`**: Enables graphics support by adding **GLFW**, **GLM**, and **GLAD** as dependencies.

### `build`

The **`build`** command builds your project. You can run this command from within the project's root directory.

```bash
harbour build [options] [path]
```

**Options:**

  * **`-d`**: Compiles the project in debug mode.
  * **`-c`** or **`--clean`**: Performs a clean build by removing the existing build directory before compiling.
  * **`[path]`**: The path to the project you want to build. Defaults to the current directory.

### `run`

The **`run`** command executes your compiled project.

```bash
harbour run [path]
```

  * **`[path]`**: The path to the project you want to run. Defaults to the current directory.

Harbour will automatically run the newest available binary, checking whether the debug or release build is more recent.

### `make`

The **`make`** command is a convenient shortcut that first builds and then runs your project.

```bash
harbour make [options] [path]
```

The options for the `make` command are the same as the `build` command.

-----

## Building and Testing

To build Harbour and its tests, you can use CMake with the `BUILD_TESTS` option enabled:

```bash
cmake -B build/tests -S . -DBUILD_TESTS=ON
cd build/tests
make
```

You can then run the individual test executables. For example, to run the tests for the `Runner` class:

```bash
./build/tests/bin/Runner
```
