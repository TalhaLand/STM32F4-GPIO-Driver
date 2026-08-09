# STM32F4-GPIO-Driver
STM32 HAL, GPIO Driver kodları

# STM32F4 GPIO Driver & Debounce Implementation

# 🚀 STM32F4 Non-Blocking I/O & Debounce Sürücüsü

Bu proje, STM32F4 serisi mikrodenetleyiciler için geliştirilmiş, **bloklamayan (non-blocking) buton arkı (debounce) engelleme** ve **GPIO çıkış yönetimini** kolaylaştıran modüler bir C sürücüsüdür.

İşlemciyi `HAL_Delay()` gibi fonksiyonlarla durdurmadan, `HAL_GetTick()` zamanlayıcısını kullanarak buton okumalarını güvenli hale getirir ve tüm giriş/çıkış pinlerini tek bir yapı (`IO_Info_t`) altında yönetir.

---

## 📌 Özellikler

- **Bloklamayan Debounce Algoritması:** Butonlardaki mekanik sıçramaları (ark/noise) işlemciyi kilitlenmeden milisaniye bazlı filtreler.
- **Merkezi I/O Yönetimi:** Giriş ve çıkış pinlerini tek bir struct yapısında toplayarak temiz bir kod mimarisi sunar.
- **Kolay Entegrasyon:** STM32CubeIDE ve HAL kütüphaneleri ile tam uyumludur.

---

## 🛠️ Kullanım ve Kod Örneği

### 1. Sürücüyü Dahil Etme
`main.c` dosyanızda ilgili başlık dosyasını projenize ekleyin:

#include "io_driver.h"

### 2. Sürücüyü Başlatma ve Döngüde Çağırma
`IO_Info_t` türünde bir değişken tanımlayın,

`main()` içinde `IO_Initialization()` ile yapılandırın

ve

`while(1)` döngüsü içerisinde `IO_Status_Control()` fonksiyonunu sürekli çağırın:

/* Global veya main içi değişken tanımlaması */
IO_Info_t ioInfo;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init(); // STM32CubeMX tarafında pinlerin modları ayarlanmış olmalıdır

    // I/O yapısını ve pin eşlemelerini başlat
    IO_Initialization(&ioInfo);

    while (1)
    {
        // Tüm çıkış durumlarını günceller ve buton debounce kontrollerini yapar
        IO_Status_Control(&ioInfo);

        // Debounce süzgecinden geçmiş buton durumuna göre işlem yapma
        if (ioInfo.inputs_Info.User_Button.inputStatus == Input_Status_HIGH)
        {
            ioInfo.outputs_Info.ledGreen.pinStates  = GPIO_PIN_SET;
            ioInfo.outputs_Info.ledBlue.pinStates   = GPIO_PIN_SET;
            ioInfo.outputs_Info.ledYellow.pinStates = GPIO_PIN_SET;
            ioInfo.outputs_Info.ledRed.pinStates    = GPIO_PIN_SET;
        }
        else
        {
            ioInfo.outputs_Info.ledGreen.pinStates  = GPIO_PIN_RESET;
            ioInfo.outputs_Info.ledBlue.pinStates   = GPIO_PIN_RESET;
            ioInfo.outputs_Info.ledYellow.pinStates = GPIO_PIN_RESET;
            ioInfo.outputs_Info.ledRed.pinStates    = GPIO_PIN_RESET;
        }
    }
}

---

## ⚙️ Fonksiyonlar ve Ayarlar

- **IO_Initialization(IO_Info_t *ioInfo)**: `main.h` içinde tanımlı donanım pinlerini sürücüye bağlar ve ilk durumlarını ayarlar.
- **IO_Status_Control(IO_Info_t *ioInfo)**: Ana döngü içinde periyodik olarak çalıştırılmalıdır. Çıkış durumlarını fiziksel pinlere yansıtır ve girişlerin debounce işlemlerini yürütür.
- **DEBOUNCE_TIME**: `io_driver.h` içerisinde tanımlı ark engelleme süresidir (Varsayılan: 100 ms).

---

