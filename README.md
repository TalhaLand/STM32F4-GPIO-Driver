# STM32F4 GPIO Driver & Non-Blocking Debounce

STM32F4 mikrodenetleyicilerinde GPIO giriş ve çıkışlarını yönetmek için geliştirilmiş, **modüler ve non-blocking (işlemciyi bekletmeyen) bir GPIO driver** çalışmasıdır.

Projede özellikle mekanik butonlarda oluşan **debounce (buton sıçraması)** problemi `HAL_Delay()` kullanılmadan çözülmüştür.

Driver içerisinde butonlar ve LED gibi GPIO elemanları `struct` yapıları içerisinde gruplanarak daha düzenli ve tekrar kullanılabilir bir yapı oluşturulmuştur.

---

# 🚀 Hızlı Başlangıç

Driver'ı kendi STM32CubeIDE projenize dahil etmek için aşağıdaki adımları uygulayın.

## 1. Driver dosyalarını projeye ekleyin

Aşağıdaki dosyaları projenize dahil edin:

```text
io_driver.c
io_driver.h
```

Daha sonra driver'ı kullanacağınız `.c` dosyasına:

```c
#include "io_driver.h"
```

ekleyin.

---

## 2. GPIO pinlerini STM32CubeMX üzerinden ayarlayın

Kullanacağınız buton, LED veya diğer GPIO pinlerini STM32CubeMX üzerinden yapılandırın.

Örneğin:

```text
Buton → GPIO Input
LED   → GPIO Output
```

GPIO pinlerinin Pull-Up / Pull-Down ayarlarının devrenize uygun olduğundan emin olun.

> GPIO pinlerinin isimleri ve portları driver içerisinde kullanılan tanımlarla aynı olmalıdır.

---

## 3. IO_Info_t değişkenini oluşturun

`main.c` içerisinde driver yapısını oluşturun:

```c
IO_Info_t ioInfo;
```

---

## 4. Driver'ı başlatın

`MX_GPIO_Init()` fonksiyonundan sonra `IO_Initialization()` fonksiyonunu bir kez çağırın:

```c
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    IO_Initialization(&ioInfo);

    while (1)
    {
        // ...
    }
}
```

`IO_Initialization()` fonksiyonu driver içerisindeki GPIO elemanlarının başlangıç durumlarını ve ilgili yapıların eşlemesini gerçekleştirir.

---

## 5. Ana döngüde driver'ı çalıştırın

`while(1)` içerisinde:

```c
IO_Status_Control(&ioInfo);
```

fonksiyonunu çağırın.

Örneğin:

```c
while (1)
{
    IO_Status_Control(&ioInfo);

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
```

Buradaki temel mantık:

```text
GPIO
 ↓
IO_Status_Control()
 ↓
Debounce
 ↓
Filtrelenmiş buton durumu
 ↓
Uygulama
 ↓
LED çıkış durumu
```

Artık uygulama kodu doğrudan buton pinini okumak yerine driver tarafından işlenmiş durumu kullanır.

---

# 📚 Driver Nasıl Çalışıyor?

Driver iki temel görevi yerine getirir:

```text
GPIO Input Yönetimi
        +
GPIO Output Yönetimi
```

Input tarafında özellikle mekanik butonların oluşturduğu sıçramalar filtrelenir.

Output tarafında ise LED veya başka GPIO çıkışlarının durumları driver üzerinden yönetilir.

Genel yapı:

```text
                IO_Info_t
                    │
          ┌─────────┴─────────┐
          │                   │
       Inputs              Outputs
          │                   │
     User Button        LED Green
                       LED Blue
                       LED Yellow
                       LED Red
```

Bu yapı sayesinde bütün GPIO elemanları tek bir `IO_Info_t` yapısı üzerinden yönetilebilir.

---

# 🧩 IO_Info_t Yapısı

`IO_Info_t`, driver içerisindeki tüm giriş ve çıkış bilgilerini bir arada tutan ana yapıdır.

Mantıksal olarak:

```text
IO_Info_t
│
├── inputs_Info
│   └── User_Button
│
└── outputs_Info
    ├── ledGreen
    ├── ledBlue
    ├── ledYellow
    └── ledRed
```

Bu yapı sayesinde `main.c` içerisinde onlarca GPIO değişkeniyle uğraşmak yerine tek bir yapı üzerinden erişim sağlanır.

Örneğin:

```c
ioInfo.inputs_Info.User_Button.inputStatus
```

ifadesi User Button'ın filtrelenmiş giriş durumuna erişir.

LED için ise:

```c
ioInfo.outputs_Info.ledGreen.pinStates
```

ifadesi LED'in istenen çıkış durumunu temsil eder.

