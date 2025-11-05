#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cryptomath/core.hpp>

using namespace cryptomath;

// Вспомогательная функция для чтения множества целых чисел
Set<int> read_set_from_input(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    
    Set<int> result;
    std::istringstream iss(line);
    int value;
    while (iss >> value) {
        result.insert(value);
    }
    return result;
}

// Вспомогательная функция для вывода множества
void print_set(const Set<int>& s, const std::string& name = "") {
    if (!name.empty()) {
        std::cout << name << " = {";
    } else {
        std::cout << "{";
    }
    bool first = true;
    for (const auto& elem : s) {
        if (!first) std::cout << ", ";
        std::cout << elem;
        first = false;
    }
    std::cout << "}";
    if (!name.empty()) {
        std::cout << ", размер: " << s.size();
    }
    std::cout << "\n";
}

void show_menu() {
    std::cout << "\n==========================================\n";
    std::cout << "   CryptoMath Library - Интерактивный режим\n";
    std::cout << "==========================================\n";
    std::cout << "1. Операции над множествами (объединение, пересечение, разность)\n";
    std::cout << "2. Декартово произведение множеств\n";
    std::cout << "3. Булеан (множество всех подмножеств)\n";
    std::cout << "4. Функция Эйлера φ(n)\n";
    std::cout << "5. Мощность множества\n";
    std::cout << "6. Проверка свойств отношений\n";
    std::cout << "0. Выход\n";
    std::cout << "==========================================\n";
    std::cout << "Выберите операцию: ";
}

void menu_set_operations() {
    std::cout << "\n--- Операции над множествами ---\n";
    std::cout << "Введите элементы первого множества (через пробел): ";
    
    Set<int> set1 = read_set_from_input("");
    print_set(set1, "A");
    
    std::cout << "Введите элементы второго множества (через пробел): ";
    Set<int> set2 = read_set_from_input("");
    print_set(set2, "B");
    
    std::cout << "\nРезультаты:\n";
    
    auto union_set = set1.union_with(set2);
    std::cout << "  A ∪ B = ";
    print_set(union_set);
    std::cout << "    Размер: " << union_set.size() << "\n";
    
    auto intersection_set = set1.intersection(set2);
    std::cout << "  A ∩ B = ";
    print_set(intersection_set);
    std::cout << "    Размер: " << intersection_set.size() << "\n";
    
    auto difference_set = set1.difference(set2);
    std::cout << "  A \\ B = ";
    print_set(difference_set);
    std::cout << "    Размер: " << difference_set.size() << "\n";
    
    auto sym_diff = set1.symmetric_difference(set2);
    std::cout << "  A Δ B = ";
    print_set(sym_diff);
    std::cout << "    Размер: " << sym_diff.size() << "\n";
}

void menu_cartesian_product() {
    std::cout << "\n--- Декартово произведение ---\n";
    std::cout << "Введите элементы первого множества (через пробел): ";
    Set<int> set1 = read_set_from_input("");
    print_set(set1, "A");
    
    std::cout << "Введите элементы второго множества (через пробел): ";
    Set<int> set2 = read_set_from_input("");
    print_set(set2, "B");
    
    auto cartesian = cartesian_product(set1, set2);
    std::cout << "\nA × B содержит " << cartesian.size() << " пар:\n";
    for (const auto& pair : cartesian) {
        std::cout << "  (" << pair.first << ", " << pair.second << ")\n";
    }
}

void menu_power_set() {
    std::cout << "\n--- Булеан (множество всех подмножеств) ---\n";
    std::cout << "Введите элементы множества (через пробел): ";
    Set<int> input_set = read_set_from_input("");
    print_set(input_set, "Исходное множество");
    
    if (input_set.size() > 6) {
        std::cout << "Внимание: множество содержит " << input_set.size() 
                  << " элементов.\n";
        std::cout << "Булеан будет содержать 2^" << input_set.size() 
                  << " = " << power_set_cardinality(input_set) << " подмножеств.\n";
        std::cout << "Это может занять много времени. Продолжить? (y/n): ";
        char choice;
        std::cin >> choice;
        std::cin.ignore();
        if (choice != 'y' && choice != 'Y') {
            return;
        }
    }
    
    auto power_set_result = power_set(input_set);
    std::cout << "\nБулеан P(A) содержит " << power_set_result.size() 
              << " подмножеств:\n";
    
    int count = 0;
    for (const auto& subset : power_set_result) {
        std::cout << "  " << (count + 1) << ". ";
        print_set(subset);
        count++;
    }
}

