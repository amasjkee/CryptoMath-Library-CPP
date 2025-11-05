#pragma once

#include "subgroup.hpp"
#include "group.hpp"
#include <concepts>
#include <map>

namespace cryptomath {

/**
 * @brief Смежный класс подгруппы
 * 
 * Для подгруппы H группы G и элемента g ∈ G:
 * - Левый смежный класс: g ∘ H = {g ∘ h | h ∈ H}
 * - Правый смежный класс: H ∘ g = {h ∘ g | h ∈ H}
 * 
 * Свойства:
 * - Два смежных класса либо равны, либо не пересекаются
 * - Все смежные классы имеют одинаковый размер |H|
 * - Теорема Лагранжа: |G| = |H| × [G : H], где [G : H] - индекс
 */
template<typename T, typename Op>
    requires GroupConcept<T, Op>
class Coset {
public:
    using element_type = T;
    using group_type = Group<T, Op>;
    using subgroup_type = Subgroup<T, Op>;
    using set_type = Set<T>;

    enum class CosetType {
        LEFT,   // Левый смежный класс: g ∘ H
        RIGHT   // Правый смежный класс: H ∘ g
    };

    /**
     * @brief Построить левый смежный класс: g ∘ H
     */
    Coset(const group_type& group, const subgroup_type& subgroup,
          const T& representative, CosetType type = CosetType::LEFT)
        : group_(group), subgroup_(subgroup), representative_(representative), type_(type) {
        build_coset();
    }

    /**
     * @brief Получить смежный класс как множество
     */
    const set_type& get_coset() const noexcept {
        return coset_;
    }

    /**
     * @brief Получить представителя (элемент-представитель)
     */
    const T& representative() const noexcept {
        return representative_;
    }

    /**
     * @brief Получить подгруппу
     */
    const subgroup_type& get_subgroup() const noexcept {
        return subgroup_;
    }

    /**
     * @brief Проверить, находится ли элемент в смежном классе
     */
    bool contains(const T& element) const {
        return coset_.contains(element);
    }

    /**
     * @brief Получить размер смежного класса (всегда равен |H|)
     */
    size_t size() const noexcept {
        return coset_.size();
    }

    /**
     * @brief Проверить равенство смежных классов
     * 
     * Два смежных класса равны тогда и только тогда, когда они имеют одинаковые элементы
     */
    bool operator==(const Coset& other) const {
        return &group_ == &other.group_ &&
               &subgroup_ == &other.subgroup_ &&
               coset_ == other.coset_;
    }

    /**
     * @brief Проверить неравенство
     */
    bool operator!=(const Coset& other) const {
        return !(*this == other);
    }

private:
    void build_coset() {
        const auto& H = subgroup_.get_subset();
        
        if (type_ == CosetType::LEFT) {
            // Левый смежный класс: g ∘ H
            for (const auto& h : H) {
                T element = group_.operate(representative_, h);
                coset_.insert(element);
            }
        } else {
            // Правый смежный класс: H ∘ g
            for (const auto& h : H) {
                T element = group_.operate(h, representative_);
                coset_.insert(element);
            }
        }
    }

    const group_type& group_;
    const subgroup_type& subgroup_;
    T representative_;
    CosetType type_;
    set_type coset_;
};

/**
 * @brief Теорема Лагранжа
 * 
 * Для конечной группы G и подгруппы H:
 *   |G| = |H| × [G : H]
 * 
 * где [G : H] - индекс H в G (количество смежных классов).
 * 
 * Это означает: |H| делит |G|
 */
template<typename T, typename Op>
class LagrangesTheorem {
public:
    using group_type = Group<T, Op>;
    using subgroup_type = Subgroup<T, Op>;

    /**
     * @brief Проверить теорему Лагранжа для подгруппы
     * 
     * @return true если |G| = |H| × [G : H]
     */
    static bool verify(const group_type& group, const subgroup_type& subgroup) {
        size_t group_order = group.get_set().size();
        size_t subgroup_order = subgroup.size();
        size_t index = compute_index(group, subgroup);

        return group_order == subgroup_order * index;
    }