---

# 📥 GPIO Input Yönetimi

Buton gibi girişlerde temel olarak iki bilgi önemlidir:

```text
GPIO Pin
+
Butonun Durumu
```

Driver GPIO pinini okuyarak butonun mevcut durumunu belirler.

Ancak mekanik butonlarda fiziksel olarak tek bir basış sırasında sinyal kısa süre içerisinde HIGH/LOW şeklinde birkaç kez değişebilir.

Bu probleme **button bouncing** veya **debounce problemi** denir.

Örneğin gerçek bir basış:

```text
İdeal:

LOW ─────────────── HIGH


Gerçekte:

LOW ──┐ ┌─┐ ┌────── HIGH
      └─┘ └─┘
```

Mikrodenetleyici bu kısa değişimleri gerçek birden fazla basış olarak algılayabilir.

Driver'ın debounce mekanizması bu problemi filtrelemek için kullanılır.

---

# ⏱️ Non-Blocking Debounce

Bu projede debounce işlemi için:

```c
HAL_Delay()
```

kullanılmamıştır.

Bunun yerine `HAL_GetTick()` kullanılarak geçen zaman takip edilir.

Temel mantık:

```text
Buton durumu değişti
        ↓
Zaman kaydedildi
        ↓
DEBOUNCE_TIME kadar bekle
        ↓
Durum hâlâ aynı mı?
        ↓
Evet → Yeni durum kabul edilir
Hayır → Değişiklik yok sayılır
```

Buradaki önemli nokta, CPU'nun gerçekten beklememesidir.

Örneğin:

```c
HAL_Delay(100);
```

kullanılsaydı CPU yaklaşık 100 ms boyunca beklemek zorunda kalacaktı.

Bu driver'da ise:

```c
HAL_GetTick();
```

ile zaman kontrol edilir ve program çalışmaya devam eder.

Bu nedenle yapı **non-blocking debounce** olarak adlandırılır.

---

# 🧠 Debounce Neden Non-Blocking Yapıldı?

Blocking debounce:

```text
Butona basıldı
     ↓
HAL_Delay(100)
     ↓
CPU bekliyor
     ↓
100 ms sonra devam
```

Non-blocking debounce:

```text
Butona basıldı
     ↓
Zamanı kaydet
     ↓
Program çalışmaya devam et
     ↓
Geçen zamanı kontrol et
     ↓
Süre dolduysa durumu kabul et
```

Bu yaklaşım özellikle aynı anda:

* UART
* ADC
* Timer
* Motor kontrolü
* Sensör okuma
* Kullanıcı arayüzü

gibi başka işlemlerin de yürütüldüğü embedded sistemlerde daha kullanışlıdır.

---

# ⏲️ DEBOUNCE_TIME

Debounce süresi `io_driver.h` içerisinde tanımlanmıştır:

```c
#define DEBOUNCE_TIME 100
```

Bu değer milisaniye cinsindendir.

Örneğin:

```c
#define DEBOUNCE_TIME 50
```

yapılırsa buton durumunun yaklaşık 50 ms boyunca kararlı kalması beklenir.

Çok düşük bir değer seçilirse buton sıçramaları tekrar algılanabilir.

Çok yüksek bir değer seçilirse butonun tepki süresi gereksiz şekilde uzayabilir.

---

# 📤 GPIO Output Yönetimi

LED gibi GPIO çıkışlarının istenen durumları `IO_Info_t` içerisinde tutulur.

Örneğin:

```c
ioInfo.outputs_Info.ledGreen.pinStates = GPIO_PIN_SET;
```

LED'in açık olması istenmektedir.

Söndürmek için:

```c
ioInfo.outputs_Info.ledGreen.pinStates = GPIO_PIN_RESET;
```

kullanılır.

Burada önemli nokta şudur:

`pinStates` değişkeni uygulamanın **istenen çıkış durumunu** temsil eder.

Driver daha sonra bu bilgiyi gerçek GPIO pinine uygular.

Genel yapı:

```text
Uygulama
   ↓
pinStates
   ↓
GPIO Driver
   ↓
HAL_GPIO_WritePin()
   ↓
GPIO Pin
```

---

# 🔄 IO_Status_Control()

Driver'ın ana kontrol fonksiyonudur:

```c
IO_Status_Control(&ioInfo);
```

Bu fonksiyon `while(1)` içerisinde sürekli çağrılır.

Temel görevi:

```text
1. GPIO inputlarını oku
2. Debounce işlemini gerçekleştir
3. Filtrelenmiş input durumunu güncelle
4. Output durumlarını GPIO pinlerine uygula
```

şeklindedir.

