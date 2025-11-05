#pragma once

#include "group.hpp"
#include "element_order.hpp"
#include "group_exponent.hpp"
#include "subgroup.hpp"
#include "set.hpp"
#include <concepts>
#include <vector>
#include <algorithm>

namespace cryptomath {

/**
 * @brief Циклическая группа
 * 
 * Группа G циклическая, если существует элемент g ∈ G такой, что
 * G = ⟨g⟩ = {g^n | n ∈ Z}
 * 
 * Элемент g называется порождающим (генератором) группы G.
 * 
 * Свойства:
 * - Каждая циклическая группа абелева
 * - Каждая подгруппа циклической группы циклическая
 * - Если |G| = n, то G имеет ровно φ(n) порождающих (где φ - функция Эйлера)
 * - Для каждого делителя d числа n существует ровно одна подгруппа порядка d
 */
template<typename T, typename Op>
    requires GroupConcept<T, Op>
class CyclicGroup {
public:
    using group_type = Group<T, Op>;
    using element_type = T;
    using set_type = Set<T>;

    /**
     * @brief Проверить, является ли группа циклической
     */
    static bool is_cyclic(const group_type& group) {
        // Группа циклическая, если существует порождающий элемент
        return find_generator(group).has_value();
    }

    /**
     * @brief Найти порождающий элемент группы
     * 
     * @return Порождающий элемент, если группа циклическая, или std::nullopt иначе
     */
    static std::optional<T> find_generator(const group_type& group) {
        size_t group_order = group.get_set().size();

        // Пробуем каждый элемент как потенциальный порождающий
        for (const auto& candidate : group.get_set()) {
            if (is_generator(group, candidate)) {
                return candidate;
            }
        }

        return std::nullopt;
    }

    /**
     * @brief Проверить, является ли элемент порождающим
     * 
     * Элемент g является порождающим, если ord(g) = |G|
     */
    static bool is_generator(const group_type& group, const T& element) {
        if (!group.get_set().contains(element)) {
            return false;
        }

        auto order = ElementOrder<T, Op>::compute(group, element);
        if (!order.has_value()) {
            return false; // Бесконечные группы здесь не обрабатываются
        }

        size_t group_order = group.get_set().size();
        return *order == group_order;
    }

    /**
     * @brief Найти все порождающие элементы циклической группы
     * 
     * Для циклической группы порядка n существует φ(n) порождающих элементов
     */
    static set_type find_all_generators(const group_type& group) {
        if (!is_cyclic(group)) {
            return set_type{}; // Пустое множество, если не циклическая
        }

        set_type generators;
        size_t group_order = group.get_set().size();

        for (const auto& element : group.get_set()) {
            if (is_generator(group, element)) {
                generators.insert(element);
            }
        }

        return generators;
    }

    /**
     * @brief Сгенерировать циклическую подгруппу из элемента
     * 
     * Возвращает ⟨g⟩ = {g^n | n = 0, 1, 2, ..., ord(g) - 1}
     */
    static set_type generate_cyclic_subgroup(const group_type& group, const T& generator) {
        if (!group.get_set().contains(generator)) {
            throw std::domain_error("Element not in group");
        }

        set_type subgroup;
        subgroup.insert(group.identity());

        T current = generator;
        auto order = ElementOrder<T, Op>::compute(group, generator);
        
        if (!order.has_value()) {
            // Для бесконечного порядка генерируем конечное подмножество
            // На практике это ограничено размером группы
            size_t max_iterations = group.get_set().size();
            for (size_t i = 0; i < max_iterations; ++i) {
                if (current == group.identity() && i > 0) {
                    break; // Нашли цикл
                }
                subgroup.insert(current);
                current = group.operate(current, generator);
            }
        } else {
            // Генерируем ровно ord(g) элементов
            for (size_t i = 1; i < *order; ++i) {
                subgroup.insert(current);
                current = group.operate(current, generator);
            }
        }

        return subgroup;
    }

