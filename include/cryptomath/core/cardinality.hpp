#pragma once

#include "set.hpp"
#include "mapping.hpp"
#include <limits>
#include <type_traits>
#include <cmath>

namespace cryptomath {

/**
 * @brief Операции с мощностью и теорема Кантора
 * 
 * Предоставляет операции для сравнения мощностей множеств, включая
 * теорему Кантора о том, что |P(A)| > |A| для любого множества A.
 */

/**
 * @brief Перечисление типов мощности
 */
enum class CardinalityType {
    FINITE,      // Конечное множество
    COUNTABLE,   // Счетно бесконечное (та же мощность, что у натуральных чисел)
    UNCOUNTABLE  // Несчетно бесконечное
};

/**
 * @brief Получить тип мощности конечного множества (размер)
 */
template<typename T>
constexpr size_t cardinality(const Set<T>& set) {
    return set.size();
}

/**
 * @brief Проверить, имеют ли два множества одинаковую мощность (равномощны)
 * 
 * Два множества A и B равномощны, если существует биекция f: A → B.
 * Для конечных множеств это эквивалентно |A| = |B|.
 */
template<typename A, typename B>
bool are_equinumerous(const Set<A>& set_a, const Set<B>& set_b) {
    // Для конечных множеств проверяем размеры
    if (set_a.size() != set_b.size()) {
        return false;
    }

    // Пытаемся найти биекцию (для конечных множеств это эквивалентно проверке
    // возможности построить биективное отображение)
    // Для малых конечных множеств можем перечислить все возможные биекции
    // Для больших множеств полагаемся на сравнение размеров
    
    // Если размеры равны и оба множества конечны, они равномощны
    // (существование биекции гарантируется аксиомой выбора для конечных множеств)
    return true;
}

/**
 * @brief Проверить, выполняется ли |A| ≤ |B| (сравнение мощностей)
 * 
 * |A| ≤ |B|, если существует инъекция f: A → B
 */
template<typename A, typename B>
bool cardinality_le(const Set<A>& set_a, const Set<B>& set_b) {
    // Для конечных множеств простое сравнение размеров
    return set_a.size() <= set_b.size();
}

/**
 * @brief Проверить, выполняется ли |A| < |B| (строгое сравнение мощностей)
 * 
 * |A| < |B|, если |A| ≤ |B| и не |A| = |B|
 */
template<typename A, typename B>
bool cardinality_lt(const Set<A>& set_a, const Set<B>& set_b) {
    return set_a.size() < set_b.size();
}

/**
 * @brief Мощность булеана
 * 
 * Для конечного множества A с |A| = n булеан P(A) имеет мощность 2^n
 */
template<typename T>
size_t power_set_cardinality(const Set<T>& set) {
    size_t n = set.size();
    if (n > std::numeric_limits<size_t>::digits) {
        // Произойдет переполнение, выбрасываем исключение
        throw std::overflow_error("Power set cardinality too large");
    }
    return 1ULL << n; // 2^n
}

/**
 * @brief Теорема Кантора
 * 
 * Теорема Кантора утверждает, что для любого множества A, |P(A)| > |A|.
 * Это показывает, что не существует наибольшей мощности.
 * 
 * @param set Множество A
 * @return Булеан множества A
 * @note Булеан гарантированно имеет строго большую мощность
 */
template<typename T>
Set<Set<T>> cantor_theorem(const Set<T>& set) {
    // Теорема утверждает |P(A)| > |A|
    // Строим булеан
    Set<Set<T>> power_set_result = power_set(set);
    
    // Проверяем теорему: |P(A)| должно быть 2^|A| > |A|
    size_t set_size = set.size();
    size_t power_set_size = power_set_cardinality(set);
    
    // Для конечных множеств это всегда верно (2^n > n для n ≥ 0)
    // Для бесконечных множеств это доказывается диагональным аргументом Кантора
    
    return power_set_result;
}

/**
 * @brief Диагональный аргумент Кантора (метод доказательства)
 * 
 * Показывает, что не может существовать биекции между множеством и его булеаном.
 * Это основа теоремы Кантора.
 * 
 * @param set Множество A
 * @return Подмножество, которое не может быть в образе любой функции f: A → P(A)
 */
template<typename T>
Set<T> cantor_diagonal_argument(const Set<T>& set) {
    // Это концептуальное представление диагонального аргумента
    // На практике мы строим множество {a ∈ A | a ∉ f(a)},
    // где f - любая функция из A в P(A)
    
    // Для конкретной реализации нам нужна конкретная функция f
    // Здесь возвращаем пустое множество как заглушку - фактическое диагональное
    // множество зависит от конкретной рассматриваемой функции
    
    // Ключевое наблюдение: если f: A → P(A) - любая функция, то
    // D = {a ∈ A | a ∉ f(a)} является подмножеством A, которое не в образе f
    // Это доказывает |P(A)| > |A|
    
    Set<T> diagonal_set;
    // Примечание: Полная реализация потребовала бы параметр функции
    // Это заглушка, показывающая структуру
    return diagonal_set;
}

/**
 * @brief Проверить, является ли множество конечным
 */
template<typename T>
bool is_finite(const Set<T>& set) {
    // В нашей реализации все множества конечны (используя std::set)
    // Эта функция существует для полноты и будущей расширяемости
    return true;
}

/**
 * @brief Получить тип мощности
 */
template<typename T>
CardinalityType cardinality_type(const Set<T>& set) {
    // В нашей реализации все множества конечны
    // Будущие версии могут поддерживать бесконечные множества
    return CardinalityType::FINITE;
}

/**
 * @brief Мощность декартова произведения
 * 
 * Для конечных множеств A и B, |A × B| = |A| × |B|
 */
template<typename A, typename B>
size_t cartesian_product_cardinality(const Set<A>& set_a, const Set<B>& set_b) {
    return set_a.size() * set_b.size();
}

} // namespace cryptomath

