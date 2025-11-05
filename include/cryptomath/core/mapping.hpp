#pragma once

#include "set.hpp"
#include <map>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <type_traits>

namespace cryptomath {

/**
 * @brief Математическое отображение (функция) из области определения в область значений
 * 
 * Представляет отображение f: A → B, где A - область определения, а B - область значений.
 * Поддерживает проверку инъективности, сюръективности, биективности и композиции.
 */
template<typename Domain, typename Codomain>
class Mapping {
public:
    using domain_type = Domain;
    using codomain_type = Codomain;
    using domain_set = Set<Domain>;
    using codomain_set = Set<Codomain>;

    /**
     * @brief Построить отображение из области определения, области значений и функции
     */
    Mapping(const domain_set& domain, const codomain_set& codomain,
            std::function<Codomain(const Domain&)> func)
        : domain_(domain), codomain_(codomain), function_(func) {
        // Проверяем, что функция отображает в область значений
        for (const auto& x : domain_) {
            Codomain y = function_(x);
            if (!codomain_.contains(y)) {
                throw std::invalid_argument("Function maps outside codomain");
            }
            mapping_[x] = y;
        }
    }

    /**
     * @brief Построить отображение из явной карты
     */
    Mapping(const domain_set& domain, const codomain_set& codomain,
            const std::map<Domain, Codomain>& mapping_map)
        : domain_(domain), codomain_(codomain), mapping_(mapping_map) {
        // Проверяем отображение
        for (const auto& [x, y] : mapping_) {
            if (!domain_.contains(x)) {
                throw std::invalid_argument("Mapping contains element not in domain");
            }
            if (!codomain_.contains(y)) {
                throw std::invalid_argument("Mapping contains element not in codomain");
            }
        }
        // Убеждаемся, что все элементы области определения отображены
        for (const auto& x : domain_) {
            if (mapping_.find(x) == mapping_.end()) {
                throw std::invalid_argument("Not all domain elements are mapped");
            }
        }
    }

    /**
     * @brief Применить отображение к элементу
     */
    Codomain operator()(const Domain& x) const {
        auto it = mapping_.find(x);
        if (it == mapping_.end()) {
            throw std::domain_error("Element not in domain");
        }
        return it->second;
    }

    /**
     * @brief Получить область определения
     */
    const domain_set& domain() const noexcept {
        return domain_;
    }

    /**
     * @brief Получить область значений
     */
    const codomain_set& codomain() const noexcept {
        return codomain_;
    }

    /**
     * @brief Получить образ (диапазон) отображения
     */
    codomain_set image() const {
        codomain_set result;
        for (const auto& [x, y] : mapping_) {
            result.insert(y);
        }
        return result;
    }

    /**
     * @brief Проверить, является ли отображение инъективным (взаимно однозначным)
     * 
     * Отображение f: A → B является инъективным, если f(a₁) = f(a₂) влечет a₁ = a₂
     */
    bool is_injective() const {
        std::unordered_map<Codomain, Domain> seen;
        for (const auto& [x, y] : mapping_) {
            auto it = seen.find(y);
            if (it != seen.end() && it->second != x) {
                return false; // Два разных элемента отображаются в одно значение
            }
            seen[y] = x;
        }
        return true;
    }

    /**
     * @brief Проверить, является ли отображение сюръективным (на)
     * 
     * Отображение f: A → B является сюръективным, если для каждого b ∈ B существует a ∈ A такое, что f(a) = b
     */
    bool is_surjective() const {
        codomain_set image_set = image();
        return image_set == codomain_;
    }

    /**
     * @brief Проверить, является ли отображение биективным (взаимно однозначным и на)
     * 
     * Отображение является биективным, если оно одновременно инъективно и сюръективно
     */
    bool is_bijective() const {
        return is_injective() && is_surjective();
    }

    /**
     * @brief Получить обратное отображение (только для биективных отображений)
     * 
     * @throws std::logic_error если отображение не является биективным
     */
    Mapping<Codomain, Domain> inverse() const {
        if (!is_bijective()) {
            throw std::logic_error("Inverse only exists for bijective mappings");
        }

        std::map<Codomain, Domain> inverse_map;
        for (const auto& [x, y] : mapping_) {
            inverse_map[y] = x;
        }

        return Mapping<Codomain, Domain>(codomain_, domain_, inverse_map);
    }

    /**
     * @brief Получить прообраз элемента
     * 
     * Возвращает множество всех элементов области определения, которые отображаются в данный элемент области значений
     */
    domain_set preimage(const Codomain& y) const {
        domain_set result;
        for (const auto& [x, mapped_y] : mapping_) {
            if (mapped_y == y) {
                result.insert(x);
            }
        }
        return result;
    }

    /**
     * @brief Получить прообраз множества
     */
    domain_set preimage(const codomain_set& Y) const {
        domain_set result;
        for (const auto& y : Y) {
            domain_set preimage_y = preimage(y);
            result = result.union_with(preimage_y);
        }
        return result;
    }

    /**
     * @brief Композиция отображений: (g ∘ f)(x) = g(f(x))
     * 
     * Составляет композицию этого отображения f: A → B с g: B → C, чтобы получить g ∘ f: A → C
     */
    template<typename OtherCodomain>
    Mapping<Domain, OtherCodomain> compose(const Mapping<Codomain, OtherCodomain>& g) const {
        // Проверяем, что область значений этого отображения совпадает с областью определения g
        if (codomain_ != g.domain()) {
            throw std::domain_error("Codomain of first mapping must equal domain of second");
        }

        std::map<Domain, OtherCodomain> composed_map;
        for (const auto& [x, y] : mapping_) {
            composed_map[x] = g(y);
        }

        return Mapping<Domain, OtherCodomain>(domain_, g.codomain(), composed_map);
    }

    /**
     * @brief Проверить равенство отображений
     */
    bool operator==(const Mapping& other) const {
        return domain_ == other.domain_ && 
               codomain_ == other.codomain_ &&
               mapping_ == other.mapping_;
    }

    /**
     * @brief Проверить неравенство отображений
     */
    bool operator!=(const Mapping& other) const {
        return !(*this == other);
    }

private:
    domain_set domain_;
    codomain_set codomain_;
    std::map<Domain, Codomain> mapping_;
    std::function<Codomain(const Domain&)> function_;
};

/**
 * @brief Оператор композиции: g ∘ f
 */
template<typename A, typename B, typename C>
Mapping<A, C> operator*(const Mapping<B, C>& g, const Mapping<A, B>& f) {
    return f.compose(g);
}

/**
 * @brief Тождественное отображение: id_A: A → A, id_A(x) = x
 */
template<typename T>
Mapping<T, T> identity_mapping(const Set<T>& domain) {
    std::map<T, T> identity_map;
    for (const auto& x : domain) {
        identity_map[x] = x;
    }
    return Mapping<T, T>(domain, domain, identity_map);
}

} // namespace cryptomath

