#!/bin/bash

# --- Function to handle libedit installation ---
install_libedit() {
    echo "The required libedit development package was not found."
    echo "Attempting to install 'libedit-dev'..."

    if command -v apt &> /dev/null; then
        # Debian/Ubuntu
        sudo apt update
        sudo apt install -y libedit-dev
    elif command -v dnf &> /dev/null; then
        # Fedora/RHEL/CentOS
        sudo dnf install -y libedit-devel
    elif command -v pacman &> /dev/null; then
        # Arch Linux
        sudo pacman -S --noconfirm libedit
    else
        echo "--- 🛑 ERROR ---"
        echo "Could not find a supported package manager (apt, dnf, pacman)."
        echo "Please install the 'libedit-dev' or 'libedit-devel' package manually."
        exit 1
    fi

    if [ $? -ne 0 ]; then
        echo "--- 🛑 INSTALLATION FAILED ---"
        echo "The package installation failed. Check your permissions (sudo) and internet connection."
        exit 1
    fi
    echo "libedit installed successfully. 👍"
}

# --- 1. Check for libedit dependency ---
# We use pkg-config, the standard tool for checking dev library dependencies.
if ! command -v pkg-config &> /dev/null; then
    echo "Warning: 'pkg-config' is not installed, skipping dependency check. Proceeding with compilation."
else
    if ! pkg-config --exists libedit; then
        install_libedit
    else
        echo "libedit development package already installed. ✅"
    fi
fi

# --- 2. Compile the program ---
echo "------------------------------------------"
echo "Compiling rossy program..."

# The compilation command provided by the user:
COMPILATION_CMD="gcc -std=c99 -Wall main.c mpc.c eval.c env.c -ledit -lm -o rossy"

if $COMPILATION_CMD; then
    echo "Compilation successful! Executable 'rossy' created. 🎉"
    echo "------------------------------------------"

    # --- 3. Launch the program ---
    ./rossy
else
    echo "------------------------------------------"
    echo "--- 🛑 COMPILATION FAILED ---"
    echo "Please ensure 'main.c' and 'mpc.c' files exist in the current directory."
    exit 1
fi