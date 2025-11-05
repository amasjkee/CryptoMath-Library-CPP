#pragma once

#include "normal_subgroup.hpp"
#include "group.hpp"
#include "set.hpp"
#include "coset.hpp"
#include <concepts>
#include <map>
#include <functional>

namespace cryptomath {

/**
 * @brief Фактор-группа (группа классов вычетов)
 * 
 * Для группы G и нормальной подгруппы N фактор-группа G/N - это множество
 * всех смежных классов N в G с операцией:
 *   (a ∘ N) * (b ∘ N) = (a ∘ b) ∘ N
 * 
 * Эта операция корректно определена, потому что N нормальна.
 * 
 * Фактор-группа G/N имеет:
 * - Элементы: смежные классы N (которые являются множествами)
 * - Операцию: (g1 ∘ N) * (g2 ∘ N) = (g1 ∘ g2) ∘ N
 * - Единицу: N (смежный класс, содержащий единицу)
 */
template<typename T, typename Op>
    requires GroupConcept<T, Op>
class FactorGroup {
public:
    using group_type = Group<T, Op>;
    using normal_subgroup_type = NormalSubgroup<T, Op>;
    using element_type = Set<T>; // Смежные классы являются множествами
    using set_type = Set<Set<T>>;
    using operation_type = std::function<Set<T>(const Set<T>&, const Set<T>&)>;

    /**
     * @brief Построить фактор-группу из группы и нормальной подгруппы
     */
    FactorGroup(const group_type& group, const normal_subgroup_type& normal_subgroup)
        : parent_group_(group), normal_subgroup_(normal_subgroup) {
        
        // Строим множество смежных классов
        build_coset_set();
        
        // Строим операцию
        build_operation();
    }

    /**
     * @brief Получить множество смежных классов (элементов фактор-группы)
     */
    const set_type& get_cosets() const noexcept {
        return cosets_;
    }

    /**
     * @brief Получить родительскую группу
     */
    const group_type& parent_group() const noexcept {
        return parent_group_;
    }

    /**
     * @brief Получить нормальную подгруппу
     */
    const normal_subgroup_type& normal_subgroup() const noexcept {
        return normal_subgroup_;
    }

    /**
     * @brief Применить операцию фактор-группы: (aN) * (bN) = (ab)N
     */
    Set<T> operate(const Set<T>& coset_a, const Set<T>& coset_b) const {
        // Проверяем, что смежные классы валидны
        if (!cosets_.contains(coset_a) || !cosets_.contains(coset_b)) {
            throw std::domain_error("Invalid cosets");
        }

        // Выбираем представителей из каждого смежного класса
        T rep_a = *coset_a.begin();
        T rep_b = *coset_b.begin();

        // Вычисляем произведение в родительской группе
        T product = parent_group_.operate(rep_a, rep_b);

        // Находим смежный класс, содержащий произведение
        return find_coset_containing(product);
    }

    /**
     * @brief Получить единичный элемент (смежный класс, содержащий единицу)
     */
    Set<T> identity() const {
        return find_coset_containing(parent_group_.identity());
    }

    /**
     * @brief Получить обратный элемент смежного класса
     */
    Set<T> inverse(const Set<T>& coset) const {
        if (!cosets_.contains(coset)) {
            throw std::domain_error("Invalid coset");
        }

        // Выбираем представителя
        T rep = *coset.begin();
        T rep_inverse = parent_group_.inverse(rep);

        // Возвращаем смежный класс, содержащий обратный элемент
        return find_coset_containing(rep_inverse);
    }

    /**
     * @brief Получить размер фактор-группы (индекс нормальной подгруппы)
     */
    size_t size() const noexcept {
        return cosets_.size();
    }

    /**
     * @brief Проверить свойства фактор-группы
     */
    bool verify_factor_group() const {
        // Проверяем единицу
        Set<T> identity_coset = identity();
        for (const auto& coset : cosets_) {
            Set<T> left = operate(identity_coset, coset);
            Set<T> right = operate(coset, identity_coset);
            if (left != coset || right != coset) {
                return false;
            }
        }

        // Проверяем обратные элементы
        for (const auto& coset : cosets_) {
            Set<T> inv = inverse(coset);
            Set<T> left = operate(coset, inv);
            Set<T> right = operate(inv, coset);
            if (left != identity_coset || right != identity_coset) {
                return false;
            }
        }

        // Проверяем ассоциативность
        for (const auto& a : cosets_) {
            for (const auto& b : cosets_) {
                for (const auto& c : cosets_) {
                    Set<T> left = operate(operate(a, b), c);
                    Set<T> right = operate(a, operate(b, c));
                    if (left != right) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    /**
     * @brief Проверить равенство
     */
    bool operator==(const FactorGroup& other) const {
        return &parent_group_ == &other.parent_group_ &&
               &normal_subgroup_ == &other.normal_subgroup_;
    }

private:
    void build_coset_set() {
        // Получаем все левые смежные классы (так как N нормальна, левые = правые)
        cosets_ = LagrangesTheorem<T, Op>::find_all_cosets(
            parent_group_,
            normal_subgroup_
        );

        // Строим отображение из элемента в его смежный класс
        for (const auto& coset : cosets_) {
            for (const auto& element : coset) {
                element_to_coset_[element] = coset;
            }
        }
    }

    void build_operation() {
        // Операция строится в методе operate()
        // Это заглушка для будущей оптимизации
    }

    Set<T> find_coset_containing(const T& element) const {
        auto it = element_to_coset_.find(element);
        if (it != element_to_coset_.end()) {
            return it->second;
        }
        throw std::logic_error("Element not found in any coset");
    }

    const group_type& parent_group_;
    const normal_subgroup_type& normal_subgroup_;
    set_type cosets_;
    std::map<T, Set<T>> element_to_coset_;
};

/**
 * @brief Первая теорема об изоморфизме
 * 
 * Если φ: G → H - гомоморфизм групп, то:
 *   G / ker(φ) ≅ im(φ)
 * 
 * Это фундаментальная теорема, связывающая гомоморфизмы и фактор-группы.
 */
template<typename T, typename Op>
class FirstIsomorphismTheorem {
public:
    using group_type = Group<T, Op>;

    /**
     * @brief Проверить, изоморфна ли фактор-группа образу
     * 
     * Это концептуальная проверка - полная реализация потребовала бы
     * структуру гомоморфизма.
     */
    static bool verify(const FactorGroup<T, Op>& factor_group,
                       const Set<T>& image) {
        // Это потребовало бы реализации гомоморфизмов
        // Заглушка для будущей реализации
        return factor_group.size() == image.size();
    }
};

} // namespace cryptomath

