# Лабораторные работы — Деев

## Лабораторная работа 1 — Django REST API

**Расположение:** `lab-1/`

Django-приложение с REST API для викторины. PostgreSQL, без Docker.

**Запуск:**
```bash
pip install -r requirements.txt
python manage.py migrate
python manage.py runserver
```

---

## Лабораторная работа 2 — Docker

**Расположение:** `lab-2/`

То же приложение, упакованное в Docker-контейнеры. Три варианта запуска:

| Папка | Описание |
|-------|----------|
| `local/` | Сборка образов из исходников, PostgreSQL в контейнере |
| `web_lite/` | Готовые образы с `dcr.deev.su`, PostgreSQL внешний |
| `web_pg/` | Готовые образы с `dcr.deev.su`, PostgreSQL в контейнере |

**Образы на registry:**
- `dcr.deev.su/deevev/lab2-backend:1.0.0`
- `dcr.deev.su/deevev/lab2-nginx:1.0.0`

**Запуск (любой из вариантов):**
```bash
cd lab-2/local       # или web_lite / web_pg
docker compose up
```

Перед запуском `web_lite` и `web_pg` нужен файл `.env` — пример в `lab-2/local/.env`.
