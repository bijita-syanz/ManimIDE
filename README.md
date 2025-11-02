# 🎬 Manim Script Runner with Audio Merger (Qt + C++)

A modern Qt-based desktop application that lets you **write, save, and render Manim scripts** visually — perfect for quickly generating **math or educational Instagram reels** using Manim.  
It also includes a built-in **audio merging tool** that uses `ffmpeg` to add background music to your rendered videos.

---

## 🚀 Features

✅ **Live Code Editor** — Write and edit Python Manim scripts directly inside the app.  
✅ **Auto Save & Random Naming** — Each script is saved with a unique random filename (e.g., `TempManimScript_1234.py`).  
✅ **Render Manim Animations** — Automatically creates a virtual environment and runs your Manim render command.  
✅ **Integrated Terminal Output** — Real-time display of logs and errors from the rendering process.  
✅ **Audio Merge Tool** — Merge any `.mp3` or `.wav` audio with your rendered `.mp4` video via FFmpeg.  
✅ **Cross-Platform Ready** — Works on Linux and can easily be adapted for Windows or macOS.

---

## 🧰 Requirements

Make sure you have the following installed:

- **Qt 5** or **Qt 6**
- **g++** (or any C++17 compiler)
- **Python 3.11+**
- **pip** and **virtualenv**
- **ffmpeg**
- Internet connection (for first-time Manim installation)

---

## ⚙️ Build Instructions

### 1️⃣ Clone the Repository
```bash
git clone https://github.com/yourusername/manim-script-runner.git
cd manim-script-runner

