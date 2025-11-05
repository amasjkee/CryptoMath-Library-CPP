#pragma once

#include "group.hpp"
#include <concepts>
#include <optional>
#include <limits>

namespace cryptomath {

/**
 * @brief Порядок элемента в группе
 * 
 * Порядок элемента a в группе G, обозначаемый ord(a), - это наименьшее
 * положительное целое число n такое, что a^n = e (где e - единица).
 * 
 * Если такого n не существует, элемент имеет бесконечный порядок.
 * 
 * Свойства:
 * - ord(e) = 1
 * - ord(a) = ord(a⁻¹)
 * - Если a^n = e, то ord(a) делит n
 * - ord(a^k) = ord(a) / gcd(ord(a), k)
 */
template<typename T, typename Op>
    requires GroupConcept<T, Op>
class ElementOrder {
public:
    using group_type = Group<T, Op>;
    using element_type = T;

    /**
     * @brief Вычислить порядок элемента
     * 
     * @return Порядок, если конечен, или std::nullopt, если бесконечен
     */
    static std::optional<size_t> compute(const group_type& group, const T& element) {
        if (!group.get_set().contains(element)) {
            throw std::domain_error("Element not in group");
        }

        // Единица всегда имеет порядок 1
        if (element == group.identity()) {
            return 1;
        }

        // Для конечных групп можем вычислить итерацией
        size_t group_order = group.get_set().size();
        
        // Пробуем степени до порядка группы
        T current = element;
        for (size_t n = 1; n <= group_order; ++n) {
            if (current == group.identity()) {
                return n;
            }
            current = group.operate(current, element);
        }

        // Если не нашли единицу после |G| шагов, элемент имеет бесконечный порядок
        // (или группа бесконечна, в этом случае мы не можем определить за конечное время)
        return std::nullopt;
    }

    /**
     * @brief Проверить, имеет ли элемент конечный порядок
     */
    static bool is_finite(const group_type& group, const T& element) {
        return compute(group, element).has_value();
    }

    /**
     * @brief Проверить, имеет ли элемент бесконечный порядок
     */
    static bool is_infinite(const group_type& group, const T& element) {
        return !is_finite(group, element);
    }

    /**
     * @brief Получить порядок (выбрасывает исключение, если бесконечен)
     * 
     * @throws std::logic_error если элемент имеет бесконечный порядок
     */
    static size_t get_order(const group_type& group, const T& element) {
        auto order = compute(group, element);
        if (!order.has_value()) {
            throw std::logic_error("Element has infinite order");
        }
        return *order;
    }

    /**
     * @brief Проверить, имеет ли элемент конкретный порядок
     */
    static bool has_order(const group_type& group, const T& element, size_t n) {
        auto order = compute(group, element);
        return order.has_value() && *order == n;
    }

    /**
     * @brief Проверить, выполняется ли a^n = e для некоторого n
     * 
     * Это эквивалентно проверке, имеет ли элемент конечный порядок
     */
    static bool satisfies_identity_power(const group_type& group,
                                         const T& element,
                                         size_t n) {
        T power_result = group.power(element, static_cast<long long>(n));
        return power_result == group.identity();
    }

    /**
     * @brief Найти все элементы заданного порядка
     */
    static Set<T> elements_of_order(const group_type& group, size_t order) {
        Set<T> result;
        for (const auto& element : group.get_set()) {
            if (has_order(group, element, order)) {
                result.insert(element);
            }
        }
        return result;
    }

    /**
     * @brief Свойства порядка элемента
     */
    class Properties {
    public:
        /**
         * @brief Свойство: ord(a) = ord(a⁻¹)
         */
        static bool order_equals_inverse_order(const group_type& group,
                                                const T& element) {
            auto order = compute(group, element);
            if (!order.has_value()) {
                return false; // Нельзя легко сравнить бесконечные порядки
            }
            auto inv_order = compute(group, group.inverse(element));
            return order == inv_order;
        }

        /**
         * @brief Свойство: Если a^n = e, то ord(a) делит n
         */
        static bool order_divides_power(const group_type& group,
                                        const T& element,
                                        size_t n) {
            if (!satisfies_identity_power(group, element, n)) {
                return false; // a^n != e, so property doesn't apply
            }

            auto order = compute(group, element);
            if (!order.has_value()) {
                return false;
            }

            return n % *order == 0;
        }

        /**
         * @brief Свойство: ord(a^k) = ord(a) / gcd(ord(a), k)
         */
        static bool order_of_power(const group_type& group,
                                   const T& element,
                                   size_t k) {
            auto ord_a = compute(group, element);
            if (!ord_a.has_value()) {
                return false; // Нельзя вычислить для бесконечного порядка
            }

            size_t gcd_value = gcd(*ord_a, k);
            size_t expected_order = *ord_a / gcd_value;

            T power_element = group.power(element, static_cast<long long>(k));
            auto actual_order = compute(group, power_element);

            return actual_order.has_value() && *actual_order == expected_order;
        }
    };

private:
    // Вспомогательная функция для НОД
    static size_t gcd(size_t a, size_t b) {
        while (b != 0) {
            size_t temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }
};

/**
 * @brief Порядок элемента в циклической подгруппе
 * 
 * Порядок элемента равен порядку циклической подгруппы, которую он порождает.
 */
template<typename T, typename Op>
class CyclicSubgroupOrder {
public:
    using group_type = Group<T, Op>;

    /**
     * @brief Вычислить порядок через циклическую подгруппу
     */
    static std::optional<size_t> via_cyclic_subgroup(const group_type& group,
                                                      const T& element) {
        Set<T> cyclic_subgroup;
        cyclic_subgroup.insert(group.identity());
        
        T current = element;
        size_t max_iterations = group.get_set().size();
        
        for (size_t n = 1; n <= max_iterations; ++n) {
            if (current == group.identity()) {
                return n;
            }
            cyclic_subgroup.insert(current);
            current = group.operate(current, element);
            
            // Если мы уже видели этот элемент, мы нашли цикл
            if (cyclic_subgroup.contains(current) && current != group.identity()) {
                // Это не должно происходить, если мы строим правильно
                // Но обрабатываем на всякий случай
                return n;
            }
        }
        
        return std::nullopt;
    }
};

} // namespace cryptomath

