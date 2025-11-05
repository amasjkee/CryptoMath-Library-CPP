#pragma once

#include "group.hpp"
#include "set.hpp"
#include <concepts>
#include <stdexcept>

namespace cryptomath {

/**
 * @brief Подгруппа группы
 * 
 * Подгруппа H группы G - это подмножество H ⊆ G, которое образует группу
 * относительно той же операции, что и G.
 * 
 * Критерий подгруппы: H является подгруппой тогда и только тогда, когда:
 * 1. H непусто
 * 2. Для всех a, b ∈ H выполняется a ∘ b⁻¹ ∈ H
 * 
 * Альтернативный критерий (для конечных групп):
 * 1. H непусто
 * 2. H замкнуто относительно операции
 */
template<typename T, typename Op>
    requires GroupConcept<T, Op>
class Subgroup {
public:
    using element_type = T;
    using operation_type = Op;
    using group_type = Group<T, Op>;
    using set_type = Set<T>;

    /**
     * @brief Построить подгруппу из группы и подмножества
     * 
     * Проверяет, что подмножество образует подгруппу, используя критерий подгруппы.
     * 
     * @throws std::invalid_argument если подмножество не является подгруппой
     */
    Subgroup(const group_type& parent_group, const set_type& subset)
        : parent_group_(parent_group), subset_(subset) {
        
        if (!verify_subgroup_criterion()) {
            throw std::invalid_argument("Subset does not satisfy subgroup criterion");
        }
    }

    /**
     * @brief Проверить критерий подгруппы
     * 
     * H является подгруппой, если:
     * 1. H непусто
     * 2. Для всех a, b ∈ H выполняется a ∘ b⁻¹ ∈ H
     */
    bool verify_subgroup_criterion() const {
        // Проверяем непустоту
        if (subset_.empty()) {
            return false;
        }

        // Verify all elements are in parent group
        for (const auto& a : subset_) {
            if (!parent_group_.get_set().contains(a)) {
                return false;
            }
        }

        // Проверяем критерий: для всех a, b ∈ H выполняется a ∘ b⁻¹ ∈ H
        for (const auto& a : subset_) {
            for (const auto& b : subset_) {
                T b_inverse = parent_group_.inverse(b);
                T product = parent_group_.operate(a, b_inverse);
                if (!subset_.contains(product)) {
                    return false;
                }
            }
        }

        return true;
    }

    /**
     * @brief Альтернативная проверка для конечных групп
     * 
     * Для конечных групп достаточно проверить:
     * 1. H непусто
     * 2. H замкнуто относительно операции
     */
    bool verify_finite_subgroup_criterion() const {
        if (subset_.empty()) {
            return false;
        }

        // Проверяем, что все элементы в родительской группе
        for (const auto& a : subset_) {
            if (!parent_group_.get_set().contains(a)) {
                return false;
            }
        }

        // Проверяем замкнутость
        for (const auto& a : subset_) {
            for (const auto& b : subset_) {
                T product = parent_group_.operate(a, b);
                if (!subset_.contains(product)) {
                    return false;
                }
            }
        }

        return true;
    }

    /**
     * @brief Получить родительскую группу
     */
    const group_type& parent_group() const noexcept {
        return parent_group_;
    }

    /**
     * @brief Получить подмножество
     */
    const set_type& get_subset() const noexcept {
        return subset_;
    }

    /**
     * @brief Получить единичный элемент (тот же, что в родительской группе)
     */
    T identity() const {
        return parent_group_.identity();
    }

    /**
     * @brief Проверить, находится ли элемент в подгруппе
     */
    bool contains(const T& element) const {
        return subset_.contains(element);
    }

    /**
     * @brief Получить размер подгруппы
     */
    size_t size() const noexcept {
        return subset_.size();
    }

    /**
     * @brief Пересечение двух подгрупп
     * 
     * Пересечение двух подгрупп также является подгруппой.
     */
    static Subgroup intersection(const Subgroup& H1, const Subgroup& H2) {
        if (&H1.parent_group_ != &H2.parent_group_) {
            throw std::domain_error("Subgroups must be from the same parent group");
        }

        set_type intersection_set = H1.subset_.intersection(H2.subset_);
        return Subgroup(H1.parent_group_, intersection_set);
    }

    /**
     * @brief Произведение двух подгрупп: H1 ∘ H2 = {h1 ∘ h2 | h1 ∈ H1, h2 ∈ H2}
     * 
     * Примечание: Произведение двух подгрупп не обязательно является подгруппой.
     * Оно является подгруппой тогда и только тогда, когда H1 ∘ H2 = H2 ∘ H1.
     */
    static set_type product(const Subgroup& H1, const Subgroup& H2) {
        if (&H1.parent_group_ != &H2.parent_group_) {
            throw std::domain_error("Subgroups must be from the same parent group");
        }

        set_type product_set;
        for (const auto& h1 : H1.subset_) {
            for (const auto& h2 : H2.subset_) {
                T product = H1.parent_group_.operate(h1, h2);
                product_set.insert(product);
            }
        }

        return product_set;
    }

    /**
     * @brief Check if product of two subgroups is a subgroup
     */
    static bool is_product_subgroup(const Subgroup& H1, const Subgroup& H2) {
        set_type H1H2 = product(H1, H2);
        set_type H2H1 = product(H2, H1);
        
        // Произведение является подгруппой тогда и только тогда, когда H1H2 = H2H1
        if (H1H2 != H2H1) {
            return false;
        }

        // Проверяем критерий подгруппы
        Subgroup temp(H1.parent_group_, H1H2);
        return temp.verify_subgroup_criterion();
    }

    /**
     * @brief Проверить равенство
     */
    bool operator==(const Subgroup& other) const {
        return &parent_group_ == &other.parent_group_ && subset_ == other.subset_;
    }

    /**
     * @brief Проверить неравенство
     */
    bool operator!=(const Subgroup& other) const {
        return !(*this == other);
    }

private:
    const group_type& parent_group_;
    set_type subset_;
};

/**
 * @brief Тривиальная подгруппа: {e} (только единичный элемент)
 */
template<typename T, typename Op>
Subgroup<T, Op> trivial_subgroup(const Group<T, Op>& group) {
    Set<T> trivial_set{group.identity()};
    return Subgroup<T, Op>(group, trivial_set);
}

/**
 * @brief Несобственная подгруппа: сама группа
 */
template<typename T, typename Op>
Subgroup<T, Op> improper_subgroup(const Group<T, Op>& group) {
    return Subgroup<T, Op>(group, group.get_set());
}

} // namespace cryptomath

