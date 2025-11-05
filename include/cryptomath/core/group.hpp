#pragma once

#include "monoid.hpp"
#include "concepts.hpp"
#include <concepts>
#include <map>
#include <stdexcept>

namespace cryptomath {

/**
 * @brief Группа: моноид, где каждый элемент имеет обратный
 * 
 * Группа (G, ∘, e) состоит из:
 * - Множества G
 * - Ассоциативной бинарной операции ∘: G × G → G
 * - Единичного элемента e ∈ G
 * - Для каждого a ∈ G существует a⁻¹ ∈ G такое, что a ∘ a⁻¹ = a⁻¹ ∘ a = e
 * 
 * Свойства:
 * - Единичный элемент единственен
 * - Обратный элемент каждого элемента единственен
 * - (a⁻¹)⁻¹ = a
 * - (a ∘ b)⁻¹ = b⁻¹ ∘ a⁻¹
 */
template<typename T, typename Op>
    requires GroupConcept<T, Op>
class Group : public Monoid<T, Op> {
public:
    using base_type = Monoid<T, Op>;
    using element_type = T;
    using operation_type = Op;
    using set_type = Set<T>;

    /**
     * @brief Построить группу из множества, ассоциативной операции, единицы и функции обратного элемента
     * 
     * @throws std::invalid_argument если свойства группы не выполнены
     */
    Group(const set_type& elements, Op op, const T& identity,
          std::function<T(const T&)> inverse_func)
        : base_type(elements, op, identity), inverse_func_(inverse_func) {
        // Проверяем, что каждый элемент имеет обратный
        for (const auto& a : this->elements_) {
            T inv_a = inverse_func_(a);
            
            // Проверяем, что обратный элемент в множестве
            if (!this->elements_.contains(inv_a)) {
                throw std::invalid_argument(
                    "Inverse element not in the set"
                );
            }

            // Проверяем свойство обратного элемента
            if (this->operate(a, inv_a) != this->identity()) {
                throw std::invalid_argument(
                    "Inverse does not satisfy a ∘ a⁻¹ = e"
                );
            }
            if (this->operate(inv_a, a) != this->identity()) {
                throw std::invalid_argument(
                    "Inverse does not satisfy a⁻¹ ∘ a = e"
                );
            }
        }

        // Строим карту обратных элементов для эффективного поиска
        for (const auto& a : this->elements_) {
            inverse_map_[a] = inverse_func_(a);
        }
    }

    /**
     * @brief Построить группу из моноида (проверяет, что все элементы обратимы)
     * 
     * @throws std::logic_error если моноид не является группой
     */
    explicit Group(const Monoid<T, Op>& monoid)
        : base_type(monoid) {
        // Проверяем, что все элементы обратимы
        set_type invertible = this->invertible_elements();
        if (invertible != this->get_set()) {
            throw std::logic_error(
                "Not all elements are invertible; monoid is not a group"
            );
        }

        // Строим карту обратных элементов
        for (const auto& a : this->elements_) {
            inverse_map_[a] = this->Monoid<T, Op>::inverse(a);
        }
    }

    /**
     * @brief Получить обратный элемент
     * 
     * Обратный элемент единственен (доказано математически).
     */
    T inverse(const T& a) const {
        if (!this->elements_.contains(a)) {
            throw std::domain_error("Element not in group");
        }
        auto it = inverse_map_.find(a);
        if (it != inverse_map_.end()) {
            return it->second;
        }
        // Запасной вариант (не должно произойти, если inverse_map_ построен правильно)
        return inverse_func_(a);
    }

    /**
     * @brief Операция деления: a / b = a ∘ b⁻¹
     */
    T divide(const T& a, const T& b) const {
        return this->operate(a, inverse(b));
    }

    /**
     * @brief Левое деление: b \ a = b⁻¹ ∘ a
     */
    T left_divide(const T& a, const T& b) const {
        return this->operate(inverse(b), a);
    }

