🎨 Kreate — KDE Plasma 6 Theme Studio

A next-generation theme design tool for KDE Plasma 6

---

🧭 Overview

Kreate is a modern, visual designer for KDE Plasma 6 color themes and full Plasma style packages — no manual editing required.
Built with Qt 6, Kirigami 3, and KDE Frameworks 6, Kreate aims to be the spiritual successor to classic theme editors (like the Windows 3.1 UI editor) while supporting KDE’s fully modern theming stack.

> ⚠️ Kreate is currently in beta.
Core features work, but many roadmap items are still in development.

---

✨ Current Features (Beta)

KDE Plasma 6 color scheme editor
Live theme preview (Kirigami-based)
Wallpaper-based palette extraction
Randomized theme generation with seed control
.colors export
CMake-built KF6 application

---

🧱 Build Instructions (Kubuntu 24.04+ / KDE Neon)

Dependencies:

sudo apt install qt6-base-dev qt6-declarative-dev qt6-tools-dev \
                 libkf6config-dev libkf6i18n-dev libkf6kirigami-dev \
                 libkf6coreaddons-dev libkf6package-dev cmake build-essential

Compile & Run:

git clone https://github.com/DvorakUser/Kreate.git

cd Kreate

cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

cmake --build build

./build/kreate

---

🗺️ Kreate Roadmap (KF6 Edition)

Phase 1 — Foundation (Done)
Minimal UI, navigation scaffold, backend stubs.

Phase 2 — Functional MVP (Done)
Color editor, pickers, .colors export.

Phase 3 — Complete MVP (In testing)
Validation, packaging, installer, improved preview.

Phase 4 — Visual Expansion (Planned)
SVG Plasma theme editing, wallpaper studio, templates.

Phase 5 — Theme Management (Design stage)
Local theme browser, metadata handling, KDE Store integration.

Phase 6 —  Kreate Web Studio (Future)
WebAssembly build, cloud sync, collaborative editing.

---

🧾 License

GPL-3.0-or-later
© 2025 Thomas L. and contributors.

---

🤝 Contributing

Contributions welcome! Areas of interest:
QML / Kirigami UI
KF6 packaging backend
Artwork & Plasma SVG assets

Color science / AI-enhanced palette generation
