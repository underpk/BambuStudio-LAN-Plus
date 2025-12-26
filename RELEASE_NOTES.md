# BambuStudio LAN Plus

---

# v1.1.0

## English

### New Features
- **Space Saving Arrangement** (from Rhoban/Plater): Bitmap-based arrangement that packs objects tighter using actual shape collision detection
- **Duplicate Object Count**: Shows (1/6), (2/6), etc. for same-named models in the object list
- **Prepare Page Default**: App opens directly to the Prepare page instead of Home
- **Ironing Calibration** (from QD3D/MakerWorld): Test pattern to find optimal ironing settings
- **Resonance Avoidance** (from QIDIStudio): Reduces print vibrations for better quality

### Known Issues
- **Space Saving**: Does not support manually rotated models. Arrange models before rotating them.

---

## ภาษาไทย

### ฟีเจอร์ใหม่
- **Space Saving Arrangement** (จาก Rhoban/Plater): จัดเรียงโมเดลแบบประหยัดพื้นที่ ใช้ bitmap ตรวจจับการชนกันของรูปร่างจริง
- **แสดงจำนวนโมเดลซ้ำ**: แสดง (1/6), (2/6) สำหรับโมเดลที่มีชื่อเดียวกันในรายการ
- **หน้า Prepare เป็นค่าเริ่มต้น**: เปิดแอปแล้วเข้าหน้า Prepare โดยตรง
- **Ironing Calibration** (จาก QD3D/MakerWorld): รูปแบบทดสอบเพื่อหาค่า ironing ที่เหมาะสม
- **Resonance Avoidance** (จาก QIDIStudio): ลดการสั่นสะเทือนขณะพิมพ์เพื่อคุณภาพที่ดีขึ้น

### ข้อจำกัดที่ทราบ
- **Space Saving**: ยังไม่รองรับโมเดลที่หมุนด้วยตนเอง ให้จัดเรียงก่อนแล้วค่อยหมุน

---

# v1.0.0

## English

### New Features
- **LAN Device Persistence**: LAN-connected printers are now saved and persist after app restart
- **Custom Device Nicknames**: Set custom names for your LAN printers for easy identification
- **Offline Status Display**: Saved devices show as "offline" until discovered on the network
- **YOLO Flow Calibration** (from OrcaSlicer):
  - **YOLO (Recommended)**: Single-pass flow calibration with 0.01 step precision
  - **YOLO (Perfectionist)**: Higher precision calibration with 0.005 step
- **CMake 4.x Compatibility**: Builds with modern CMake 4.x

### How to Use
1. Connect to a printer via LAN (enter IP address and access code)
2. Right-click on the printer to set a custom nickname
3. The printer will be remembered and show in your device list

---

## ภาษาไทย

### ฟีเจอร์ใหม่
- **จดจำเครื่องพิมพ์ LAN**: เครื่องพิมพ์ที่เชื่อมต่อผ่าน LAN จะถูกบันทึกและแสดงหลังจากเปิดแอปใหม่
- **ตั้งชื่อเล่นเครื่องพิมพ์**: ตั้งชื่อเล่นให้เครื่องพิมพ์ LAN เพื่อให้จำได้ง่าย
- **แสดงสถานะออฟไลน์**: เครื่องพิมพ์ที่บันทึกไว้จะแสดงเป็น "ออฟไลน์" จนกว่าจะพบในเครือข่าย
- **YOLO Flow Calibration** (จาก OrcaSlicer):
  - **YOLO (แนะนำ)**: ปรับเทียบ flow ในครั้งเดียว ความละเอียด 0.01
  - **YOLO (สำหรับคนพิถีพิถัน)**: ความละเอียดสูงขึ้น 0.005
- **รองรับ CMake 4.x**: สามารถ build ด้วย CMake เวอร์ชันใหม่ได้

### วิธีใช้งาน
1. เชื่อมต่อเครื่องพิมพ์ผ่าน LAN (ใส่ IP address และ access code)
2. คลิกขวาที่เครื่องพิมพ์เพื่อตั้งชื่อเล่น
3. เครื่องพิมพ์จะถูกจดจำและแสดงในรายการอุปกรณ์ของคุณ
