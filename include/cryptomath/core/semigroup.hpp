#pragma once

#include "groupoid.hpp"
#include "concepts.hpp"
#include <concepts>

namespace cryptomath {

/**
 * @brief Полугруппа: группоид с ассоциативной операцией
 * 
 * Полугруппа (S, ∘) состоит из:
 * - Множества S
 * - Ассоциативной бинарной операции ∘: S × S → S
 * 
 * Свойство ассоциативности: (a ∘ b) ∘ c = a ∘ (b ∘ c) для всех a, b, c ∈ S
 */
template<typename T, typename Op>
    requires SemigroupConcept<T, Op>
class Semigroup : public Groupoid<T, Op> {
public:
    using base_type = Groupoid<T, Op>;
    using element_type = T;
    using operation_type = Op;
    using set_type = Set<T>;

    /**
     * @brief Построить полугруппу из множества и ассоциативной операции
     * 
     * @throws std::invalid_argument если операция не является ассоциативной
     */
    Semigroup(const set_type& elements, Op op)
        : base_type(elements, op) {
        // Проверяем ассоциативность
        if (!this->is_associative()) {
            throw std::invalid_argument(
                "Operation must be associative for semigroup"
            );
        }
    }

    /**
     * @brief Вычислить произведение нескольких элементов
     * 
     * Использует ассоциативность для вычисления a₁ ∘ a₂ ∘ ... ∘ aₙ
     */
    template<typename InputIt>
    T product(InputIt first, InputIt last) const {
        if (first == last) {
            throw std::invalid_argument("Empty product is not defined in semigroup");
        }

        T result = *first;
        ++first;
        while (first != last) {
            result = this->operate(result, *first);
            ++first;
        }
        return result;
    }

    /**
     * @brief Вычислить степень элемента: a^n = a ∘ a ∘ ... ∘ a (n раз)
     */
    T power(const T& a, size_t n) const {
        if (n == 0) {
            throw std::invalid_argument("Zero power not defined in semigroup");
        }

        if (n == 1) {
            return a;
        }

        // Используем бинарное возведение в степень для эффективности
        T result = a;
        size_t exp = n - 1;
        T current_power = a;

        while (exp > 0) {
            if (exp % 2 == 1) {
                result = this->operate(result, current_power);
            }
            current_power = this->operate(current_power, current_power);
            exp /= 2;
        }

        return result;
    }

    /**
     * @brief Проверить, является ли полугруппа коммутативной
     */
    bool is_commutative_semigroup() const {
        return this->is_commutative();
    }

    /**
     * @brief Проверить, имеет ли полугруппа единичный элемент
     * 
     * Если true, эта полугруппа является моноидом
     */
    bool has_identity() const {
        for (const auto& candidate : this->elements_) {
            bool is_identity = true;
            for (const auto& a : this->elements_) {
                if (this->operate(candidate, a) != a ||
                    this->operate(a, candidate) != a) {
                    is_identity = false;
                    break;
                }
            }
            if (is_identity) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Найти единичный элемент, если он существует
     * 
     * @return Единичный элемент, или выбрасывает исключение, если его нет
     * @throws std::logic_error если единичный элемент не существует
     */
    T find_identity() const {
        for (const auto& candidate : this->elements_) {
            bool is_identity = true;
            for (const auto& a : this->elements_) {
                if (this->operate(candidate, a) != a ||
                    this->operate(a, candidate) != a) {
                    is_identity = false;
                    break;
                }
            }
            if (is_identity) {
                return candidate;
            }
        }
        throw std::logic_error("Semigroup has no identity element");
    }
};

} // namespace cryptomath