    /**
     * @brief Получить циклическую подгруппу как объект Subgroup
     */
    static Subgroup<T, Op> cyclic_subgroup(const group_type& group, const T& generator) {
        set_type subgroup_set = generate_cyclic_subgroup(group, generator);
        return Subgroup<T, Op>(group, subgroup_set);
    }

    /**
     * @brief Свойства циклических групп
     */
    class Properties {
    public:
        /**
         * @brief Свойство: Каждая циклическая группа абелева
         */
        static bool is_abelian(const group_type& group) {
            if (!is_cyclic(group)) {
                return false; // Свойство применимо только к циклическим группам
            }
            return group.is_abelian();
        }

        /**
         * @brief Свойство: Каждая подгруппа циклической группы циклическая
         */
        static bool all_subgroups_cyclic(const group_type& group) {
            if (!is_cyclic(group)) {
                return false; // Свойство применимо только к циклическим группам
            }

            // Для конечных циклических групп каждая подгруппа циклическая
            // Это фундаментальное свойство
            // Можем проверить, проверив, что все подгруппы порождаются элементами
            // На практике это верно по структурной теореме для циклических групп
            return true; // Это математический факт
        }

        /**
         * @brief Свойство: Для каждого делителя d числа |G| существует ровно одна подгруппа порядка d
         */
        static bool unique_subgroup_for_each_divisor(const group_type& group) {
            if (!is_cyclic(group)) {
                return false;
            }

            size_t group_order = group.get_set().size();

            // Для каждого делителя d проверяем, что существует ровно одна подгруппа порядка d
            for (size_t d = 1; d <= group_order; ++d) {
                if (group_order % d == 0) {
                    // Подсчитываем подгруппы порядка d
                    size_t count = 0;
                    for (const auto& element : group.get_set()) {
                        auto order = ElementOrder<T, Op>::compute(group, element);
                        if (order.has_value() && *order == d) {
                            // Проверяем, порождает ли это подгруппу порядка d
                            set_type subgroup = generate_cyclic_subgroup(group, element);
                            if (subgroup.size() == d) {
                                count++;
                            }
                        }
                    }

                    // Должна быть ровно одна подгруппа порядка d
                    // (На самом деле существует φ(d) порождающих, но все они порождают одну и ту же подгруппу)
                    // Поэтому нужно проверить различные подгруппы
                    Set<set_type> distinct_subgroups;
                    for (const auto& element : group.get_set()) {
                        auto order = ElementOrder<T, Op>::compute(group, element);
                        if (order.has_value() && *order == d) {
                            set_type subgroup = generate_cyclic_subgroup(group, element);
                            distinct_subgroups.insert(subgroup);
                        }
                    }

                    if (distinct_subgroups.size() != 1) {
                        return false;
                    }
                }
            }

            return true;
        }

        /**
         * @brief Свойство: exp(G) = |G| для циклических групп
         */
        static bool exponent_equals_order(const group_type& group) {
            if (!is_cyclic(group)) {
                return false;
            }

            auto exp = GroupExponent<T, Op>::compute(group);
            if (!exp.has_value()) {
                return false;
            }

            return *exp == group.get_set().size();
        }
    };
};

/**
 * @brief Структурная теорема для циклических групп
 * 
 * Каждая циклическая группа изоморфна либо:
 * - Z (бесконечная циклическая группа)
 * - Z/nZ (конечная циклическая группа порядка n)
 */
template<typename T, typename Op>
class CyclicGroupStructure {
public:
    using group_type = Group<T, Op>;

    /**
     * @brief Проверить, изоморфна ли группа Z/nZ
     * 
     * Это верно тогда и только тогда, когда группа циклическая порядка n
     */
    static bool is_isomorphic_to_Zn(const group_type& group, size_t n) {
        if (!CyclicGroup<T, Op>::is_cyclic(group)) {
            return false;
        }

        return group.get_set().size() == n;
    }

    /**
     * @brief Получить порядок циклической группы
     */
    static size_t get_order(const group_type& group) {
        if (!CyclicGroup<T, Op>::is_cyclic(group)) {
            throw std::logic_error("Group is not cyclic");
        }

        return group.get_set().size();
    }
};

} // namespace cryptomath