    /**
     * @brief Вычислить индекс [G : H] (количество левых смежных классов)
     */
    static size_t compute_index(const group_type& group, const subgroup_type& subgroup) {
        return find_all_cosets(group, subgroup).size();
    }

    /**
     * @brief Найти все различные левые смежные классы
     */
    static Set<Set<T>> find_all_cosets(const group_type& group,
                                       const subgroup_type& subgroup) {
        Set<Set<T>> cosets;
        Set<T> processed;

        for (const auto& g : group.get_set()) {
            // Пропускаем, если уже в обработанном смежном классе
            bool in_processed = false;
            for (const auto& coset : cosets) {
                if (coset.contains(g)) {
                    in_processed = true;
                    break;
                }
            }
            if (in_processed) {
                continue;
            }

            // Строим смежный класс для этого представителя
            Coset<T, Op> coset(group, subgroup, g, Coset<T, Op>::CosetType::LEFT);
            cosets.insert(coset.get_coset());

            // Помечаем все элементы этого смежного класса как обработанные
            for (const auto& element : coset.get_coset()) {
                processed.insert(element);
            }
        }

        return cosets;
    }

    /**
     * @brief Проверить, делит ли порядок подгруппы порядок группы
     * 
     * Это необходимое условие по теореме Лагранжа
     */
    static bool order_divides_group_order(const group_type& group,
                                          const subgroup_type& subgroup) {
        size_t group_order = group.get_set().size();
        size_t subgroup_order = subgroup.size();
        return group_order % subgroup_order == 0;
    }

    /**
     * @brief Получить все возможные порядки подгрупп по теореме Лагранжа
     * 
     * Возвращает все делители |G|, которые могут быть порядками подгрупп
     */
    static Set<size_t> possible_subgroup_orders(const group_type& group) {
        size_t group_order = group.get_set().size();
        Set<size_t> divisors;

        for (size_t d = 1; d <= group_order; ++d) {
            if (group_order % d == 0) {
                divisors.insert(d);
            }
        }

        return divisors;
    }
};

/**
 * @brief Разбиение на смежные классы
 * 
 * Множество всех смежных классов подгруппы разбивает группу.
 */
template<typename T, typename Op>
class CosetPartition {
public:
    using group_type = Group<T, Op>;
    using subgroup_type = Subgroup<T, Op>;
    using coset_set = Set<Set<T>>;

    /**
     * @brief Получить разбиение группы на левые смежные классы
     */
    static coset_set left_coset_partition(const group_type& group,
                                          const subgroup_type& subgroup) {
        return LagrangesTheorem<T, Op>::find_all_cosets(group, subgroup);
    }

    /**
     * @brief Получить разбиение группы на правые смежные классы
     */
    static coset_set right_coset_partition(const group_type& group,
                                           const subgroup_type& subgroup) {
        coset_set cosets;
        Set<T> processed;

        for (const auto& g : group.get_set()) {
            // Пропускаем, если уже обработан
            bool in_processed = false;
            for (const auto& coset : cosets) {
                if (coset.contains(g)) {
                    in_processed = true;
                    break;
                }
            }
            if (in_processed) {
                continue;
            }

            // Строим правый смежный класс
            Coset<T, Op> coset(group, subgroup, g, Coset<T, Op>::CosetType::RIGHT);
            cosets.insert(coset.get_coset());
        }

        return cosets;
    }

    /**
     * @brief Проверить, что смежные классы образуют разбиение
     * 
     * Проверяет:
     * 1. Объединение всех смежных классов равно группе
     * 2. Смежные классы попарно не пересекаются
     */
    static bool verify_partition(const group_type& group,
                                 const coset_set& cosets) {
        // Проверяем, что объединение равно группе
        Set<T> union_set;
        for (const auto& coset : cosets) {
            union_set = union_set.union_with(coset);
        }
        if (union_set != group.get_set()) {
            return false;
        }

        // Проверяем попарную непересекаемость
        for (auto it1 = cosets.begin(); it1 != cosets.end(); ++it1) {
            auto it2 = it1;
            ++it2;
            for (; it2 != cosets.end(); ++it2) {
                Set<T> intersection = it1->intersection(*it2);
                if (!intersection.empty()) {
                    return false;
                }
            }
        }

        return true;
    }
};

} // namespace cryptomath

