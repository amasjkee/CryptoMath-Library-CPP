#pragma once

#include "set.hpp"
#include <set>
#include <vector>
#include <map>
#include <functional>

namespace cryptomath {

/**
 * @brief Бинарное отношение на множестве
 * 
 * Представляет бинарное отношение R ⊆ A × A на множестве A.
 */
template<typename T>
class Relation {
public:
    using element_type = T;
    using pair_type = std::pair<T, T>;
    using set_type = Set<T>;

    /**
     * @brief Построить отношение из множества и пар отношения
     */
    Relation(const set_type& set, const Set<pair_type>& pairs)
        : set_(set), pairs_(pairs) {
        // Проверяем, что все пары из множества × множество
        for (const auto& [a, b] : pairs_) {
            if (!set_.contains(a) || !set_.contains(b)) {
                throw std::invalid_argument("Relation contains pairs outside set");
            }
        }
    }

    /**
     * @brief Построить отношение из предикатной функции
     */
    Relation(const set_type& set, std::function<bool(const T&, const T&)> predicate)
        : set_(set) {
        for (const auto& a : set_) {
            for (const auto& b : set_) {
                if (predicate(a, b)) {
                    pairs_.insert(std::make_pair(a, b));
                }
            }
        }
    }

    /**
     * @brief Проверить, находится ли (a, b) в отношении
     */
    bool related(const T& a, const T& b) const {
        if (!set_.contains(a) || !set_.contains(b)) {
            return false;
        }
        return pairs_.contains(std::make_pair(a, b));
    }

    /**
     * @brief Получить базовое множество
     */
    const set_type& get_set() const noexcept {
        return set_;
    }

    /**
     * @brief Получить все пары в отношении
     */
    const Set<pair_type>& get_pairs() const noexcept {
        return pairs_;
    }

    /**
     * @brief Проверить, является ли отношение рефлексивным
     * 
     * Отношение R является рефлексивным, если (a, a) ∈ R для всех a ∈ A
     */
    bool is_reflexive() const {
        for (const auto& a : set_) {
            if (!related(a, a)) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Проверить, является ли отношение симметричным
     * 
     * Отношение R является симметричным, если (a, b) ∈ R влечет (b, a) ∈ R
     */
    bool is_symmetric() const {
        for (const auto& [a, b] : pairs_) {
            if (!related(b, a)) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Проверить, является ли отношение антисимметричным
     * 
     * Отношение R является антисимметричным, если (a, b) ∈ R и (b, a) ∈ R влечет a = b
     */
    bool is_antisymmetric() const {
        for (const auto& [a, b] : pairs_) {
            if (related(b, a) && a != b) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Проверить, является ли отношение транзитивным
     * 
     * Отношение R является транзитивным, если (a, b) ∈ R и (b, c) ∈ R влечет (a, c) ∈ R
     */
    bool is_transitive() const {
        for (const auto& [a, b] : pairs_) {
            for (const auto& c : set_) {
                if (related(b, c) && !related(a, c)) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * @brief Проверить, является ли отношение отношением эквивалентности
     * 
     * Отношение эквивалентности является рефлексивным, симметричным и транзитивным
     */
    bool is_equivalence_relation() const {
        return is_reflexive() && is_symmetric() && is_transitive();
    }

    /**
     * @brief Проверить, является ли отношение частичным порядком
     * 
     * Частичный порядок является рефлексивным, антисимметричным и транзитивным
     */
    bool is_partial_order() const {
        return is_reflexive() && is_antisymmetric() && is_transitive();
    }

    /**
     * @brief Получить классы эквивалентности для отношения эквивалентности
     * 
     * @throws std::logic_error если отношение не является отношением эквивалентности
     */
    Set<set_type> equivalence_classes() const {
        if (!is_equivalence_relation()) {
            throw std::logic_error("Relation must be an equivalence relation");
        }

        Set<set_type> classes;
        std::map<T, bool> processed;

        for (const auto& a : set_) {
            if (processed[a]) {
                continue;
            }

            set_type equivalence_class;
            for (const auto& b : set_) {
                if (related(a, b)) {
                    equivalence_class.insert(b);
                    processed[b] = true;
                }
            }
            classes.insert(equivalence_class);
        }

        return classes;
    }

    /**
     * @brief Получить класс эквивалентности конкретного элемента
     * 
     * @throws std::logic_error если отношение не является отношением эквивалентности
     */
    set_type equivalence_class(const T& a) const {
        if (!is_equivalence_relation()) {
            throw std::logic_error("Relation must be an equivalence relation");
        }

        set_type result;
        for (const auto& b : set_) {
            if (related(a, b)) {
                result.insert(b);
            }
        }
        return result;
    }

    /**
     * @brief Получить фактор-множество (множество классов эквивалентности)
     */
    Set<set_type> quotient_set() const {
        return equivalence_classes();
    }

    /**
     * @brief Транзитивное замыкание отношения
     */
    Relation transitive_closure() const {
        // Используем алгоритм Флойда-Уоршелла
        std::map<std::pair<T, T>, bool> closure;
        
        // Инициализируем текущими парами
        for (const auto& [a, b] : pairs_) {
            closure[std::make_pair(a, b)] = true;
        }
        
        // Добавляем рефлексивные пары
        for (const auto& a : set_) {
            closure[std::make_pair(a, a)] = true;
        }

        // Floyd-Warshall
        for (const auto& k : set_) {
            for (const auto& i : set_) {
                for (const auto& j : set_) {
                    auto ik = std::make_pair(i, k);
                    auto kj = std::make_pair(k, j);
                    auto ij = std::make_pair(i, j);
                    
                    if (closure.find(ik) != closure.end() && 
                        closure.find(kj) != closure.end()) {
                        closure[ij] = true;
                    }
                }
            }
        }

        Set<pair_type> closure_pairs;
        for (const auto& [pair, value] : closure) {
            if (value) {
                closure_pairs.insert(pair);
            }
        }

        return Relation(set_, closure_pairs);
    }

    /**
     * @brief Композиция отношений: R ∘ S = {(a, c) | ∃b: (a, b) ∈ S и (b, c) ∈ R}
     */
    Relation compose(const Relation& other) const {
        if (set_ != other.set_) {
            throw std::domain_error("Relations must be on the same set");
        }

        Set<pair_type> composed_pairs;
        for (const auto& [a, b] : other.pairs_) {
            for (const auto& c : set_) {
                if (related(b, c)) {
                    composed_pairs.insert(std::make_pair(a, c));
                }
            }
        }

        return Relation(set_, composed_pairs);
    }

    /**
     * @brief Проверить равенство
     */
    bool operator==(const Relation& other) const {
        return set_ == other.set_ && pairs_ == other.pairs_;
    }

    /**
     * @brief Проверить неравенство
     */
    bool operator!=(const Relation& other) const {
        return !(*this == other);
    }

private:
    set_type set_;
    Set<pair_type> pairs_;
};

} // namespace cryptomath

