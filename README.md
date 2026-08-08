# STM32F4-GPIO-Driver
STM32 HAL, GPIO Driver kodları

# STM32F4 GPIO Driver & Debounce Implementation

Bu proje, STM32 mikrodenetleyicileri için HAL katmanı kullanılarak yazılmış **modüler GPIO sürücüsü** ve **yazılımsal buton debounce (sıçrama önleme)** algoritmalarını içermektedir.

## 🛠️ Özellikler

- **Modüler Mimari:** Giriş (`Input_State_t`) ve Çıkış (`Output_States_t`) durumları için `struct` tabanlı veri yapıları.
- **Yazılımsal Debounce:** `HAL_GetTick()` tabanlı, bloklamayan (non-blocking) buton sıçrama önleme algoritması.
- **Çoklu LED Kontrolü:** Kolay genişletilebilir çıkış yönetimi.

## 📁 Proje Yapısı

- `io_driver.h` : Pin yapılandırmaları, veri tipleri ve fonksiyon prototipleri.
- `io_driver.c` : Debounce algoritması, giriş/çıkış kontrol mantığı.
- `main.c` : Sürücü entegrasyonu ve örnek kullanım döngüsü.

---
*Öğrenme ve pratik yapma amacıyla geliştirilmiştir.*
