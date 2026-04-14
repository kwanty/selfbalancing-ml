---
description: Skompiluj i wgraj oprogramowanie PlatformIO bezpośrednio na urządzenie
---
Ten workflow należy wywołać wyłączenie wtedy, gdy Arduino jest podpięte komputerem bezpośrednim kablem USB (a odpięte są piny od RS232/Bluetooth z TB6612).

// turbo-all
1. Sflashuj i zapisz nową pamięć z wyłączeniem oprogramowania w tle.
Uruchom komendę: `conda run --no-capture-output -n selfbalancing pio run -t upload -d firmware/`
