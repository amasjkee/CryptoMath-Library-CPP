#pragma once

#include "subgroup.hpp"
#include "group.hpp"
#include <concepts>

namespace cryptomath {

/**
 * @brief Нормальная подгруппа
 * 
 * Подгруппа N группы G является нормальной (обозначается N ⊲ G), если:
 *   g ∘ n ∘ g⁻¹ ∈ N для всех g ∈ G и n ∈ N
 * 
 * Эквивалентно:
 *   g ∘ N = N ∘ g для всех g ∈ G (левые и правые смежные классы совпадают)
 * 
 * Нормальные подгруппы важны, потому что они позволяют строить фактор-группы.
 */
template<typename T, typename Op>
    requires GroupConcept<T, Op>
class NormalSubgroup : public Subgroup<T, Op> {
public:
    using base_type = Subgroup<T, Op>;
    using group_type = Group<T, Op>;
    using set_type = Set<T>;

    /**
     * @brief Построить нормальную подгруппу из подгруппы
     * 
     * Проверяет, что подгруппа является нормальной.
     * 
     * @throws std::invalid_argument если подгруппа не является нормальной
     */
    explicit NormalSubgroup(const Subgroup<T, Op>& subgroup)
        : base_type(subgroup) {
        if (!verify_normal()) {
            throw std::invalid_argument("Subgroup is not normal");
        }
    }

    /**
     * @brief Построить нормальную подгруппу из группы и подмножества
     * 
     * Проверяет, что подмножество образует нормальную подгруппу.
     */
    NormalSubgroup(const group_type& parent_group, const set_type& subset)
        : base_type(parent_group, subset) {
        if (!verify_normal()) {
            throw std::invalid_argument("Subset does not form a normal subgroup");
        }
    }

    /**
     * @brief Проверить, что подгруппа является нормальной
     * 
     * Проверяет: g ∘ n ∘ g⁻¹ ∈ N для всех g ∈ G и n ∈ N
     */
    bool verify_normal() const {
        const auto& G = this->parent_group();
        const auto& N = this->get_subset();

        for (const auto& g : G.get_set()) {
            for (const auto& n : N) {
                T g_inverse = G.inverse(g);
                T conjugate = G.operate(G.operate(g, n), g_inverse);
                if (!N.contains(conjugate)) {
                    return false;
                }
            }
        }

        return true;
    }

    /**
     * @brief Альтернативная проверка с использованием смежных классов
     * 
     * N является нормальной тогда и только тогда, когда g ∘ N = N ∘ g для всех g ∈ G
     */
    bool verify_normal_via_cosets() const {
        const auto& G = this->parent_group();

        for (const auto& g : G.get_set()) {
            // Вычисляем левый смежный класс: g ∘ N
            set_type left_coset;
            for (const auto& n : this->get_subset()) {
                left_coset.insert(G.operate(g, n));
            }

            // Вычисляем правый смежный класс: N ∘ g
            set_type right_coset;
            for (const auto& n : this->get_subset()) {
                right_coset.insert(G.operate(n, g));
            }

            if (left_coset != right_coset) {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Проверить, является ли подгруппа нормальной (статический метод)
     */
    static bool is_normal(const Subgroup<T, Op>& subgroup) {
        const auto& G = subgroup.parent_group();
        const auto& N = subgroup.get_subset();

        for (const auto& g : G.get_set()) {
            for (const auto& n : N) {
                T g_inverse = G.inverse(g);
                T conjugate = G.operate(G.operate(g, n), g_inverse);
                if (!N.contains(conjugate)) {
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
        return this->base_type::parent_group();
    }
};

/**
 * @brief Тривиальная нормальная подгруппа: {e}
 */
template<typename T, typename Op>
NormalSubgroup<T, Op> trivial_normal_subgroup(const Group<T, Op>& group) {
    Subgroup<T, Op> trivial = trivial_subgroup(group);
    return NormalSubgroup<T, Op>(trivial);
}

/**
 * @brief Несобственная нормальная подгруппа: сама группа
 */
template<typename T, typename Op>
NormalSubgroup<T, Op> improper_normal_subgroup(const Group<T, Op>& group) {
    Subgroup<T, Op> improper = improper_subgroup(group);
    return NormalSubgroup<T, Op>(improper);
}

/**
 * @brief Проверить, является ли подгруппа нормальной в абелевой группе
 * 
 * В абелевой группе все подгруппы являются нормальными.
 */
template<typename T, typename Op>
bool is_normal_in_abelian_group(const Subgroup<T, Op>& subgroup) {
    return subgroup.parent_group().is_abelian();
    // Если группа абелева, все подгруппы автоматически нормальны
}

} // namespace cryptomath

