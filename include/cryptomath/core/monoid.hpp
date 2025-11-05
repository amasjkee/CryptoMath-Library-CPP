#pragma once

#include "semigroup.hpp"
#include "concepts.hpp"
#include <concepts>
#include <optional>

namespace cryptomath {

/**
 * @brief Моноид: полугруппа с единичным элементом
 * 
 * Моноид (M, ∘, e) состоит из:
 * - Множества M
 * - Ассоциативной бинарной операции ∘: M × M → M
 * - Единичного элемента e ∈ M такого, что e ∘ a = a ∘ e = a для всех a ∈ M
 * 
 * Единичный элемент единственен (доказано математически).
 */
template<typename T, typename Op>
    requires MonoidConcept<T, Op>
class Monoid : public Semigroup<T, Op> {
public:
    using base_type = Semigroup<T, Op>;
    using element_type = T;
    using operation_type = Op;
    using set_type = Set<T>;

    /**
     * @brief Построить моноид из множества, ассоциативной операции и единичного элемента
     * 
     * @throws std::invalid_argument если единичный элемент невалиден или операция не ассоциативна
     */
    Monoid(const set_type& elements, Op op, const T& identity)
        : base_type(elements, op), identity_(identity) {
        // Проверяем единичный элемент
        if (!this->elements_.contains(identity_)) {
            throw std::invalid_argument("Identity element must be in the set");
        }

        // Проверяем свойство единицы
        for (const auto& a : this->elements_) {
            if (this->operate(identity_, a) != a) {
                throw std::invalid_argument(
                    "Element does not satisfy left identity property"
                );
            }
            if (this->operate(a, identity_) != a) {
                throw std::invalid_argument(
                    "Element does not satisfy right identity property"
                );
            }
        }

        // Единственность единицы: если e и e' обе являются единицами,
        // то e = e ∘ e' = e', поэтому e = e'
        // Это обеспечивается принятием только одного единичного элемента в конструкторе
    }

    /**
     * @brief Построить моноид из полугруппы (пытается найти единицу)
     * 
     * @throws std::logic_error если полугруппа не имеет единицы
     */
    explicit Monoid(const Semigroup<T, Op>& semigroup)
        : base_type(semigroup), identity_(semigroup.find_identity()) {
    }

    /**
     * @brief Получить единичный элемент
     * 
     * Единичный элемент единственен (доказано математически).
     */
    const T& identity() const noexcept {
        return identity_;
    }

    /**
     * @brief Вычислить степень элемента: a^n
     * 
     * Для моноида, a^0 = e (единичный элемент)
     */
    T power(const T& a, size_t n) const {
        if (n == 0) {
            return identity_;
        }

        if (n == 1) {
            return a;
        }

        // Используем бинарное возведение в степень
        T result = identity_;
        T current_power = a;
        size_t exp = n;

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
     * @brief Получить множество обратимых элементов
     * 
     * Элемент a обратим, если существует b такое, что a ∘ b = b ∘ a = e.
     * Множество всех обратимых элементов образует группу (группу единиц).
     */
    set_type invertible_elements() const {
        set_type invertible;
        for (const auto& a : this->elements_) {
            // Проверяем, имеет ли a обратный элемент
            bool has_inverse = false;
            for (const auto& b : this->elements_) {
                if (this->operate(a, b) == identity_ &&
                    this->operate(b, a) == identity_) {
                    has_inverse = true;
                    break;
                }
            }
            if (has_inverse) {
                invertible.insert(a);
            }
        }
        return invertible;
    }

    /**
     * @brief Проверить, является ли элемент обратимым
     */
    bool is_invertible(const T& a) const {
        if (!this->elements_.contains(a)) {
            return false;
        }
        for (const auto& b : this->elements_) {
            if (this->operate(a, b) == identity_ &&
                this->operate(b, a) == identity_) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Найти обратный элемент обратимой величины
     * 
     * @throws std::logic_error если элемент не обратим
     */
    T inverse(const T& a) const {
        if (!is_invertible(a)) {
            throw std::logic_error("Element is not invertible");
        }

        // Находим обратный элемент (он единственен, если существует)
        for (const auto& b : this->elements_) {
            if (this->operate(a, b) == identity_ &&
                this->operate(b, a) == identity_) {
                return b;
            }
        }

        // Не должны попасть сюда, если is_invertible вернул true
        throw std::logic_error("Failed to find inverse");
    }

    /**
     * @brief Проверить, является ли моноид коммутативным
     */
    bool is_commutative_monoid() const {
        return this->is_commutative();
    }

private:
    T identity_; // Единственный единичный элемент (доказано математически)
};

/**
 * @brief Единственность единичного элемента
 * 
 * Теорема: В моноиде единичный элемент единственен.
 * Доказательство: Если e и e' обе являются единицами, то:
 *   e = e ∘ e' = e'
 * Следовательно e = e'.
 * 
 * Это обеспечивается в нашей реализации:
 * 1. Принятием только одного единичного элемента в конструкторе
 * 2. Проверкой, что он удовлетворяет свойству единицы
 * 3. Математическое доказательство гарантирует единственность
 */

/**
 * @brief Единственность обратного элемента
 * 
 * Теорема: В моноиде, если элемент имеет обратный, обратный элемент единственен.
 * Доказательство: Если b и b' оба являются обратными к a, то:
 *   b = b ∘ e = b ∘ (a ∘ b') = (b ∘ a) ∘ b' = e ∘ b' = b'
 * Следовательно b = b'.
 * 
 * Поэтому наша функция inverse() возвращает единственное значение.
 */

} // namespace cryptomath

