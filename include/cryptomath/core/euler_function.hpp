#pragma once

#include "cyclic_group.hpp"
#include <vector>
#include <numeric>
#include <algorithm>

namespace cryptomath {

namespace detail {
    // Вспомогательная функция НОД (совместимо с C++17)
    inline size_t gcd(size_t a, size_t b) {
        while (b != 0) {
            size_t temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }
}

/**
 * @brief Функция Эйлера φ(n)
 * 
 * Функция Эйлера φ(n) подсчитывает количество положительных целых чисел до n,
 * которые взаимно просты с n.
 * 
 * Свойства:
 * - φ(1) = 1
 * - φ(p) = p - 1 для простого p
 * - φ(p^k) = p^k - p^(k-1) для простого p и k ≥ 1
 * - φ(mn) = φ(m)φ(n) если gcd(m, n) = 1 (мультипликативное свойство)
 * 
 * В теории групп:
 * - Для циклической группы порядка n существует ровно φ(n) порождающих элементов
 * - φ(n) равно количеству элементов порядка n в циклической группе
 */
class EulerFunction {
public:
    /**
     * @brief Вычислить функцию Эйлера φ(n)
     * 
     * Использует формулу: φ(n) = n × ∏(1 - 1/p) для всех различных простых p, делящих n
     */
    static size_t compute(size_t n) {
        if (n == 0) {
            return 0;
        }
        if (n == 1) {
            return 1;
        }

        size_t result = n;
        
        // Факторизуем n и применяем формулу
        size_t temp = n;
        for (size_t p = 2; p * p <= temp; ++p) {
            if (temp % p == 0) {
                // p - простой делитель
                while (temp % p == 0) {
                    temp /= p;
                }
                // Умножаем на (1 - 1/p) = (p - 1)/p
                result = result / p * (p - 1);
            }
        }

        // Если temp > 1, это простой делитель
        if (temp > 1) {
            result = result / temp * (temp - 1);
        }

        return result;
    }

    /**
     * @brief Вычислить используя разложение на простые множители
     * 
     * Если n = p1^k1 × p2^k2 × ... × pr^kr, то:
     * φ(n) = n × ∏(1 - 1/pi)
     */
    static size_t compute_from_prime_factors(const std::vector<std::pair<size_t, size_t>>& factors) {
        size_t n = 1;
        for (const auto& [p, k] : factors) {
            for (size_t i = 0; i < k; ++i) {
                n *= p;
            }
        }

        size_t result = n;
        for (const auto& [p, k] : factors) {
            result = result / p * (p - 1);
        }

        return result;
    }

    /**
     * @brief Вычислить для степени простого числа: φ(p^k) = p^k - p^(k-1)
     */
    static size_t compute_prime_power(size_t p, size_t k) {
        if (k == 0) {
            return 1;
        }
        if (k == 1) {
            return p - 1;
        }

        size_t p_to_k = 1;
        for (size_t i = 0; i < k; ++i) {
            p_to_k *= p;
        }

        size_t p_to_k_minus_1 = p_to_k / p;

        return p_to_k - p_to_k_minus_1;
    }

    /**
     * @brief Проверить мультипликативное свойство: φ(mn) = φ(m)φ(n) если gcd(m, n) = 1
     */
    static bool verify_multiplicative_property(size_t m, size_t n) {
        if (detail::gcd(m, n) != 1) {
            return false; // Свойство выполняется только когда числа взаимно просты
        }

        size_t phi_m = compute(m);
        size_t phi_n = compute(n);
        size_t phi_mn = compute(m * n);

        return phi_mn == phi_m * phi_n;
    }

    /**
     * @brief Подсчитать числа, взаимно простые с n (наивный метод для проверки)
     */
    static size_t count_coprime(size_t n) {
        if (n == 0) {
            return 0;
        }
        if (n == 1) {
            return 1;
        }

        size_t count = 0;
        for (size_t i = 1; i < n; ++i) {
            if (std::gcd(i, n) == 1) {
                count++;
            }
        }
        return count;
    }

    /**
     * @brief Проверить, что φ(n) равно количеству взаимно простых чисел
     */
    static bool verify_count(size_t n) {
        return compute(n) == count_coprime(n);
    }

    /**
     * @brief Получить все числа, взаимно простые с n
     */
    static std::vector<size_t> get_coprime_numbers(size_t n) {
        std::vector<size_t> result;
        for (size_t i = 1; i < n; ++i) {
            if (detail::gcd(i, n) == 1) {
                result.push_back(i);
            }
        }
        return result;
    }

    /**
     * @brief Сумма функции Эйлера: ∑_{d|n} φ(d) = n
     * 
     * Это фундаментальное свойство
     */
    static bool verify_sum_over_divisors(size_t n) {
        size_t sum = 0;
        for (size_t d = 1; d <= n; ++d) {
            if (n % d == 0) {
                sum += compute(d);
            }
        }
        return sum == n;
    }

    /**
     * @brief Применение к циклическим группам: количество порождающих элементов
     * 
     * Для циклической группы порядка n существует ровно φ(n) порождающих элементов
     */
    template<typename T, typename Op>
        requires cryptomath::GroupConcept<T, Op>
    static size_t number_of_generators(const cryptomath::Group<T, Op>& group) {
        if (!CyclicGroup<T, Op>::is_cyclic(group)) {
            return 0;
        }

        size_t group_order = group.get_set().size();
        return compute(group_order);
    }

    /**
     * @brief Применение: количество элементов порядка n в циклической группе
     * 
     * В циклической группе порядка m существует ровно φ(n) элементов порядка n,
     * если n делит m, и 0 в противном случае
     */
    template<typename T, typename Op>
        requires cryptomath::GroupConcept<T, Op>
    static size_t elements_of_order_in_cyclic_group(const cryptomath::Group<T, Op>& group,
                                                     size_t order) {
        if (!CyclicGroup<T, Op>::is_cyclic(group)) {
            return 0;
        }

        size_t group_order = group.get_set().size();
        if (group_order % order != 0) {
            return 0; // Порядок должен делить порядок группы
        }

        return compute(order);
    }
};

} // namespace cryptomath

