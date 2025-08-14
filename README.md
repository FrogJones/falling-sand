# 🧪 Falling Sand Simulator

A physics-based falling sand simulator inspired by games like *Noita* and *The Powder Toy*. Built with modern OpenGL for real-time particle simulation and rendering.

![Demo](https://img.shields.io/badge/Status-Early%20Development-orange)
![C++](https://img.shields.io/badge/Language-C++-blue)
![OpenGL](https://img.shields.io/badge/Graphics-OpenGL%203.3-green)

## ✨ Features

- **Real-time Physics**: Smooth falling sand simulation with proper collision detection
- **Sliding Mechanics**: Particles slide and stack naturally to form realistic sand piles
- **Interactive Controls**: Click and drag to spawn sand particles
- **Grid-based Simulation**: Efficient 100x100 grid system for particle management
- **Modern Rendering**: Hardware-accelerated rendering using OpenGL 3.3 with instanced drawing

## 🎮 Controls

| Input | Action |
|-------|--------|
| **Left Mouse Button (Hold)** | Spawn sand particles |
| **ESC** | Exit application |

## 🚀 Getting Started

### Prerequisites

Make sure you have the following installed:

- **C++ Compiler** (GCC 7+, Clang 5+, or MSVC 2017+)
- **CMake** 3.10 or higher
- **GLFW** 3.3+
- **GLAD** (OpenGL 3.3 loader)

### Building

1. **Clone the repository**
   ```bash
   git clone https://github.com/yourusername/falling-sand-simulator.git
   cd falling-sand-simulator
   ```

2. **Create build directory**
   ```bash
   mkdir build && cd build
   ```

3. **Generate build files**
   ```bash
   cmake ..
   ```

4. **Compile**
   ```bash
   make -j4  # Linux/macOS
   # or
   cmake --build . --config Release  # Windows
   ```

5. **Run**
   ```bash
   ./sand_simulator
   ```

### Manual Build (Alternative)

If you prefer to build without CMake:

```bash
g++ -std=c++17 main.cpp -lglfw -lGL -ldl -o sand_simulator
```

## 🏗️ Project Structure

```
falling-sand-simulator/
├── main.cpp              # Main application logic
├── shader.h              # Shader loading utilities
├── shaders/
│   ├── test.vert         # Vertex shader
│   └── test.frag         # Fragment shader
├── CMakeLists.txt        # Build configuration
└── README.md
```

## 🔬 Technical Details

### Physics System
- **Grid Resolution**: 100x100 cells
- **Fall Speed**: 3 cells per second
- **Spawn Rate**: 10 particles per second
- **Sliding Logic**: Particles attempt to slide left/right when blocked

### Rendering Pipeline
- **Instanced Rendering**: Efficiently renders up to 1000 particles
- **Dynamic Buffers**: Real-time position and timing updates
- **Grid Snapping**: Particles align to grid cells for consistent physics

### Performance
- **Target**: 60 FPS at 1000x1000 window resolution
- **Memory**: Minimal heap allocation during simulation
- **GPU**: Leverages hardware instancing for particle rendering

## 🐛 Known Issues

- [ ] Particles occasionally get stuck in certain configurations
- [ ] No particle cleanup (particles persist indefinitely)
- [ ] Limited to single particle type
- [ ] No save/load functionality

## 🛣️ Roadmap

### Short Term
- [ ] Fix particle sticking bugs
- [ ] Add particle cleanup/removal system
- [ ] Implement proper CMakeLists.txt
- [ ] Add build instructions for Windows

### Long Term
- [ ] Multiple particle types (water, fire, etc.)
- [ ] Particle interactions and reactions
- [ ] Brush size controls
- [ ] Performance optimizations
- [ ] Save/load simulation states

## 🤝 Contributing

This is an experimental project, but contributions are welcome! Please feel free to:

1. Report bugs or issues
2. Suggest new features
3. Submit pull requests
4. Share improvements to the physics system

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Inspired by [Nolla Games'](https://nollagames.com/) *Noita*
- Built with [GLFW](https://www.glfw.org/) and [GLAD](https://glad.dav1d.de/)
- Physics concepts from *The Powder Toy* community

---

**⚠️ Note**: This project is in early development. Expect bugs, missing features, and frequent changes!