void menu_euler_function() {
    std::cout << "\n--- Функция Эйлера φ(n) ---\n";
    std::cout << "Введите число n: ";
    size_t n;
    std::cin >> n;
    std::cin.ignore();
    
    size_t phi = EulerFunction::compute(n);
    std::cout << "φ(" << n << ") = " << phi << "\n";
    
    std::cout << "\nОбъяснение: φ(" << n << ") - это количество чисел от 1 до " << n
              << ", которые взаимно просты с " << n << ".\n";
    
    // Показываем числа, взаимно простые с n
    auto coprimes = EulerFunction::get_coprime_numbers(n);
    std::cout << "Взаимно простые числа: ";
    for (size_t i = 0; i < coprimes.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << coprimes[i];
    }
    std::cout << "\n";
}

void menu_cardinality() {
    std::cout << "\n--- Мощность множества ---\n";
    std::cout << "Введите элементы множества (через пробел): ";
    Set<int> input_set = read_set_from_input("");
    print_set(input_set, "Множество");
    
    size_t card = cardinality(input_set);
    std::cout << "\nМощность: |A| = " << card << "\n";
    
    size_t power_set_card = power_set_cardinality(input_set);
    std::cout << "Мощность булеана: |P(A)| = " << power_set_card 
              << " (2^" << card << " = " << power_set_card << ")\n";
}

void menu_relations() {
    std::cout << "\n--- Свойства отношений ---\n";
    std::cout << "Введите элементы области определения (через пробел): ";
    Set<int> domain = read_set_from_input("");
    print_set(domain, "Область определения");
    
    std::cout << "\nВведите пары отношения в формате 'a b' (по одной паре на строку).\n";
    std::cout << "Для завершения ввода введите пустую строку:\n";
    
    std::vector<std::pair<int, int>> pairs;
    std::string line;
    while (std::getline(std::cin, line) && !line.empty()) {
        std::istringstream iss(line);
        int a, b;
        if (iss >> a >> b) {
            pairs.push_back({a, b});
            std::cout << "  Добавлена пара: (" << a << ", " << b << ")\n";
        }
    }
    
    Set<std::pair<int, int>> pair_set(pairs.begin(), pairs.end());
    Relation<int> relation(domain, pair_set);
    
    std::cout << "\nСвойства отношения:\n";
    std::cout << "  Рефлексивность: " << (relation.is_reflexive() ? "да" : "нет") << "\n";
    std::cout << "  Симметричность: " << (relation.is_symmetric() ? "да" : "нет") << "\n";
    std::cout << "  Антисимметричность: " << (relation.is_antisymmetric() ? "да" : "нет") << "\n";
    std::cout << "  Транзитивность: " << (relation.is_transitive() ? "да" : "нет") << "\n";
    std::cout << "  Отношение эквивалентности: " << (relation.is_equivalence_relation() ? "да" : "нет") << "\n";
    std::cout << "  Частичный порядок: " << (relation.is_partial_order() ? "да" : "нет") << "\n";
}

int main() {
    std::cout << "Добро пожаловать в CryptoMath Library!\n";
    
    int choice;
    while (true) {
        show_menu();
        std::cin >> choice;
        std::cin.ignore(); // Очистить буфер после ввода числа
        
        switch (choice) {
            case 1:
                menu_set_operations();
                break;
            case 2:
                menu_cartesian_product();
                break;
            case 3:
                menu_power_set();
                break;
            case 4:
                menu_euler_function();
                break;
            case 5:
                menu_cardinality();
                break;
            case 6:
                menu_relations();
                break;
            case 0:
                std::cout << "\nДо свидания!\n";
                return 0;
            default:
                std::cout << "\nНеверный выбор. Попробуйте снова.\n";
                break;
        }
        
        std::cout << "\nНажмите Enter для продолжения...";
        std::cin.get();
    }
    
    return 0;
}
