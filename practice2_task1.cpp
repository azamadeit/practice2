#include <iostream>      // cout
#include <cstdlib>       // rand, srand
#include <ctime>         // time
#include <chrono>        // измерение времени
#include <omp.h>         // OpenMP

// ================= ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ =================

void fill_array(int* a, int n) {                           // Заполнение массива
    for (int i = 0; i < n; ++i)
        a[i] = std::rand() % 100000;
}

void copy_array(const int* src, int* dst, int n) {         // Копирование массива
    for (int i = 0; i < n; ++i)
        dst[i] = src[i];
}

// ================= ПОСЛЕДОВАТЕЛЬНЫЕ СОРТИРОВКИ =================

// Пузырьковая сортировка
void bubble_sort_seq(int* a, int n) {
    for (int i = 0; i < n - 1; ++i)
        for (int j = 0; j < n - i - 1; ++j)
            if (a[j] > a[j + 1])
                std::swap(a[j], a[j + 1]);
}

// Сортировка выбором
void selection_sort_seq(int* a, int n) {
    for (int i = 0; i < n - 1; ++i) {
        int min_idx = i;
        for (int j = i + 1; j < n; ++j)
            if (a[j] < a[min_idx])
                min_idx = j;
        std::swap(a[i], a[min_idx]);
    }
}

// Сортировка вставками
void insertion_sort_seq(int* a, int n) {
    for (int i = 1; i < n; ++i) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
    }
}

// ================= ПАРАЛЛЕЛЬНЫЕ ВЕРСИИ (OpenMP) =================

// Параллельный пузырёк (odd-even)
void bubble_sort_par(int* a, int n) {
    for (int i = 0; i < n; ++i) {
        #pragma omp parallel for
        for (int j = i % 2; j < n - 1; j += 2)
            if (a[j] > a[j + 1])
                std::swap(a[j], a[j + 1]);
    }
}

// Параллельный выбор (распараллеливание поиска минимума)
void selection_sort_par(int* a, int n) {
    for (int i = 0; i < n - 1; ++i) {
        int min_idx = i;

        #pragma omp parallel for
        for (int j = i + 1; j < n; ++j) {
            #pragma omp critical
            {
                if (a[j] < a[min_idx])
                    min_idx = j;
            }
        }
        std::swap(a[i], a[min_idx]);
    }
}

// Параллельные вставки (условно, для учебных целей)
void insertion_sort_par(int* a, int n) {
    #pragma omp parallel for
    for (int i = 1; i < n; ++i) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
    }
}

// ================= ЗАМЕР ВРЕМЕНИ =================

template<typename Func>
long long measure(Func sort_func, int* a, int n) {
    auto start = std::chrono::high_resolution_clock::now();
    sort_func(a, n);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

// ================= MAIN =================

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    const int sizes[] = {1000, 10000, 100000};

    for (int n : sizes) {
        std::cout << "\nArray size: " << n << "\n";

        int* original = new int[n];
        int* temp = new int[n];

        fill_array(original, n);

        copy_array(original, temp, n);
        std::cout << "Bubble seq: " << measure(bubble_sort_seq, temp, n) << " ms\n";

        copy_array(original, temp, n);
        std::cout << "Bubble par: " << measure(bubble_sort_par, temp, n) << " ms\n";

        copy_array(original, temp, n);
        std::cout << "Selection seq: " << measure(selection_sort_seq, temp, n) << " ms\n";

        copy_array(original, temp, n);
        std::cout << "Selection par: " << measure(selection_sort_par, temp, n) << " ms\n";

        copy_array(original, temp, n);
        std::cout << "Insertion seq: " << measure(insertion_sort_seq, temp, n) << " ms\n";

        copy_array(original, temp, n);
        std::cout << "Insertion par: " << measure(insertion_sort_par, temp, n) << " ms\n";

        delete[] original;
        delete[] temp;
    }

    std::cout << "\nThreads used: " << omp_get_max_threads() << "\n";
    return 0;
}
