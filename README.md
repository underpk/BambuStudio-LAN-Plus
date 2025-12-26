![image](https://user-images.githubusercontent.com/106916061/179006347-497d24c0-9bd6-45b7-8c49-d5cc8ecfe5d7.png)
# BambuStudio LAN Plus

A modified version of BambuStudio with LAN device persistence, calibration tools, and arrangement improvements.

## Downloads

**Pre-built releases are available on the [Releases page](../../releases).**

- Windows x64: `BambuStudio-Windows-x64.zip`
- macOS: `BambuStudio-macOS.zip`

## New Features

### LAN Device Persistence
- LAN-connected printers are saved and persist after app restart
- Custom nicknames can be set for LAN printers
- Devices show as "offline" until discovered on the network

### Space Saving Arrangement (from Rhoban/Plater)
- Bitmap-based collision detection for tighter object packing
- Uses actual 2D silhouette instead of convex hull
- Respects concave shapes and holes when arranging
- Enable via: Arrange menu > Space Saving checkbox

### Calibration Tools
- **YOLO Flow Calibration** (from OrcaSlicer) - Single-pass flow calibration
- **Ironing Calibration** (from QD3D/MakerWorld) - Test pattern for optimal ironing settings
- **Resonance Avoidance** (from QIDIStudio) - Reduces print vibrations

### UI Improvements
- **Prepare Page Default** - App opens directly to Prepare page
- **Duplicate Object Count** - Shows (1/6), (2/6), etc. for same-named models in object list

### Build Improvements
- CMake 4.x compatibility
- Fixed dependency build issues

## Known Issues
- **Space Saving**: Does not support manually rotated models. Arrange models before rotating them.

See [RELEASE_NOTES.md](RELEASE_NOTES.md) for version history.

---

## Original BambuStudio

Bambu Studio is a cutting-edge, feature-rich slicing software.
It contains project-based workflows, systematically optimized slicing algorithms, and an easy-to-use graphic interface, bringing users an incredibly smooth printing experience.

Original releases are available through the [official releases page](https://github.com/bambulab/BambuStudio/releases/).

Bambu Studio is based on [PrusaSlicer](https://github.com/prusa3d/PrusaSlicer) by Prusa Research, which is from [Slic3r](https://github.com/Slic3r/Slic3r) by Alessandro Ranellucci and the RepRap community.

See the [wiki](https://github.com/bambulab/BambuStudio/wiki) and the [documentation directory](https://github.com/bambulab/BambuStudio/tree/master/doc) for more information.

# What are Bambu Studio's main features?
Key features are:
- Basic slicing features & GCode viewer
- Multiple plates management
- Remote control & monitoring
- Auto-arrange objects
- Auto-orient objects
- Hybrid/Tree/Normal support types, Customized support
- multi-material printing and rich painting tools
- multi-platform (Win/Mac/Linux) support
- Global/Object/Part level slicing parameters

Other major features are:
- Advanced cooling logic controlling fan speed and dynamic print speed
- Auto brim according to mechanical analysis
- Support arc path(G2/G3)
- Support STEP format
- Assembly & explosion view
- Flushing transition-filament into infill/object during filament change

# How to compile
Following platforms are currently supported to compile:
- Windows 64-bit, [Compile Guide](https://github.com/bambulab/BambuStudio/wiki/Windows-Compile-Guide)
- Mac 64-bit, [Compile Guide](https://github.com/bambulab/BambuStudio/wiki/Mac-Compile-Guide)
- Linux, [Compile Guide](https://github.com/bambulab/BambuStudio/wiki/Linux-Compile-Guide)
  - currently we only provide linux appimages on [github releases](https://github.com/bambulab/BambuStudio/releases) for Ubuntu/Fedora, and a [flathub version](https://flathub.org/apps/com.bambulab.BambuStudio) can be used for all the linux platforms

# Report issue
You can add an issue to the [github tracker](https://github.com/bambulab/BambuStudio/issues) if **it isn't already present.**

# License
Bambu Studio is licensed under the GNU Affero General Public License, version 3. Bambu Studio is based on PrusaSlicer by PrusaResearch.

PrusaSlicer is licensed under the GNU Affero General Public License, version 3. PrusaSlicer is owned by Prusa Research. PrusaSlicer is originally based on Slic3r by Alessandro Ranellucci.

Slic3r is licensed under the GNU Affero General Public License, version 3. Slic3r was created by Alessandro Ranellucci with the help of many other contributors.

The GNU Affero General Public License, version 3 ensures that if you use any part of this software in any way (even behind a web server), your software must be released under the same license.

The bambu networking plugin is based on non-free libraries. It is optional to the Bambu Studio and provides extended networking functionalities for users.
By default, after installing Bambu Studio without the networking plugin, you can initiate printing through the SD card after slicing is completed.
