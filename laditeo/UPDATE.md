# Telegram_laditeo

Форк [telegramdesktop/tdesktop](https://github.com/telegramdesktop/tdesktop), ветка `dev`.
Свои правки только на ветке `laditeo`. Ветка `dev` следит за upstream и не содержит патчей.

This is a fork of official tdesktop. Custom work stays on branch `laditeo`.
`dev` tracks upstream so it can be updated without mixing in local edits.

## Что изменено / What changed

Флаги в `Telegram/SourceFiles/laditeo/features.h` (по умолчанию выключены):

- `Laditeo::kShowMessageTimes` — часы у сообщения. `false`: не рисуются.
- `Laditeo::kShowLastSeen` — текст «был(а) недавно / online / last seen». `false`: не рисуется. Это только скрытие в интерфейсе, настройки приватности аккаунта не меняются.

Клавиатура, когда фокус на списке сообщений (не в поле ввода):

- `J` — следующее сообщение, прыжок (без плавной прокрутки).
- `K` — предыдущее сообщение, так же прыжком.
- `L` — быстрая реакция (`reactions().favoriteId()`, обычно thumbs-up) на сообщение в фокусе. Повторное нажатие снимает её, как обычный quick reaction.
- У сообщения с этим фокусом рисуется рамка.

`J` / `K` / `L` без модификаторов в tdesktop не были заняты.
Уже заняты и не трогались: `Ctrl+J` открывает контакты (`ShowContacts`), `Ctrl+L` блокирует приложение (`Lock`).

Голые буквы нельзя вешать на глобальный `ApplicationShortcut`: из‑за этого перестанут набираться `j`/`k`/`l` в поле сообщения. Экран настроек шорткатов тоже не даёт назначить букву без модификатора (`Shortcuts::AllowWithoutModifiers`). Поэтому команды зарегистрированы в `Shortcuts::Command`, а сами клавиши ловятся в `HistoryInner::keyPressEvent` и уходят в `Shortcuts::Launch` только когда фокус у списка истории.

Чтобы `J`/`K`/`L` заработали, кликни по истории сообщений (список принимает фокус). Пока курсор в поле ввода, буквы печатаются как обычно.

## Файлы / Files

Поиск по комментарию `LADITEO`.

| Файл | Зачем |
| --- | --- |
| `Telegram/SourceFiles/laditeo/features.h` | два флага |
| `Telegram/SourceFiles/core/shortcuts.h` | команды `LaditeoMessageNext/Previous/React` |
| `Telegram/SourceFiles/core/shortcuts.cpp` | имена команд, без глобальных клавиш |
| `Telegram/SourceFiles/history/history_inner_widget.h` | фокус и шаг J/K |
| `Telegram/SourceFiles/history/history_inner_widget.cpp` | J/K/L, прыжок, рамка |
| `Telegram/SourceFiles/history/history_widget.cpp` | обработка команд шорткатов |
| `Telegram/SourceFiles/history/view/history_view_bottom_info.cpp` | часы в пузыре, пересылке, подписи медиа |
| `Telegram/SourceFiles/history/view/history_view_element.cpp` | тултип с датой сообщения |
| `Telegram/SourceFiles/history/view/history_view_context_menu.cpp` | пункт меню с временем отправки |
| `Telegram/SourceFiles/history/view/media/history_view_call.cpp` | часы на звонке в чате |
| `Telegram/SourceFiles/history/view/media/history_view_location.cpp` | часы у live location |
| `Telegram/SourceFiles/dialogs/ui/dialogs_layout.cpp` | время последнего сообщения в списке чатов |
| `Telegram/SourceFiles/media/view/media_view_overlay_widget.cpp` | время в просмотрщике медиа |
| `Telegram/SourceFiles/data/data_peer_values.cpp` | текст last seen / online |

Не спрятано специально: разделители дней («5 октября»), копирование текста сообщения в буфер (там время остаётся), озвучка screen reader. Код форматирования времени не удалён.

## Обновление upstream / Rebase

```bat
cd /d C:\Users\user\Documents\Github\Telegram_laditeo
git fetch upstream
git checkout dev
git merge --ff-only upstream/dev
git checkout laditeo
git rebase upstream/dev
```

Конфликты ищи по `LADITEO`. Не коммить патчи в `dev`. Не force-push. PR в telegramdesktop/tdesktop не открывать.

Сабмодули для сборки огромные и в этот клон не инициализировались. Перед сборкой:

```bat
git submodule update --init --recursive
```

## Сборка на Windows / Lightest real build

Официальная инструкция: `docs/building-win.md` на ветке `dev`.

Коротко: **MinGW и «просто Qt Creator + его kit» текущий tdesktop не собирают.** В документации нет пути через MinGW. Генератор — MSVC (`Visual Studio 18 2026` или Ninja Multi-Config из CI). Отдельный порт под MinGW здесь не делался.

Qt для приложения **не** берётся из установщика Qt Creator. Его собирает `Telegram\build\prepare\win.bat` (в дереве библиотек, сейчас это Qt 5.15.x, в CI есть и матрица qt6). Qt Creator можно поставить только как редактор исходников. Сборка — CMake + MSVC.

Полная Visual Studio IDE **не обязательна**. Официальный CI (`.github/workflows/win.yml`) для arm64 ставит Build Tools, а x64 собирает командой `cmake --build`, без открытия IDE:

```bat
cmake --build ..\out --config Debug --parallel
```

Нужный набор (имена пакетов как в инсталляторе VS 2026 / в workflow):

1. Установщик Build Tools, не IDE: <https://aka.ms/vs/18/stable/vs_buildtools.exe>
2. Workload: `Microsoft.VisualStudio.Workload.VCTools` (Desktop development with C++)
3. Toolset, который требует документация, не дефолтный v145: `Microsoft.VisualStudio.Component.VC.14.44.17.14.x86.x64` (MSVC v143, v14.44-17.14). Дефолтный v145 не умеет Windows 7, поэтому везде `-vcvars_ver=14.44`.
4. SDK из `docs/building-win.md`: **10.0.26100.0**, компонент `Microsoft.VisualStudio.Component.Windows11SDK.26100`
5. CMake из workload: `Microsoft.VisualStudio.Component.VC.CMake.Project` (рекомендуемый компонент VCTools). В CI для части сборок генератор `Ninja Multi-Config`.
6. Python **3.10** в PATH и Git — это прямо написано в `docs/building-win.md`.
7. Для arm64 CI дополнительно ставит `Microsoft.VisualStudio.Component.VC.Tools.ARM64` и `Microsoft.VisualStudio.Component.VC.ATL.ARM64`. Для обычного x64 это не требуется документацией.

Пример тихой установки Build Tools (x64, без IDE):

```bat
vs_buildtools.exe --quiet --wait --norestart --nocache ^
  --add Microsoft.VisualStudio.Workload.VCTools ^
  --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 ^
  --add Microsoft.VisualStudio.Component.VC.14.44.17.14.x86.x64 ^
  --add Microsoft.VisualStudio.Component.Windows11SDK.26100 ^
  --add Microsoft.VisualStudio.Component.VC.CMake.Project
```

Дальше терминал **не** Community, а Build Tools. Путь в официальном документе указан для Community (`...\18\Community\...`). У Build Tools тот же скрипт лежит рядом с установкой, обычно:

```bat
"%ProgramFiles(x86)%\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat" -vcvars_ver=14.44
```

Если установщик положил дерево в `C:\Program Files\Microsoft Visual Studio\18\BuildTools`, используй его. Проверка: в этом окне `cl` есть, и `echo %VCToolsVersion%` начинается с `14.44`.

Потом, уже в этом окружении (не в обычном PowerShell):

```bat
git submodule update --init --recursive
Telegram\build\prepare\win.bat
cd Telegram
configure.bat x64 -D TDESKTOP_API_ID=YOUR_API_ID -D TDESKTOP_API_HASH=YOUR_API_HASH
cmake --build ..\out --config Debug --parallel
```

`api_id` / `api_hash` — свои, с https://my.telegram.org . Без них `configure.bat` не настраивает проект. Открывать `out\Telegram.slnx` в полной Visual Studio не нужно: `cmake --build` делает то же, что CI.

Эта копия **не собиралась**: на машине не проверялся и не ставился toolchain Qt/MSVC.
