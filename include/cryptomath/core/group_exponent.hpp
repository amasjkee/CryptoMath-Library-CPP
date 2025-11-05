#pragma once

#include "group.hpp"
#include "element_order.hpp"
#include <concepts>
#include <optional>
#include <numeric>
#include <algorithm>

namespace cryptomath {

/**
 * @brief Показатель группы
 * 
 * Показатель конечной группы G, обозначаемый exp(G), - это наименьшее положительное
 * целое число n такое, что a^n = e для всех a ∈ G.
 * 
 * Эквивалентно, exp(G) - это наименьшее общее кратное (НОК) порядков
 * всех элементов в G.
 * 
 * Свойства:
 * - exp(G) делит |G|
 * - exp(G) = |G| тогда и только тогда, когда G циклическая
 * - exp(G) = 1 тогда и только тогда, когда G тривиальна
 */
template<typename T, typename Op>
    requires GroupConcept<T, Op>
class GroupExponent {
public:
    using group_type = Group<T, Op>;
    using element_type = T;

    /**
     * @brief Вычислить показатель группы
     * 
     * @return Показатель, если конечен, или std::nullopt, если бесконечен
     */
    static std::optional<size_t> compute(const group_type& group) {
        // Для бесконечных групп показатель может не существовать
        // Для конечных групп вычисляем НОК всех порядков элементов

        std::vector<size_t> orders;
        
        for (const auto& element : group.get_set()) {
            auto order = ElementOrder<T, Op>::compute(group, element);
            if (!order.has_value()) {
                // Если какой-либо элемент имеет бесконечный порядок, показатель группы бесконечен
                return std::nullopt;
            }
            orders.push_back(*order);
        }

        if (orders.empty()) {
            return std::nullopt;
        }

        // Вычисляем НОК всех порядков
        return lcm_vector(orders);
    }

    /**
     * @brief Получить показатель (выбрасывает исключение, если бесконечен)
     * 
     * @throws std::logic_error если группа имеет бесконечный показатель
     */
    static size_t get_exponent(const group_type& group) {
        auto exponent = compute(group);
        if (!exponent.has_value()) {
            throw std::logic_error("Group has infinite exponent");
        }
        return *exponent;
    }

    /**
     * @brief Проверить, имеет ли группа конечный показатель
     */
    static bool is_finite(const group_type& group) {
        return compute(group).has_value();
    }

    /**
     * @brief Проверить, что a^n = e для всех элементов a и заданного n
     */
    static bool satisfies_exponent(const group_type& group, size_t n) {
        for (const auto& element : group.get_set()) {
            T power_result = group.power(element, static_cast<long long>(n));
            if (power_result != group.identity()) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Проверить, делит ли показатель порядок группы
     * 
     * Свойство: exp(G) всегда делит |G|
     */
    static bool divides_group_order(const group_type& group) {
        auto exp = compute(group);
        if (!exp.has_value()) {
            return false; // Нельзя проверить для бесконечного
        }

        size_t group_order = group.get_set().size();
        return group_order % *exp == 0;
    }

    /**
     * @brief Проверить, является ли группа циклической через показатель
     * 
     * Группа циклическая тогда и только тогда, когда exp(G) = |G|
     */
    static bool is_cyclic(const group_type& group) {
        auto exp = compute(group);
        if (!exp.has_value()) {
            return false;
        }

        size_t group_order = group.get_set().size();
        return *exp == group_order;
    }

    /**
     * @brief Проверить, имеет ли группа показатель n
     * 
     * Группа имеет показатель n, если exp(G) = n
     */
    static bool has_exponent(const group_type& group, size_t n) {
        auto exp = compute(group);
        return exp.has_value() && *exp == n;
    }

    /**
     * @brief Найти все группы с заданным показателем (для классификации)
     * 
     * Это вспомогательная функция для теоретических целей - проверяет, соответствует ли
     * текущая группа заданному значению показателя.
     */
    static bool matches_exponent(const group_type& group, size_t exponent) {
        return has_exponent(group, exponent) && satisfies_exponent(group, exponent);
    }

private:
    // Вспомогательная функция НОД (совместимо с C++17)
    static size_t gcd(size_t a, size_t b) {
        while (b != 0) {
            size_t temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

    /**
     * @brief Вычислить НОК двух чисел
     */
    static size_t lcm(size_t a, size_t b) {
        if (a == 0 || b == 0) {
            return 0;
        }
        return (a / gcd(a, b)) * b;
    }

    /**
     * @brief Вычислить НОК вектора чисел
     */
    static size_t lcm_vector(const std::vector<size_t>& numbers) {
        if (numbers.empty()) {
            return 1;
        }

        size_t result = numbers[0];
        for (size_t i = 1; i < numbers.size(); ++i) {
            result = lcm(result, numbers[i]);
        }

        return result;
    }
};

/**
 * @brief Связь между показателем и порядком
 * 
 * Показатель группы - это НОК порядков всех элементов.
 */
template<typename T, typename Op>
class ExponentOrderRelation {
public:
    using group_type = Group<T, Op>;

    /**
     * @brief Проверить, что показатель равен НОК всех порядков элементов
     */
    static bool verify_relation(const group_type& group) {
        auto computed_exponent = GroupExponent<T, Op>::compute(group);
        if (!computed_exponent.has_value()) {
            return false;
        }

        // Вычисляем НОК вручную
        std::vector<size_t> orders;
        for (const auto& element : group.get_set()) {
            auto order = ElementOrder<T, Op>::compute(group, element);
            if (order.has_value()) {
                orders.push_back(*order);
            }
        }

        if (orders.empty()) {
            return false;
        }

        // Вычисляем НОК вручную (совместимо с C++17)
        size_t lcm_orders = orders[0];
        for (size_t i = 1; i < orders.size(); ++i) {
            // НОК(a, b) = (a * b) / НОД(a, b)
            size_t a = lcm_orders;
            size_t b = orders[i];
            size_t gcd_val = GroupExponent<T, Op>::gcd(a, b);
            lcm_orders = (a / gcd_val) * b;
        }

        return lcm_orders == *computed_exponent;
    }

    /**
     * @brief Проверить, делят ли все порядки элементов показатель
     */
    static bool orders_divide_exponent(const group_type& group) {
        auto exp = GroupExponent<T, Op>::compute(group);
        if (!exp.has_value()) {
            return false;
        }

        for (const auto& element : group.get_set()) {
            auto order = ElementOrder<T, Op>::compute(group, element);
            if (order.has_value()) {
                if (*exp % *order != 0) {
                    return false;
                }
            }
        }

        return true;
    }
};

} // namespace cryptomath

