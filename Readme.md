# Bowl came score

## Сборка

- `make FLAVOR=release` - сбока резлизной версии.
- `make FLAVOR=debug run-test` - запуск тестов.
- `make FLAVOR=coverage gen-test-coverage` - сборка + запуск тестов + html отчет о покрытии тестами.

## Запуск

- `./main-release xxxxxxxxxxxx` - идеальная игра.
- `./main-release 256/` - Четыре броска.
- `./test-coverage` - запуск тестов.
- `./tools/autotest.sh` - атоматический запуск тестов при изменении исходников.

## Зависимости

- `lcov` - генератор отчетов о покрытии кода.
- `inotify-tools` - для автозапускалки тестов.

## Структура

- `src/util.h` - Полезности.
- `src/attotest.h` - Набор макросов для тестов.
- `src/bowl.h`, `src/bowl.c` - Расчет очков по строке с бросками.
- `src/test.c` - Тесты.
- `src/main.c` - main().

