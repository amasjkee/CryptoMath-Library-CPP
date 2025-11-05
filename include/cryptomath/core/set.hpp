#pragma once

#include <set>
#include <algorithm>
#include <iterator>
#include <vector>

namespace cryptomath {

/**
 * @brief Операции над математическими множествами
 * 
 * Предоставляет операции над множествами: объединение, пересечение, разность, дополнение.
 * Работает с любыми типами контейнеров, которые поддерживают операции std::set или могут быть
 * преобразованы в std::set.
 */
template<typename T>
class Set {
public:
    using value_type = T;
    using container_type = std::set<T>;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;

    // Конструкторы
    Set() = default;
    
    explicit Set(const container_type& elements) : elements_(elements) {}
    
    template<typename InputIt>
    Set(InputIt first, InputIt last) : elements_(first, last) {}
    
    Set(std::initializer_list<T> init) : elements_(init) {}

    // Доступ к элементам
    bool contains(const T& element) const noexcept {
        return elements_.find(element) != elements_.end();
    }

    size_t size() const noexcept {
        return elements_.size();
    }

    bool empty() const noexcept {
        return elements_.empty();
    }

    // Итераторы
    const_iterator begin() const noexcept { return elements_.begin(); }
    const_iterator end() const noexcept { return elements_.end(); }
    iterator begin() noexcept { return elements_.begin(); }
    iterator end() noexcept { return elements_.end(); }

    // Операции над множествами
    /**
     * @brief Объединение двух множеств: A ∪ B
     */
    friend Set operator+(const Set& lhs, const Set& rhs) {
        Set result = lhs;
        result.elements_.insert(rhs.elements_.begin(), rhs.elements_.end());
        return result;
    }

    /**
     * @brief Объединение двух множеств: A ∪ B
     */
    Set union_with(const Set& other) const {
        return *this + other;
    }

    /**
     * @brief Пересечение двух множеств: A ∩ B
     */
    Set intersection(const Set& other) const {
        Set result;
        std::set_intersection(
            elements_.begin(), elements_.end(),
            other.elements_.begin(), other.elements_.end(),
            std::inserter(result.elements_, result.elements_.begin())
        );
        return result;
    }

    /**
     * @brief Разность множеств: A \ B
     */
    Set difference(const Set& other) const {
        Set result;
        std::set_difference(
            elements_.begin(), elements_.end(),
            other.elements_.begin(), other.elements_.end(),
            std::inserter(result.elements_, result.elements_.begin())
        );
        return result;
    }

    /**
     * @brief Симметрическая разность: A Δ B = (A \ B) ∪ (B \ A)
     */
    Set symmetric_difference(const Set& other) const {
        Set result;
        std::set_symmetric_difference(
            elements_.begin(), elements_.end(),
            other.elements_.begin(), other.elements_.end(),
            std::inserter(result.elements_, result.elements_.begin())
        );
        return result;
    }

    /**
     * @brief Дополнение относительно универсального множества U: U \ A
     */
    Set complement(const Set& universal_set) const {
        return universal_set.difference(*this);
    }

    /**
     * @brief Проверка, является ли это множество подмножеством другого: A ⊆ B
     */
    bool is_subset_of(const Set& other) const {
        return std::includes(
            other.elements_.begin(), other.elements_.end(),
            elements_.begin(), elements_.end()
        );
    }

    /**
     * @brief Проверка, является ли это множество собственным подмножеством: A ⊂ B
     */
    bool is_proper_subset_of(const Set& other) const {
        return is_subset_of(other) && size() < other.size();
    }

    /**
     * @brief Проверка равенства множеств
     */
    bool operator==(const Set& other) const {
        return elements_ == other.elements_;
    }

    /**
     * @brief Проверка неравенства множеств
     */
    bool operator!=(const Set& other) const {
        return !(*this == other);
    }

    /**
     * @brief Оператор сравнения для использования в std::set
     * 
     * Сравнивает множества лексикографически (по элементам в порядке их сортировки)
     */
    bool operator<(const Set& other) const {
        return elements_ < other.elements_;
    }

    /**
     * @brief Добавить элемент в множество
     */
    void insert(const T& element) {
        elements_.insert(element);
    }

    /**
     * @brief Удалить элемент из множества
     */
    void erase(const T& element) {
        elements_.erase(element);
    }

    /**
     * @brief Очистить все элементы
     */
    void clear() noexcept {
        elements_.clear();
    }

    /**
     * @brief Получить базовый контейнер
     */
    const container_type& data() const noexcept {
        return elements_;
    }

private:
    container_type elements_;
};

/**
 * @brief Декартово произведение двух множеств: A × B
 */
template<typename T, typename U>
Set<std::pair<T, U>> cartesian_product(const Set<T>& set_a, const Set<U>& set_b) {
    Set<std::pair<T, U>> result;
    for (const auto& a : set_a) {
        for (const auto& b : set_b) {
            result.insert(std::make_pair(a, b));
        }
    }
    return result;
}

/**
 * @brief Булеан множества: P(A) - все подмножества A
 */
template<typename T>
Set<Set<T>> power_set(const Set<T>& s) {
    Set<Set<T>> result;
    result.insert(Set<T>{}); // Пустое множество всегда в булеане
    
    if (s.empty()) {
        return result;
    }

    // Преобразуем в вектор для удобной итерации
    std::vector<T> elements(s.begin(), s.end());
    
    // Генерируем все подмножества с помощью битовых операций
    size_t n = elements.size();
    for (size_t mask = 1; mask < (1ULL << n); ++mask) {
        Set<T> subset;
        for (size_t i = 0; i < n; ++i) {
            if (mask & (1ULL << i)) {
                subset.insert(elements[i]);
            }
        }
        result.insert(subset);
    }
    
    return result;
}

} // namespace cryptomath

