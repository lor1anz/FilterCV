# FilterCV

FilterCV is a simple real-time image and video filtering playground built with **C++17**, **Qt6**, and **OpenCV**.  
It provides a minimal but flexible architecture to experiment with various visual filters and demonstrate them on:
- Static test images  
- Predefined test videos  
- A live camera feed  

The project uses **Qt** for the GUI layer and **OpenCV** for image processing.

---

## Features

- **Modular filter system** — each filter is a separate class derived from a common abstract interface.  
- **Real-time processing** — designed to maintain high frame rates on camera/video sources.  
- **Interactive GUI** — control filter parameters via checkboxes and sliders in dock panels.  
- **Multiple input sources** — switch between a test image, a test video file, or a camera stream.  
- **Expandable architecture** — easily add new filters with minimal boilerplate.
- **Dedicated Glitch Filter panel** - a separate dock widget that controls the **Glitch Filter**, a composite effect combining JPEG recompression, color-channel shifting, block displacement and saturation / noise alterations.


---

## Build Instructions

### Requirements
- **CMake** 3.16+
- **C++17 compiler** (GCC / Clang / MSVC)
- **Qt 6.x**
- **OpenCV 4.x**

### Building

```bash
git clone https://github.com/Lawrence1947/FilterCV.git
cd FilterCV
mkdir build && cd build
cmake ..
make
```

### Running
```bash
./FilterCV
```

---

## Adding a New Filter

To implement a new filter:

1. Derive a new class from `filters::filter`.  
2. Implement:
   - `const char *id () const override final`
   - `bool is_enabled () const override final`
   - `void set_enabled (bool on) override final`
   - `void apply (const cv::Mat &src, cv::Mat &dst) override final`
3. Register it in `main_window`:
   ```cpp
   engine->add_filter (std::make_shared<filters::your_filter> ());
   ```
4. Add a UI section with controls in the right-hand dock.

---

## Design Notes

- The main loop (`cv_engine`) manages frame grabbing and filter chaining.  
- Each filter is **stateful** — GUI events update its parameters via `set_...()` methods.  
- Filters are applied sequentially in the order they were added to the engine.  
- The GUI updates every 33 ms (~30 FPS) using a `QTimer`.  
- The right dock hosts filter controls; the left dock manages the input source.

---