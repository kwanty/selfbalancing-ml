---
description: Uruchom główny skrypt testowy w środowisku pythonowym z Conda
---
Zanim włączysz ten proces upewnij się, że konwerter FT232 dopięto do odpowiednich portów (RX,TX) i wprowadzono poprawne zasilanie do płyty TB6612.

// turbo-all
1. Uruchom skrypt czytania MPU po środowisku szeregowym Pythona.
Uruchom komendę: `conda run --no-capture-output -n selfbalancing python sbr-py/keyboard_test.py UART`
