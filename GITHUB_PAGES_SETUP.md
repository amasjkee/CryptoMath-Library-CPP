# Настройка GitHub Pages для Doxygen документации

## Автоматическая настройка

GitHub Actions workflow уже настроен и будет автоматически публиковать документацию при каждом коммите в ветку `main` или `master`.

## Первоначальная настройка GitHub Pages

1. Перейдите в настройки репозитория на GitHub:
   - Settings → Pages

2. В разделе "Source" выберите:
   - Source: **GitHub Actions**

3. GitHub Actions автоматически:
   - Установит Doxygen
   - Сгенерирует документацию
   - Опубликует её на GitHub Pages

## Ручная активация (если нужно)

Если автоматическая настройка не сработала, можно запустить workflow вручную:

1. Перейдите в раздел **Actions** на GitHub
2. Выберите workflow **"Deploy Doxygen Documentation to GitHub Pages"**
3. Нажмите **"Run workflow"** → выберите ветку → **"Run workflow"**

## Проверка

После первого успешного запуска workflow:
- Документация будет доступна по адресу: `https://amasjkee.github.io/CryptoMath-Library-CPP/`
- Время публикации: обычно 1-2 минуты после коммита

## Что делает workflow

1. **Триггеры:**
   - Push в ветки `main` или `master`
   - Изменения в `include/**`, `Doxyfile` или самом workflow
   - Ручной запуск через `workflow_dispatch`

2. **Процесс:**
   - Устанавливает Doxygen и Graphviz
   - Генерирует HTML документацию
   - Публикует на GitHub Pages

3. **Результат:**
   - Документация доступна онлайн
   - Автоматически обновляется при каждом коммите

## Troubleshooting

Если документация не публикуется:

1. Проверьте, что в настройках репозитория включен GitHub Pages:
   - Settings → Pages → Source должен быть "GitHub Actions"

2. Проверьте логи workflow:
   - Actions → выберите последний запуск → просмотрите логи

3. Убедитесь, что workflow файл находится в:
   - `.github/workflows/docs.yml`

4. Проверьте права доступа:
   - Settings → Actions → General → Workflow permissions
   - Должно быть: "Read and write permissions"

