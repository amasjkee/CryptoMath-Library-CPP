#pragma once

#include "group.hpp"
#include "subgroup.hpp"
#include "set.hpp"
#include <concepts>

namespace cryptomath {

/**
 * @brief Центр группы
 * 
 * Центр группы G, обозначаемый Z(G), - это множество всех элементов,
 * которые коммутируют с каждым элементом группы:
 *   Z(G) = {z ∈ G | z ∘ g = g ∘ z для всех g ∈ G}
 * 
 * Свойства:
 * - Центр всегда является подгруппой
 * - Центр всегда является нормальной подгруппой
 * - Центр абелев
 * - Группа абелева тогда и только тогда, когда Z(G) = G
 */
template<typename T, typename Op>
    requires GroupConcept<T, Op>
class Center {
public:
    using group_type = Group<T, Op>;
    using element_type = T;
    using set_type = Set<T>;

    /**
     * @brief Вычислить центр группы
     */
    static set_type compute(const group_type& group) {
        set_type center;

        for (const auto& candidate : group.get_set()) {
            bool commutes_with_all = true;

            for (const auto& g : group.get_set()) {
                T left = group.operate(candidate, g);
                T right = group.operate(g, candidate);
                if (left != right) {
                    commutes_with_all = false;
                    break;
                }
            }

            if (commutes_with_all) {
                center.insert(candidate);
            }
        }

        return center;
    }

    /**
     * @brief Получить центр как подгруппу
     */
    static Subgroup<T, Op> as_subgroup(const group_type& group) {
        set_type center_set = compute(group);
        return Subgroup<T, Op>(group, center_set);
    }

    /**
     * @brief Проверить, находится ли элемент в центре
     */
    static bool is_in_center(const group_type& group, const T& element) {
        if (!group.get_set().contains(element)) {
            return false;
        }

        for (const auto& g : group.get_set()) {
            T left = group.operate(element, g);
            T right = group.operate(g, element);
            if (left != right) {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Проверить, является ли группа абелевой
     * 
     * Группа абелева тогда и только тогда, когда Z(G) = G
     */
    static bool is_abelian(const group_type& group) {
        set_type center = compute(group);
        return center == group.get_set();
    }

    /**
     * @brief Получить размер центра
     */
    static size_t size(const group_type& group) {
        return compute(group).size();
    }

    /**
     * @brief Проверить, является ли группа бесцентровой (Z(G) = {e})
     */
    static bool is_centerless(const group_type& group) {
        set_type center = compute(group);
        return center.size() == 1 && center.contains(group.identity());
    }
};

/**
 * @brief Централизатор элемента
 * 
 * Централизатор элемента a в группе G:
 *   C_G(a) = {g ∈ G | g ∘ a = a ∘ g}
 * 
 * Это множество всех элементов, которые коммутируют с a.
 */
template<typename T, typename Op>
    requires GroupConcept<T, Op>
class Centralizer {
public:
    using group_type = Group<T, Op>;
    using element_type = T;
    using set_type = Set<T>;

    /**
     * @brief Вычислить централизатор элемента
     */
    static set_type compute(const group_type& group, const T& element) {
        if (!group.get_set().contains(element)) {
            throw std::domain_error("Element not in group");
        }

        set_type centralizer;

        for (const auto& g : group.get_set()) {
            T left = group.operate(g, element);
            T right = group.operate(element, g);
            if (left == right) {
                centralizer.insert(g);
            }
        }

        return centralizer;
    }

    /**
     * @brief Получить централизатор как подгруппу
     */
    static Subgroup<T, Op> as_subgroup(const group_type& group, const T& element) {
        set_type centralizer_set = compute(group, element);
        return Subgroup<T, Op>(group, centralizer_set);
    }

    /**
     * @brief Проверить, коммутирует ли элемент с другим
     */
    static bool commutes(const group_type& group, const T& a, const T& b) {
        if (!group.get_set().contains(a) || !group.get_set().contains(b)) {
            return false;
        }

        T left = group.operate(a, b);
        T right = group.operate(b, a);
        return left == right;
    }
};

} // namespace cryptomath

