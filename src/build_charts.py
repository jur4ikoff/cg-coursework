import pandas as pd
import matplotlib.pyplot as plt
import os
import sys

IS_SHOW = 1
SCRIPT_PATH = os.path.abspath(sys.argv[0])
SCRIPT_DIR = os.path.dirname(SCRIPT_PATH)
DATA_PATH_1 = f"{SCRIPT_DIR}/charts/exp_1.csv"
DATA_PATH_2 = f"{SCRIPT_DIR}/charts/exp_2.csv"

RES_PATH_1 = f"{SCRIPT_DIR}/charts/exp_1.png"
RES_PATH_2 = f"{SCRIPT_DIR}/charts/exp_2.png"

def plot_execution_times(csv_path: str) -> None:
    """
    Строит кусочно-линейный график времени выполнения последовательного
    и параллельного алгоритмов в зависимости от количества объектов.

    Параметры:
    ----------
    csv_path : str
        Путь к CSV-файлу с колонками: obj_count, seq_time, parallel_time.
        Разделитель — точка с запятой (;).
    """
    # Загрузка данных
    df = pd.read_csv(csv_path, sep=";")

    # Проверка наличия нужных колонок
    required_columns = {"obj_count", "seq_time", "parallel_time"}
    if not required_columns.issubset(df.columns):
        raise ValueError(f"Файл должен содержать колонки: {required_columns}")

    # Построение графика
    plt.figure(figsize=(10, 6))
    plt.plot(
        df["obj_count"],
        df["seq_time"],
        marker="o",
        linestyle="-",
        label="Последовательный алгоритм",
    )
    plt.plot(
        df["obj_count"],
        df["parallel_time"],
        marker="s",
        linestyle="--",
        label="Параллельный алгоритм",
    )

    plt.xlabel("Количество объектов")
    plt.ylabel("Время выполнения (мс)")
    plt.grid(True, linestyle=":", alpha=0.7)
    plt.legend()
    plt.tight_layout()

    plt.savefig(RES_PATH_1, dpi=300, bbox_inches="tight")
    plt.close()

import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors

def plot_performance_by_threads(csv_path: str) -> None:
    """
    Строит график времени выполнения в зависимости от количества видимых объектов,
    для разных значений числа потоков.

    Параметры:
    ----------
    csv_path : str
        Путь к CSV-файлу с данными.
        Первая строка — заголовок: thread_count;1;2;3;5;10;15;20;25
        Последующие строки — время выполнения для каждого числа потоков.
    """
    # Загрузка данных
    df = pd.read_csv(csv_path, sep=';')

    # Проверка структуры
    if 'thread_count' not in df.columns:
        raise ValueError("Файл должен содержать столбец 'thread_count'")

    # Ось X — это названия столбцов (кроме thread_count) — количество видимых объектов
    x_values = [int(col) for col in df.columns if col != 'thread_count']

    # Цветовая палитра и маркеры (можно настроить под себя)
    colors = list(mcolors.TABLEAU_COLORS.values())  # 10 цветов
    markers = ['o', 's', '^', 'D', 'v', 'p', '*', 'X', 'H', '+']  # 10 маркеров

    # Настройка графика
    plt.figure(figsize=(12, 8))
    plt.xlabel('Количество видимых объектов', fontsize=12)
    plt.ylabel('Время выполнения (мс)', fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)

    # Рисуем по одной линии для каждого числа потоков
    for idx, row in df.iterrows():
        thread_count = int(row['thread_count'])
        y_values = row.drop('thread_count').values.astype(float)

        color = colors[idx % len(colors)]
        marker = markers[idx % len(markers)]

        plt.plot(x_values, y_values,
                 label=f'{thread_count} потоков',
                 color=color,
                 marker=marker,
                 linestyle='-',
                 linewidth=2,
                 markersize=6)

    plt.legend(loc='upper left', fontsize=10, ncol=2)
    plt.tight_layout()

    plt.savefig(RES_PATH_2, dpi=300, bbox_inches="tight")
    plt.close()


plot_execution_times(DATA_PATH_1)
plot_performance_by_threads(DATA_PATH_2)