Bu nedenle uygulama içerisinde sürekli olarak:

```c
HAL_GPIO_ReadPin(...)
```

ve:

```c
HAL_GPIO_WritePin(...)
```

yazmak yerine bu işlemler driver içerisinde toplanmıştır.

---

# 🔧 IO_Initialization()

Driver'ın başlangıç fonksiyonudur:

```c
IO_Initialization(&ioInfo);
```

Bu fonksiyon bir kez çağrılır.

Görevi driver içerisindeki GPIO yapılarını başlangıç durumuna getirmek ve gerekli değişkenleri hazırlamaktır.

Başlatma işlemi genel olarak:

```text
main()
  ↓
HAL_Init()
  ↓
MX_GPIO_Init()
  ↓
IO_Initialization()
  ↓
while(1)
```

şeklindedir.

---

# 🧱 Input_Status_t

Butonun durumunu daha okunabilir şekilde ifade etmek için enum yapısı kullanılmıştır.

Örneğin:

```c
Input_Status_LOW
Input_Status_HIGH
```

Bunun amacı doğrudan `0` veya `1` gibi değerler kullanmak yerine kodun okunabilirliğini artırmaktır.

Örneğin:

```c
if (ioInfo.inputs_Info.User_Button.inputStatus == Input_Status_HIGH)
```

ifadesi:

```c
if (button == 1)
```

ifadesine göre neyin kontrol edildiğini daha açık şekilde gösterir.

---

# 🏗️ Driver'ın Genel Çalışma Akışı

Sistemin tamamı şu şekilde düşünülebilir:

```text
                 STM32 GPIO
                     │
          ┌──────────┴──────────┐
          │                     │
        INPUT                OUTPUT
          │                     │
      User Button              LED
          │                     │
          ▼                     ▲
   GPIO Pin Read          pinStates
          │                     │
          ▼                     │
   Debounce Filter              │
          │                     │
          ▼                     │
   inputStatus            GPIO Write
          │                     ▲
          └─────────┬───────────┘
                    │
               IO_Info_t
                    │
                    ▼
             Application
```

Böylece uygulama ile STM32 GPIO donanımı arasında bir abstraction katmanı oluşturulmuş olur.

---

# 🎯 Neden Driver Kullanıldı?

Doğrudan `main.c` içerisinde:

```c
HAL_GPIO_ReadPin(...);
HAL_GPIO_WritePin(...);
```

kullanmak mümkün olsa da proje büyüdükçe GPIO işlemleri uygulama kodunun içine dağılabilir.

Bu driver ile:

```text
Donanım işlemleri
      ↓
GPIO Driver
      ↓
Uygulama
```

şeklinde bir ayrım oluşturulmuştur.

Bunun avantajları:

* Kodun daha düzenli olması
* GPIO işlemlerinin tek yerde toplanması
* Debounce işleminin uygulamadan ayrılması
* Farklı projelerde driver'ın tekrar kullanılabilmesi
* Daha okunabilir bir `main.c` oluşturulması

---

# ⚠️ Dikkat Edilmesi Gerekenler

* `MX_GPIO_Init()` mutlaka `IO_Initialization()` çağrısından önce çalıştırılmalıdır.
* Driver'ın kullandığı GPIO pinleri STM32CubeMX üzerinden doğru yapılandırılmalıdır.
* `IO_Status_Control()` fonksiyonu `while(1)` içerisinde düzenli olarak çağrılmalıdır.
* Debounce süresi uygulamaya uygun seçilmelidir.
* `HAL_Delay()` kullanarak debounce yapılmamalıdır.
* GPIO pinlerinin Pull-Up / Pull-Down ayarları kullanılan buton devresine uygun olmalıdır.

---

# 📂 Proje Yapısı

```text
STM32F4-GPIO-Driver
│
├── io_driver.c
├── io_driver.h
└── README.md
```

---

# 🛠️ Kullanılan Teknolojiler

* STM32F4
* ARM Cortex-M4
* Embedded C
* STM32 HAL
* GPIO
* Non-Blocking Debounce
* STM32CubeMX
* STM32CubeIDE

---

# 🎯 Projenin Amacı

Bu proje STM32F4 üzerinde GPIO işlemlerini daha modüler ve tekrar kullanılabilir hale getirmek, mekanik butonlar için **non-blocking debounce** mekanizması geliştirmek ve gömülü sistemlerde driver abstraction yaklaşımını uygulamak amacıyla hazırlanmıştır.

Aynı zamanda proje; **GPIO, struct yapıları, enum kullanımı, zaman tabanlı debounce ve modüler driver tasarımı** konularını pratik olarak öğrenmek amacıyla geliştirilmiştir.
****