    /**
     * @brief Вычислить степень элемента: a^n
     * 
     * Для группы определены отрицательные степени: a^(-n) = (a⁻¹)^n
     */
    T power(const T& a, long long n) const {
        if (n == 0) {
            return this->identity();
        }

        if (n < 0) {
            return power(inverse(a), -n);
        }

        // Используем бинарное возведение в степень для положительных степеней
        T result = this->identity();
        T current_power = a;
        size_t exp = static_cast<size_t>(n);

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
     * @brief Проверить, является ли группа абелевой (коммутативной)
     */
    bool is_abelian() const {
        return this->is_commutative();
    }

    /**
     * @brief Альтернативное определение: Группа как множество с операцией, удовлетворяющей:
     * 1. Замкнутость
     * 2. Ассоциативность
     * 3. Левая единица: существует e такое, что e ∘ a = a для всех a
     * 4. Левые обратные: для каждого a существует a⁻¹ такое, что a⁻¹ ∘ a = e
     * 
     * Это эквивалентно стандартному определению.
     */
    static bool satisfies_alternative_definition(const set_type& elements, Op op) {
        // Находим левую единицу
        T left_identity{};
        bool found_left_identity = false;

        for (const auto& candidate : elements) {
            bool is_left_identity = true;
            for (const auto& a : elements) {
                if (op(candidate, a) != a) {
                    is_left_identity = false;
                    break;
                }
            }
            if (is_left_identity) {
                left_identity = candidate;
                found_left_identity = true;
                break;
            }
        }

        if (!found_left_identity) {
            return false;
        }

        // Проверяем левые обратные
        for (const auto& a : elements) {
            bool has_left_inverse = false;
            for (const auto& candidate_inv : elements) {
                if (op(candidate_inv, a) == left_identity) {
                    has_left_inverse = true;
                    break;
                }
            }
            if (!has_left_inverse) {
                return false;
            }
        }

        // Check associativity
        for (const auto& a : elements) {
            for (const auto& b : elements) {
                for (const auto& c : elements) {
                    if (op(op(a, b), c) != op(a, op(b, c))) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    /**
     * @brief Альтернативное определение: Группа как множество с операцией, удовлетворяющей:
     * 1. Замкнутость
     * 2. Ассоциативность
     * 3. Правая единица: существует e такое, что a ∘ e = a для всех a
     * 4. Правые обратные: для каждого a существует a⁻¹ такое, что a ∘ a⁻¹ = e
     */
    static bool satisfies_right_alternative_definition(const set_type& elements, Op op) {
        // Аналогично левой альтернативе, но с правой единицей и правыми обратными
        T right_identity{};
        bool found_right_identity = false;

        for (const auto& candidate : elements) {
            bool is_right_identity = true;
            for (const auto& a : elements) {
                if (op(a, candidate) != a) {
                    is_right_identity = false;
                    break;
                }
            }
            if (is_right_identity) {
                right_identity = candidate;
                found_right_identity = true;
                break;
            }
        }

        if (!found_right_identity) {
            return false;
        }

        // Проверяем правые обратные
        for (const auto& a : elements) {
            bool has_right_inverse = false;
            for (const auto& candidate_inv : elements) {
                if (op(a, candidate_inv) == right_identity) {
                    has_right_inverse = true;
                    break;
                }
            }
            if (!has_right_inverse) {
                return false;
            }
        }

        // Check associativity
        for (const auto& a : elements) {
            for (const auto& b : elements) {
                for (const auto& c : elements) {
                    if (op(op(a, b), c) != op(a, op(b, c))) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

private:
    std::function<T(const T&)> inverse_func_;
    std::map<T, T> inverse_map_; // Кэш для эффективного поиска обратных элементов
};

/**
 * @brief Свойства групп:
 * 
 * 1. Единственность единицы: доказано в Monoid
 * 2. Единственность обратного: если b и b' оба обратны к a, то b = b'
 * 3. (a⁻¹)⁻¹ = a: обратный к обратному есть исходный элемент
 * 4. (a ∘ b)⁻¹ = b⁻¹ ∘ a⁻¹: обратный к произведению есть произведение обратных в обратном порядке
 * 
 * Эти свойства обеспечиваются математической структурой и проверкой.
 */

} // namespace cryptomath

