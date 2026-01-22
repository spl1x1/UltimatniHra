# Ultimatní hra

2D survival sandbox v C++.  

![Game Screenshot](assets/screenshot.png)

---

## ✨ Hlavní vlastnosti

- **Procedurální generace mapy podle seed** — každá hra je unikátní. Mapy využívají různé biomy, distribuci nerostů a variantní rozložení entit.
- **Těžitelné suroviny:**
    - Iron (železná ruda)
    - Gold (zlatá ruda)
    - Copper (měděná ruda)
- **Chest (truhla):**
    - Uložení předmětů, práce s inventářem a persistentní uložením.
- **Save/Load:**
    - Ukládání a načítání rozpracované hry.
- **Respawn Anchor:**
    - Umožňuje znovuzrození hráče po smrti.
- **Ghost mode:**
    - Mód, kdy je hráč po smrti nehmotný („duch“). Lze obnovit z anchoru.
- **Slime Enemy:**
    - 3 druhy slime nepřátel s odlišnou AI a chováním (např. Chase, Patrol, Attack, Flee).
- **Kvalitní animační engine:**
    - Plynulé sprite animace postav, potvor i efektů. Správa směru, variant a typů animace.
- **Základní Pathfinding a AI:**
    - AI entit včetně hledání cesty k hráči a stavu (Idle, Patrol, GetUnstuck, ...).
- **Práce s inventářem a předměty.**
- **Generování struktur:**
    - Stromy, ložiska rud, respawn anchor a další objekty.
- **Uživatelské rozhraní (UI):**
    - Základní inventář a zobrazení hráčových statistik.
- **Možnost rozšířit o multiplayer prvky (architektura je navržena s ohledem na budoucí síťování).**

## 💻 Multiplatformní podpora

Ultimatní hra je plně multiplatformní:  
➡️ **Windows**  
➡️ **Linux**  
➡️ **macOS**

Projekt využívá pouze portable knihovny a je pravidelně testován na všech třech OS.

## 📦 Instalace a spuštění

> **Požadavky:**
> - CMake >= 3.20
> - Ninja build system
> - C++20 kompatibilní překladač (GCC/Clang/MSVC)
> - SDL3 a SDL_image
> - Doporučeno: Linux, Windows nebo macOS

### ⚡ Rychlá kompilace (Linux/macOS/WSL/Windows s Ninja)

```bash
mkdir -p build 
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=/usr/bin/ninja -G Ninja -S .. -B .
cmake --build . --config Release --target all -- -j4
```

> **Poznámka:**  
> Cestu k Ninja (`-DCMAKE_MAKE_PROGRAM`) upravte podle své instalace.  
> Na Windows použijte příslušnou cestu (například `C:/Tools/Ninja/ninja.exe`).

**Spuštění:**
```bash
./UltimatniHra
```
(nebo `UltimatniHra.exe` na Windows)

## 📚 Použité knihovny

Projekt využívá tyto open-source knihovny a závislosti (viz `cmake/FetchDependecies.cmake` a další build scripty):

- [SDL3](https://github.com/libsdl-org/SDL) — hlavní grafická/zvuková knihovna (vstup, vykreslování, zvuk)
- [SDL_image](https://github.com/libsdl-org/SDL_image) — načítání obrázků
- [SDL_ttf](https://github.com/libsdl-org/SDL_ttf) - načítání textu
- [RmlUI](https://github.com/mikke89/RmlUi) — moderní HTML/CSS-like UI systém
- [FreeType](https://download.savannah.gnu.org/releases/freetype/) — vykreslování fontů
- [Lua-CPP](https://github.com/jordanvrtanoski/luacpp) — binding Lua do C++
- [HWInfo](https://github.com/C-And-Cpp-Libraries/hwinfo-machine-id) — systémové informace
- [ImGui](https://github.com/ocornut/imgui) — integrované debugovací a vývojářské rozhraní (viz soubor `cmake/ImguiDependency.cmake`)
- [Windows.h](https://learn.microsoft.com/en-us/windows/win32/api/) — pouze na Windows

Správa ImGui:
- ImGui je připojena jako submodul (`imgui`) a buildována automaticky skrze `cmake/ImguiDependency.cmake`.
- V UI (v debug režimu) lze zobrazit ImGui debug menu, overlay i další vývojářské featury.

<details>
<summary><strong>Ukázka integrace ImGui v kódu</strong></summary>

```cpp
#include "imgui.h"
// ...
ImGui::Begin("Ultimatni debug menu!");
// ...
ImGui::End();
```
</details>

---

## 🤖 Ukázka herních mechanik

- **Mapa generovaná seedem:**
    - Každý nový svět může být unikátní, enterujte seed při zakládání mapy.
- **Ores a těžba:**
    - Naleznete a těžte copper, iron, gold! Různé typy mají různé vlastnosti.
- **Slime Enemy:**
    - Setkáte se s několika typy „slimáků“ (různý vzhled, rychlost, chování).
- **Ghost mode:**
    - Po smrti se hráč stává duchem (některé akce jsou omezené), lze se vrátit pomocí anchoru.
- **Inventář & truhly:**
    - Ukládejte věci do truhly a učte se práci s inventářem.
- **Respawn systém:**
    - Respawn anchor umožňuje návrat zpět k životu na vhodném místě.

---

## 🚧 Plánované rozšíření

- Další typy nepřátel (AI)
- Více druhů struktur a interaktivních objektů
- Zlepšení použitelnosti UI
- Multiplayer/kooperace
- Rozšířený zvukový engine

## 🏗️ Struktura projektu

- `src/` — zdrojové kódy (engine, správa entit, AI, sprajty, ...)
- `include/` — hlavičkové soubory
- `assets/` — obrázky, zvuky, data
- `README.md` — tento soubor
