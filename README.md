# STM32F4-GPIO-Driver
STM32 HAL, GPIO Driver kodları

# STM32F4 GPIO Driver & Debounce Implementation
# 🚀 STM32F4 Non-Blocking I/O & Debounce Sürücüsü

Bu proje, kendimi gömülü sistemler ve sürücü geliştirme alanında ilerletmek, STM32 mimarisinde temiz kod yapıları oluşturmak amacıyla hazırladığım bir çalışmadır. 🛠️

Projede, STM32F4 serisi kartlar için **işlemciyi kilitlemeyen (non-blocking) buton arkı (debounce) engelleme** ve **GPIO çıkış yönetimi** modüler bir C yapısıyla kurgulanmıştır.

---

## 📌 Bu Sürücü Ne İşe Yarar?

1. **İşlemciyi Durdurmaz:** Buton arkını (sıçramasını) engellemek için `HAL_Delay()` kullanılmaz. Böylece işlemci bekleme yapmaz, arka planda diğer işlerini yapmaya devam eder.
2. **Kodu Düzenli Tutur:** Bütün butonları ve LED'leri tek bir `ioInfo` paketi (struct) içinde toplar. Kodun içinde karmaşık pini bulma derdi kalmaz.

---

## 🛠️ Detaylı ve Adım Adım Kullanım Rehberi

Sürücüyü kendi projenizde kullanmak çok basittir. Sadece şu 3 adımı takip edin:

### Adım 1: Başlık Dosyasını Ekleyin
`main.c` dosyanızın en üstüne sürücü dosyasını dahil edin:

#include "io_driver.h"

### Adım 2: Değişken Tanımlayın ve Sürücüyü Başlatın
`main()` fonksiyonunuzun içinde, kart ilk açıldığında pinlerin eşlenmesi için `IO_Initialization` fonksiyonunu bir kez çağırın:

/* main.c içinde */
IO_Info_t ioInfo; // Sürücü değişkenimiz

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init(); // STM32CubeMX pin ayarları

    // Sürücüyü ve pin tanımlarını başlatıyoruz
    IO_Initialization(&ioInfo);

    while (1)
    {
        // ...
    }
}

### Adım 3: Ana Döngüde Sürücüyü Çalıştırın ve Durumları Okuyun
`while(1)` sonsuz döngünüzün en üstüne `IO_Status_Control(&ioInfo);` satırını koyun. Bu satır butonların basılma durumunu kontrol eder ve LED'lerinizi günceller.

Siz sadece butonun filtrelenmiş son durumunu okur ve LED'e ne yapmak istediğinizi söylersiniz:

while (1)
{
    // 1. Sürücünün butonları okumasını ve LED durumlarını güncellemesini sağla
    IO_Status_Control(&ioInfo);

    // 2. Buton basıldı mı kontrol et (Filtrelenmiş, temiz veri)
    if (ioInfo.inputs_Info.User_Button.inputStatus == Input_Status_HIGH)
    {
        // Butona basıldıysa tüm LED'lerin durumunu YAK (SET) olarak ayarla
        ioInfo.outputs_Info.ledGreen.pinStates  = GPIO_PIN_SET;
        ioInfo.outputs_Info.ledBlue.pinStates   = GPIO_PIN_SET;
        ioInfo.outputs_Info.ledYellow.pinStates = GPIO_PIN_SET;
        ioInfo.outputs_Info.ledRed.pinStates    = GPIO_PIN_SET;
    }
    else
    {
        // Butona basılmıyorsa tüm LED'lerin durumunu SÖNDÜR (RESET) olarak ayarla
        ioInfo.outputs_Info.ledGreen.pinStates  = GPIO_PIN_RESET;
        ioInfo.outputs_Info.ledBlue.pinStates   = GPIO_PIN_RESET;
        ioInfo.outputs_Info.ledYellow.pinStates = GPIO_PIN_RESET;
        ioInfo.outputs_Info.ledRed.pinStates    = GPIO_PIN_RESET;
    }
}

---

## ⚙️ Önemli Ayarlar

- **Debounce Süresi:** `io_driver.h` dosyasındaki `DEBOUNCE_TIME` değerini değiştirerek butonun kaç milisaniye sonra "basıldı" kabul edileceğini ayarlayabilirsiniz (Varsayılan: 100 ms).
---

