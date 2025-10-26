# 🎨 Kreate — KDE Plasma 6 Theme Studio

A modern **Kirigami 3 + Qt 6** application for designing, validating, and exporting **KDE Plasma 6 themes** — no manual `.colors` editing required.  
Built entirely with **KDE Frameworks 6**, **Kirigami**, and **Qt 6.6+**, Kreate aims to be the next-generation theme studio for KDE.

[![Built with KF6](https://img.shields.io/badge/KF6-Kirigami-blue?style=flat-square)]()
[![Qt](https://img.shields.io/badge/Qt-6.6+-green?style=flat-square)]()
[![License: GPL-3.0](https://img.shields.io/badge/license-GPL--3.0-orange?style=flat-square)]()
[![Platform](https://img.shields.io/badge/platform-KDE_Plasma_6-purple?style=flat-square)]()

---

## 🧭 Project Overview

**Kreate** provides a friendly, visual interface for building complete Plasma themes — including color schemes, metadata, SVG assets, wallpapers, and even AI-assisted design features (planned).  
It follows the KDE Human Interface Guidelines and supports full KF6 integration.

---

## 🧱 Build Instructions (Kubuntu 25.04+ / KDE Neon)

### 🔹 Dependencies
Ensure the following are installed:
```bash
sudo apt install qt6-base-dev qt6-declarative-dev qt6-tools-dev \
                 libkf6config-dev libkf6i18n-dev libkf6kirigami-dev \
                 libkf6coreaddons-dev libkf6package-dev cmake build-essential
````

### 🔹 Build & Run

```bash
git clone https://github.com/DvorakUser/Kreate.git
cd Kreate
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/kreate
```

---

## 🗺️ Kreate – Full Development Roadmap (KF6 Edition)

### 🎯 Project Goal

> Kreate is a **KDE Plasma 6 Theme Studio** — a visual tool for designing, validating, packaging, and installing KDE themes with no manual file editing.

---

### ⚙️ Phase 1 – Project Foundation *(✅ Completed)*

**Objective:**
Create the base project with a working Kirigami application that launches cleanly and provides navigation placeholders.

**Deliverables:**

* KF6-only CMake build system
* Placeholder pages: Color Editor, Plasma Theme, Wallpaper, Export
* Kirigami `ApplicationWindow` with navigation drawer
* `ColorSchemeBuilder` backend scaffold
* App metadata (`org.kde.kreate.desktop`, `metadata_kde6.json`)

**Outcome:**
✅ App compiles, runs, and presents a minimal navigable interface.

---

### 🎨 Phase 2 – Functional MVP *(✅ Completed Conceptually, Partially Implemented)*

**Objective:**
Make Kreate functional as a standalone color scheme designer for KDE Plasma 6.

**Deliverables:**

* Working color editor and dynamic color pickers
* `.colors` file save/load (KF6 format)
* Automatic `metadata.json` generation
* Export dialog with author + version fields
* Adaptive Kirigami layout with notifications

**Outcome:**
✅ Users can design and save `.colors` files and export a minimal theme template.

---

### 🚀 Phase 3 – Complete MVP *(⚙️ Testing / Debugging Stage)*

**Objective:**
Evolve Kreate into a fully functional Plasma Theme Studio — with validation, packaging, and installation.

**Backend Deliverables:**

* `Validator`: checks `.colors` and `metadata.json`
* `PackageWriter`: builds `.kpackage` archives
* `ThemeInstaller`: one-click install via `kpackagetool6`

**Frontend Deliverables:**

* “Validate Theme” + “Export Theme” actions
* Animated live preview of colors
* Progress + status messaging
* Configurable metadata editor (ID, author, license)

**Outcome:**
✅ Kreate can design, validate, package, and install Plasma 6 themes.

---

### 🖼️ Phase 4 – Visual Expansion *(🚧 Planned)*

**Objective:**
Add full visual customization: Plasma SVG elements and wallpaper editor.

**Deliverables:**

* Plasma SVG layer color editing
* Wallpaper designer (gradients, patterns, imports)
* Theme template system (Breeze, Oxygen, LCARS, etc.)
* Live Plasma desktop preview

**Outcome:**
🎨 Full visual theming beyond color schemes.

---

### 💾 Phase 5 – Smart Theme Management *(🧩 Design Stage)*

**Objective:**
Introduce smart asset management and version control.

**Deliverables:**

* Local theme library browser
* Metadata diff/merge for updates
* “Smart Sync” for installed vs. generated themes
* KDE Store publishing helper
* Optional CLI (`kreate-cli --export mytheme --install`)

**Outcome:**
📦 Theme lifecycle management and easy KDE Store integration.

---

### 🧠 Phase 6 – AI-Assisted Design Mode *(🚧 Conceptual)*

**Objective:**
Enable assisted theme generation using AI guidance.

**Deliverables:**

* “Describe your theme” input
* AI-generated `.colors` and wallpapers
* Smart Harmony color correction
* Optional API integration for image generation

**Outcome:**
✨ Creative, assisted theming for all users.

---

### 🌐 Phase 7 – Web & Cloud Integration *(🚧 Future / Stretch Goal)*

**Objective:**
Bring Kreate to the web via Qt for WebAssembly.

**Deliverables:**

* WebAssembly-based **Kreate Web Studio**
* KDE Store account integration
* Cloud sync for themes
* Collaborative editing

**Outcome:**
🌍 A web-accessible ecosystem for KDE theme creation and sharing.

---

## 🧩 Roadmap Summary

| Phase | Focus                           | Status        | Result                      |
| :---- | :------------------------------ | :------------ | :-------------------------- |
| 1     | Foundation, KF6 scaffold        | ✅ Done        | Launchable UI               |
| 2     | Functional color scheme builder | ✅ Done        | Saves `.colors`             |
| 3     | Complete MVP                    | ⚙️ In testing | Validate + export + install |
| 4     | Visual theme (SVG, wallpaper)   | 🚧 Planned    | Full visual creation        |
| 5     | Theme management                | 🧩 Design     | Smart syncing + versioning  |
| 6     | AI-assisted creation            | 🚧 Concept    | “Describe and generate”     |
| 7     | Web + cloud integration         | 🚧 Stretch    | Kreate Web Studio           |

---

## 🧾 License

Licensed under the **GNU General Public License v3.0 or later (GPL-3.0-or-later)**.
© 2025 Thomas L. and contributors.

---

## 🤝 Contributing

Kreate welcomes community contributions!
Planned contribution areas include:

* QML/Qt design (Kirigami UI)
* KF6 packaging logic
* Artwork & iconography
* AI color harmony & preview logic

---

## 🌟 Acknowledgements

Built with ❤️ using:

* **KDE Frameworks 6**
* **Kirigami 3**
* **Qt 6.6+**
* KDE’s open design philosophy

> “Design your desktop, not your config files.”

---
