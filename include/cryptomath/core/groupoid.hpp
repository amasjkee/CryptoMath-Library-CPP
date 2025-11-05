#pragma once

#include "set.hpp"
#include "concepts.hpp"
#include <functional>
#include <stdexcept>
#include <concepts>

namespace cryptomath {

/**
 * @brief Группоид: множество с замкнутой бинарной операцией
 * 
 * Группоид (G, ∘) состоит из:
 * - Множества G
 * - Бинарной операции ∘: G × G → G (свойство замкнутости)
 * 
 * Это самая базовая алгебраическая структура с одной операцией.
 */
template<typename T, typename Op>
    requires GroupoidConcept<T, Op>
class Groupoid {
public:
    using element_type = T;
    using operation_type = Op;
    using set_type = Set<T>;

    /**
     * @brief Построить группоид из множества и операции
     */
    Groupoid(const set_type& elements, Op op)
        : elements_(elements), operation_(op) {
        // Проверяем свойство замкнутости для всех пар
        for (const auto& a : elements_) {
            for (const auto& b : elements_) {
                T result = operation_(a, b);
                if (!elements_.contains(result)) {
                    throw std::invalid_argument(
                        "Operation is not closed: result not in set"
                    );
                }
            }
        }
    }

    /**
     * @brief Apply the binary operation
     */
    T operate(const T& a, const T& b) const {
        if (!elements_.contains(a) || !elements_.contains(b)) {
            throw std::domain_error("Elements not in groupoid");
        }
        T result = operation_(a, b);
        if (!elements_.contains(result)) {
            throw std::runtime_error("Closure violation detected");
        }
        return result;
    }

    /**
     * @brief Применить бинарную операцию (в форме оператора)
     */
    T operator()(const T& a, const T& b) const {
        return operate(a, b);
    }

    /**
     * @brief Получить базовое множество
     */
    const set_type& get_set() const noexcept {
        return elements_;
    }

    /**
     * @brief Получить операцию
     */
    const Op& get_operation() const noexcept {
        return operation_;
    }

    /**
     * @brief Проверить, является ли операция ассоциативной
     * 
     * Проверяет, что (a ∘ b) ∘ c = a ∘ (b ∘ c) для всех a, b, c в множестве
     */
    bool is_associative() const {
        for (const auto& a : elements_) {
            for (const auto& b : elements_) {
                for (const auto& c : elements_) {
                    T left = operate(operate(a, b), c);
                    T right = operate(a, operate(b, c));
                    if (left != right) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    /**
     * @brief Проверить, является ли операция коммутативной
     * 
     * Проверяет, что a ∘ b = b ∘ a для всех a, b в множестве
     */
    bool is_commutative() const {
        for (const auto& a : elements_) {
            for (const auto& b : elements_) {
                if (operate(a, b) != operate(b, a)) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * @brief Проверить, является ли операция идемпотентной
     * 
     * Операция является идемпотентной, если a ∘ a = a для всех a
     */
    bool is_idempotent() const {
        for (const auto& a : elements_) {
            if (operate(a, a) != a) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Проверить, имеет ли операция свойство левой сократимости
     * 
     * Левая сократимость: если a ∘ b = a ∘ c, то b = c
     */
    bool has_left_cancellation() const {
        for (const auto& a : elements_) {
            for (const auto& b : elements_) {
                for (const auto& c : elements_) {
                    if (operate(a, b) == operate(a, c) && b != c) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    /**
     * @brief Проверить, имеет ли операция свойство правой сократимости
     * 
     * Правая сократимость: если b ∘ a = c ∘ a, то b = c
     */
    bool has_right_cancellation() const {
        for (const auto& a : elements_) {
            for (const auto& b : elements_) {
                for (const auto& c : elements_) {
                    if (operate(b, a) == operate(c, a) && b != c) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    /**
     * @brief Проверить, имеет ли операция оба свойства сократимости
     */
    bool has_cancellation() const {
        return has_left_cancellation() && has_right_cancellation();
    }

    /**
     * @brief Проверить равенство
     */
    bool operator==(const Groupoid& other) const {
        return elements_ == other.elements_;
        // Примечание: мы не можем сравнивать функциональные объекты напрямую, поэтому сравниваем только множества
    }

    /**
     * @brief Проверить неравенство
     */
    bool operator!=(const Groupoid& other) const {
        return !(*this == other);
    }

protected:
    set_type elements_;
    Op operation_;
};

} // namespace cryptomath

