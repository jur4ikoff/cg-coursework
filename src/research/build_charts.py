import pandas as pd
import matplotlib.pyplot as plt
import os
import sys

IS_SHOW = 1
SCRIPT_PATH = os.path.abspath(sys.argv[0])
SCRIPT_DIR = os.path.dirname(SCRIPT_PATH)

# Пути к новым файлам
SAMPLES_FILE = f"{SCRIPT_DIR}/sample_exp.csv"
DEPTH_FILE = f"{SCRIPT_DIR}/depth_exp.csv"

CHARTS_DIR = f"{SCRIPT_DIR}/../../report/img"
if not os.path.exists(CHARTS_DIR):
    os.makedirs(CHARTS_DIR, exist_ok=True)


def plot_samples_vs_time():
    """Строит график: количество сэмплов (x) vs время (y)"""
    if not os.path.exists(SAMPLES_FILE):
        print(f"Файл {SAMPLES_FILE} не найден. Пропуск графика 'samples vs time'.")
        return

    df = pd.read_csv(SAMPLES_FILE, sep=";")
    df.columns = df.columns.str.strip()

    x = df["samples_count"]
    y = df["time"]

    plt.figure(figsize=(8, 5))
    plt.plot(x, y, marker="o", markersize=5, color="blue", linestyle="-")
    
    # 🔑 Ключевое исправление: явно задаём метки по оси X
    plt.xticks(x)  # только значения из данных
    
    plt.title("Фиксированная глубина рекурсии = 10")
    plt.xlabel("Количество испускаемых лучей на пиксель")
    plt.ylabel("Время (мс)")
    plt.grid(True, linewidth=0.5)
    plt.tight_layout()

    filename = f"{CHARTS_DIR}/sample_chart.png"
    plt.savefig(filename, dpi=300, bbox_inches="tight")
    plt.close()
    print(f"График 'samples vs time' сохранён как {filename}")


def plot_depth_vs_time():
    """Строит график: глубина рекурсии (x) vs время (y)"""
    if not os.path.exists(DEPTH_FILE):
        print(f"Файл {DEPTH_FILE} не найден. Пропуск графика 'depth vs time'.")
        return

    df = pd.read_csv(DEPTH_FILE, sep=";")
    df.columns = df.columns.str.strip()

    x = df["depth"]
    y = df["time"]

    plt.figure(figsize=(8, 5))
    plt.plot(x, y, marker="s", markersize=5, color="green", linestyle="-")
    
    # 🔑 Ключевое исправление: явно задаём метки по оси X
    plt.xticks(x)  # только значения из данных

    plt.title("Фиксированное количество лучей (10)")
    plt.xlabel("Глубина рекурсии")
    plt.ylabel("Время (мкс)")
    plt.grid(True, linewidth=0.5)
    plt.tight_layout()

    filename = f"{CHARTS_DIR}/depth_chart.png"
    plt.savefig(filename, dpi=300, bbox_inches="tight")
    plt.close()
    print(f"График 'depth vs time' сохранён как {filename}")


if __name__ == "__main__":
    plot_samples_vs_time()
    plot_depth_vs_time()